'use strict';

/**
 * @ngdoc overview
 * @name flampeFrontendAngularApp
 * @description
 * # flampeFrontendAngularApp
 *
 * Main module of the application.
 */
angular
  .module('flampeFrontendAngularApp', [
    'ngMaterial',
    'pascalprecht.translate',
    'ngAnimate',
    'ngAria',
    'ngCookies',
    'ngMessages',
    'ngResource',
    'ngRoute',
    'ngSanitize',
    'ngWebSocket'
  ])
  .config(['$routeProvider','$mdIconProvider','$translateProvider',function ($routeProvider,$mdIconProvider,$translateProvider) { // $mdThemingProvider
    $routeProvider
      .when('/control', {
        templateUrl: 'views/color.html',
        controller: 'ColorCtrl',
        controllerAs: 'color'
      })
      .when('/animation', {
        templateUrl: 'views/animation.html',
        controller: 'AnimationCtrl',
        controllerAs: 'animation'
      })
      .when('/gesture', {
        templateUrl: 'views/gesture.html',
        controller: 'GestureCtrl',
        controllerAs: 'gesture'
      })
      .when('/wifi', {
        templateUrl: 'views/wifi.html',
        controller: 'WifiCtrl',
        controllerAs: 'wifictrl'
      })
      .when('/mqtt', {
        templateUrl: 'views/mqtt.html',
        controller: 'MqttCtrl',
        controllerAs: 'mqttctrl'
      })
      .when('/update', {
        templateUrl: 'views/update.html',
        controller: 'UpdateCtrl',
        controllerAs: 'update'
      })
      .when('/help', {
        templateUrl: 'views/help.html',
        controller: 'HelpCtrl',
        controllerAs: 'help'
      })
      .when('/setup', {
        templateUrl: 'views/setup.html',
        controller: 'SetupCtrl',
        controllerAs: 'setup'
      })
      .otherwise({
        redirectTo: '/control'
      });
    $mdIconProvider
      .icon('help', 'images/ic_help_black_24px.svg', 24)
      .icon('home', 'images/ic_home_black_24px.svg', 24)
      .icon('network_wifi', 'images/ic_network_wifi_black_24px.svg', 24)
      .icon('system_update_alt', 'images/ic_system_update_alt_black_24px.svg', 24)
      .icon('wifi_lock', 'images/ic_wifi_lock_black_24px.svg', 24)
      .icon('wifi_tethering', 'images/ic_wifi_tethering_black_24px.svg', 24)
      .icon('gesture', 'images/gesture.svg', 24)
      .icon('filter_list', 'images/filter_list.svg', 24)
      .icon('color_lens', 'images/color_lens.svg', 24)
      .icon('color_lens_white', 'images/ic_color_lens_white_24px.svg', 24)
      .icon('brightness', 'images/ic_brightness_medium_black_24px.svg', 24)
      .icon('cloud', 'images/ic_cloud_black_24px.svg', 24)
      .icon('linear_scale', 'images/ic_linear_scale_black_24px.svg', 24)
      .icon('menu', 'images/menu.svg', 24)
      .icon('share', 'images/share.svg', 24)
      .icon('mail', 'images/ic_email_black_24px.svg', 24)
      .icon('security', 'images/ic_security_black_24px.svg', 24)
      .icon('blur_on', 'images/ic_blur_on_black_24px.svg', 24)
      .icon('timer', 'images/ic_timer_black_24px.svg', 24)
      .icon('timelapse', 'images/ic_timelapse_black_24px.svg', 24)
      .icon('settings_black', 'images/ic_settings_black_24px.svg', 24);
    // $translateProvider.preferredLanguage('en');
    $translateProvider
      .uniformLanguageTag('bcp47')
      .determinePreferredLanguage()
      .fallbackLanguage('en-GB')
      .useSanitizeValueStrategy('escape');


    // $mdThemingProvider.theme('default')
    //   .primaryPalette('green')
    //   .accentPalette('yellow');

  }])
  // must use flWebsocket once, so it loads data and establishes web socket. Afterwards just $rootScope is used
  .controller('NavigationController',['$scope','$mdSidenav','$location','flWebsocket',function($scope,$mdSidenav,$location,flWebsocket){
    flWebsocket.get();

    $scope.toggleMenu = function() {
      $mdSidenav('left').toggle();
    };
    $scope.goMenu = function(textkey) {
      $scope.menu
        // find textkey
        .filter(function(item){return textkey === item.textkey;})
        // need only link
        .map(function(item){return item.link;})
        // should be just 1
        .forEach(function(link){$location.url(link);});
    };

    $scope.menu = [
      {textkey:'CONTROLS',link:'/control',icon:'color_lens'},
      {textkey:'ANIMATION',link:'/animation',icon:'blur_on'},
      {type:'divider'},
      {textkey:'GESTURE',link:'/gesture',icon:'gesture'},
      {textkey:'WIFI',link:'/wifi',icon:'network_wifi'},
      {textkey:'MQTT',link:'/mqtt',icon:'home'},
      {textkey:'UPDATE',link:'/update',icon:'system_update_alt'},
      {type:'divider'},
      {textkey:'HELP',link:'/help',icon:'help'}
    ];
  }]);
