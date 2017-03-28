'use strict';

/**
 * @ngdoc service
 * @name flampeFrontendAngularApp.flWebsocketClient
 * @description
 * # flWebsocketClient
 * Service in the flampeFrontendAngularApp.
 */
angular.module('flampeFrontendAngularApp')
  .factory('flWebsocket',['$websocket','$http','$rootScope','$interval','$location','$mdToast',function ($websocket,$http,$rootScope,$interval,$location,$mdToast) {

    var wsApiUri,httpApiUri;
    if ($location.port() === 80) {
      wsApiUri = 'ws://' + $location.host() + ':81/api';
      httpApiUri = 'http://' + $location.host() + ':80/api';
    } else {
      var part = '://' + $location.host() + ':3000/api';
      wsApiUri = 'ws' + part;
      httpApiUri = 'http' + part;
    }

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
     * @param trail
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
        reverse: false
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

    /**
     * Load the current state via HTTP GET request
     * @returns {*} promise
     */
    function getState() {
      return $http({
        method: 'GET',
        url: httpApiUri
      }).then(function successCallback(response) {
        upstreamState = response.data.data;
        softCopy(upstreamState, $rootScope.state);
      }, function errorCallback(response) {
        console.log('backend get request failed',response);
        showActionToast('Connection damaged');
      });
    }

    var websocketConnectionCount = 0;

    function setupWebsocket() {
      dataStream = $websocket(wsApiUri);

      dataStream.onMessage(function(message) {
        var msg = JSON.parse(message.data);
        if (msg.action === 'push') {
          softCopy(msg.data, upstreamState);
          softCopy(upstreamState, $rootScope.state);
        }
      });
      dataStream.onOpen(function(){
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
      dataStream.onError(function(){
        showActionToast('Socket connection damaged');
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
        .action('Reconnect')
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
        return getState();
      },
      push: function(data) {
        dataStream.send(JSON.stringify({action:'push', data: data}));
      }
    };

    return methods;
  }]);
