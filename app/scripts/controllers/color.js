'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('ColorCtrl', ['$scope','$rootScope',function ($scope,$rootScope) {
    function randomColor() {
      var color = {
        red: Math.floor(Math.random() * 255),
        green: Math.floor(Math.random() * 255),
        blue: Math.floor(Math.random() * 255)
      };
      return color;
    }

    $scope.state = $rootScope.state;

    $scope.randomizeColor = function(name) {
      $scope.state[name] = randomColor();
    };
  }]);
