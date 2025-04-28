// https://arduinogetstarted.com/tutorials/arduino-keypad
// https://esp32io.com/tutorials/esp32-keypad
// https://docs.arduino.cc/libraries/keypad/

#include <Keypad.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>

RTC_DS3231 rtc;

#define LEDPIN 8     // GPIO connected to the data input of the NeoPixel
#define NUMPIXELS 1  // Number of NeoPixels

Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

const int ROW_NUM = 2;
const int COLUMN_NUM = 2;


char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2' },
  { '3', '4' },
};

byte pin_rows[ROW_NUM] = { 11, 10 };     //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = { 2, 3 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

char lastkey = 'a';

uint8_t colorIndex = 0;
unsigned long lastChange = 0;
const unsigned long interval = 1000;
char lastKey = NO_KEY;
KeyState lastKeyState = IDLE;

void setup() {

  Serial.begin(115200);
  delay(100);  // Give time for Serial to stabilize

  pixels.begin();
  pixels.setBrightness(1);
  pixels.setPixelColor(0, pixels.Color(random(0, 256), random(0, 256), random(0, 256)));

  pixels.show();

  Serial.printf("%s %s\n", __TIME__, __DATE__);
}

void loop() {
  char key = keypad.getKey();
  KeyState newKeyState = keypad.getState();

  if (newKeyState != IDLE) {
    if (lastKey == key) {
      return;
    }

    lastKey = key;
    if (key == NO_KEY) {
      return;
    }

    Serial.printf("Key pressed: %c %d\n", key, newKeyState);
    pixels.setBrightness(50);

    switch (lastKey) {
      case '1':
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));  // Red
        break;
      case '2':
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // Green
        break;
      case '3':
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));  // Blue
        break;
      case '4':
        pixels.setPixelColor(0, pixels.Color(128, 0, 128));  // Purple
        break;
      default:
        return;  // Exit early if key isn't 1-4
    }

    pixels.show();
  } else {
    // No key pressed — rotate through colors
    if (millis() - lastChange >= interval) {
      lastChange = millis();
      pixels.setBrightness(1);

      switch (colorIndex) {
        case 0:
          pixels.setPixelColor(0, pixels.Color(255, 0, 0));  // Red
          break;
        case 1:
          pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // Green
          break;
        case 2:
          pixels.setPixelColor(0, pixels.Color(0, 0, 255));  // Blue
          break;
        case 3:
          pixels.setPixelColor(0, pixels.Color(128, 0, 128));  // Purple
          break;
      }

      pixels.show();
      colorIndex = (colorIndex + 1) % 4;  // Loop from 0 to 3
    }
  }
}
