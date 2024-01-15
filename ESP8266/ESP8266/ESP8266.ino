// Programmed by: Rico Coelen
//
// The goal of the program is to throw dice from 1, 20 and display it on an LCD. 
// Then save the data to the server, which enables us to see history and statistics of dice throws.
// A led is used to indicate when controller is ready or not to throw dice, and potentiometer to change max possible number in random generation.
//
// MIT License
// Copyright (c) 2023 Rico Coelen

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h> // liquid crystal display with i2c backpack
#include <Wire.h> //i2c communication
#include <ArduinoJson.h>
#include "settings.h" 
#include "config.h"

String chipID;
int buttonPressed = false;
long currentMillis = 0; // miliseconds currently since last save
long oldTime = 0; // saved milliseconds since last save

LiquidCrystal_I2C lcd(0x27, WIDTH_LCD, HEIGHT_LCD);  // set the LCD address to 0x27 for a 20 chars and 4 line display

/*  
 *   This function is used to connect the device to the internet using the default WiFi network and password.
 *   It initiates the connection process. If the connection is not established within a certain time, it tries to reconnect.
 *   If the connection fails, it launches the WiFiManager to allow the user to manually connect and change the access points.
 *   
 *   @param BACKUP_SSID .
 *   @param BACKUP_PASSWORD .
 *   @return void (connection)
 */
void connectToDefault() {

  // initialize wifi modules
  WiFi.mode(WIFI_STA);
  WiFi.begin(BACKUP_SSID, BACKUP_PASSWORD);

  // create a local scoped timer that starts at 0
  int timer = 0; 

  // when not connected try reconeccting
  while (WiFi.status() != WL_CONNECTED && DEBUG_MODE) { 
    delay(DOT_DELAY);
    Serial.print(".");
    timer += DOT_DELAY;
    if (timer > RECONNECT_CANCEL_DELAY)
      break;
  }

  // when connected show debugging information else let the user be able to connect and change access points by himself
  if (WiFi.status() == WL_CONNECTED) { 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("WiFi Status: ");
    Serial.println(WiFi.status());
  }
  else { // if not connection
    WiFiManager wifiManager;
    wifiManager.autoConnect(CONFIG_SSID);
  }
}

/*   
 *   This function is used to initialize the device during the first-time setup process.
 *   Starts the Serial monitor with the specified baud rate. Generates a unique chip ID using the generateChipID function.
 *   Initialized all the pins, connect to the internet using previous function connecttodefault();
 *   Then shows idle screen that the device is ready for user input.
 *  
 *   @return void
 */
void setup() {

  // essentials
  Serial.begin(SERIAL_BAUD_RATE);  // starts the serial monitor

  // generate chipID
  chipID = String(generateChipID());

  // make button pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // register led and turn off
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // initialize LCD
  lcd.init();
  
  // turn on LCD backlight
  lcd.backlight();

  // showing welcome message
  lcd.setCursor(LEFT_LCD, TOP_LCD + 1);  
  lcd.print("Starting:");
  
  lcd.setCursor(LEFT_LCD, TOP_LCD + 2);
  lcd.print(String(PROJECT_SHORT_NAME) + "....");

  // connect to internet using function
  connectToDefault();

  // showing READY message
  lcd.clear();
  lcd.setCursor(LEFT_LCD, TOP_LCD);  
  lcd.print(String(PROJECT_SHORT_NAME));
  lcd.setCursor(LEFT_LCD, TOP_LCD + 1);  
  lcd.print("Is ready,");
  lcd.setCursor(LEFT_LCD, TOP_LCD + 2);
  lcd.print("Press any button");
  lcd.setCursor(LEFT_LCD, TOP_LCD + 3);
  lcd.print("to start!");
}

/* 
 *  This function represents the default loop of the program, 
 *  it reads the analog data from the potentiometer, maps the value to the desired range, and sends a request to the server after a button press. 
 *  It also displays feedback on an LCD screen, showing the maximum number that can be rolled, the rolled number (when thrown), and idle screen with last thrown dice numbers.
 *  
 *  @param potentioMeter The current value read from the potentiometer.
 *  @return void
 */
