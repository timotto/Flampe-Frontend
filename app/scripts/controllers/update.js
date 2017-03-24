'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('UpdateCtrl', ['$scope','$timeout',function ($scope,$timeout) {
    $scope.status = 'UPDATE_CONTENT_NO_UPDATES';
    $scope.textValues = {
      version: '123.456'
    };
    $scope.showCheck = true;
    $scope.showUpdate = false;
    $scope.showProgress = false;
    $scope.enableCheck = true;
    $scope.enableUpdate = true;
    $scope.enableProgress = true;

    $scope.check = function() {
      if ($scope.status === 'UPDATE_CONTENT_INSTALLING') {
        return;
      } else {
        var butThisTime = $scope.status === 'UPDATE_CONTENT_NO_UPDATES';
        $scope.status = 'UPDATE_CONTENT_CHECKING';
        $scope.enableCheck = false;
        $timeout(function(){
          if (butThisTime) {
            $scope.status = 'UPDATE_CONTENT_UPDATES';
            $scope.showCheck = false;
            $scope.enableCheck = true;
            $scope.showUpdate = true;
            $scope.enableUpdate = true;
          } else {
            $scope.status = 'UPDATE_CONTENT_NO_UPDATES';
            $scope.showCheck = true;
            $scope.enableCheck = true;
            $scope.showUpdate = false;
            $scope.enableUpdate = true;
          }
        }, 1000);
      }
    };
    $scope.doUpdate = function() {
      $scope.status = 'UPDATE_CONTENT_INSTALLING';
      $scope.enableUpdate = false;
      $scope.enableCheck = true;
      $scope.showCheck = true;
      $scope.showProgress = true;
      $timeout(function(){
        $scope.status = 'UPDATE_CONTENT_INSTALLED';
        $scope.enableUpdate = true;
        $scope.showUpdate = false;
        $scope.showProgress = false;
        $scope.textValues.version = parseInt(1+Math.random()*1000)+'.'+parseInt(Math.random()*10000);
      }, 10000);
    };
  }]);
