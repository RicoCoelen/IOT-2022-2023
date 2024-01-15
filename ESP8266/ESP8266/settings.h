#ifndef _SETTINGS_H
#define _SETTINGS_H

/* 
 *  Program settings
 */
#define DEBUG_MODE 1
#define SERIAL_BAUD_RATE 115200
#define RECORD_LIMIT 5 // set the maximum records for api requests
#define MAX_NUM 20 // set the maximum number for random generation
#define MAX_CHIPID_CHARACTERS 4
#define DOUBLE_DIGITS 9 // where double digits starts like 10 -_-
#define STARTING_DIGIT 1 // like in dice from 1-6
#define MIN_VALUE_POTENTIOMETER 0 
#define MAX_VALUE_POTENTIOMETER 1023 
#define STUTTERING_DELAY 20 // delay to remove spikes from analog read
#define MINIMUM_CODE 0 // minimum acceptable http code

/* 
 *  LCD BOILERPLATE SETTINGS
 */
#define WIDTH_LCD 20 
#define HEIGHT_LCD 4
#define TOP_LCD 0
#define LEFT_LCD 0

/* 
 *  Pins
 */
#define BUTTON_PIN 14
#define POTENTIOMETER_PIN A0
#define LED_PIN 0

/* 
 *  Delays
 */
#define DICE_THROW_DELAY 2000
#define REQUEST_DELAY 1000
#define RECONNECT_CANCEL_DELAY 10000
#define DOT_DELAY 500

#endif
