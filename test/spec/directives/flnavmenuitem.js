'use strict';

describe('Directive: flNavMenuItem', function () {

  // load the directive's module
  beforeEach(module('flampeFrontendAngularApp'));

  var element,
    scope;

  beforeEach(inject(function ($rootScope) {
    scope = $rootScope.$new();
  }));

  it('should make hidden element visible', inject(function ($compile) {
    element = angular.element('<fl-nav-menu-item></fl-nav-menu-item>');
    element = $compile(element)(scope);
    expect(element.text()).toBe('this is the flNavMenuItem directive');
  }));
});
