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
    'ngAnimate',
    'ngAria',
    'ngCookies',
    'ngMessages',
    'ngResource',
    'ngRoute',
    'ngSanitize'
  ])
  .config(function ($routeProvider,$mdIconProvider) { // $mdThemingProvider
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
        controllerAs: 'wifi'
      })
      .when('/mqtt', {
        templateUrl: 'views/mqtt.html',
        controller: 'MqttCtrl',
        controllerAs: 'mqtt'
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
      .icon('mail', 'images/mail.svg', 24);

    // $mdThemingProvider.theme('default')
    //   .primaryPalette('green')
    //   .accentPalette('yellow');

  })
  .controller('NavigationController',['$scope','$mdSidenav',function($scope,$mdSidenav){
    $scope.toggleMenu = function() {
      $mdSidenav('left').toggle();
    };

    $scope.menu = [
      {title:'Controls',link:'#!/control',icon:'color_lens',text:'Adjust the brightness and base colors'},
      {title:'Animation',link:'#!/animation',icon:'filter_list',text:'Animations change hue and brightness in patterns over time'},
      {type:'divider'},
      {title:'Gestures',link:'#!/gesture',icon:'gesture',text:'Configure the motion sensor'},
      {title:'WiFi',link:'#!/wifi',icon:'network_wifi',text:'Set up WiFi parameters'},
      {title:'MQTT',link:'#!/mqtt',icon:'home',text:'Integrate Flampe into your home automation setup'},
      {title:'Update',link:'#!/update',icon:'system_update_alt',text:'Update the Flampe firmware'},
      {type:'divider'},
      {title:'Help',link:'#!/help',icon:'help',text:'Find out what\'s possible with your Flampe'}
    ];
  }]);
