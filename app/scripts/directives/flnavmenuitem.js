'use strict';

/**
 * @ngdoc directive
 * @name flampeFrontendAngularApp.directive:flNavMenuItem
 * @description
 * # flNavMenuItem
 */
angular.module('flampeFrontendAngularApp')
  .directive('flNavMenuItem', function () {
    return {
      templateUrl: 'views/nav-menu-item.html',
      restrict: 'E',
      scope: {
        item: '='
      }
    };
  });
