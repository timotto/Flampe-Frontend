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
        if (!itemLink) {
          return false;
        }
        return itemLink === $location.path();
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
