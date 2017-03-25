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
      $rootScope.$watch(['state',key].join('.'), function(newValue,oldValue){
        angular.copy($rootScope.state[key], $scope.state[key]);
      }, true);
    });

    $scope.connectWifi = function(ev) {
      youAreConnectedWarning(ev, 'wifi');
    };
    $scope.saveHotspot = function(ev) {
      youAreConnectedWarning(ev, 'hotspot');
    };
    $scope.savePasswords = function() {
      var stateName = 'access';
      angular.copy($scope.state[stateName], $rootScope.state[stateName]);
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
          angular.copy($scope.state[stateName], $rootScope.state[stateName]);
        }, function() {
          angular.copy($rootScope.state[stateName], $scope.state[stateName]);
        });
      });
    }
  }]);
