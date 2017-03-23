'use strict';

describe('Directive: flColorSelector', function () {

  // load the directive's module
  beforeEach(module('flampeFrontendAngularApp'));

  var element,
    scope;

  beforeEach(inject(function ($rootScope) {
    scope = $rootScope.$new();
  }));

  it('should make hidden element visible', inject(function ($compile) {
    element = angular.element('<fl-color-selector></fl-color-selector>');
    element = $compile(element)(scope);
    expect(element.text()).toBe('this is the flColorSelector directive');
  }));
});
