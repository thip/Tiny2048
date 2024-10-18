#ifndef   ARDUINO_H
#define   ARDUINO_H

#include <Arduino.h>

#endif

#include <Adafruit_LIS3DH.h>

#include <math.h>
#include <FastLED.h>
#include <Wire.h>
#include <Ticker.h>

#include <Game.hpp>

#define LED_PIN     GPIO_NUM_46
#define COLOR_ORDER GRB
#define BRIGHTNESS  25
#define CHIPSET     WS2812

#define NUM_LEDS    16

Ticker ticker;

Adafruit_LIS3DH lis = Adafruit_LIS3DH();

CRGB leds[NUM_LEDS];

volatile int count = 0;

Game game;

Direction direction = NONE;
bool reset = true;

CRGB value_color(int value){
  switch (value)
  {
  case 2:
    return CHSV( 0, 255, 200);
  
  case 4:
    return CHSV( 50, 255, 200);

  case 8:
    return CHSV( 100, 255, 200);

  case 16:
    return CHSV( 150, 255, 200);

  case 32:
    return CHSV( 200, 255, 200);

  case 64:
    return CHSV( 250, 255, 170+(sin8((count/4)%255)/3));

  case 128:
    return CHSV( 50, 255, 170+(sin8((count/4)%255)/3));

  case 256: 
    return CHSV( 100, 255, 170+(sin8((count/4)%255)/3));

  case 512: 
    return CHSV( 150, 255, 170+(sin8((count/4)%255)/3));

  case 1024:
    return CHSV( 200, 255, 170+(sin8((count/4)%255)/3));

  case 2048:
    return CHSV(sin8(count%255), 255, 255);
  
  default:
    return CRGB::Black;
  }
}

void draw() {
  for (int x = 0; x < 4; x++){
    for (int y = 0; y < 4; y++){
      if (y & 0b01){
        leds[(3-x)+(y*4)] = value_color(game.Tile(x, y));
      } else {
        leds[(x)+(y*4)] = value_color(game.Tile(x, y));
      }
    }
  }
  
  FastLED.show();
}

void showGameWonSequence()
{
  for (int i = 0; i < 3000; i += 50)
  {
    draw();
    delay(50);
  }
  for (int i = 0; i < 5000; i += 50)
  {
    fill_rainbow(leds, 16, (millis() / 10) % 255);
    FastLED.show();
    delay(50);
  }
}

void showGameLostSequence()
{
  delay(2000);

  for (int i = 0; i < 3; i++){
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(500);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(250);
  }

  delay(1000);
}

void every_milli(){
  count++;
}

void setup() {
  USBSerial.begin(9600);
  USBSerial.setDebugOutput(true);
  USBSerial.write("Starting up\n");
  delay(500);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear(true);
  
  Wire.begin(GPIO_NUM_17, GPIO_NUM_18);
  if (!lis.begin()){
    USBSerial.println("Failed to start accelerometer");
    while(1) yield();
  }

  USBSerial.println("LIS3DH connected");

  lis.read();
  randomSeed(lis.x_g*lis.y_g);

  ticker.attach_ms(1, every_milli);

  game.New();
}

void loop() {  
  lis.read();
  float xAccel = lis.x_g;
  float yAccel = lis.y_g;

  if (reset && !game.Animating()){
    direction = NONE;
    
    if(xAccel > 0.3f){
      direction = RIGHT;
    }

    if(xAccel < -0.3f){
      direction = LEFT;
    }

    if(yAccel > 0.3f){
      direction = UP;
    }

    if(yAccel < -0.3f){
      direction = DOWN;
    }

    if (direction != NONE){
      game.PlayMove(direction);
      reset = false;
    } else if (game.GameWon()){
      showGameWonSequence();
      game.New();
    } else if (!game.MovesAvailable()) {
      showGameLostSequence();
      game.New();
    }
  }

  do {
    game.Step();
    draw();
    delay(50);
    yield();
  } while (game.Animating());


  if (!reset && abs(xAccel) < 0.2f && abs(yAccel) < 0.2f){
    reset = true;
  }

  yield();
}
