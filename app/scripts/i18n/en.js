'use strict';

angular.module('flampeFrontendAngularApp')
  .config(['$translateProvider',function($translateProvider){
    $translateProvider.translations('en-GB', {
      TITLE: 'Flamp',
      CONTROLS: 'Controls',
      CONTROLS_TEXT: 'Adjust the brightness and base color',
      ANIMATION: 'Animation',
      ANIMATION_TEXT: 'Animations change hue and brightness over time',
      GESTURE: 'Gestures',
      GESTURE_TEXT: 'Configure the motion sensor',
      WIFI: 'WiFi',
      WIFI_TEXT: 'Set up WiFi parameters',
      MQTT: 'MQTT',
      MQTT_TEXT: 'Integrate Flamp into your home automation setup',
      HELP: 'Help',
      HELP_TEXT: 'Find out what\'s possible with your Flampe',
      UPDATE: 'Update',
      UPDATE_TEXT: 'Update the Flampe firmware',
      BRIGHTNESS: 'Brightness',
      BRIGHTNESS_TEXT: '',
      BRIGHTNESS_FULL: '100%',
      BRIGHTNESS_OFF: 'Off',
      COLOR_PRIMARY: 'Primary color',
      COLOR_PRIMARY_TEXT: 'You see this often',
      COLOR_SECONDARY: 'Secondary color',
      COLOR_SECONDARY_TEXT: 'You see this in animations',
      COLOR_RGB: 'RGB',
      COLOR_RED_SHORT: 'R',
      COLOR_RED_LONG: 'Red',
      COLOR_GREEN_SHORT: 'G',
      COLOR_GREEN_LONG: 'Green',
      COLOR_BLUE_SHORT: 'B',
      COLOR_BLUE_LONG: 'Blue',
      ANIMATION_CURRENT: 'Current animation',
      ANIMATION_CURRENT_TEXT: 'View and modify the current animation',
      ANIMATION_IDLE: 'Idle animation',
      ANIMATION_IDLE_TEXT: 'The idle animation will play after some time',
      TIMEOUT: 'Timeout',
      GESTURE_ABOUT: 'About gestures',
      GESTURE_ABOUT_TEXT: 'An introduction to sensor capabilities and controllable actions',
      GESTURE_ABOUT_CONTENT: 'Some text? Idk. A video maybe, or an interactive demo',
      HELP_CONTENT: 'There is nothing here. Go away and explore.',
      CONTACT_TITLE: 'Contact',
      CONTACT_TEXT: 'Get in touch',
      CONTACT_CONTENT: 'You can always send an email and attach a screen shot, video or description of your problem.',
      MAIL_BUTTON: 'Mail',
      UPDATE_CONTENT: 'You are on firmware {{version}}',
      UPDATE_CONTENT_CHECKING: 'Checking for updates...',
      UPDATE_CONTENT_NO_UPDATES: 'There are currently no updates available for your Flamp.',
      UPDATE_CONTENT_UPDATES: 'There is an updates waiting for your Flamp!',
      UPDATE_CONTENT_INSTALLING: 'Downloading and installing the update. Please do not power off your Flamp!',
      UPDATE_CONTENT_INSTALLED: 'Your Flamp just installed a new firmware version!',
      UPDATE_RECHECK: 'Check again',
      WIFI_CLIENT_TEXT: 'WiFi client',
      WIFI_CLIENT_TITLE: 'Connect to your home WiFi',
      WIFI_CLIENT_CONTENT: 'Enter the credentials for your home WiFi to use this web interface and connect your home automation',
      WIFI_CONNECTED_AS: 'Connected as',
      HOTSPOT_TITLE: 'WiFi hotspot',
      HOTSPOT_TEXT: 'Flamp creates this network if none are found',
      HOTSPOT_CONTENT: 'This will help you set up your Flampe in another environment or when you change your home WiFi.',
      CONNECTED_TITLE: 'You are connected using this network',
      CONNECTED_TEXT: 'You may need to change your WiFi connection to re-connect with your Flamp.',
      OK: 'Ok',
      CANCEL: 'Cancel',
      PASSWORDS_TITLE: 'Passwords',
      PASSWORDS_TEXT: 'Protect this web interface',
      PASSWORDS_MASTER_CONTENT: 'The master login grants access to all controls and settings.',
      PASSWORDS_VIEWER_CONTENT: 'The viewer password grants access to brightness, color, and animation control.',
      MASTER_LOGIN: 'Master login',
      VIEWER_LOGIN: 'Viewer login',
      SSID: 'SSID',
      PASSWORD: 'Password',
      CONNECT: 'Connect',
      SAVE: 'Save',
      MQTT_SERVER_TITLE: 'MQTT Server',
      MQTT_SERVER_TEXT: 'Gateway parameters',
      MQTT_SERVER_CONTENT: 'Your home automation setup most certainly includes an MQTT server. Use it here!',
      MQTT_MSG_TITLE: 'Subjects',
      MQTT_MSG_TEXT: 'Message parameters',
      MQTT_MSG_LISTEN: 'Listen for commands',
      MQTT_MSG_PUB_STATE: 'Publish general state',
      MQTT_MSG_PUB_GESTURE: 'Publish gestures',
      MQTT_MSG_INTOPIC: 'in topic',
      MQTT_MSG_OUTTOPIC: 'out topic',
      DEVICE_NAME: 'Device name',
      HOSTNAME: 'Hostname',
      PORT: 'Port',
      LOGIN: 'Login',
      PALETTE: 'Palette',
      PALETTE_BASE: 'Base colors',
      PALETTE_HEAT: 'Heat',
      PALETTE_OCEAN: 'Ocean',
      PALETTE_CLOUD: 'Clouds',
      PALETTE_FOREST: 'Forest',
      PALETTE_LAVA: 'Lava',
      PALETTE_RAINBOW: 'Rainbow',
      PALETTE_PARTY: 'Party',
      ANIM_SOLID: 'Solid color',
      ANIM_GRADIENT: 'Color gradient',
      ANIM_FIRE: 'Fire',
      ANIM_PULSE: 'Pulse',
      ANIM_JUGGLE: 'Juggle',
      ANIM_RAINBOW: 'Rainbow',
      RANDOMIZE: 'Randomize',
      RECONNECT: 'Reconnect',
      CONNECTED: 'connected',
      DISCONNECTED: 'disconnected',
      CONNECTING: 'connecting',

      BUTTON_LANG_EN: 'English',
      BUTTON_LANG_DE: 'German'
    });
  }]);
