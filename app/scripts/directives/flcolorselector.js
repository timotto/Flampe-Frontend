'use strict';

/**
 * @ngdoc directive
 * @name flampeFrontendAngularApp.directive:flColorSelector
 * @description
 * # flColorSelector
 */
angular.module('flampeFrontendAngularApp')
  .directive('flColorSelector', function () {
    return {
      restrict: 'E',
      scope: {
        color: '='
      },
      templateUrl: 'views/color-selector.html'
    };
  });
