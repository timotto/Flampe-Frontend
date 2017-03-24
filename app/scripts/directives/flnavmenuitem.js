'use strict';

/**
 * @ngdoc directive
 * @name flampeFrontendAngularApp.directive:flNavMenuItem
 * @description
 * # flNavMenuItem
 */
angular.module('flampeFrontendAngularApp')
  .directive('flNavMenuItem', ['$location', function ($location) {
    function link(scope){
      scope.currentItem = function(itemLink) {
        if (!(itemLink !== undefined)) return false;
        var pot = '#!'+$location.path();
        return itemLink === pot;
      };
    }
    return {
      templateUrl: 'views/nav-menu-item.html',
      restrict: 'E',
      scope: {
        item: '='
      },
      link: link
    };
  }]);
