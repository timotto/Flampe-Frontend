'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('WifiCtrl', ['$scope','$mdDialog','$translate',function ($scope,$mdDialog,$translate) {
    $scope.wifi = {
      ssid: 'Some ssid',
      password: 'Some password'
    };
    $scope.hotspot = {
      ssid: 'Some ssid',
      password: 'Some password'
    };
    $scope.access = {
      master: {
        login: 'Some login',
        password: 'Some password'
      },
      viewer: {
        login: 'Some login',
        password: 'Some password'
      }
    };
    $scope.connectWifi = function(ev) {
      youAreConnectedWarning(ev, function(){

      });
    };
    $scope.saveHotspot = function(ev) {
      youAreConnectedWarning(ev, function(){

      });
    };
    $scope.savePasswords = function() {

    };

    function youAreConnectedWarning(ev,goOnCb) {
      // Appending dialog to document.body to cover sidenav in docs app
      $translate(['CONNECTED_TITLE','CONNECTED_TEXT','OK','CANCEL']).then(function(translations){
        var confirm = $mdDialog.confirm()
          .title(translations.CONNECTED_TITLE)
          .textContent(translations.CONNECTED_TEXT)
          .ariaLabel('Connected warning')
          .targetEvent(ev)
          .ok(translations.OK)
          .cancel(translations.CANCEL);

        $mdDialog.show(confirm).then(function() {
          goOnCb();
        }, function() {
        });
      });
    }
  }]);
