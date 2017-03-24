'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('MqttCtrl', ['$scope',function ($scope) {
    $scope.mqtt = {
      host: 'mqtt-server.local',
      port: '1883',
      name: 'flampe-8',
      intopic: 'bude/flampe8/set',
      outtopic: 'bude/flampe8/status',
      listen: {
        commands: true
      },
      publish: {
        state: true
      }
    };
  }]);
