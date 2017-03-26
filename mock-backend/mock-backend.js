var express = require('express');
var expressWs = require('express-ws')(express());
var app = expressWs.app;

var state = {
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
    animations: {'ANIM_SOLID':{},'ANIM_GRADIENT':{},'ANIM_FIRE':{},'ANIM_PULSE':{},'ANIM_JUGGLE':{},'ANIM_RAINBOW':{}},
    palettes: {'PALETTE_BASE':{},'PALETTE_HEAT':{},'PALETTE_OCEAN':{},'PALETTE_CLOUD':{},'PALETTE_FOREST':{},'PALETTE_LAVA':{},'PALETTE_RAINBOW':{},'PALETTE_PARTY':{}},
    currentAnimation: 'ANIM_FIRE',
    idleAnimation: 'ANIM_PULSE',
    currentPalette: 'PALETTE_OCEAN',
    idlePalette: 'PALETTE_BASE',
    idleBrightness: 10,
    idleTimeout: 1800
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
    password: 'Some password 123'
  },
  hotspot: {
    ssid: 'Meine Flampe',
    password: 'Mein Passwort'
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
  }
};

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

app.use(function(req,res,next){
  res.header('Access-Control-Allow-Origin','*');
  return next();
});

app.get('/api', function(req, res, next){
  res.json({status:'OK',data: state});
});

app.ws('/api', function(ws, req) {
  ws.on('message', function(msgText) {
    var msg = JSON.parse(msgText);
    if (msg.action === 'push') {
      var data = msg.data;
      softCopy(data, state);

      var aWss = expressWs.getWss('/');
      aWss.clients.forEach(function (client) {
        client.send(msgText);
      });
      console.log('forward push as broadcast [%s]',msgText)
    }
  });
});

app.listen(3000);
console.log('mock backend started');