void loop() {

  // turn on led when ready
  digitalWrite(LED_PIN, HIGH);

  // check with delay agains stuttering
  delay(STUTTERING_DELAY); 
  
  // read values
  if (digitalRead(BUTTON_PIN) == LOW) buttonPressed = true; // when button pressed just wait for next cycle
  int potRead = analogRead (POTENTIOMETER_PIN); // read analog data
  int mappedVal = map(potRead, MIN_VALUE_POTENTIOMETER, MAX_VALUE_POTENTIOMETER, STARTING_DIGIT, MAX_NUM); // map the potentiometer value to the desired range
  //if (DEBUG_MODE) Serial.println(mappedVal);

   // update the potentiometer value in the idle screen
  lcd.setCursor(LEFT_LCD, TOP_LCD + 1);  

  // check if value is double digits then clean the end for that unupdated zero
  if (mappedVal > DOUBLE_DIGITS) {
    lcd.print("Max Number: " + String(mappedVal));
  } else {
    lcd.print("Max Number: " + String(mappedVal) + "  ");
  }
  
  // get deltatime
  currentMillis = millis();

  if(buttonPressed == true) { //  check if button is pressed last cycle

    if (DEBUG_MODE) Serial.println("Button Pressed!");

    //Every requestDelay, send a request to the server
    if (currentMillis > oldTime + REQUEST_DELAY) {

      if (DEBUG_MODE) Serial.println("Sending Event!");

      // turn off led to show its thinking
      digitalWrite(LED_PIN, LOW);
      
      // generate random number, using potentiometer
      int r = random(STARTING_DIGIT, mappedVal);
  
      // send data if connected
      if (WiFi.status() == WL_CONNECTED) {
        sendData(r);
      }
  
      // display feedback rolled number
      lcd.clear();
      displayDice(r);

      // wait when showing the thrown ascii dice art
      delay(DICE_THROW_DELAY); 

      // reset delay counter
      oldTime = currentMillis;
      buttonPressed = false;

      // update screen
      displayIdle(mappedVal);
    }
  }
}

/* 
 *  Displays the idle screen on the LCD with the current potentiometer value and a list of previous throws
 *  obtained from the server. If the request is successful, the text is displayed on the LCD. Otherwise, an error message is displayed.
 *  
 *  @param potentioMeter The current value read from the potentiometer.
 *  @return void
 */
void displayIdle(int potentioMeter) {
  
  //Declare object of class HTTPClient
  HTTPClient http;   
  WiFiClient client;

  // clear screen
  lcd.clear();
  
  // showing homescreen message
  lcd.setCursor(LEFT_LCD, TOP_LCD);  
  lcd.print(String(PROJECT_SHORT_NAME));
  
  lcd.setCursor(LEFT_LCD, TOP_LCD + 1);
  lcd.print("Max Number: " + String(potentioMeter));
  
  lcd.setCursor(LEFT_LCD, TOP_LCD + 2);
  lcd.print("Previous Throws:");
  
  lcd.setCursor(LEFT_LCD, TOP_LCD + 3);

  String message = String(SERVER_URL) + "/api.php?action=read&chipid=" + chipID + "&limit=" + RECORD_LIMIT + "&textready=true";
  if (DEBUG_MODE) Serial.println(message);

  http.begin(client, message); //Specify request destination
  int httpCode = http.GET();
  String payload = http.getString();

  if (httpCode > MINIMUM_CODE) { //Check for the returning code
    lcd.print(PreviousList(payload).substring(LEFT_LCD, WIDTH_LCD));
    if (DEBUG_MODE) {
      Serial.println(httpCode);
      Serial.println(payload);
    }
  }
  else { // else if no success
    lcd.print("! no connection !");
    if (DEBUG_MODE) {
      Serial.println("Error on HTTP request");
      Serial.println(httpCode);
      Serial.println(payload);      
    }
  }
} 


