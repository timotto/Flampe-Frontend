'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('MqttCtrl', ['$scope','$rootScope',function ($scope,$rootScope) {
    var sections = {
      'connection': ['host','port','login','password'],
      'messages': ['name','intopic','outtopic','listen','publish']
    };
    function forKeyInSection(section,f) {
      var keys = sections[section];
      if (keys !== undefined) {
        keys.forEach(f);
      }
    }
    $scope.state = angular.copy($rootScope.state);
    $rootScope.$watch('state.mqtt', function(){
      $scope.state.mqtt = angular.copy($rootScope.state.mqtt);
    }, true);
    $scope.save = function(section) {
      forKeyInSection(section, function(key){
        $rootScope.state.mqtt[key] = angular.copy($scope.state.mqtt[key]);
      });
    };
    $scope.reset = function(section) {
      forKeyInSection(section, function(key){
        $scope.state.mqtt[key] = angular.copy($rootScope.state.mqtt[key]);
      });
    };
  }]);
