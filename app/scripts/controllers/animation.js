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
    $scope.animation = {};

    $scope.animations = [
      {textkey:'ANIM_SOLID'},
      {textkey:'ANIM_GRADIENT'},
      {textkey:'ANIM_FIRE'},
      {textkey:'ANIM_PULSE'},
      {textkey:'ANIM_JUGGLE'},
      {textkey:'ANIM_RAINBOW'}
    ];

    $scope.palettes = [
      {textkey:'PALETTE_BASE'},
      {textkey:'PALETTE_HEAT'},
      {textkey:'PALETTE_OCEAN'},
      {textkey:'PALETTE_CLOUD'},
      {textkey:'PALETTE_FOREST'},
      {textkey:'PALETTE_LAVA'},
      {textkey:'PALETTE_RAINBOW'},
      {textkey:'PALETTE_PARTY'}
    ];

    this.awesomeThings = [
      'HTML5 Boilerplate',
      'AngularJS',
      'Karma'
    ];
  }]);
