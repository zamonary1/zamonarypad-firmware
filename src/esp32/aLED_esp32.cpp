#include <Arduino.h>
#include <projectDefs.h>

#include <Adafruit_NeoPixel.h>

const int NUM_LEDS = 2;


Adafruit_NeoPixel strip(2, leds_pin, NEO_RGB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void led_init() {
    pinMode(leds_pin, OUTPUT);
    strip.begin();
    strip.show();
}

void led1_rgb(uint8_t red_col, uint8_t green_col, uint8_t blue_col){
    strip.setPixelColor(1, red_col, green_col, blue_col);
}

void led2_rgb(uint8_t red_col, uint8_t green_col, uint8_t blue_col){
    strip.setPixelColor(0, red_col, green_col, blue_col);
}

void led_update(){
    strip.show();
}
