'use strict';

/**
 * @ngdoc service
 * @name flampeFrontendAngularApp.flWebsocketClient
 * @description
 * # flWebsocketClient
 * Service in the flampeFrontendAngularApp.
 */
angular.module('flampeFrontendAngularApp')
  .factory('flWebsocket',['$websocket','$http','$rootScope','$interval','$location',function ($websocket,$http,$rootScope,$interval,$location) {

    /**
     * prefix with http, https, ws, wss
     * @type {string}
     */
    var wsApiUri,httpApiUri;
    if ($location.port() === 80) {
      wsApiUri = 'ws' + $location.host() + ':81/';
      httpApiUri = 'http' + $location.host() + ':80/api';
    } else {
      var part = '://' + $location.host() + ':3000/';
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
     * ID of pushChanges() $interval
     * @type {undefined}
     */
    var uiPusherId = undefined;

    /**
     * contains changes done in the UI waiting to be sent to the backend
     *
     * @type {{}}
     */
    var uiChanges = {};

    /**
     * Sends content of uiChanges that has changed for some time or that
     * has been changing for some time constantly
     */
    function pushChanges(unchangedMs, ageMs) {
      var acceptedChanges = {};
      var now = new Date().getTime();
      var unchangedLimit = now - unchangedMs;
      var ageLimit = now - ageMs;
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
        // console.log('change',change);
        softCopy(change, changes);
        delete uiChanges[path];
      });
      console.log('changes',changes);
      methods.push(changes);
      if (Object.keys(uiChanges).length === 0) {
        $interval.cancel(uiPusherId);
        uiPusherId = undefined;
        // console.log('pusher canceled')
      }
    }

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
      }
    };
    softCopy(upstreamState, $rootScope.state);
    objectPaths(upstreamState).forEach(function(path){
      $rootScope.$watch(['state',path].join('.'), function(newState,oldState){
        if (subState(upstreamState, path) !== newState) {
          // console.log('UI change @ [%s] from [%s] to [%s]', path, oldState, newState);
          var now = new Date().getTime();
          var first = now;
          if (uiChanges[path] !== undefined) {
            first = uiChanges[path].first;
          }
          uiChanges[path] = {
            value: newState,
            time: now,
            first: first
          };
          if (uiPusherId === undefined) {
            uiPusherId = $interval(function(){
              pushChanges(200, 1000);
            }, 100);
            // console.log('pusher started')
          }
        }
      });
    });

    function getState() {
      return $http({
        method: 'GET',
        url: httpApiUri
      }).then(function successCallback(response) {
        upstreamState = response.data.data;
        softCopy(upstreamState, $rootScope.state);
      }, function errorCallback(response) {
        console.log('backend get request failed',response)
      });
    }

    var dataStream = $websocket(wsApiUri);

    dataStream.onMessage(function(message) {
      var msg = JSON.parse(message.data);
      if (msg.action === 'push') {
        softCopy(msg.data, upstreamState);
        softCopy(upstreamState, $rootScope.state);
      }
    });

    var methods = {
      state: $rootScope.state,
      get: function() {
        getState();
      },
      push: function(data) {
        dataStream.send(JSON.stringify({action:'push', data: data}));
      }
    };

    getState().then(function(){console.log('getState ok');},function(){console.log('getState failed');});

    return methods;
  }]);
