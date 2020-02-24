#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#include <PinButton.h>
#include <SimpleSleep.h>
#include <EEPROM.h>
#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    18
CRGB leds[NUM_LEDS];
#define BRIGHTNESS  120

SimpleSleep Sleep;
PinButton fButton(3);

const long interval = 5000; // seconds for waiting a button push
unsigned long actualMillis = 0;
int cRed = 255;
int cGreen = 0;
int cBlue = 0;
int mSpeed = 1;
int mPause = 750;
boolean mDisplay = 0;
int mTiming = 0;
int mColor = 0;

// the setup routine runs once when you press reset:
void setup() {
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	FastLED.setBrightness(BRIGHTNESS);
	// initialize the digital pin as an output.
	pinMode(0, OUTPUT);
	mTiming = EEPROM.read(1);
	mColor = EEPROM.read(2);
	digitalWrite(0, HIGH);
}

// the loop routine runs over and over again forever:
void loop() {
	fButton.update();
	if (fButton.isLongClick() && mDisplay == 0) {
		mDisplay = 1;
		EEPROM.update(1, mTiming);
		EEPROM.update(2, mColor);
	}
	if (fButton.isSingleClick() && mDisplay == 0) {
		actualMillis = millis();
		leds[mTiming + 1].setRGB(0, 0, 0);
		FastLED.show();
		if (mTiming < 7 ) mTiming++; else mTiming = 0;
		//set_Timing();
	}
	if (fButton.isDoubleClick() && mDisplay == 0) {
		actualMillis = millis();
		if (mColor < 3) mColor++; else mColor = 0;
		//set_Color();
	}
	if (mDisplay == 1){
		display();
	}
	else {
    set_Timing();
    set_Color();
    leds[mTiming + 1].setRGB(0, 0, 255);
		leds[0].setRGB(cRed, cGreen, cBlue);
		battery_level();
		FastLED.show();
		if (millis() - actualMillis > interval) {
			digitalWrite(0, LOW);
			pinMode(2, INPUT);
			Sleep.forever();
		}
	}
}

void display() {
	for (int i = -1; i < NUM_LEDS; i++) {
		leds[i].setRGB(cRed, cGreen, cBlue);
		FastLED.show();
		Sleep.deeplyFor(mSpeed);
		leds[i].setRGB(0, 0, 0);
		FastLED.show();
	}
	for (int i = NUM_LEDS - 1; i >= 0; i--) {
		leds[i].setRGB(cRed, cGreen, cBlue);
		FastLED.show();
		Sleep.deeplyFor(mSpeed);
		leds[i].setRGB(0, 0, 0);
		FastLED.show();
	}
	digitalWrite(0, LOW);
	pinMode(2, INPUT);
	Sleep.deeplyFor(mPause);
	pinMode(2, OUTPUT);
	digitalWrite(0, HIGH);
}

void set_Timing() {
	switch (mTiming) {
	case 0:
		mPause = 750; mSpeed = 1;
		break;
	case 1:
		mPause = 750; mSpeed = 10;
		break;
	case 2:
		mPause = 750; mSpeed = 30;
		break;
	case 3:
		mPause = 750; mSpeed = 50;
		break;
	case 4:
		mPause = 250; mSpeed = 1;
		break;
	case 5:
		mPause = 250; mSpeed = 10;
		break;
	case 6:
		mPause = 250; mSpeed = 30;
		break;
	case 7:
		mPause = 250; mSpeed = 50;
		break;
	}
}

void set_Color() {
	switch (mColor) {
	case 0: // Red
		cRed = 255; cGreen = 0; cBlue = 0;
		break;
	case 1: //Orange
		cRed = 255; cGreen = 165; cBlue = 0;
		break;
	case 2: // Yellow
		cRed = 255; cGreen = 255; cBlue = 0;
		break;
	case 3: // Green
		cRed = 0; cGreen = 255; cBlue = 0;
		break;
	}
}

void battery_level () {
	int vcc = getVcc();
	if (vcc > 3800) {
		leds[9].setRGB(0, 125, 0);
	}
	else if (vcc > 3400 && vcc < 3799) {
		leds[9].setRGB(125, 125, 0);
	}
	else if (vcc > 3000 && vcc < 3399) {
		leds[9].setRGB(125, 61, 0);
	}
	else if (vcc > 2400 && vcc < 2999 ) {
		leds[9].setRGB(125, 0, 0);
	}
}

long getVcc() {
	// read 1.1V reference against AVcc
	// set the reference to Vcc and the measurement to the internal 1.1V reference
	ADMUX = _BV(MUX3) | _BV(MUX2);
	delay(2); // wait for Vref to settle
	ADCSRA |= _BV(ADSC); // start conversion
	while (bit_is_set(ADCSRA, ADSC)); // measuring
	uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
	uint8_t high = ADCH; // unlocks both
	long result = (high << 8) | low;
	result = 1126400L / result; // calculate Vcc (in mV); 1126400 = 1.1*1024*1000 // generally
	return (int) result;
}
