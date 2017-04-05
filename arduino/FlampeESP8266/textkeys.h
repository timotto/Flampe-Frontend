#pragma once

const char *animationTextkeys[] = {"ANIM_SOLID","ANIM_GRADIENT","ANIM_FIRE","ANIM_PULSE","ANIM_JUGGLE","ANIM_RAINBOW"};
const char *paletteTextkeys[] = {"PALETTE_BASE","PALETTE_BASE2","PALETTE_BASE3","PALETTE_HEAT","PALETTE_OCEAN","PALETTE_CLOUD","PALETTE_FOREST","PALETTE_LAVA",
                                 "PALETTE_RAINBOW","PALETTE_PARTY"};
const int animationTextkeysCount = 6;
const int paletteTextkeysCount = 10;

const char *JS_setup = "setup";
const char *JS_ledpin = "ledpin";
const char *JS_ledcount = "ledcount";
const char *JS_stripcount = "stripcount";
const char *JS_orientation = "orientation";
const char *JS_orientationStrip = "strip";
const char *JS_orientationZigzag = "zigzag";
const char *JS_orientationSpiral = "spiral";
const char *JS_reverse = "reverse";
const char *JS_data = "data";
const char *JS_brightness = "brightness";
const char *JS_currentAnimation = "currentAnimation";
const char *JS_currentPalette = "currentPalette";
const char *JS_xdir = "xdir";
const char *JS_ydir = "ydir";
const char *JS_gesture = "gesture";
const char *JS_animation = "animation";
const char *JS_xfunc= "xfunc";
const char *JS_yfunc= "yfunc";
const char *JS_gestureFunctionNone = "GESTURE_NONE";
const char *JS_gestureFunctionPalette = "GESTURE_PALETTE";
const char *JS_gestureFunctionAnimation = "GESTURE_ANIMATION";
const char *JS_gestureFunctionOnOff = "GESTURE_ONOFF";

const char *WS_action = "action";
const char *WS_actionGet = "get";
const char *WS_actionPush = "push";
const char *WS_actionPing = "ping";
const char *WS_actionPong = "pong";

const char* TK_colorSsid = "ssid";
const char* TK_colorPassword = "password";

const char* TK_colorRed = "red";
const char* TK_colorGreen = "green";
const char* TK_colorBlue = "blue";

