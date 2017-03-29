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

      // result.push(num);
      return result;
    }

    $scope.stripcountindex = 0;
    $scope.stripcounts = factorize($scope.state.setup.ledcount);
    $scope.stripcountindex = $scope.stripcounts.indexOf($scope.state.setup.stripcount);
    $scope.ledsPer = ledsPer;
    $scope.stripCount = stripCount;

    $scope.$watch('state.setup.ledcount',function(){
      $scope.stripcounts = factorize($scope.state.setup.ledcount);
      if ($scope.stripcounts.indexOf($scope.state.setup.stripcount) === -1) {
        $scope.stripcountindex = Math.floor($scope.stripcounts.length / 2);
        $scope.state.setup.stripcount = $scope.stripcounts[$scope.stripcountindex];
        console.log('address! index=%d count=%d',$scope.stripcountindex,$scope.state.setup.stripcount)
      }
    });
  }]);
