'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('AnimationCtrl', ['$scope',function ($scope) {
    $scope.animationCurrent = '';
    $scope.animationIdle = '';

    this.awesomeThings = [
      'HTML5 Boilerplate',
      'AngularJS',
      'Karma'
    ];
  }]);
