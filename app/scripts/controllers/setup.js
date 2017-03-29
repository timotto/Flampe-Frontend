'use strict';

/**
 * @ngdoc function
 * @name flampeFrontendAngularApp.controller:MainCtrl
 * @description
 * # MainCtrl
 * Controller of the flampeFrontendAngularApp
 */
angular.module('flampeFrontendAngularApp')
  .controller('SetupCtrl', ['$scope','$rootScope',function ($scope,$rootScope) {

    function ledsPer(x,n) {
      if (x === undefined) { return NaN; }
      if (n === undefined) { n = 0; }
      return parseFloat($rootScope.state.setup.ledcount / x).toFixed(n);
    }

    function stripCount(ledsPerStrip) {
      if (ledsPerStrip === undefined) { return NaN; }
      return Math.ceil($rootScope.state.setup.ledcount / ledsPerStrip);
    }

    /**
     * http://stackoverflow.com/questions/22130043/trying-to-find-factors-of-a-number-in-js/30964096#30964096
     * @param num number to factorize
     * @returns {[number]} array of factors including 1 and the number itself
     */
    function factorize(num) {
      var result = [1];
      var half = Math.floor(num / 2), // Ensures a whole number <= num.
        i, j;

      // Determine our increment value for the loop and starting point.
      // num % 2 === 0 ? (i = 2, j = 1) : (i = 3, j = 2);
      if (num % 2 === 0) {
        // even numbers get divided by everything
        i=2;
        j=1;
      } else {
        // even numbers get divided by odds only
        i=3;
        j=2;
      }

      for (i; i <= half; i += j) {
        // num % i === 0 ? result.push(i) : false;
        if (num % i === 0) { result.push(i); }
      }

      result.push(num);
      console.log('factorize result for %d = [%s]', num, result.join(','));
      return result.reverse();
    }

    function updateStriplengths() {
      $scope.striplengths = factorize($scope.state.setup.ledcount);
      if ($scope.striplengths.indexOf($scope.stripcount) === -1) {
        console.log('before $scope.stripcount=%d',$scope.stripcount);
        $scope.stripcount = $scope.striplengths.map(function (l) {
          return {length:l,distance:Math.abs($scope.stripcount - l)};
        }).sort(function(a,b){
          return a.distance - b.distance;
        }).shift().length;
        if (isNaN($scope.stripcount)) {
          console.log('isNan situation', $scope.stripcount);
          $scope.stripcount = $scope.striplengths[Math.floor($scope.striplengths / 2)];
        }
        $scope.stripcountindex = $scope.striplengths.indexOf($scope.stripcount);
        console.log('stripcountindex=%d $scope.stripcount=%d striplengths=%s',$scope.stripcountindex,$scope.stripcount,$scope.striplengths.join(','));
      }
    }
    $scope.stripcount = 1;
    $scope.striplengths = [0];
    $scope.stripcountindex = 1;
    $scope.stripcount = $scope.striplengths[0];
    $scope.ledsPer = ledsPer;
    $scope.stripCount = stripCount;
    $scope.striplength = $scope.striplengths[$scope.stripcountindex-1];

    $scope.$watch('state.setup.ledcount',function(){
      updateStriplengths();
    });

  }]);
