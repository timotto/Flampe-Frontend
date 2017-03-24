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

    function randomOf(a,key){
      var randomIndex = parseInt(Math.random() * a.length);
      var randomEntry = a[randomIndex];
      var randomValue = randomEntry[key];
      return randomValue;
    }

    $scope.settings = {
      current: randomOf($scope.animations,'textkey'),
      idle: randomOf($scope.animations,'textkey'),
      paletteCurrent: randomOf($scope.palettes,'textkey'),
      paletteIdle: randomOf($scope.palettes,'textkey'),
      idleBrightness: parseInt(Math.random() * 100),
      idleTimeout: parseInt(Math.random() * 100)
    };
  }]);
