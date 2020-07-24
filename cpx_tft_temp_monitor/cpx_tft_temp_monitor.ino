// https://github.com/adafruit/Adafruit_CircuitPlayground/tree/master/examples

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define CPLAY_SPEAKER_SHUTDOWN 11
#define CPLAY_LEFTBUTTON        4  ///< left button pin
#define CPLAY_RIGHTBUTTON       5 ///< right button pin
#define CPLAY_SLIDESWITCHPIN    7 ///< slide switch pin
#define CPLAY_NEOPIXELPIN       8 ///< neopixel pin
#define CPLAY_REDLED           13 ///< red led pin
#define CPLAY_IR_EMITTER       25 ///< IR emmitter pin
#define CPLAY_IR_RECEIVER      26 ///< IR receiver pin
#define CPLAY_BUZZER           A0 ///< buzzer pin
#define CPLAY_LIGHTSENSOR      A8 ///< light sensor pin
#define CPLAY_THERMISTORPIN    A9 ///< thermistor pin
#define CPLAY_SOUNDSENSOR      A4 ///< TBD I2S
#define CPLAY_LIS3DH_CS        -1 ///< LIS3DH chip select pin
#define CPLAY_LIS3DH_INTERRUPT 27 ///< LIS3DH interrupt pin
#define CPLAY_LIS3DH_ADDRESS   0x19 ///< LIS3DH I2C address

#define DHTPIN                 A1
#define DHTTYPE    DHT11

// ST7789 display driver
#define TFT_CS                  0
#define TFT_RST                 -1
#define TFT_DC                  1
#define TFT_BACKLIGHT           PIN_A3
#define TFT_MOSI                PIN_A5  // Data out
#define TFT_SCLK                PIN_A4  // Clock out

#define PIXEL_BRIGHTNESS        20
#define PIXEL_COUNT             10
#define SERIESRESISTOR          10000 ///< series resistor for thermistor
#define THERMISTORNOMINAL       10000 ///< resistance of thermistor at 25 degrees C
#define TEMPERATURENOMINAL      25 ///< temp. for nominal resistance (almost always 25 C)
#define BCOEFFICIENT            3380 ///< The beta coefficient of the thermistor (usually 3000-4000)
#define LIGHT_SETTLE_MS         100  
#define INPUT_FLOOR             56  // Lower range of mic sensitivity in dB SPL
#define INPUT_CEILING           110  // Upper range of mic sensitivity in db SPL
#define SAMPLE_WINDOW           10  // Sample window for average level

SPIClass* spi = &SPI;
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
Adafruit_NeoPixel strip(PIXEL_COUNT, CPLAY_NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
DHT_Unified dht(DHTPIN, DHTTYPE);

// Define range of possible acceleration values.
#define MIN_ACCEL -10.0
#define MAX_ACCEL 10.0

uint32_t delayMS;
float p = 3.1415926;

void setup(void) {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting");

  Serial.println("    Setting up buttons");
  pinMode(CPLAY_REDLED, OUTPUT);
  pinMode(CPLAY_LEFTBUTTON, INPUT_PULLDOWN);
  pinMode(CPLAY_RIGHTBUTTON, INPUT_PULLDOWN);
  pinMode(CPLAY_SLIDESWITCHPIN, INPUT_PULLUP);
  
  Serial.println("    Setting up Speaker");
  pinMode(CPLAY_SPEAKER_SHUTDOWN, OUTPUT);
  digitalWrite(CPLAY_SPEAKER_SHUTDOWN, HIGH);

  Serial.println("    Setting up Neopixels");
  strip.begin();
  strip.clear();
  strip.show();
  strip.setBrightness(PIXEL_BRIGHTNESS);

  Serial.println("    Setting up dht11");
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
   
  Serial.println("    Setting up TFT");
  tft.init(240, 240);                // Init ST7789 240x240
  tft.setRotation(2);  
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(5);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);
  tft.setCursor(0, 0);
  tft.print("Starting.");
  
  Serial.println("    Setup complete");
  delay(1000);
}

void loop() {


  tft.fillScreen(ST77XX_BLACK);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  
  float tempInF = (event.temperature * (9.0 / 5.0)) + 32.0;

  tft.setCursor(50, 0);
  tft.print("Temp:");
  //tft.fillRect(50, 50, 200, 100, ST77XX_BLACK);
  tft.setCursor(50, 50);
  tft.print(tempInF);

  dht.humidity().getEvent(&event);

  tft.setCursor(50, 100);
  tft.print("Hdty:");
  //tft.fillRect(50, 150, 200, 200, ST77XX_BLACK);
  tft.setCursor(50, 150);
  tft.print(event.relative_humidity);

  float r = 0.0f;
  float g = 155.0f;
  float b = 0.0f;

  if (tempInF >= 70.0f && tempInF < 76.0f) {

  } else if (tempInF < 70.0f) {

      float howCold = abs(tempInF - 70.0f);
      b = min(255.0f, howCold * 18.0f);
      g = max(0.0f, g - b);

  } else if (tempInF >= 76.0f) {

      float howHot = abs(tempInF - 76.0f);
      r = min(255.0f, howHot * 18.0f);
      g = max(0.0f, g - r);
  }

  if (digitalRead(CPLAY_SLIDESWITCHPIN)) {
    for(int i = 0; i < PIXEL_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(r, g, b));
    }
  } else {
    for(int i = 0; i < PIXEL_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }
  strip.show();

  delay(30000);
}
