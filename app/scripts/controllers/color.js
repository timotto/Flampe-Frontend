'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('ColorCtrl', ['$scope',function ($scope) {
    function randomColor() {
      var color = {
        red: Math.floor(Math.random() * 255),
        green: Math.floor(Math.random() * 255),
        blue: Math.floor(Math.random() * 255)
      };
      return color;
    }

    $scope.brightness = Math.floor(Math.random() * 100);
    $scope.primary=randomColor();
    $scope.accent=randomColor();

    $scope.randomizeColor = function(name) {
      $scope[name] = randomColor();
    };
  }]);
