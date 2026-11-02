#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TRIG_PIN 2
#define ECHO_PIN 3
#define ONE_WIRE_BUS 4
#define LED_PIN 5
#define NUM_LEDS 16

#define DIST_FAR 150
#define DIST_WARN 60
#define DIST_CRITICAL 10

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
OneWire onewire(ONE_WIRE_BUS);
DallasTemperature sensors(&onewire);

unsigned long lastFlashTime = 0;
bool flashState = false;

void setup() {
    Serial.begin(9600);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    sensors.begin();
    strip.begin();
    strip.show();
}

void loop() {
    sensors.requestTemperatures();
    float currentTemp = sensors.getTempCByIndex(0);

    if (currentTemp == DEVICE_DISCONNECTED_C) {
        currentTemp = 20.0;
    }

    float speedOfSound = 331.3 + (0.606 * currentTemp);
    float speedCmPerMicro = speedOfSound / 10000.0;

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);

    float distance = (duration * speedCmPerMicro) / 2.0;

    Serial.print("Temp: ");
    Serial.print(currentTemp);
    Serial.print("C | ");
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance > DIST_FAR || distance <= 0) {
        setStripColor(strip.Color(0, 255, 0));
    } else if (distance > DIST_WARN) {
        setStripColor(strip.Color(255, 180, 0));
    } else if (distance > DIST_CRITICAL) {
        setStripColor(strip.Color(255, 0, 0));
    } else {
        unsigned long currentMillis = millis();
        if (currentMillis - lastFlashTime >= 150) {
            lastFlashTime = currentMillis;
            flashState = !flashState;
        }

        if (flashState) {
            setStripColor(strip.Color(255, 0, 0));
        } else {
            setStripColor(strip.Color(0, 0, 0));
        }
    }

    delay(50);
}

void setStripColor(uint32_t color) {
    for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}
