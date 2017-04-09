var express = require('express');
var app = express();
var expressWs = require('express-ws')(app);

app.ws('/api', function(ws, req) {
  // ws.send(JSON.stringify({action:'push',data: {status: 'connected'}}));
  ws.send(JSON.stringify({action:'push',data: state}));
  ws.on('message', function(msgText) {
    var msg = JSON.parse(msgText);
    if (msg.action === 'get') {
      // ws.send(JSON.stringify({action:'push',data: state}));
      // console.log('pushed state as get response');
      console.log('received get message - ignored');
    } else if (msg.action === 'push') {
      var data = msg.data;
      softCopy(data, state);

      var aWss = expressWs.getWss('/');
      aWss.clients.forEach(function (client) {
        client.send(msgText);
      });
      console.log('forward push as broadcast [%s]',msgText)
    } else {
      console.error('got unknown ws msg',msg);
    }
  });
});

app.listen(9001);
console.log('mock backend started');

function softCopy(src,dst) {
  Object.keys(src).forEach(function(srcKey){
    var type = typeof src[srcKey];
    if ('object' === type) {
      if (dst[srcKey] === undefined) {
        dst[srcKey] = {};
      }
      softCopy(src[srcKey], dst[srcKey]);
    } else {
      dst[srcKey] = src[srcKey];
    }
  });
}

var state = {
  status: 'connected',
  connection: 'sta',
  brightness: parseInt(Math.random() * 100),
  primary: {
    red: Math.floor(Math.random() * 255),
    green: Math.floor(Math.random() * 255),
    blue: Math.floor(Math.random() * 255)
  },
  accent: {
    red: Math.floor(Math.random() * 255),
    green: Math.floor(Math.random() * 255),
    blue: Math.floor(Math.random() * 255)
  },
  animation:{
    animations: {'ANIM_SOLID':{},'ANIM_GRADIENT':{},'ANIM_FIRE':{},'ANIM_PULSE':{},'ANIM_JUGGLE':{},'ANIM_RAINBOW':{},'ANIM_TIMER':{}},
    palettes: {'PALETTE_BASE':{},'PALETTE_HEAT':{},'PALETTE_OCEAN':{},'PALETTE_CLOUD':{},'PALETTE_FOREST':{},'PALETTE_LAVA':{},'PALETTE_RAINBOW':{},'PALETTE_PARTY':{}},
    currentAnimation: 'ANIM_FIRE',
    idleAnimation: 'ANIM_PULSE',
    currentPalette: 'PALETTE_OCEAN',
    idlePalette: 'PALETTE_BASE',
    idleBrightness: 10,
    idleTimeout: 1800
  },
  timer: {
    duration: 120,
    elapsed: 30,
    running: false
  },
  mqtt: {
    host: 'mqtt-server.local',
    port: '1883',
    login: undefined,
    password: undefined,
    name: 'flampe-8',
    intopic: 'bude/flampe8/set',
    outtopic: 'bude/flampe8/status',
    listen: {
      commands: true
    },
    publish: {
      state: true,
      gesture: false
    }
  },
  wifi: {
    ssid: 'FritzBox23492',
    password: 'Some password 123',
    connected: true,
    ip: '192.168.56.123',
    name: 'flampe-123'
  },
  hotspot: {
    ssid: 'Meine Flampe',
    password: 'Mein Passwort',
    ip: '192.168.4.1'
  },
  access: {
    master: {
      login: 'Some login',
      password: 'Some password'
    },
    viewer: {
      login: '',
      password: ''
    }
  },
  setup: {
    ledcount: 119,
    reverse: false,
    orientation: 'spiral',
    colororder: 'RGB',
    stripcount: 7,
    turns: 19,
    dimensions: 3
  }
};