/*
 *  This function takes a JSON string containing an array of rolled numbers, parses it using the ArduinoJson library, and creates a string listing the rolled numbers separated by "<" characters.
 *  The numbers represent the rolled numbers in chronological order and minified for the device.
 *  It then iterates over the entries in the document and appends each rolled number to a holder string.
 *  It uses the textready minified data version in the API to prevent out of memory bug on the device.
 * 
 *  @param json - The json data from the the all the throws of the device with the same chipid.
 *  @return String - a list of numbers in order example: "0 < 2 < 4".
 */
String PreviousList(String json) {
  String holder;
  StaticJsonDocument<200> doc; // Create a StaticJsonDocument object to hold the parsed JSON data
  deserializeJson(doc, json); // Parse the JSON data and store it in the doc object

  int numEntries = doc.size(); // Get the number of entries in the JSON data

  // Iterate over the entries in the original order
  for (int i = 0; i < numEntries; i++) {
    int rolledNumber = doc[i]; // Get the rolled number from the current entry

    // Append the rolled number and separator to the holder string
    holder += rolledNumber;
    if (i != numEntries - 1) { // if not last one
      holder += " < ";
    }
  }
  if (DEBUG_MODE) Serial.println(holder);
  return holder; // return the list
}

/* 
 * Sends an HTTP GET request with the rolled dice number to the API server.
 * 
 * @param number The thrown dice number to be sent in the request.
 * @return void
 */
void sendData(int number) {

  // Declare object of class HTTPClient
  HTTPClient http;  
  WiFiClient client;
  
  // create api call, form the link with the data in the GET parameters
  String message = String(SERVER_URL) + "/api.php?action=create&number="+ number +"&chipid=" + chipID;
  if (DEBUG_MODE) Serial.println(message);

  http.begin(client, message); //Specify request destination
  int httpCode = http.GET(); // do request
  String payload = http.getString(); // Get the response payload

  if (httpCode > MINIMUM_CODE) { //Check for the returning code
    String payload = http.getString();
    if (DEBUG_MODE) {
      Serial.println(httpCode);
      Serial.println(payload);
    }
  }
  else {
    if (DEBUG_MODE) {
      Serial.println("Error on HTTP request");
      Serial.println(httpCode);
      Serial.println(payload);
    }
  }

  http.end();  //Close connection
}

/* 
 *  Display the result of a dice throw with ASCII art on the LCD screen
 *  
 *  @param number: the number thrown by the dice
 *  @return: void
 */
void displayDice(int number) {

  // update visuals ascii dice
  lcd.setCursor(LEFT_LCD, TOP_LCD);
  lcd.print("________");
  
  lcd.setCursor(LEFT_LCD, TOP_LCD + 1);
  lcd.print("|      |  You Threw");
  
  lcd.setCursor(LEFT_LCD, TOP_LCD + 2);

  // check if double digitss to avoid misalignment
  String diceNumber = String(number) + " ";
  if(number > DOUBLE_DIGITS) {
    diceNumber = String(number);
  }

  String diceText; // empty holder string

  switch(number) { // for offical dice between 1-6 show nice text
    case 1:
      diceText = "A One!";
      break;
    case 2:
      diceText = "A Two!";
      break;
    case 3:
      diceText = "A Three!";
      break;
    case 4:
      diceText = "A Four!";
      break;
    case 5:
      diceText = "A Five!";
      break;
    case 6:
      diceText = "A Six!";
      break;   
    default:
      diceText = "A " + String(number) + "!"; // else show the number
      break;   
  }  

  lcd.print("|  " + diceNumber + "  |  " + diceText); // show text or number

  lcd.setCursor(LEFT_LCD, TOP_LCD + 3);
  lcd.print("|______|");
}

/*
 *  Generates a unique chip ID for the ESP8266 based on its hardware address.
 * 
 *  @return int - A four character string representing the ESP8266's chip ID in hexadecimal format.
 */
String generateChipID() {
  String chipIDString = String(ESP.getChipId() & 0xffff, HEX);

  chipIDString.toUpperCase();
  while (chipIDString.length() < MAX_CHIPID_CHARACTERS) // four characters
    chipIDString = String("0") + chipIDString;

  return chipIDString;
}
