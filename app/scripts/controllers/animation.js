'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('AnimationCtrl', ['$scope','$rootScope',function ($scope,$rootScope) {
    $scope.settings = $rootScope.state.animation;
  }]);
