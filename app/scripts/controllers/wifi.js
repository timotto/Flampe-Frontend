'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('WifiCtrl', ['$rootScope','$scope','$mdDialog','$translate',function ($rootScope,$scope,$mdDialog,$translate) {

    // working on a copy here, changes are sent via button
    $scope.state = angular.copy($rootScope.state);

    // watch those in rootScope, will be updated there if upstream changes
    ['wifi','hotspot','access'].forEach(function(key){
      $rootScope.$watch(['state',key].join('.'), function(){
        angular.copy($rootScope.state[key], $scope.state[key]);
      }, true);
    });

    function confirmState(stateName) {
      angular.copy($scope.state[stateName], $rootScope.state[stateName]);
    }

    $scope.connectWifi = function(ev) {
      if ($scope.state.connection === 'sta') {
        youAreConnectedWarning(ev, 'wifi');
      } else {
        confirmState('wifi');
      }
    };
    $scope.saveHotspot = function(ev) {
      if ($scope.state.connection === 'ap') {
        youAreConnectedWarning(ev, 'hotspot');
      } else {
        confirmState('hotspot');
      }
    };
    $scope.savePasswords = function() {
      confirmState('access');
    };

    function youAreConnectedWarning(ev,stateName) {
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
          confirmState(stateName);
        }, function() {
          angular.copy($rootScope.state[stateName], $scope.state[stateName]);
        });
      });
    }
  }]);
