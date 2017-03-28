'use strict';

/**
 * @ngdoc service
 * @name flampeFrontendAngularApp.flWebsocketClient
 * @description
 * # flWebsocketClient
 * Service in the flampeFrontendAngularApp.
 */
angular.module('flampeFrontendAngularApp')
  .factory('flWebsocket',['$websocket','$http','$rootScope','$interval','$timeout','$location','$mdToast','$translate', function ($websocket,$http,$rootScope,$interval,$timeout,$location,$mdToast,$translate) {

    // different WebSocket port when served from port 80
    var wsPort = $location.port() === 80?81:3081;
    var wsProto = $location.protocol() === 'https'?'wss':'ws';
    var wsApiUri = wsProto + '://' + $location.host() + ':' + wsPort + '/api';

    var texts = {};
    $translate(['CONNECTED','CONNECTING','DISCONNECTED','RECONNECT']).then(function(translations){texts = translations;});

    /**
     * assumes dst is not undefined and of same type as src (object or Array)
     * @param src
     * @param dst
     */
    function softCopy(src,dst) {
      Object.keys(src).forEach(function(srcKey){
        var item = src[srcKey];

        var type = typeof item;
        if ('object' === type) {
          if (dst[srcKey] === undefined) {
            if (Array.isArray(item)) {
              dst[srcKey] = [];
            } else {
              dst[srcKey] = {};
            }
          }
          softCopy(item, dst[srcKey]);
        } else {
          dst[srcKey] = item;
        }
      });
    }

    /**
     * {a:'a',b:'b'} -> ['a','b']
     * {a:{b:'b'},c:'c'} -> ['a.b','c']
     * {a:{b:'b',c:{d:'d',e:'e'}},f:'f'} -> ['a.b','a.c.d','a.c.e','f']
     * @param o
     * @returns {Array}
     */
    function objectPaths(o) {
      var result = [];
      Object.keys(o).forEach(function(key){
        if (typeof o[key] !== 'object') {
          result.push(key);
        } else {
          var subResults = objectPaths(o[key]);
          subResults.forEach(function(r){
            result.push([key,r].join('.'));
          });
        }
      });
      return result;
    }

    function subState(state,path) {
      var x = state;
      var steps = path.split('\.');
      steps.forEach(function(step){
        x = x[step];
      });
      return x;
    }

    /**
     * Utility class to send changes to the backend after some grace time to prevent
     * sliders in the UI used on mobile devices from flooding the backend with updates.
     * The first change is sent instantaneously, consecutive changes are collected for
     * at least unchangedMs ms and up to ageMs ms before they are send as collection with
     * other also expired changes.
     * @param unchangedMs ms that must have passed since the last update to this value
     * @param ageMs ms that must have passed since the first update to this value
     * @param longTimeMs ms that must have passed since that last change to consider this value for instant send
     * @constructor
     */
    function ChangePusher(unchangedMs, ageMs, longTimeMs) {
      this.unchangedMs = unchangedMs;
      this.ageMs = ageMs;
      this.longTimeMs = longTimeMs;
      this.tickMs = unchangedMs / 2;

      this.timerId = undefined;
      this.lastChangeTime = undefined;

      this.uiChanges = {};
    }

    /**
     * Private function called by change() method. Starts the $interval function.
     */
    ChangePusher.prototype.startTimer = function(){
      if (this.timerId !== undefined) {
        // already started
        return;
      }
      var that = this;
      this.timerId = $interval(function(){
        that.push(that.unchangedMs, that.ageMs);
      }, that.tickMs);
    };

    /**
     * Private function called by push() method. Stops the $interval function.
     */
    ChangePusher.prototype.stopTimer = function(){
      // check if running
      if (this.timerId !== undefined) {
        $interval.cancel(this.timerId);
        this.timerId = undefined;
      }
    };

    /**
     * Private function called by the $interval. Checks the age of pending changes, collects the
     * expired ones, builds an Object from their values and sends it to the backend.
     * @param unchangedMs ms that must have passed since the last modification of a value
     * @param ageMs ms that must have passed since the first modification of a value
     */
    ChangePusher.prototype.push = function(unchangedMs, ageMs){
      var acceptedChanges = {};
      var now = new Date().getTime();
      var unchangedLimit = now - unchangedMs;
      var ageLimit = now - ageMs;
      var uiChanges = this.uiChanges;
      Object.keys(uiChanges).forEach(function(path){
        if (uiChanges[path].time > unchangedLimit && uiChanges[path].first < ageLimit) {
          return;
        }
        acceptedChanges[path] = uiChanges[path].value;
      });

      var changes = {};
      Object.keys(acceptedChanges).forEach(function(path){
        var change = {};
        var x = change;
        var steps = path.split('\.');
        var lastStep = steps.pop();
        steps.forEach(function(p){
          x[p] = {};
          x = x[p];
        });
        x[lastStep] = acceptedChanges[path];
        softCopy(change, changes);
        delete uiChanges[path];
      });
      console.log('changes',changes);
      methods.push(changes);
      if (Object.keys(uiChanges).length === 0) {
        // stop ticker if there are no more changes left
        this.stopTimer();
      }
    };

    /**
     * Private function called by change(). Push the stored changes
     * now, regardless of their age or more recently added changes.
     */
    ChangePusher.prototype.pushNow = function(){this.push(0, 0);};

    /**
     * Notify the ChangePusher of a new change to push to the backend.
     * @param path object path in . notation (eg data.some.value)
     * @param value new value
     */
    ChangePusher.prototype.change = function(path, value) {
      var now = new Date().getTime();

      var first = this.uiChanges[path] !== undefined?this.uiChanges[path].first:now;

      this.uiChanges[path] = {
        value: value,
        time: now,
        first: first
      };

      if (this.lastChangeTime === undefined || (this.lastChangeTime + this.longTimeMs < now)) {
        this.pushNow();
      } else {
        this.startTimer();
      }

      this.lastChangeTime = now;
    };

    var changePusher = new ChangePusher(300, 1000, 3000);

    $rootScope.state = {};
    var upstreamState = {
      status: 'disconnected',
      brightness: undefined,
      primary: {red:undefined,green:undefined,blue:undefined},
      accent: {red:undefined,green:undefined,blue:undefined},
      animation: {
        animations: {},
        palettes: {},
        currentAnimation: undefined,
        idleAnimation: undefined,
        currentPalette: undefined,
        idlePalette: undefined,
        idleBrightness: undefined,
        idleTimeout: undefined
      },
      mqtt: {
        host: undefined,
        port: undefined,
        login: undefined,
        password: undefined,
        name: undefined,
        intopic: undefined,
        outtopic: undefined,
        listen: {
          commands: undefined
        },
        publish: {
          state: undefined,
          gesture: undefined
        }
      },
      wifi: {ssid:undefined,password:undefined},
      hotspot: {ssid:undefined,password:undefined},
      access: {
        master: {
          login: undefined,
          password: undefined
        },
        viewer: {
          login: undefined,
          password: undefined
        }
      },
      setup: {
        ledcount: 0,
        reverse: false,
        orientation: 'vstrip',
        colororder: 'RGB'
      }
    };
    softCopy(upstreamState, $rootScope.state);

    // register individual watchers for each and every property
    objectPaths(upstreamState).forEach(function(path){
      // TODO might the third boolean in $watch do some good here? To allow a single watcher.
      $rootScope.$watch(['state',path].join('.'), function(newState){
        if (subState(upstreamState, path) !== newState) {
          changePusher.change(path, newState);
        }
      });
    });

    var websocketConnectionCount = 0;

    var websocketReconnectTimerId;
    const websocketReconnectTimeoutMin = 500;
    const websocketReconnectTimeoutMax = 5000;
    var websocketReconnectTimeout = 0;

    function cancelWebsocketReconnect() {
      if (websocketReconnectTimerId !== undefined) {
        $timeout.cancel(websocketReconnectTimerId);
        websocketReconnectTimerId = undefined;
      }
    }

    function startWebsocketReconnect() {
      if (websocketReconnectTimeout < websocketReconnectTimeoutMin) {
        websocketReconnectTimeout = websocketReconnectTimeoutMin;
      } else if (websocketReconnectTimeout < websocketReconnectTimeoutMax) {
        // exponential backoff
        websocketReconnectTimeout = Math.min(websocketReconnectTimeoutMax, websocketReconnectTimeout * 2);
      }
      websocketReconnectTimerId = $timeout(function(){
        websocketReconnectTimerId = undefined;
        setupWebsocket();
      }, websocketReconnectTimeout);
    }

    function setupWebsocket() {

      cancelWebsocketReconnect();

      if (dataStream !== undefined) {
        // if there still was some stream, close it now
        dataStream.close(true);
      }
      console.log('ws connecting');
      dataStream = $websocket(wsApiUri);

      dataStream.onMessage(function(message) {
        var msg = JSON.parse(message.data);
        if (msg.action === 'push') {
          softCopy(msg.data, upstreamState);
          softCopy(upstreamState, $rootScope.state);
        }
      });
      dataStream.onOpen(function(){
        console.log('ws connected');
        dataStream.send(JSON.stringify({action:'get'}));

        websocketReconnectTimeout = 0;
        cancelWebsocketReconnect();
        // don't show 'connected' toast on first connection, only after re-connects from errors
        if (websocketConnectionCount++ > 0) {
          var pinTo = getToastPosition();

          $mdToast.show(
            $mdToast.simple()
              .textContent('Socket connected!')
              .position(pinTo )
              .hideDelay(3000)
          );
        }
      });
      dataStream.onClose(function(){
          console.log('ws closed');
          $rootScope.state.status = upstreamState.status = 'disconnected';
          startWebsocketReconnect();
          showActionToast(texts.DISCONNECTED);
        }
      );
      dataStream.onError(function(e){
        console.error('ws error',e);
      });
    }

    function getToastPosition() {
      var position = {
        bottom: true,
        top: false,
        left: false,
        right: false
      };
      return Object.keys(position)
        .filter(function(pos) { return position[pos]; })
        .join(' ');
    }

    function showActionToast(text) {
      var pinTo = getToastPosition();
      var toast = $mdToast.simple()
        .textContent(text)
        .action(texts.RECONNECT)
        .hideDelay(5000)
        .highlightAction(true)
        .highlightClass('md-warn')// Accent is used by default, this just demonstrates the usage.
        .position(pinTo);

      $mdToast.show(toast).then(function(response) {
        if ( response === 'ok' ) {
          methods.get().then(function(){},function(){});
          dataStream.close(true);
          setupWebsocket();
        }
      });
    }

    var dataStream;

    setupWebsocket();

    var methods = {
      state: $rootScope.state,
      get: function() {
        dataStream.send(JSON.stringify({action:'get'}));
      },
      push: function(data) {
        dataStream.send(JSON.stringify({action:'push', data: data}));
      },
      login: function(){}
    };

    return methods;
  }]);
