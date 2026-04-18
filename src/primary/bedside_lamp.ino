/* =================================================================
 * Bedside Lamp: Ultimate bedside dual-lamp and clock
 * April, 2026
 * Version 0.37
 * Copyright ResinChemTech - released under the Apache 2.0 license
 * ================================================================= */
#include <WiFi.h>               //Core WiFi services
#include <WiFiClient.h>         //Arduino ESP Core - creates a client that can connect to an IP address
#include <ESPping.h>            //v1.0.5 - ping remote IP to test connectivity (e.g bulb, display, etc.)
#include <ESPmDNS.h>            //mDNS service for Arduino OTA updates
#include <WebServer.h>          //Local web server (for settings pages)
#include <LittleFS.h>           //Read and write to flash storage
#include <Wire.h>               //I2C
#include <HTTPClient.h>         //For syncing weather from OpenWeatherMap (HTTP Get command) / Controlling bulb (HTTP Post command)
#include <ArduinoJson.h>        //Reading and writing JSON files (e.g. config file)
#include <WiFiUdp.h>            //WiFi UDP functions
#include <ArduinoOTA.h>         //OTA Updates via Arduino IDE
#include <Update.h>             //OTA Updates via web page
#define FASTLED_INTERNAL        //Suppress FastLED SPI/bitbanged compiler warnings (only applies after first compile)
#include <FastLED.h>            //LED control
#include <PubSubClient.h>       //https://github.com/knolleary/pubsubclient  Provides MQTT functions (v2.8)
#include "html.h"               //html code for the web settings pages

#define VERSION "v0.37"
#define APPNAME "BEDSIDE LAMP"
#define WIFIMODE 2                                    // 0 = Only Soft Access Point, 1 = Only connect to local WiFi network with UN/PW, 2 = Both (both needed for onboarding)
#define SERIAL_DEBUG 0                                // 0 = Disable (must be disabled if using RX/TX pins), 1 = enable
//#define MQTTMODE 1                                    // 0 = Disable MQTT, 1 = Enable (will only be enabled if WiFi mode = 1 or 2 - broker must be on same network)

#define FORMAT_LITTLEFS_IF_FAILED true                // DO NOT CHANGE

// ==========================================================================
//  *** REVIEW AND UPDATE THESE VARIABLES TO MATCH YOUR ENVIRONMENT/BUILD ***
// ==========================================================================
//Pin Definitions - Update if your build is different
#define LED_DATA_PIN 4    //GPIO Pin for LED Strip Data Signal
#define TOUCH1_PIN 18     //GPIO Pin for Touch Sensor 1
#define TOUCH2_PIN 19     //GPIO Pin for Touch Sensor 2

//Misc Definitions - Only change if your build is different
#define NUM_LEDS 30       // Default to 30 LED's (can be changed in settings)
#define NUM_LEDS_MAX 99   // Maximum number of LEDs allowed (enforced via web page)
#define MILLI_AMPS 1000   // Limit to 1A if circuit shared with USB charging ports
//MQTT Handling - needed to handle in main loop and to avoid thread conflicts
#define MSK_BULBSTATE   0x0001
#define MSK_BULBMODE    0x0002
#define MSK_BULBCOLOR   0x0004
#define MSK_BULBTEMP    0x0008
#define MSK_BULBBRIGHT  0x0010
#define MSK_LEDSTATE    0x0020
#define MSK_LEDCOLOR    0x0040
#define MSK_LEDBRIGHT   0x0080
#define MSK_TOUCH1_STATE 0x0100
#define MSK_TOUCH1_FUNC  0x0200
#define MSK_TOUCH1_FUNCA 0x0400
#define MSK_TOUCH2_STATE 0x0800
#define MSK_TOUCH2_FUNC  0x1000
#define MSK_TOUCH2_FUNCA 0x2000


/* ================ Default Starting Values ====================================
 * All of these following values can be modified and saved via
 * the web interface and the values below normally do not need to be modified.
 * Using the web settings is the preferred way to change these default values.  
 * Values listed here are just starting values before the config file exists or
 * if it can't be read.
 ===============================================================================*/
//----------------------------------
//Local Variables (wifi/onboarding)
//----------------------------------
String deviceName = "BedsideLamp01";  // Default Device Name - 16 chars max, no spaces.
String wifiHostName = deviceName;     // Use Device Name for wifi and mDNS name
String wifiSSID = "";                 
String wifiPW = "";
byte macAddr[6];                      // Array of device mac address as hex bytes (reversed)
String strMacAddr;                    // Formatted string of device mac address
String baseIPAddress;                 // Device assigned IP Address (this controller)
bool onboarding = false;              // Will be set to true if no config file or wifi cannot be joined

//----------------------------------
//Local Variables (MQTT/Discovery)
//----------------------------------
//  MQTT will only be used if a server address other than '0.0.0.0' is entered via portal
byte mqttAddr_1 = 0;
byte mqttAddr_2 = 0;
byte mqttAddr_3 = 0;
byte mqttAddr_4 = 0;
int mqttPort = 0;
String mqttIPAddress = "0.0.0.0";   //IP address of MQTT broker (will be concatenated from 4 vars above)
String mqttClient = "blprimary'";   //Unique client name for MQTT
String mqttUser = "myusername";     //MQTT broker user name
String mqttPW = "mypassword";       //MQTT broker password
uint32_t mqttTelePeriod = 60;       //seconds - how often MQTT values published (outside of local state changes)
uint32_t lastMqttRetryAttempt = 0;  //Non-blocking handler for MQTT reconnection
const int MQTT_RETRY_DELAY = 5000;  //Delay (ms) between reconnect attempts
String mqttTopicSub ="bedlamp";     //Can be changed via web app (preferred method)
String mqttTopicPub = "bedlamp";    //Can be changed via web app (preferred method)
bool mqttEnabled = false;           //Will be enabled/disabled depending on whether a valid IP address is defined in Settings (0.0.0.0 disables MQTT)
bool mqttConnected = false;         //Will be enabled if defined and successful connnection made.  This var should be checked upon any MQTT action.
bool forceMQTTUpdate = false;       //For forcing MQTT update on state change
bool initialSyncRequired = false;   //Flag for initial publish of MQTT data after boot
bool rebootRequired = false;        //Needed for saveconfig command to allow return vals and to flush buffer before a reboot
bool touch1MqttLatched = false;     //For publishing touch sensor state changes via MQTT
bool touch2MqttLatched = false;
int syncStep = 0;
unsigned long lastSyncStepTime = 0;
volatile uint32_t mqttUpdateMask = 0;  //Volatile so it can be updated by out-of-thread background tasks
const int INIT_SYNC_INTERVAL = 100;    //100ms between publishing

struct DiscoverySettings {        //Structure for HA MQTT Discovery 
  bool exists = false;
  String deviceName = "";
  bool lightBulb = true;
  bool ledStrip = true;
  bool display = true;
  bool touch = true;
  bool alarms = true;
  bool diagnostics = true;
};
DiscoverySettings currentDiscovery;
//----------------------------------
// Local Variables (OTA Updates)
//----------------------------------
//OTA Variables
String otaHostName = deviceName + "_OTA";  // Will be updated by device name from onboarding + _OTA
bool ota_flag = true;                      // Must leave this as true for board to broadcast port to IDE upon boot
uint16_t ota_boot_time_window = 2500;      // Minimum time on boot for IP address to show in IDE ports, in millisecs
uint16_t ota_time_window = 20000;          // Time allowed to start file upload when ota_flag set to true (after initial boot), in millsecs
uint16_t ota_time_elapsed = 0;             // Counter when OTA active
uint16_t ota_time = ota_boot_time_window;
uint8_t web_otaDone = 0;                   

//----------------------------------
//Local Variables (Lights: LED/Bulb)
//----------------------------------
//LED strip
uint16_t numLEDs = NUM_LEDS;                // Total number of LED pixels
bool useLEDs = true;                        // Enable/disable LED strip functionality
bool defaultLEDState = false;               // Default LED state after boot
bool activeLEDState = false;                // Current State of LEDs (on/off)
String defaultLEDColor = "#FFA500";         // Orange
String activeLEDColor = "#FFA500";          // Orange
byte defaultLEDBrightness = 64;             // Default starting brightness at boot. 255=max brightness (based on milliamp rating of power supply)
byte activeLEDBrightness = 64;              // Active brighness. LEDs considered "off" when value = 0.
byte lastLEDBrightness = 64;                // Holding varible to return to prior state when LEDs have been off.
byte defaultLEDEffect = 0;                  // 0 = Solid.  See documentation for other values/effects.
byte activeLEDEffect = 0;                   // Active effect
String defaultLEDEffectColor1 = "#FF0000";  // Red
String defaultLEDEffectColor2 = "#00FF00";  // Green
String defaultLEDEffectColor3 = "#0000FF";  // Blue
String activeLEDEffectColor1 = "#FF0000";   // Red
String activeLEDEffectColor2 = "#00FF00";   // Green
String activeLEDEffectColor3 = "#0000FF";   // Blue 
bool useBootLightsLED = true;               // Test/Use LED strip to indicate connectivity/boot stages (LEDs flash lights during boot)
bool ledTestFlag = false;                   // Flag to indicate whether testing LED settings via web app
uint16_t ledTestNumLeds = 30;               // Holding var for running Integration/LED page tests
byte ledTestBrightness = 128;               // Holding var for running LED page tests
String ledTestColor = "#FF0000";            // Holding var for running LED page tests
byte ledTestStartState = false;             // Holding var for running LED page tests
bool ledTestBootLights = false;             // Holding var for running LED page tests

//RGB Bulb
byte bulbIPAddr_1 = 0;
byte bulbIPAddr_2 = 0;
byte bulbIPAddr_3 = 0;
byte bulbIPAddr_4 = 0;
String bulbIPAddress = "0.0.0.0";           // IP address of WiFi RGBW Light bulb (will be concatenated from 4 vars above)
String bulbServerPath = "";                 // Full URL for HTTP bulb control
String bulbName = "";                       // ESPHome bulb name (see documentation)
bool bulbEnabled = false;                   // Whether bulb is enabled (based on IP. False = 0.0.0.0)
bool bulbConnected = false;                 // Whether bulb is connected / disable bulb use by setting to false
bool defaultBulbState = false;              // Whether bulb is on (true) or off (false) after boot
bool activeBulbState = false;               // Current State of Bulb (on/off)
byte defaultBulbColorMode = 0;              // 0=RGB, 1=White Color Temp
byte activeBulbColorMode = 0;               // 0=RGB, 1=White Color Temp
String defaultBulbColor = "#FFA500";        // Orange
String activeBulbColor = "#FFA500";         // Orange
uint16_t defaultBulbTemp = 225;             // Bulb white temperature in mirads: 150 (cool) - 350 (warm)
uint16_t activeBulbTemp = 255;              // Current active bulb temperature 
byte defaultBulbBrightness = 128;           // 50% brightness
byte activeBulbBrightness = 128;            // Active brightness. Some bulbs may fail to light at low values.
bool useBootLightsBulb = false;             // Test/use LED bulb to indicate connectivity (bulb will flash red, green blue during boot)
bool bulbTestFlag = false;                  // Flag to indicate whether testing bulb settings via web app
byte bulbTestStartState = false;            // Holding var for running Integration page tests
bool bulbTestBootLights = false;            // Holding var for running Integration page tests

//----------------------------------
//Local Variables (Touch Sensors)
//----------------------------------
bool touch1Enabled = false;
uint16_t touch1Duration = 250;    //Touch duration (250-2000 in ms) for 'activation' - debounces sensor
byte touch1Control1 = 0;
byte touch1Control2 = 0;
bool touch2Enabled = false;
uint16_t touch2Duration = 250;    //Touch duration (250-2000 in ms) for 'activation' - debounces sensor
byte touch2Control1 = 0;
byte touch2Control2 = 0;
uint32_t touch1LastTouch = 0;
uint32_t touch2LastTouch = 0;
byte touch1State = 0;
byte touch2State = 0;
//----------------------------------
//Local Variables (Display/Clock)
//----------------------------------
byte dispIPAddr_1 = 0;
byte dispIPAddr_2 = 0;
byte dispIPAddr_3 = 0;
byte dispIPAddr_4 = 0;
String dispIPAddress = "0.0.0.0";       // IP address of touch display (will be concatenated from 4 vars above)
bool dispEnabled = false;               // Based on IP address (false if IP = 0.0.0.0)
bool dispConnected = false;             // Whether a successful connection is made to the display (always false if IP address=0.0.0.0.)
byte defaultDisplayBrightness = 128;    // Default brightness at boot (0-255)
bool autoDimDisplay = false;            // Auto-dim display based on ambient light level
byte autoDimBrightnessMax = 0;          // Max bright level when ambient light > LightLevel1  (0-255)
uint16_t autoDimLightLevel1 = 0;        // Ambient Light level 1 for auto-dim (LightLevel1 > LightLevel2 > LightLevel3)
byte autoDimBrightness12 = 0;           // Brightness when ambient light between LightLevel1 and LightLevel2 (0-255)
uint16_t autoDimLightLevel2 = 0;        // Ambient Light level 2 for auto-dim (LightLevel1 > LightLevel2 > LightLevel3)
byte autoDimBrightness23 = 0;           // Brightness when ambient light between LightLevel2 and LightLevel3 (0-255)
uint16_t autoDimLightLevel3 = 0;        // Ambient Light level 3 for auto-dim (LightLevel1 > LightLevel2 > LightLevel3)
byte autoDimBrightnessMin = 0;          // Minimum brightness when ambient light < LightLevel3
byte touch1Control = 0;                 // Function of touch control 1 (0 none, 1 bulb, 2 LEDs, 3 +DispBright, 4-DispBright, 5 Snooze, 6 Stop Alarm)
byte touch2Control = 0;                 // Function of touch control 2 (0 none, 1 bulb, 2 LEDs, 3 +DispBright, 4-DispBright, 5 Snooze, 6 Stop Alarm)

//--------------------------------
//Local Variables (Alarms)
//--------------------------------
bool alarmScheduled = false;            // True if any upcoming alarms scheduled
bool alarmActive = false;               // True if an active alarm is occurring (also needed for touch sensors Func2)

//-------------------------------------
//Local Variables (Weather/Temperature)
//-------------------------------------
byte defaultTempUnits = 13;           // Show temp in Celcius (12) or Fahrenheit (13)
byte weatherSource = 0;               // 0 None, 1 OpenWeatherMap (OWM), 2 MQTT, 3 API
String owmKey = "";                   // Open Weather Map API Key (https://openweathermap.org/api) 
String owmLat = "39.8083";            // Lat/Long defaults point to Lebanon, KS (geographic center of US)
String owmLong = "-98.555";
bool owmConnected = false;            // Boolean indicating if initial connection to OWM is valid (only used if weatherSource is set to OWM)
uint16_t tempUpdatePeriod = 15;       // How often in minutes to request temperature from OWM (minimum 15 min. to limit API calls)


/* =============== Misc Local Variables ========================
 * These are globals used for sketch functionality.  They should 
 * not be changed here unless you have a very specific need and 
 * fully understand what you are changing.  Any config/runtime
 * options can be changed either through the above #DEFINES or
 * thru the web application.
 ===============================================================*/
int milliamps = MILLI_AMPS;        // Limited to 500 - 3,000 milliamps.
uint32_t bootDelay = 0;            // Delay boot to allow other components to finish booting (in ms)
uint32_t bootStartTime = 0;        // Counter for delay
bool isBooting = false;

//---------------------------
// Instantiate objects
//---------------------------
WiFiClient espClient;
WebServer server(80);
PubSubClient client(espClient);

time_t now;
tm timeinfo;
HTTPClient http;

TwoWire bus1 = TwoWire(0);     //I2C Bus 1
TwoWire bus2 = TwoWire(1);     //I2C Bus 2

CRGB LEDs[NUM_LEDS_MAX];       //Defined for 99 maxvoid toggleLEDState(

//===============================================
// Function Prototypes - code follows main loop()
//===============================================
//Config file
void readConfigFile();
void writeConfigFile(bool restart_ESP);
String getCurrentConfig();
//WiFi
void setupSoftAP();
bool setupWifi();
//LED Functions
bool currentLEDState();
void toggleLEDState(bool state, String color=activeLEDColor, byte brightness=activeLEDBrightness);
void setLEDBrightness(byte brightness);
void setLEDColor(String hexColor, byte brightness=activeLEDBrightness);
//Bulb Functions
bool bulbIsConnected();
bool currentBulbState();
void toggleBulbState(bool state, String hexcolor=activeBulbColor, byte brightness=activeBulbBrightness, uint16_t temperature=activeBulbTemp);
void setBulbBrightness(byte newValue);
void setBulbBrightnessNoState(byte newValue);
void setBulbColor(String hexColor, byte brightness=activeBulbBrightness);
void setBulbTemperature(int newTemp, byte brightness=activeBulbBrightness);
//Display Functions
bool displayIsConnected();
bool exportToDisplay();
bool reconnectDisplay(String dispIPAddress);
//MQTT
bool setup_mqtt();
void reconnect();
bool reconnect_soft();
bool mqttIsConnected();
void callback(char* topic, byte* payload, unsigned int length);
void updateMQTT(String whichField);
void handleQueuedMqttUpdates();
void executeMqttUpdate(String whichField);
void handleInitMqttSync();
const char* getIdleFuncStr(int func);
const char* getAlarmFuncStr(int func);
int getIdleFuncInt(char* payload);
int getAlarmFuncInt(char* payload);
bool processCommand(String key, String val);
bool saveDiscoveryConfig();
void loadDiscoveryConfig();
void deleteDiscoveryConfig();
void publishDiscovery(bool add);
String getDiscoveryConfig();
//Weather and Temp
bool owmIsConnected();
//Web Pages & Handlers
void setupWebHandlers();
void handleOnboard();
void handleLiveWebUpdate();
void handleIntegrations();
void handleOTAUpdate();
void webMainPage();
void webMainPageJson();
void webSystemsPage();
void webSystemsPageJson();
void webLightsPage();
void webLightsPageJson();
void handleLEDTest();
void handleBulbTest();
void handleLEDReset();
void handleBulbReset();
void handleLEDSave();
void handleBulbSave();
void webAdvancedPage();
void webAdvancedPageJson();
void handleTouchSave();
void handleTouchPress(byte whichSensor);
void handleDiscoverySave();
void webRestartPage();
void webRestartAllPage();
void webResetPage();
void webFirmwareUpdate();
void webConfigDump();
//API Related Functions
void handleAPI();
String forwardAPIRequest(String targetIP);
bool sendAlarmAction(String state);
bool sendBulbState();
bool sendLEDState();
void restartAllControllers();
void restartBulb();

//Misc functions
bool isValidHex(String hex);
CRGB hexToCRGB(String hexString);
CRGB rgbToCRGB(byte red, byte green, byte blue);
String rgbToHex(byte red, byte green, byte blue);
bool isNumeric(String str);

//===============================================
//  -------->   MAIN SETUP ROUTINE   <----------
//===============================================
void setup() {
  isBooting = true;
  #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
    Serial.begin(115200);
    Serial.println("Starting setup...");
  #endif
  esp_netif_init();
  setupWebHandlers();
  delay(500);  

  readConfigFile();
  if (onboarding) {
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("Entering Onboarding setup...");
    #endif
    setupSoftAP();
  } else if (!setupWifi()) {
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("Wifi connect failed. Reentering onboarding...");
    #endif
    setupSoftAP();
    onboarding = true;
  } else {
    //Connected to Wifi - Any "special" setup steps here that need to be completed
    //before rest of setup is executed. For example, if using the display or LEDs
    //to indicate boot process, initialize those here.

    //Implement any boot delay specified in config
    if (bootDelay > 0) {
      byte safetyEscape = 0;
      bootStartTime = millis();
      while ((millis() - bootStartTime) < bootDelay) {
        delay(500);  
        safetyEscape++;
        if (safetyEscape > 120) {
          //After 120 loops (max of 60 secs at 2/second), exit loop
          break;
        }
      }
    }
  
    //---- Setup hardware GPIO pins ----
    //Touch sensors
    pinMode(TOUCH1_PIN, INPUT);
    pinMode(TOUCH2_PIN, INPUT);

    //--------------
    // Setup LEDs
    //--------------
    if (useLEDs) {
      FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(LEDs, NUM_LEDS_MAX);
      FastLED.setDither(false);
      FastLED.setCorrection(TypicalLEDStrip);
      FastLED.setMaxPowerInVoltsAndMilliamps(5, milliamps);
      if (useBootLightsLED) {
        //Briefly flash LEDs Red, Green and Blue
        FastLED.setBrightness(64);  //Approx 25% brightness
        fill_solid(LEDs, numLEDs, CRGB::Red);
        FastLED.show();
        delay(500);
        fill_solid(LEDs, numLEDs, CRGB::Green);
        FastLED.show();
        delay(500);
        fill_solid(LEDs, numLEDs, CRGB::Blue);
        FastLED.show();
        delay(500);
      }
      fill_solid(LEDs, numLEDs, CRGB::Black);
      FastLED.show();
    }
    //-----------------
    // Setup RGBW Bulb
    // ----------------
    if (bulbEnabled) {
      //Ping bulb to see if it can be reached
      if (bulbIsConnected()) {
        bulbConnected = true;
        //Set active vars to the default vals from config
        if (useBootLightsBulb) {
          byte tempBright = defaultBulbBrightness;
          if (tempBright == 0) tempBright = 128;
          //Briefly flash bulb red, green and blue
          activeBulbColorMode = 0;                     //temporarily force change to RGB mode
          setBulbColor("#ff0000", tempBright);
          delay(500);
          setBulbColor("#00ff00", tempBright);
          delay(500);
          setBulbColor("#0000ff", tempBright);
          delay(500);
          //Flash light with default color/temp/brightness so that defaults are set
          if (defaultBulbColorMode) {
            setBulbTemperature(defaultBulbTemp);
          } else {
            setBulbColor(defaultBulbColor, defaultBulbBrightness);
          }
          delay(500);
          toggleBulbState(0);
        }
      } else {
        bulbConnected = false;
      }
    }

    //---------------
    // Setup Display
    // --------------
    if (dispEnabled) {
      String serverPath = "http://" + dispIPAddress + "/api";
      String requestData = "ping=1";
      http.begin(serverPath);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int response = http.POST(requestData);
      if (response > 0) {
        if (http.getString() == "OK") {
          dispConnected = true;
        } else {
          dispConnected = false;
        }
      } else {
        dispConnected = false;
      }
      http.end();
    }
  }

  if (onboarding) {
    //Skip all other processes

  } else {
    //Setup Arduino OTA Updates
    ArduinoOTA.setHostname(otaHostName.c_str());
    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_FS
        type = "filesystem";
      }
    });
    ArduinoOTA.begin();

    //-------------------------------------------
    // Apply Config file vars to starting states
    //-------------------------------------------
    // LEDs
    if (useLEDs) {
      if (defaultLEDState) {
        //Default LED strip to ON with default settings
        if (defaultLEDEffect == 0) {
          //Solid effect
          FastLED.setBrightness(defaultLEDBrightness);  
          fill_solid(LEDs, numLEDs, hexToCRGB(defaultLEDColor));
          FastLED.show();      
        } else {
          //Apply default effect here
        }
        activeLEDState = true;
      } else {
        activeLEDState = false;
      }
      activeLEDBrightness = defaultLEDBrightness;
      lastLEDBrightness = activeLEDBrightness;
      activeLEDColor = defaultLEDColor;
      activeLEDEffect = defaultLEDEffect;
      activeLEDEffectColor1 = defaultLEDEffectColor1;
      activeLEDEffectColor2 = defaultLEDEffectColor2;
      activeLEDEffectColor3 = defaultLEDEffectColor3;
    }

    //RGB Bulb
    if (bulbConnected) {
      if (defaultBulbState) {
        //Default bulb to 'on'
        if (defaultBulbColorMode == 1) {
          //white temp mode
          setBulbTemperature(defaultBulbTemp);
        } else {
          setBulbColor(defaultBulbColor, defaultBulbBrightness);
        }
      }
      activeBulbState = defaultBulbState;
      activeBulbColorMode = defaultBulbColorMode;
      activeBulbColor = defaultBulbColor;
      activeBulbTemp = defaultBulbTemp;
      activeBulbBrightness = defaultBulbBrightness;
    }
    // MQTT Setup
    if (mqttEnabled) {
      //Attempt to connect to MQTT broker - if fails, disable MQTT
      if (!setup_mqtt()) {
        mqttConnected = false;
      } else {
        initialSyncRequired = true;
        syncStep = 0;
      }
    }

    //Weather/Temperature
    if (weatherSource == 1) {
      if ((owmKey == "NA") || (owmKey == "")) {
        owmConnected = false;
      } else {
        owmConnected = true;
      } 
    }
    //Other hardware and setup

    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
      Serial.println("Setup complete. Entering main loop...");
    #endif
  }
  isBooting = false;
}

//===============================================
//    -------->   MAIN LOOP   <----------
//===============================================
void loop() {
  // When OTA flag set via HTML call, time to upload set at 20 sec. via server callback above.  Alter there if more time desired.
  if ((ota_flag) && (!onboarding)) {
    displayOTA();
    uint32_t ota_time_start = millis();
    while (ota_time_elapsed < ota_time) {
      ArduinoOTA.handle();
      ota_time_elapsed = millis() - ota_time_start;
      delay(10);
    }
    ota_flag = false;
  }
  server.handleClient();

  if (!onboarding) {
    //Rest of loop - only execute once onboarded
    uint32_t currentMillis = millis();
    static bool lastBulbState;
    static bool lastLEDState;
    //Verify MQTT connection if enabled
    if (mqttEnabled && (WIFIMODE > 0 && WiFi.status() == WL_CONNECTED)) {
      mqttConnected = client.connected();
    }

    //Touch Sensors
    if (touch1Enabled) {
      if (digitalRead(TOUCH1_PIN) == HIGH) {
        //Handle Touch1 Touch if detected
        if (!touch1State) {
          touch1State = true;
          touch1LastTouch = currentMillis;
          if (mqttConnected) updateMQTT("touch1state");
          handleTouchPress(1);
        }
      } else {
        if (touch1State && (currentMillis - touch1LastTouch > touch1Duration)) {
          touch1State = false;
          if (mqttConnected) updateMQTT("touch1state");
        }
      }
    }
    if (touch2Enabled) {
      if (digitalRead(TOUCH2_PIN) == HIGH) {
        if (!touch2State) {
          touch2State = true;
          touch2LastTouch = currentMillis;
          if (mqttConnected) updateMQTT("touch2state");
          handleTouchPress(2);
        }
      } else {
        if (touch2State && (currentMillis - touch2LastTouch > touch2Duration)) {
          touch2State = false;
          if (mqttConnected) updateMQTT("touch2state");
        }
      }
    }

    // If MQTT enabled, check connection (Non-Blocking) and reestablish if needed.
    if (mqttEnabled && (WIFIMODE > 0 && WiFi.status() == WL_CONNECTED)) {
      if (!mqttConnected) {
        // Calls non-blocking reconnect_soft
        reconnect_soft(); 
      } else {
        client.loop(); 
      }
    }
    //Other loop functions/processes
    //Update display with current light states if changed
    if (dispConnected) {
      if (lastBulbState != activeBulbState) {
        bool x = sendBulbState();
        lastBulbState = activeBulbState;
      }
      if (lastLEDState != activeLEDState) {
        bool x = sendLEDState();
        lastLEDState = activeLEDState;
      }
    }
    //Handle any required MQTT state publishes (thread-safe)
    if ((mqttConnected) && (!isBooting)) {
      unsigned long curMillis = millis();
      if (curMillis - lastSyncStepTime > INIT_SYNC_INTERVAL) {
        if (initialSyncRequired) {
          handleInitMqttSync(); 
        }
        handleQueuedMqttUpdates(); 
        lastSyncStepTime = curMillis;
      }
    }
    if (rebootRequired) {
      delay(1000);
      writeConfigFile(true);
    }
  }  
}

//========================
// CONFIG FILE FUNCTIONS
//========================
//---------------------------------------
// Read config file from flash (LittleFS)
//---------------------------------------
void readConfigFile() {

  if (LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("mounted file system");
    #endif
    if (LittleFS.exists("/config.json")) {
      //file exists, reading and loading
      #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
            Serial.println("reading config file");
      #endif
      File configFile = LittleFS.open("/config.json", "r");
      if (configFile) {
        #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
                Serial.println("opened config file");
        #endif
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        JsonDocument json;
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if (!deserializeError) {
          #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
            Serial.println("\nparsed json");
          #endif
          // Read values here from LittleFS (use defaults for all values in case they don't exist to avoid potential boot loop)
          //DON'T NEED TO STORE OR RECALL WIFI INFO - Written to flash automatically by library when successful connection.
          deviceName = json["device_name"] | "BedsideLamp01";
          //LED Strip
          bootDelay = json["boot_delay"] | 0;                  
          numLEDs = json["led_count"] | NUM_LEDS;
          defaultLEDState = ((json["led_state"] == 1)) | false;
          defaultLEDBrightness = json["led_brightness"] | 64;
          defaultLEDColor = json["led_color"] | "#FFA500";                //default to orange
          defaultLEDEffect = json["led_effect"] | 0;                      //default to solid (0)
          defaultLEDEffectColor1 = json["led_effect_color1"] | "#FF0000"; //default to red
          defaultLEDEffectColor2 = json["led_effect_color2"] | "#00FF00"; //default to green
          defaultLEDEffectColor3 = json["led_effect_color3"] | "#0000FF"; //default to blue
          useBootLightsLED = ((json["use_boot_lights_led"])) | 1;
          //Light Bulb
          bulbIPAddr_1 = json["bulb_addr_1"] | 0;
          bulbIPAddr_2 = json["bulb_addr_2"] | 0;
          bulbIPAddr_3 = json["bulb_addr_3"] | 0;
          bulbIPAddr_4 = json["bulb_addr_4"] | 0;
          bulbName = json["bulb_name"] | "";
          defaultBulbState = ((json["bulb_state"] == 1)) | false;
          defaultBulbColorMode = json["bulb_color_mode"] | 0;     //Default to RGB mode
          defaultBulbColor = json["bulb_color"] | "#FFA500";      //Default to Orange
          defaultBulbTemp = json["bulb_temp"] | 225;              //Default to midrange
          defaultBulbBrightness = json["bulb_brightness"] | 128;  //Default to 50%
          useBootLightsBulb = ((json["use_boot_lights_bulb"] == 1)) | true;
          //Touch Sensors
          touch1Enabled = ((json["touch1_enabled"] == 1)) | false;
          touch1Duration = json["touch1_duration"] | 250;
          touch1Control1 = json["touch1_control_1"] | 0;
          touch1Control2 = json["touch1_control_2"] | 0;
          touch2Enabled = ((json["touch2_enabled"] == 1)) | false;
          touch2Duration = json["touch2_duration"] | 250;
          touch2Control1 = json["touch2_control_1"] | 0;
          touch2Control2 = json["touch2_control_2"] | 0;
          //Touch Display
          dispIPAddr_1 = json["disp_addr_1"] | 0;
          dispIPAddr_2 = json["disp_addr_2"] | 0;
          dispIPAddr_3 = json["disp_addr_3"] | 0;
          dispIPAddr_4 = json["disp_addr_4"] | 0;
          defaultDisplayBrightness = json["disp_brightness"] | 128; //Default to 50%
          autoDimDisplay = ((json["disp_auto_dim"] == 1)) | 0;      //Default to false
          autoDimBrightnessMax = json["disp_dim_bright_max"] | 0;
          autoDimLightLevel1 = json["disp_dim_light_1"] | 0;
          autoDimBrightness12 = json["disp_dim_bright_12"] | 0;
          autoDimLightLevel2 = json["disp_dim_light_2"] | 0;
          autoDimBrightness23 = json["disp_dim_bright_23"] | 0;
          autoDimLightLevel3 = json["disp_dim_light_3"] | 0;
          autoDimBrightnessMin = json["disp_dim_bright_min"] | 0;
          //MQTT
          mqttAddr_1 = json["mqtt_addr_1"] | 0;
          mqttAddr_2 = json["mqtt_addr_2"] | 0;
          mqttAddr_3 = json["mqtt_addr_3"] | 0;
          mqttAddr_4 = json["mqtt_addr_4"] | 0;
          mqttPort = json["mqtt_port"] | 1883;
          mqttTelePeriod = json["mqtt_tele_period"] | 60;      //in seconds
          mqttUser = json["mqtt_user"] | "mqttuser";
          mqttPW = json["mqtt_pw"] | "mqttpw";
          mqttTopicSub = json["mqtt_topic_sub"] | "bedlamp";
          mqttTopicPub = json["mqtt_topic_pub"] | "bedlamp";
          //Weather/Temp
          defaultTempUnits = json["temp_units"] | 13;         //12=°C, 13=°F
          weatherSource = json["weather_source"] | 0;         //0 none (disabled), 1 OWM, 2 MQTT, 3 API
          owmKey = json["owm_key"] | "N/A";
          owmLat = json["own_lat"] | "39.8083";               //Default Lat/Long is Lebanon, KS (center of US)
          owmLong = json["owm_long"] | "-98.555";             //Stored as strings, since that's what the API call needs
          tempUpdatePeriod = json["temp_update_period"] | 15;

          //-------------------------------------------
          // Set initial testing values to default vals
          //-------------------------------------------
          ledTestFlag = false;
          ledTestNumLeds = numLEDs;
          ledTestStartState = defaultLEDState;
          ledTestBootLights = useBootLightsLED;
          ledTestColor = defaultLEDColor;
          ledTestBrightness = defaultLEDBrightness;

          //------------------------------------------------
          //Set or update local vars based on other settings
          //------------------------------------------------
          //Disable LEDs if number not > 0
          if (numLEDs == 0) {
            useLEDs = false;
          } else {
            useLEDs = true;
          }
          //Enable/Disable Bulb - use bulbIPAddress and enabled flag if non-zero
          if ((bulbIPAddr_1 == 0) && (bulbIPAddr_2 == 0) && (bulbIPAddr_3 == 0) && (bulbIPAddr_4 == 0)) {
            bulbIPAddress = "0.0.0.0";
            bulbEnabled = false;  
            bulbServerPath = "";
          } else {
            bulbIPAddress = String(bulbIPAddr_1) + "." + String(bulbIPAddr_2) + "." + String(bulbIPAddr_3) + "." + String(bulbIPAddr_4);
            //Bulb must have name to be enabled/reachable (ESPHome node name)
            if (bulbName.length() == 0 ) {
              bulbEnabled = false;
            } else {
              bulbEnabled = true;
              bulbServerPath = "http://" + bulbIPAddress + "/light/" + bulbName;
            }
          }
          //Enable/Disable Touch Display
          if ((dispIPAddr_1 == 0) && (dispIPAddr_2 == 0) && (dispIPAddr_3 == 0) && (dispIPAddr_4 == 0)) {
            dispIPAddress = "0.0.0.0";
            dispEnabled = false;
          } else {
            dispIPAddress = String(dispIPAddr_1) + "." + String(dispIPAddr_2) + "." + String(dispIPAddr_3) + "." + String(dispIPAddr_4);
            dispEnabled = true;
          }
          //Disable or enable MQTT
          if ((mqttAddr_1 == 0) && (mqttAddr_2 == 0) && (mqttAddr_3 == 0) && (mqttAddr_4 == 0)) {
            mqttIPAddress = "0.0.0.0";
            mqttEnabled = false;
          } else {
            mqttIPAddress = String(mqttAddr_1) + "." + String(mqttAddr_2) + "." + String(mqttAddr_3) + "." + String(mqttAddr_4);
            mqttEnabled = true;
          }
        } else {
         #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
            Serial.println("failed to load json config");
         #endif
          onboarding = true;
        }
        configFile.close();
      } else {
        onboarding = true;
      }
    } else {
      // No config file found - set to onboarding
      onboarding = true;
    }

    LittleFS.end();  //End - needed to prevent issue with OTA updates
  } else {
    //could not mount filesystem
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("failed to mount FS");
        Serial.println("LittleFS Formatted. Restarting ESP.");
    #endif
    onboarding = true;
  }
}

String getCurrentConfig() {
  //This function just reads and returns the raw contents of the config file.
  //No app vars are changed.  Used for web app config file dump page (instead of global string).
  String fileContents = "";
  if (LittleFS.begin(false)) {  //do not format on failure
    if (LittleFS.exists("/config.json")) {
      File configFile = LittleFS.open("/config.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        configFile.close();
        JsonDocument json;
        auto deserializeError = deserializeJson(json, buf.get());
        if (!deserializeError) {        
          serializeJsonPretty(json, fileContents);
        } else {
          fileContents = "ERROR RETURNED: Could not deserialize config file (does not appear to be JSON).";
        }
      } else {
        fileContents = "ERROR RETURNED: 'config.json' could not be opened - See Troubleshooting.";
      }
    } else {
      fileContents = "ERROR RETURNED: 'config.json' file does not exist.  Be sure you have saved the config file at least one time.";
    }
    LittleFS.end();
  } else {
    //LittleFS.begin failed
    fileContents = "ERROR RETURNED: Failed to mount LittleFS - See Troubleshooting.";
  }
  return fileContents;
}

String getDiscoveryConfig() {
  //This function just reads and returns the raw contents of the config file.
  //No app vars are changed.  Used for web app config file dump page (instead of global string).
  String fileContents = "";
  if (LittleFS.begin(false)) {  //do not format on failure
    if (LittleFS.exists("/discovery.json")) {
      File configFile = LittleFS.open("/discovery.json", "r");
      if (configFile) {
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        configFile.close();  
        JsonDocument json;
        auto deserializeError = deserializeJson(json, buf.get());
        if (!deserializeError) {        
          serializeJsonPretty(json, fileContents);
        } else {
          fileContents = "ERROR RETURNED: Could not deserialize discovery file (does not appear to be JSON).";
        }
      } else {
        fileContents = "ERROR RETURNED: 'discovery.json' could not be opened - See Troubleshooting.";
      }
    } else {
      fileContents = "'discovery.json' file does not exist.  This will be the case if Discovery has not been enabled.";
    }
    LittleFS.end();  
  } else {
    //LittleFS.begin failed
    fileContents = "ERROR RETURNED: Failed to mount LittleFS - See Troubleshooting.";
  }
  return fileContents;
}
//--------------------------------------
// Write config file to flash (LittleFS)
//---------------------------------------
void writeConfigFile(bool restart_ESP) {
  //Write settings to LittleFS (reboot to save)

  if (LittleFS.begin()) {
    JsonDocument doc;
    doc.clear();
    //Add any values to save to JSON document
    //General Globals
    doc["boot_delay"] = bootDelay;
    doc["device_name"] = deviceName;

    //LEDs
    doc["led_count"] = numLEDs;
    doc["led_state"] = ((defaultLEDState) ? 1 : 0);
    doc["led_brightness"] = defaultLEDBrightness;
    doc["led_color"] = defaultLEDColor;
    doc["led_effect"] = defaultLEDEffect;
    doc["led_effect_color1"] = defaultLEDEffectColor1;
    doc["led_effect_color2"] = defaultLEDEffectColor2;
    doc["led_effect_color3"] = defaultLEDEffectColor3;
    doc["use_boot_lights_led"] = ((useBootLightsLED) ? 1 : 0);
    //Bulb
    doc["bulb_addr_1"] = bulbIPAddr_1;
    doc["bulb_addr_2"] = bulbIPAddr_2;
    doc["bulb_addr_3"] = bulbIPAddr_3;
    doc["bulb_addr_4"] = bulbIPAddr_4;
    doc["bulb_name"] = bulbName;
    doc["bulb_state"] = ((defaultBulbState) ? 1 : 0);
    doc["bulb_color_mode"] = defaultBulbColorMode;
    doc["bulb_color"] = defaultBulbColor;
    doc["bulb_temp"] = defaultBulbTemp;
    doc["bulb_brightness"] = defaultBulbBrightness;
    doc["use_boot_lights_bulb"] = ((useBootLightsBulb) ? 1 : 0);
    //Touch Sensors
    doc["touch1_enabled"] = ((touch1Enabled) ? 1 : 0);
    doc["touch1_duration"] = touch1Duration;
    doc["touch1_control_1"] = touch1Control1;
    doc["touch1_control_2"] = touch1Control2;
    doc["touch2_enabled"] = ((touch2Enabled) ? 1 : 0);
    doc["touch2_duration"] = touch2Duration;
    doc["touch2_control_1"] = touch2Control1;
    doc["touch2_control_2"] = touch2Control2;
    //Touch Display
    doc["disp_addr_1"] = dispIPAddr_1;
    doc["disp_addr_2"] = dispIPAddr_2;
    doc["disp_addr_3"] = dispIPAddr_3;
    doc["disp_addr_4"] = dispIPAddr_4;
    //doc["time_style"] = defaultTimeStyle;               //byte, e.g. 0 = Arial, 1 = Roman, etc.
    doc["disp_brightness"] = defaultDisplayBrightness;  //Display brightness at boot
    doc["disp_auto_dim"] = ((autoDimDisplay) ? 1 : 0);  //Auto adjust brightness based on ambient light level
    doc["disp_dim_bright_max"] = autoDimBrightnessMax;  //Auto Dim brightness when ambient light > LightLevel1
    doc["disp_dim_light_1"] = autoDimLightLevel1;       //ambient light level 1  (0-65535)
    doc["disp_dim_bright_12"] = autoDimBrightness12;    //brightness when light between LightLevel1 and LightLevel2
    doc["disp_dim_light_2"] = autoDimLightLevel2;       //ambient light level 2
    doc["disp_dim_bright_23"] = autoDimBrightness23;    //brightness when light between LightLevel2 and LightLevel3
    doc["disp_dim_light_3"] = autoDimLightLevel3;       //ambient light level 3
    doc["disp_dim_bright_min"] = autoDimBrightnessMin;  //brightness when light level < LightLevel3
    //MQTT
    doc["mqtt_addr_1"] = mqttAddr_1;
    doc["mqtt_addr_2"] = mqttAddr_2;
    doc["mqtt_addr_3"] = mqttAddr_3;
    doc["mqtt_addr_4"] = mqttAddr_4;
    doc["mqtt_port"] = mqttPort;
    doc["mqtt_tele_period"] = mqttTelePeriod;
    doc["mqtt_user"] = mqttUser;
    doc["mqtt_pw"] = mqttPW;
    doc["mqtt_topic_sub"] = mqttTopicSub;
    doc["mqtt_topic_pub"] = mqttTopicPub;
    //Weather/Temp
    doc["temp_units"] = defaultTempUnits;   //12=°C, 13=°F
    doc["weather_source"] = weatherSource;  //0 none (disabled), 1 OWM, 2 MQTT, 3 API
    doc["owm_key"] = owmKey;
    doc["own_lat"] = owmLat;
    doc["owm_long"] = owmLong;
    doc["temp_update_period"] = tempUpdatePeriod;

    //Additional fields here

    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile) {
      #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("failed to open config file for writing");
      #endif
      configFile.close();
      LittleFS.end();
      return;
    } else {
      serializeJson(doc, configFile);
      #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("Settings saved.");
      #endif
      configFile.close();
      if (exportToDisplay()) {
        //
      } else {
        //handle error?
      }
      if (restart_ESP) {
        //Turn off any lights prior to reboot
        if (useLEDs) {
          toggleLEDState(0);
        }
        if (bulbEnabled) {
          toggleBulbState(0);
        }
        ESP.restart();
      }
    }
    LittleFS.end();
  } else {
    //could not mount filesystem
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
      Serial.println("failed to mount FS");
    #endif
  }
}

/* =====================================
    WIFI SETUP & Functions
   =====================================
*/
void setupSoftAP() {
  //for onboarding
  WiFi.mode(WIFI_AP);
  WiFi.softAP(deviceName + "_AP");
  IPAddress Ip(192, 168, 4, 1);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
#if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
  Serial.println("SoftAP Created");
  Serial.println("Web server starting...");
#endif
  server.begin();
}

bool setupWifi() {
  byte count = 0;
  //attempt connection
  //if successful, return true else false
  delay(200);
  WiFi.hostname(wifiHostName);
  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {
#if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
    Serial.print(".");
#endif
    // Stop if cannot connect
    if (count >= 60) {
// Could not connect to local WiFi
#if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
      Serial.println();
      Serial.println("Could not connect to WiFi.");
#endif
      return false;
    }
    delay(500);
    yield();
    count++;
  }
  //Successfully connected
  wifiSSID = WiFi.SSID();
  baseIPAddress = WiFi.localIP().toString();
  WiFi.macAddress(macAddr);
  strMacAddr = WiFi.macAddress();
#if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
  Serial.println("Connected to wifi... yay!");
  Serial.print("MAC Address: ");
  Serial.println(strMacAddr);
  Serial.print("IP Address: ");
  Serial.println(baseIPAddress);
  Serial.println("Starting main setup...");
#endif
  server.begin();
  return true;
}

// ===================================
//  SETUP MQTT AND CALLBACKS
// ===================================
bool setup_mqtt() {
  byte mcount = 0;
  espClient.setTimeout(500);
  IPAddress myserver = IPAddress(mqttAddr_1, mqttAddr_2, mqttAddr_3, mqttAddr_4);
  String statusTopic = "stat/" + mqttTopicPub + "/primary/status";
  client.setServer(myserver, mqttPort);
  client.setBufferSize(1024); 
  client.setCallback(callback);
  #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
    Serial.print("Connecting to MQTT broker.");
  #endif
  while (!client.connected( )) {
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
      Serial.print(".");
    #endif
    client.connect(mqttClient.c_str(), mqttUser.c_str(), mqttPW.c_str(), statusTopic.c_str(), 1, true, "offline");

    if (mcount >= 30) {
      #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println();
        Serial.println("Could not connect to MQTT broker. MQTT disabled.");
      #endif
      // Could not connect to MQTT broker
      return false;
    }
    delay(500);
    yield();
    mcount++;
  }
  #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
    Serial.println();
    Serial.println("Successfully connected to MQTT broker.");
  #endif
  //Publish LWT, current IP and MAC addresses
  String curVer = VERSION;
  client.publish((statusTopic).c_str(), "online", true);  //LWT
  client.publish(("stat/" + mqttTopicPub + "/bulb/status").c_str(), bulbConnected ? "online" : "offline", true); //Special bulb LWT
  client.publish(("stat/" + mqttTopicPub + "/primary/ipaddr").c_str(), baseIPAddress.c_str(), true);
  client.publish(("stat/" + mqttTopicPub + "/primary/macaddr").c_str(), strMacAddr.c_str(), true);
  client.publish(("stat/" + mqttTopicPub + "/primary/version").c_str(), curVer.c_str(), true);
  client.publish(("stat/" + mqttTopicPub + "/bulb/ipaddr").c_str(), bulbIPAddress.c_str(), true);

  client.subscribe(("cmnd/" + mqttTopicSub + "/#").c_str());
  mqttConnected = true;
  return true;
}

void reconnect() {
  //Attempt to reconnect to MQTT broker. 
  if (!client.connected()) {
    String statusTopic = "stat/" + mqttTopicPub + "/primary/status";
    // Attempt one connection
    if (client.connect(mqttClient.c_str(), mqttUser.c_str(), mqttPW.c_str(), statusTopic.c_str(), 1, true, "offline")) {
      client.publish((statusTopic).c_str(), "online", true);  //LWT
      client.publish(("stat/" + mqttTopicPub + "/bulb/status").c_str(), bulbConnected ? "online" : "offline", true);  //Bulb special LWT
      client.subscribe(("cmnd/" + mqttTopicSub + "/#").c_str());
      mqttConnected = true;
    } else {
      // Set the flag and let the main loop continue.
      mqttConnected = false;
    }
  }
}

bool reconnect_soft() {
  //Attempt quick MQTT reconnect.  If fails, return false instead of forcing ESP Reboot.
  //This should be used instead of full "reconnect" function to just check or re-establish 
  //a connection to a good, known broker.
  if (!client.connected()) {
    unsigned long now = millis();
    String statusTopic = "stat/" + mqttTopicPub + "/primary/status";
    // Only try to connect every 5 seconds to avoid blocking the loop
    if (now - lastMqttRetryAttempt > MQTT_RETRY_DELAY) {
      lastMqttRetryAttempt = now;
      // Try to connect once
      if (client.connect(mqttClient.c_str(), mqttUser.c_str(), mqttPW.c_str(), statusTopic.c_str(), 1, true, "offline")) {
        client.publish((statusTopic).c_str(), "online", true);  //LWT
        client.publish(("stat/" + mqttTopicPub + "/bulb/status").c_str(), bulbConnected ? "online" : "offline", true);  //Bulb special LWT
        client.subscribe(("cmnd/" + mqttTopicSub + "/#").c_str());
        mqttConnected = true;
        return true;
      } else {
        // Failed this attempt, but we return false so the loop() continues
        mqttConnected = false;
        return false;
      }
    }
    return false; // Not time to retry yet
  }
  
  mqttConnected = true;
  return true;  
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (isBooting) return;
  if (!mqttIsConnected()) return;
  char* subTopic = strrchr(topic, '/');
  if (subTopic == NULL) return;
  subTopic++;

  char buf[length + 1];
  memcpy(buf, payload, length);
  buf[length] = '\0';
  String val = String(buf);

  //Reboot Bulb or All (display controller handled via its MQTT callback)
  if (strcmp(subTopic, "bulbrestart") == 0) {
    //Restart bulb first
    restartBulb();
    return;
  }
  //Reboot Primary (this) Controller.  
  if (strcmp(subTopic, "primaryrestart") == 0) {
    ESP.restart();
    return;  
  }   

  //Reboot all devices (bulb, display and this primary controller)
  if (strcmp(subTopic, "restartall") == 0) {
    restartAllControllers();
    return;
  }
  //Force a state refresh (publish) of all MQTT topics
  if ((strcmp(subTopic, "primaryrefresh") == 0) || (strcmp(subTopic, "refreshall") == 0)) {
    //payload ignored. Any payload can be passed
    syncStep = 0;               // Reset the counter to the first case
    initialSyncRequired = true; // Tell the loop to start the sequence
    lastSyncStepTime = millis(); // Initialize the timer    
    return;
  }
  //Save current settings as new boot defaults and reboot
  if (strcmp(subTopic, "primsaveconfig") == 0) {
    delay(500); 
    writeConfigFile(true); // This saves and triggers ESP.restart()
    return;
  } 
  processCommand(String(subTopic), val);
}

//------------------------------------------
// Process Commands - shared by MQTT and API
//------------------------------------------
bool processCommand(String key, String val) {
  bool recognized = true;
  key.trim();
  val.trim();
  // --- LED & BULB COLOR (Unified Logic) ---
  if ((key == "ledcolor") || (key == "bulbcolor")) {
    String colorVal = val;
    bool valid = false;

    // Check for RGB (comma) format
    int firstComma = colorVal.indexOf(',');
    int secondComma = colorVal.indexOf(',', firstComma + 1);
    
    if (firstComma != -1 && secondComma != -1) {
      // Use constrain to prevent byte overflow (e.g., if 300 is passed)
      byte r = constrain(colorVal.substring(0, firstComma).toInt(), 0, 255);
      byte g = constrain(colorVal.substring(firstComma + 1, secondComma).toInt(), 0, 255);
      byte b = constrain(colorVal.substring(secondComma + 1).toInt(), 0, 255);
      colorVal = rgbToHex(r, g, b); 
      valid = true;
    //Check for Hex format (#ffffff or ffffff)
    } else if (isValidHex(colorVal)) {
      if (!colorVal.startsWith("#")) colorVal = "#" + colorVal;
      colorVal.toLowerCase();
      valid = true;
    }

    if (valid) {
      if (key == "ledcolor") setLEDColor(colorVal);
      else {
        setBulbColor(colorVal, activeBulbBrightness);
      }
    } else {
      recognized = false;
    }
  // --- NUMERIC SETTINGS (Standardized constrain) ---
  } else if (key == "ledbrightness") {
    byte newBright = constrain(val.toInt(), 0, 255);
    setLEDBrightness(newBright);
  } else if (key == "bulbbrightness") {
    byte newBright = constrain(val.toInt(), 0, 255);
    setBulbBrightness(newBright);
  } else if (key == "bulbtemp") {
    uint16_t newTemp = constrain(val.toInt(), 150, 350);
    setBulbTemperature(newTemp, ((activeBulbBrightness == 0) ? 96:activeBulbBrightness));
  // --- STATE COMMANDS ---
  } else if (key == "ledstate") {
    toggleLEDState(((val.equalsIgnoreCase("on")) || (val.toInt() == 1)) ? 1 : 0);
  } else if (key == "bulbstate") {
    toggleBulbState(((val.equalsIgnoreCase("on")) || (val.toInt() == 1)) ? 1 : 0);
  // --- TOUCH MAPPINGS ---
  } else if (key == "touch1func") {
    touch1Control1 = constrain(val.toInt(), 0, 8);
    if ((!isBooting) & (mqttConnected)) updateMQTT("touch1func");
  } else if (key == "touch1funca") {
    touch1Control2 = constrain(val.toInt(), 0, 2);
    if ((!isBooting) & (mqttConnected)) updateMQTT("touch1funca");
  } else if (key == "touch2func") {
    touch2Control1 = constrain(val.toInt(), 0, 8);
    if ((!isBooting) & (mqttConnected)) updateMQTT("touch2func");
  } else if (key == "touch2funca") {
    touch2Control2 = constrain(val.toInt(), 0, 2);
    if ((!isBooting) & (mqttConnected)) updateMQTT("touch2funca");
  // ---- SYSTEM ----
  } else if (key == "primsaveconfig") {
    //Will force a write of the config file and a reboot (in main loop() )
    rebootRequired = true;
  } else {
    recognized = false;
  }

  return recognized;
}
void restartBulb() {
  WiFiClient localClient;
  HTTPClient localHttp;
  //Bulb
  if (bulbEnabled) {
    String serverPath = "http://" + bulbIPAddress + "/reboot";
    localHttp.begin(localClient, serverPath);
    int response = localHttp.POST("");
    localHttp.end();
  }  
}
void restartAllControllers() {
  bool retValue = true;
  WiFiClient localClient;
  HTTPClient localHttp;
  //Bulb
  if (bulbEnabled) {
    String serverPath = "http://" + bulbIPAddress + "/reboot";
    localHttp.begin(localClient, serverPath);
    int response = localHttp.POST("");
    if ((response < 200) || (response >= 300)) {
      //non-2xx codes means command failed or did not send back "OK" result
      retValue = false;
    }
    localHttp.end();
  }
  delay(500);
  if ((dispEnabled) && (retValue)) {  //don't proceed if prior reboot command failed
    String serverPath = "http://" + dispIPAddress + "/api?displayrestart=1";
    localHttp.begin(localClient, serverPath);
    int response = localHttp.GET();
    if ((response < 200) || (response >= 300)) {
      retValue = false;
      localHttp.end();
    }
  }
  delay(500);
  //Finally restart this controller if all others returned OK
  if (retValue) {
    ESP.restart();  
  }
}

//--------------------------------
// Home Assistant MQTT Discovery
//--------------------------------
void loadDiscoveryConfig() {
  if (LittleFS.begin(false)) {
    if (LittleFS.exists("/discovery.json")) {
      File file = LittleFS.open("/discovery.json", "r");
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, file);
      if (!error) {
        currentDiscovery.deviceName = doc["devname"].as<String>();
        currentDiscovery.lightBulb = doc["bulb"];
        currentDiscovery.ledStrip = doc["led"];
        currentDiscovery.display = doc["disp"];
        currentDiscovery.touch = doc["touch"];
        currentDiscovery.alarms = doc["alarms"];
        currentDiscovery.diagnostics = doc["diag"];
        currentDiscovery.exists = true;
      }
      file.close();
      LittleFS.end();
    } else {
      // Defaults if file doesn't exist
      currentDiscovery.exists = false;
      currentDiscovery.deviceName = deviceName; // Use global variable
      currentDiscovery.lightBulb = true;
      currentDiscovery.ledStrip = true;
      currentDiscovery.display = true;
      currentDiscovery.touch = true;
      currentDiscovery.alarms = true;
      currentDiscovery.diagnostics = true;
    }
  }


}

bool saveDiscoveryConfig() {
  if (!LittleFS.begin(false)) return false;
  JsonDocument doc;
  doc["devname"] = currentDiscovery.deviceName;
  doc["bulb"] = currentDiscovery.lightBulb;
  doc["led"] = currentDiscovery.ledStrip;
  doc["disp"] = currentDiscovery.display;
  doc["touch"] = currentDiscovery.touch;
  doc["alarms"] = currentDiscovery.alarms;
  doc["diag"] = currentDiscovery.diagnostics;

  File file = LittleFS.open("/discovery.json", "w");
  if (!file) return false;
  if (serializeJson(doc, file) == 0) {
    file.close();
    LittleFS.end();
    return false;
  }
  currentDiscovery.exists = true;
  file.close();
  LittleFS.end();
  return true;
}

void deleteDiscoveryConfig() {
  if (LittleFS.begin(false)) {
    if (LittleFS.exists("/discovery.json")) {
      LittleFS.remove("/discovery.json");
    }
    LittleFS.end();
  }  
  currentDiscovery.exists = false;
}

void publishDiscovery(bool enable) {
  // This function handles the publishing of the discovery messages to Home Assistant
  if (!mqttConnected) return;

  // Initialize filesystem
  if (!LittleFS.begin(false)) return;

  JsonDocument cfg; 
  File file = LittleFS.open("/discovery.json", "r");
  if (file) {
    deserializeJson(cfg, file);
    file.close();
  }
  LittleFS.end();

  // Use "devname" from the selection file; fallback to global deviceName variable
  String discoveryName = cfg["devname"] | deviceName;
  if (discoveryName.length() == 0) discoveryName = deviceName;

  String safeID = discoveryName;
  safeID.replace(" ", "_");
  safeID.toLowerCase();

  struct DiscoveryEntity {
    const char* group;      
    const char* component;  
    const char* suffix;     
    const char* name;  
    const char* category;     
  };

const DiscoveryEntity list[] = {
    {"bulb", "light", "bulbstate", "Light Bulb", ""}, 
    {"led", "light", "ledstate", "LED Strip", ""},    
    {"disp", "number", "dispbrightness", "Display Brightness", ""},
    {"disp", "switch", "autodim", "Auto-Dimming", "config"},
    {"disp", "text", "clockcolor", "Clock Color", "config"},
    {"touch", "binary_sensor", "touch1state", "Touch 1 State", ""},
    {"touch", "select", "touch1func", "Touch 1 Main Function", "config"}, 
    {"touch", "select", "touch1funca", "Touch 1 Alarm Function", "config"}, 
    {"touch", "binary_sensor", "touch2state", "Touch 2 State", ""},
    {"touch", "select", "touch2func", "Touch 2 Main Function", "config"}, 
    {"touch", "select", "touch2funca", "Touch 2 Alarm Function", "config"}, 
    {"alarms", "number", "alarmvolume", "Alarm Volume", "config"},
    {"alarms", "switch", "gentlewake", "Gentle Wake", "config"},
    {"alarms", "number", "alarmtrack", "Alarm Track", "config"},
    {"alarms", "number", "snoozetime", "Snooze Duration", "config"},
    {"alarms", "button", "alarm_stop", "Stop Alarm", ""},
    {"alarms", "button", "alarm_snooze", "Snooze Alarm", ""},
    {"alarms", "button", "playalarm", "Play Alarm Sound", ""},
    {"diag", "sensor", "ip_primary", "IP Addr Primary", "diagnostic"},
    {"diag", "sensor", "ip_display", "IP Addr Display", "diagnostic"},
    {"diag", "sensor", "ip_bulb", "IP Addr Light Bulb", "diagnostic"},
    {"diag", "sensor", "mac_primary", "MAC Addr Primary", "diagnostic"},
    {"diag", "sensor", "mac_display", "MAC Addr Display", "diagnostic"},
    {"diag", "sensor", "fw_primary", "Firmware Ver Primary", "diagnostic"},
    {"diag", "sensor", "fw_display", "Firmware Ver Display", "diagnostic"},
    {"diag", "button", "rb_primary", "Restart Primary Controller", "diagnostic"},
    {"diag", "button", "rb_display", "Restart Display Controller", "diagnostic"}
  };

  String cleanMac = strMacAddr;
  cleanMac.replace(":", "");

  for (const auto& ent : list) {
    String comp = String(ent.component);
    String sfx = String(ent.suffix);
    String grp = String(ent.group);
    String cat = String(ent.category);

    // uniqueId format: [suffix]_[MAC]
    String uniqueId = sfx + "_" + cleanMac;
    String discoveryTopic = "homeassistant/" + comp + "/" + uniqueId + "/config";
    
    bool groupActive = cfg[ent.group].as<bool>();

    if (!enable || !groupActive) {
      // Sends empty string to remove deselected entities from HA registry
      client.publish(discoveryTopic.c_str(), "", true);
      continue; 
    }

    // --- Generate Discovery Payload ---
    JsonDocument doc; 
    doc["uniq_id"] = uniqueId;
    doc["name"] = ent.name;
    doc["def_ent_id"] = comp + "." + safeID + "_" + sfx; 

    // ent_cat mapping for UI categorization
    if (cat.length() > 0) {
      doc["ent_cat"] = cat;
    }

    // Availability and LWT logic
    if (grp == "bulb" || sfx.indexOf("bulb") != -1) {
      JsonArray avail = doc["avty"].to<JsonArray>();
      JsonObject pStatus = avail.add<JsonObject>();
      pStatus["t"] = "stat/" + mqttTopicPub + "/primary/status";
      JsonObject bStatus = avail.add<JsonObject>();
      bStatus["t"] = "stat/" + mqttTopicPub + "/bulb/status";
      doc["avty_mode"] = "all"; 
    } 
    else if (grp == "disp" || sfx.indexOf("display") != -1) {
      doc["avty_t"] = "stat/" + mqttTopicPub + "/display/status";
    } 
    else {
      doc["avty_t"] = "stat/" + mqttTopicPub + "/primary/status";
    }
    
    doc["pl_avail"] = "online";
    doc["pl_not_avail"] = "offline";

    // Device Information (Grouping entities under one device via MAC)
    JsonObject dev = doc["dev"].to<JsonObject>();
    dev["ids"][0] = cleanMac; 
    dev["name"] = discoveryName;
    dev["mdl"] = "Ultimate Bedside Lamp"; 
    dev["mf"] = "Resinchem Tech";
    dev["cu"] = "http://" + baseIPAddress;
    dev["hw"] = strMacAddr;

    // Common Origin metadata
    JsonObject origin = doc["origin"].to<JsonObject>();
    origin["name"] = "Ultimate Bedside Lamp";
    origin["url"] = "https://Resinchem.github.io/Ultimate-Beside-Lamp/";

    // --- Component Specific Logic (MQTT Light with Color Mode) ---
    if (comp == "light") {
      doc["stat_t"] = "stat/" + mqttTopicPub + "/" + sfx;
      doc["cmd_t"] = "cmnd/" + mqttTopicSub + "/" + sfx;
      
      // Standard topics for brightness and RGB (R,G,B strings)
      doc["bri_stat_t"] = "stat/" + mqttTopicPub + "/" + grp + "brightness";
      doc["bri_cmd_t"] = "cmnd/" + mqttTopicSub + "/" + grp + "brightness";
      doc["rgb_stat_t"] = "stat/" + mqttTopicPub + "/" + grp + "color";
      doc["rgb_cmd_t"] = "cmnd/" + mqttTopicSub + "/" + grp + "color";
      doc["clrm_stat_t"] = "stat/" + mqttTopicPub + "/" + grp + "mode";
      
      JsonArray modes = doc["sup_clrm"].to<JsonArray>();
      if (grp == "bulb") {
        modes.add("rgb");
        modes.add("color_temp");
        doc["clr_temp_stat_t"] = "stat/" + mqttTopicPub + "/bulbtemp";
        doc["clr_temp_cmd_t"] = "cmnd/" + mqttTopicSub + "/bulbtemp";
      } else if (grp == "led") {
        modes.add("rgb");
        doc["icon"] = "mdi:led-strip-variant";
      }
    } else if (grp == "diag") {
      doc["ent_cat"] = "diagnostic";
      if (sfx.indexOf("primary") != -1) {
        doc["stat_t"] = "stat/" + mqttTopicPub + "/primary/" + (sfx == "fw_primary" ? "version" : sfx == "ip_primary" ? "ipaddr" : "macaddr");
        if (comp == "button") doc["cmd_t"] = "cmnd/" + mqttTopicSub + "/primaryrestart";
      } else if (sfx.indexOf("display") != -1) {
        doc["stat_t"] = "stat/" + mqttTopicPub + "/display/" + (sfx == "fw_display" ? "version" : sfx == "ip_display" ? "ipaddr" : "macaddr");
        if (comp == "button") doc["cmd_t"] = "cmnd/" + mqttTopicSub + "/displayrestart";
      } else if (sfx == "ip_bulb") {
        doc["stat_t"] = "stat/" + mqttTopicPub + "/bulb/ipaddr";
      }
      if ((sfx.startsWith("mac_")) || (sfx.startsWith("fw_"))) doc["enabled_by_default"] = false;
      if (comp == "button") {
        doc.remove("stat_t");
        doc["pl_prs"] = "PRESS";
        doc["dev_cla"] = "restart";
      }
    } else {
      doc["stat_t"] = "stat/" + mqttTopicPub + "/" + sfx;
      if (comp != "sensor" && comp != "binary_sensor") {
        doc["cmd_t"] = "cmnd/" + mqttTopicSub + "/" + sfx;
      }
    }

    // Alarm Specific Handling and Button Payloads
    if (sfx == "alarm_stop" || sfx == "alarm_snooze") {
      doc["cmd_t"] = "cmnd/" + mqttTopicSub + "/alarmupdate";
      doc["pl_prs"] = (sfx == "alarm_stop") ? "stop" : "snooze";
      doc.remove("stat_t");
    } else if (sfx == "playalarm") {
      doc["pl_prs"] = "15"; 
      doc.remove("stat_t");
    } else if (comp == "select") {
      static const char* mainLabels[] = {"None", "Toggle Bulb", "Toggle LEDs", "Bulb Brighter", "Bulb Dimmer", "LEDs Brighter", "LEDs Dimmer", "Display Brighter", "Display Dimmer"};
      static const char* alarmLabels[] = {"None", "Snooze", "Stop"};
      const char** labels = (sfx.endsWith("funca")) ? alarmLabels : mainLabels;
      int count = (sfx.endsWith("funca")) ? 2 : 8; 

      JsonArray options = doc["ops"].to<JsonArray>();
      String cmdTemp = "{% set m = {";
      for (int i = 0; i <= count; i++) {
        options.add(labels[i]);
        cmdTemp += "'" + String(labels[i]) + "':'" + String(i) + "'" + (i < count ? "," : ""); 
      }
      doc["cmd_tpl"] = cmdTemp + "} %}{{ m[value] if value in m else value }}";
    } else if (comp == "number") {
      if (sfx == "alarmtrack") {
        doc["min"] = 1; doc["max"] = 255; doc["mode"] = "box";
      } else if (sfx == "alarmvolume") {
        doc["min"] = 0; doc["max"] = 30; doc["mode"] = "slider";
      } else if (sfx == "snoozetime") {
        doc["min"] = 0; doc["max"] = 60; doc["mode"] = "box";
      } else if (sfx == "dispbrightness") {
        doc["min"] = 0; doc["max"] = 255; doc["mode"] = "slider";
      }
    }
    String buffer;
    serializeJson(doc, buffer);
    client.publish(discoveryTopic.c_str(), buffer.c_str(), true);
    delay(25); 
  }
}

//================================
//  WEB PAGES AND HANDLERS
//================================
// Most HTML / code found in html.h

//------------------------
// Web Page Handlers Defs
//------------------------
void setupWebHandlers() {
  server.on("/", webMainPage);
  server.on("/onboard", handleOnboard);
  server.on("/otaupdate", handleOTAUpdate); //OTAUpdate via Arduino IDE
  server.on("/jsonmain", webMainPageJson);  //Get main settings page live data
  server.on("/webliveupdate", handleLiveWebUpdate);  //Process immediate updates for "live controls"
  server.on("/system", webSystemsPage);
  server.on("/jsonsystems", webSystemsPageJson);
  server.on("/applyintegrate", handleIntegrations);
  server.on("/restart", webRestartPage);
  server.on("/restartall", webRestartAllPage);
  server.on("/reset", webResetPage);
  server.on("/firmwareupdate", webFirmwareUpdate);
  server.on("/configdump", webConfigDump);
  server.on("/lights", webLightsPage);
  server.on("/jsonlights", webLightsPageJson);
  server.on("/testleds", handleLEDTest);
  server.on("/testbulb", handleBulbTest);
  server.on("/resetleds", handleLEDReset);
  server.on("/resetbulb", handleBulbReset);
  server.on("/saveleds", handleLEDSave);
  server.on("/savebulb", handleBulbSave);
  server.on("/advanced", webAdvancedPage);
  server.on("/jsonadvanced", webAdvancedPageJson);
  server.on("/savetouch", handleTouchSave);
  server.on("/savediscovery", handleDiscoverySave);
  server.on("/api", handleAPI);
}

//--------------------------------------
// Main Web Page / Onboarding functions
//--------------------------------------
void webMainPage() {
  if (onboarding) {
    //Onboarding page
    String page = String(onboard);
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*SSID*", wifiSSID);
    page.replace("*WIFIPW*", wifiPW);
    page.replace("*DEVICENAME*", deviceName);
    server.send(200, "text/html", page);
  } else {
    //Main Settings and Options page
    String page;
    page.reserve(strlen(mainpage) + 2000);
    page = mainpage;
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*SSID*", wifiSSID);
    page.replace("*MACADDR*", strMacAddr);
    page.replace("*IPADDR*", baseIPAddress);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");  //sends headers
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");  //tells browser send is complete
  }
}
// ** Onboarding **
void handleOnboard() {
  byte count = 0;
  bool wifiConnected = true;
  uint32_t currentMillis = millis();
  uint32_t pageDelay = currentMillis + 5000;
  String webPage = "";
  //Output web page to show while trying wifi join
  webPage = "<html><head>\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\ 
    <meta http-equiv=\"refresh\" content=\"1\">";  //make page responsive and refresh once per second
  webPage += "<title>VAR_APP_NAME Onboarding</title>\
      <style>\
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\
      </style>\
      </head>\
      <body>";
  webPage += "<h3>Attempting to connect to Wifi</h3><br>";
  webPage += "Please wait...  If WiFi connection is successful, device will reboot and you will be disconnected from the VAR_APP_NAME AP.<br><br>";
  webPage += "Reconnect to normal WiFi, obtain the device's new IP address and go to that site in your browser.<br><br>";
  webPage += "If the VAR_APP_NAME AP is still broadcasting after two minutes, reset the controller and attempt the onboarding again.<br>";
  webPage += "</body></html>";
  webPage.replace("VAR_APP_NAME", APPNAME);
  server.send(200, "text/html", webPage);
  while (pageDelay > millis()) {
    yield();
  }
  //Handle initial onboarding - called from main page
  //Get vars from web page
  wifiSSID = server.arg("ssid");
  wifiPW = server.arg("wifipw");
  deviceName = server.arg("devicename");
  wifiHostName = deviceName;

  //Attempt wifi connection
  WiFi.setSleep(false);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(wifiHostName);
  WiFi.begin(wifiSSID, wifiPW);
#if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
  Serial.print("SSID:");
  Serial.println(wifiSSID);
  Serial.print("password: ");
  Serial.println(wifiPW);
  Serial.print("Connecting to WiFi (onboarding)");
#endif
  while (WiFi.status() != WL_CONNECTED) {
#if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
    Serial.print(".");
#endif
    // Stop if cannot connect
    if (count >= 60) {
// Could not connect to local WiFi
#if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
      Serial.println();
      Serial.println("Could not connect to WiFi during onboarding.");
#endif
      wifiConnected = false;
      break;
    }
    delay(500);
    yield();
    count++;
  }
  if (wifiConnected) {
    //Save settings to LittleFS and reboot
    writeConfigFile(true);
  }
}
// **Main Web Page
void webMainPageJson() {
  String jsonString;
  JsonDocument doc;
  // Current Bulb State and Settings
  // retrieved via API call in currentBulbState(), which sets all active vars to current Bulb values
  if (currentBulbState()) {
   doc["top_bulb_brightness"] = (activeBulbBrightness * 100) / 255;
   doc["top_bulb_temperature"] = activeBulbTemp;
    if (activeBulbState) {
      doc["top_bulb_state"] = "On"; 
    } else {
      doc["top_bulb_state"] = "Off";
    }
    //set irregardless of current state
    if (activeBulbColorMode == 1) {
      doc["top_bulb_color"] = "FFFFFF";  //Show white when in color temperature mode
      doc["top_bulb_colormode"] = 1;
    } else {
      doc["top_bulb_color"] = activeBulbColor.substring(1);
      doc["top_bulb_colormode"] = 0;
    }
  } else {
    //Could not reach bulb (either not enabled or not conencted), default to all off/0 or min value
    doc["top_bulb_state"] = "Off";
    doc["top_bulb_brightness"] = 0;
    doc["top_bulb_color"] = "000000";
    doc["top_bulb_temperature"] = 155;  //min value
    doc["top_bulb_colormode"] = 0;      //RGB
  }
  doc["top_bulb_enabled"] = ((bulbEnabled) ? 1 : 0);
  if (bulbEnabled) {
    doc["top_bulb_connected"] = ((bulbConnected) ? 1 : 0);
  } else {
    doc["top_bulb_connected"] = 0;
  }

  //Current LED Strip state and settings
  if (currentLEDState()) {  //returns false if LEDs not enabled
    if (activeLEDState) {
      doc["led_strip_state"] = "On";
      //Convert raw 0-255 value to 0-100%
      doc["led_strip_brightness"] = (activeLEDBrightness * 100) / 255;
    } else {
      doc["led_strip_state"] = "Off";
      //Convert raw 0-255 value to 0-100%
      doc["led_strip_brightness"] = (lastLEDBrightness * 100) / 255; 
    }
    doc["led_strip_color"] = activeLEDColor.substring(1); //Convert RGB to long int
    doc["led_strip_enabled"] = 1;
  } else {
    doc["led_strip_state"] = "Off";
    doc["led_strip_brightness"] = 0;
    doc["led_strip_color"] = "000000";
    doc["led_strip_enabled"] = 0;
  }
  //Display integration (enabled or not)
  if (dispConnected) {
    doc["disp_ip"] = dispIPAddress;
  } else {
    doc["disp_id"] = "na";
  }
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleLiveWebUpdate() {
  //Get which control from web hidden field
  String whichControl = server.arg("updatefield");
  //Update values based on which control changed
  if (whichControl == "bulb_state") {
    String newState = server.arg("bulbstate");
    String color = server.arg("bulbcolorval");
    int colormode = server.arg("bulbcolormode").toInt();
    uint16_t newTemp = server.arg("bulbtempval").toInt();
    int newBrightness = server.arg("bulbbrightval").toInt();
    int newLevel = (newBrightness * 255) / 100;  //64

    activeBulbColorMode = colormode;
    if (newState == "On") {    
      toggleBulbState(1, color, newLevel, newTemp);
    } else {
      toggleBulbState(0, color, newLevel, newTemp);
    }
  } else if (whichControl == "bulb_brightness") {
    int newValue = server.arg("bulbbrightval").toInt();
    int newBrightness = newValue;
    newValue = (newBrightness * 255) / 100;
    if (newValue > 255) newValue = 255;
    setBulbBrightness(newValue);
  } else if (whichControl == "bulb_color") {
    String newColor = server.arg("bulbcolorval");
    setBulbColor(newColor);
  } else if (whichControl == "bulb_temperature") {
    int newValue = server.arg("bulbbrightval").toInt();
    int newBrightness = newValue;
    newValue = (newBrightness * 255) / 100;
    if (newValue > 255) newValue = 255;
    int newTemp = server.arg("bulbtempval").toInt();
    setBulbTemperature(newTemp, newValue);
  } else if (whichControl == "led_strip_color") {
    String newValue = server.arg("ledcolorval");
    setLEDColor(newValue);
  } else if (whichControl == "led_strip_state") {
    String newState = server.arg("ledstate");
    if (newState == "On") {
      toggleLEDState(1);
    } else {
      toggleLEDState(0);
    }
  } else if (whichControl == "led_strip_brightness") {
    int newValue = server.arg("ledbrightval").toInt();
    int newBrightness = newValue;
    newValue = (newBrightness * 255) / 100;
    if (newValue > 255) newValue = 255;
    setLEDBrightness(newValue);
  }
  //Refresh page
  webMainPage();
}

/* -------------------------------------
    Web Integrations Page and Functions
   ------------------------------------- */
void webSystemsPage() {
  //Integrations
  String page;
  page.reserve(strlen(systempage) + 3000);
  page = systempage;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*IPADDR*", baseIPAddress);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  page.replace("*MAXLEDS*", String(NUM_LEDS_MAX));
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

void webSystemsPageJson() {
  String jsonString;
  JsonDocument doc;
    doc["boot_delay"] = round(bootDelay / 1000);
    doc["max_leds"] = NUM_LEDS_MAX;
    doc["num_leds"] = numLEDs;
    doc["leds_enabled"] = ((numLEDs > 0) ? 1 : 0);

    doc["bulb_addr_1"] = bulbIPAddr_1;
    doc["bulb_addr_2"] = bulbIPAddr_2;
    doc["bulb_addr_3"] = bulbIPAddr_3;
    doc["bulb_addr_4"] = bulbIPAddr_4;
    doc["bulb_name"] = bulbName;
    doc["bulb_enabled"] = bulbEnabled;
    if ((bulbEnabled) && (bulbIsConnected())) {
      doc["bulb_connected"] = true;
      bulbConnected = true;
    } else {
      doc["bulb_connected"] = false;
      bulbConnected = false;
    }
    doc["disp_addr_1"] = dispIPAddr_1;
    doc["disp_addr_2"] = dispIPAddr_2;
    doc["disp_addr_3"] = dispIPAddr_3;
    doc["disp_addr_4"] = dispIPAddr_4;
    doc["disp_enabled"] = dispEnabled;
    if ((dispEnabled) && (displayIsConnected())) {
      doc["disp_connected"] = true;
      dispConnected = true;
    } else {
      doc["disp_connected"] = false;
      dispConnected = false;
    }
    doc["mqtt_addr_1"] = mqttAddr_1;
    doc["mqtt_addr_2"] = mqttAddr_2;
    doc["mqtt_addr_3"] = mqttAddr_3;
    doc["mqtt_addr_4"] = mqttAddr_4;
    doc["mqtt_enabled"] = mqttEnabled;
    doc["mqtt_port"] = mqttPort;
    doc["mqtt_tele_period"] = mqttTelePeriod;
    doc["mqtt_user"] = mqttUser;
    doc["mqtt_pw"] = mqttPW;
    doc["mqtt_topic_sub"] = mqttTopicSub;
    doc["mqtt_topic_pub"] = mqttTopicPub;
    if (mqttEnabled) {
        if (mqttIsConnected()) {
          doc["mqtt_connected"] = true;
          mqttConnected = true;
        } else {
          doc["mqtt_connected"] = false;
          mqttConnected = false;
        }
    } else {
      doc["mqtt_connected"] = false;
      mqttConnected = false;
    }

    doc["temp_units"] = defaultTempUnits;     //12=°C, 13=°F
    doc["weather_source"] = weatherSource;  //0 none (disabled), 1 OWM, 2 MQTT, 3 API
    doc["owm_key"] = owmKey;
    doc["owm_lat"] = owmLat;
    doc["owm_long"] = owmLong;
    doc["temp_update_period"] = tempUpdatePeriod;
    doc["owm_connected"] = owmConnected;

  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleIntegrations() {
  String page;
  page.reserve(strlen(postIntegrations) + 300);
  page = postIntegrations;
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    //Update local globals from web page
    bootDelay = (server.arg("bootdelay").toInt() * 1000);
    numLEDs = server.arg("numleds").toInt();

    bulbIPAddr_1 = server.arg("bulbip1").toInt();
    bulbIPAddr_2 = server.arg("bulbip2").toInt();
    bulbIPAddr_3 = server.arg("bulbip3").toInt();
    bulbIPAddr_4 = server.arg("bulbip4").toInt();
    bulbName = server.arg("bulbname");

    dispIPAddr_1 = server.arg("dispip1").toInt();
    dispIPAddr_2 = server.arg("dispip2").toInt();
    dispIPAddr_3 = server.arg("dispip3").toInt();
    dispIPAddr_4 = server.arg("dispip4").toInt();

    mqttAddr_1 = server.arg("mqttaddr1").toInt();
    mqttAddr_2 = server.arg("mqttaddr2").toInt();
    mqttAddr_3 = server.arg("mqttaddr3").toInt();
    mqttAddr_4 = server.arg("mqttaddr4").toInt();
    mqttPort = server.arg("mqttport").toInt();
    mqttTelePeriod = server.arg("mqttperiod").toInt();
    mqttUser = server.arg("mqttuser");
    mqttPW = server.arg("mqttpw");
    mqttTopicSub = server.arg("mqtttopicsub");
    mqttTopicPub = server.arg("mqtttopicpub");

    defaultTempUnits = server.arg("tempunits").toInt();
    weatherSource = server.arg("weathersource").toInt();
    owmKey = server.arg("owmkey");
    owmLat = server.arg("owmlat");
    owmLong = server.arg("owmlong");
    tempUpdatePeriod = server.arg("tempupdperiod").toInt();

    //return web page
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
    delay(1000);
    writeConfigFile(true);
  }
}

/* --------------------------------------
    Lights (LED & Bulb) Page & Functions
   -------------------------------------- */
void webLightsPage() {
  String page;
  page.reserve(strlen(lightspage) + 3000);
  page = lightspage;
  //return web page
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

void webLightsPageJson() {
  String jsonString;
  int dfltBrightness = defaultLEDBrightness;
  int dfltBulbBright = defaultBulbBrightness;
  JsonDocument doc;
  doc["use_leds"] = ((useLEDs) ? 1 : 0);
  doc["max_leds"] = NUM_LEDS_MAX;
  //LEDs
  if (ledTestFlag) {
    int testBrightness = activeLEDBrightness;
    doc["num_leds"] = ledTestNumLeds;
    doc["led_state"] = ledTestStartState;
    doc["led_color"] = activeLEDColor.substring(1);
    doc["led_brightness"] = (testBrightness * 100) / 255;
    doc["use_boot_lights_led"] = ledTestBootLights;
    doc["led_test_flag"] = 1;
  } else {
    doc["num_leds"] = numLEDs;
    doc["led_state"] = ((defaultLEDState) ? 1 : 0);
    doc["led_color"] = defaultLEDColor.substring(1);
    doc["led_brightness"] = (dfltBrightness * 100) / 255;
    doc["use_boot_lights_led"] = ((useBootLightsLED) ? 1 : 0);
    doc["led_test_flag"] = 0;
  }
  //Bulb
  doc["bulb_enabled"] = ((bulbEnabled) ? 1 : 0);
  doc["bulb_ip"] = bulbIPAddress;
  doc["bulb_name"] = bulbName;
  if (bulbTestFlag) {
    doc["bulb_state"] = bulbTestStartState;
    doc["bulb_color_mode"] = activeBulbColorMode;       //0 RGB, 1 WHITE
    doc["bulb_color"] = activeBulbColor.substring(1);
    doc["bulb_temperature"] = activeBulbTemp;
    doc["bulb_brightness"] = (activeBulbBrightness * 100) / 255;
    doc["use_boot_lights_bulb"] = ((bulbTestBootLights) ? 1 : 0);
    bulbTestFlag = false;
  } else {
    doc["bulb_state"] = ((defaultBulbState) ? 1 : 0);
    doc["bulb_color_mode"] = defaultBulbColorMode;      //0 RGB, 1 WHITE
    doc["bulb_color"] = defaultBulbColor.substring(1);
    doc["bulb_temperature"] = defaultBulbTemp;
    doc["bulb_brightness"] = (dfltBulbBright * 100) / 255;
    doc["use_boot_lights_bulb"] = ((useBootLightsBulb) ? 1 : 0);
  }
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleLEDTest() {
  if (server.method() != HTTP_POST || !server.hasArg("plain")) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  }
  JsonDocument doc;
  DeserializationError dsError = deserializeJson(doc, server.arg("plain"));
  if (!dsError) {
    bool startTest = doc["mode"];
    int testNumLeds = doc["numleds"];
    String testLEDColor = doc["color"];
    byte tmpBrightness = doc["brightness"];
    byte testLEDBrightness = (tmpBrightness * 255) / 100;
    if (startTest) {
      ledTestFlag = true;
      CRGB ledColor = hexToCRGB(testLEDColor);
      FastLED.setBrightness(testLEDBrightness);
      fill_solid(LEDs, testNumLeds, ledColor);
      FastLED.show();
      server.send(200, "text/plain", "LED test started.");
    } else {
      //Reset vars
      ledTestFlag = false;
      FastLED.setBrightness(defaultLEDBrightness);
      //Turn off ALL LEDs (use max num)
      fill_solid(LEDs, NUM_LEDS_MAX, CRGB::Black);
      FastLED.show();
      server.send(200, "text/plain", "LED test stopped.");
    }
  } else {
    server.send(500, "text/plain", "Failed to start LED test!");
  }
}

void handleBulbTest() {
  if (server.method() != HTTP_POST || !server.hasArg("plain")) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  }
  JsonDocument doc;
  DeserializationError dsError = deserializeJson(doc, server.arg("plain"));
  if (!dsError) {
    bool startTest = doc["mode"];
    if (startTest) {
      // Temporarily override active values with test values
      String testColor = doc["color"];
      byte tmpBright = doc["brightness"];
      int testTemp = doc["temp"];
      byte testMode = doc["colormode"];
      byte testBright = (tmpBright * 255) / 100;
      activeBulbColorMode = testMode;
      toggleBulbState(true, testColor, testBright, testTemp);
      server.send(200, "text/plain", "Bulb test started");
    } else {
      // Revert to saved defaults
      activeBulbState = 0;
      toggleBulbState(0); 
      server.send(200, "text/plain", "Bulb test stopped");
    }
  } else {
    server.send(500, "text/plain", "JSON Parse Error");
  }
}

void handleLEDSave() {

  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String page;
    page.reserve(strlen(postLEDSettings) + 300);
    page = postLEDSettings;
    byte ledStartStateTest = ((server.arg("ledstartstate") == "On") ? 1 : 0);
    byte ledBootLightsTest = server.arg("bootlightsled").toInt();

    int ledBrightnessTest = server.arg("ledbright").toInt();
    int ledBrightTest = (ledBrightnessTest * 255) / 100;
    
    numLEDs = server.arg("numleds").toInt();
    defaultLEDState = ledStartStateTest; 
    defaultLEDBrightness = ledBrightTest;
    defaultLEDColor = server.arg("ledcolor");
    useBootLightsLED = ledBootLightsTest; 

    //return web page
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");

    delay(1000);
    writeConfigFile(true);
  }
}

void handleBulbSave() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String page;
    page.reserve(strlen(postBulbSettings) + 300);
    page = postBulbSettings;
    byte bulbStartStateTest = server.arg("bulbstartstate").toInt();
    byte bulbBootLightsTest = server.arg("bootlightsbulb").toInt();
    int bulbBrightnessTest = server.arg("bulbbright").toInt();
    int bulbBrightTest = (bulbBrightnessTest * 255) / 100;

    defaultBulbState = bulbStartStateTest;
    defaultBulbColorMode = server.arg("bulbcolormode").toInt();
    defaultBulbColor = server.arg("bulbcolor");
    defaultBulbTemp = server.arg("bulbtemperature").toInt();
    defaultBulbBrightness = bulbBrightTest;
    useBootLightsBulb = bulbBootLightsTest;

    //return web page
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
    delay(1000);
    writeConfigFile(true);
  }
}

void handleLEDReset() {
  ledTestFlag = false;
  if (useLEDs) {
    //Set current active values to defaults
    CRGB ledColor = hexToCRGB(defaultLEDColor);
    FastLED.setBrightness(defaultLEDBrightness);
    fill_solid(LEDs, numLEDs, ledColor);
    FastLED.show();
    //Set active values
    activeLEDState = true;
    activeLEDColor = defaultLEDColor;
    activeLEDBrightness = defaultLEDBrightness;
  }
  webLightsPage();
}

void handleBulbReset() {
  bulbTestFlag = false;
  if (bulbEnabled) {
    if (defaultBulbColorMode) {   
      //color_temp / White
      setBulbTemperature(defaultBulbTemp, defaultBulbBrightness); 
    } else {                       
      //RGB
      setBulbColor(defaultBulbColor, defaultBulbBrightness);
    }
    //Set active values
    activeBulbState = true;
    activeBulbColorMode = defaultBulbColorMode;
    activeBulbColor = defaultBulbColor;
    activeBulbTemp = defaultBulbTemp;
    activeBulbBrightness = defaultBulbBrightness;
  }
  webLightsPage();
}

/* -----------------------------------
    Advanced Options Page & Functions
   ----------------------------------- */
void webAdvancedPage() {
  String page;
  page.reserve(strlen(advancedpage) + 3000);
  page = advancedpage;
  //return web page
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

void webAdvancedPageJson() {
  loadDiscoveryConfig();

  String jsonString;
  JsonDocument doc;
  doc["touch1_enabled"] = ((touch1Enabled) ? 1 : 0);
  doc["touch1_duration"] = touch1Duration;
  doc["touch1_control_1"] = touch1Control1;
  doc["touch1_control_2"] = touch1Control2;
  doc["touch2_enabled"] = ((touch2Enabled) ? 1 : 0);
  doc["touch2_duration"] = touch2Duration;
  doc["touch2_control_1"] = touch2Control1;
  doc["touch2_control_2"] = touch2Control2;
  // Add Discovery Data
  doc["disc_exists"] = currentDiscovery.exists;
  doc["disc_devname"] = currentDiscovery.deviceName;
  doc["disc_bulb"] = currentDiscovery.lightBulb;
  doc["disc_led"] = currentDiscovery.ledStrip;
  doc["disc_disp"] = currentDiscovery.display;
  doc["disc_touch"] = currentDiscovery.touch;
  doc["disc_alarms"] = currentDiscovery.alarms;
  doc["disc_diag"] = currentDiscovery.diagnostics;

  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}
//------ Touch Sensors -------
void handleTouchSave() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String page;
    page.reserve(strlen(postTouchSettings) + 300);
    page = postTouchSettings;
    byte touch1check = server.arg("touch1enableval").toInt();
    uint16_t touch1dur = server.arg("touch1duration").toInt();
    byte touch1func1 = server.arg("touch1control1").toInt();
    byte touch1func2 = server.arg("touch1control2").toInt();
    byte touch2check = server.arg("touch2enableval").toInt();
    uint16_t touch2dur = server.arg("touch2duration").toInt();
    byte touch2func1 = server.arg("touch2control1").toInt();
    byte touch2func2 = server.arg("touch2control2").toInt();
    //Set globals
    touch1Enabled = touch1check;
    touch1Duration = touch1dur;
    touch1Control1 = touch1func1;
    touch1Control2 = touch1func2;
    touch2Enabled = touch2check;
    touch2Duration = touch2dur;
    touch2Control1 = touch2func1;
    touch2Control2 = touch2func2;
    //return web page
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*IPADDR*", baseIPAddress);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
    if (mqttConnected) {
      updateMQTT("touchall");
    }
    delay(1000);
    writeConfigFile(true);
  }
}
//----- HA Discovery Save -----
void handleDiscoverySave() {
  String action = server.arg("discaction");

  if (action == "delete") {
    publishDiscovery(false);
    deleteDiscoveryConfig();
    //redirect to delete page
    String page;
    page.reserve(strlen(postDiscoveryRemove) + 300);
    page = postDiscoveryRemove;
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*DEVICENAME*", deviceName);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
  } else if (action == "save") {
    currentDiscovery.deviceName = server.arg("devicename");
    currentDiscovery.lightBulb = server.hasArg("bulb");
    currentDiscovery.ledStrip = server.hasArg("led");
    currentDiscovery.display = server.hasArg("display");
    currentDiscovery.touch = server.hasArg("touch");
    currentDiscovery.alarms = server.hasArg("alarms");
    currentDiscovery.diagnostics = server.hasArg("diagnostic");

    if (saveDiscoveryConfig()) {
      delay(250);    //add short delay to allow file save to complete/commit
      publishDiscovery(true); 
      String page;
      page.reserve(strlen(postDiscoveryAdd) + 300);
      page = postDiscoveryAdd;
      page.replace("*VAR_APP_NAME*", APPNAME);
      page.replace("*VAR_CURRENT_VER*", VERSION);
      page.replace("*DEVICENAME*", deviceName);
      server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      server.send(200, "text/html", "");
      server.sendContent(page);
      server.sendContent(html_footer);
      server.sendContent("");
    } else {
      server.send(500, "text/plain", "Error saving config");
    }
  }
}
/* -----------------------------------
    Restart (Reboot) Page & Functions
   ----------------------------------- */
void webRestartPage() {
  String page;
  page.reserve(strlen(restart) + 300);
  page = restart;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*IPADDR*", baseIPAddress);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
  delay(1000);
  //Turn off any lights before reboot
  if (useLEDs) {
    toggleLEDState(0);
  }
  if (bulbEnabled) {
    toggleBulbState(0);
  }
  ESP.restart();
}

/* ----------------------===--------------
    Restart (Reboot) ALL Page & Functions
   --------------------------------------- */
void webRestartAllPage() {
  String page;
  page.reserve(strlen(restartall) + 300);
  page = restartall;
  String bulbResult = "<font color=blue><i>*Not enabled*<i/></font>";
  String dispResult = "<font color=blue><i>*Not enabled*<i/></font>";
  //Bulb
  if (bulbEnabled) {
    String serverPath = "http://" + bulbIPAddress + "/button/restart_firmware/press";
    http.useHTTP10(true);
    http.begin(serverPath);
    int response = http.POST("");
    if ((response > 199) && (response < 300)) {
      //2xx codes means data sent/accepted
      bulbResult = "<font color=#13400f><b>OK<b/></font>";
    } else {
      bulbResult = "<font color=red>Error: " + String(response) + "</font>";
    }
    http.end();
  }
  if (dispEnabled) {
    String serverPath = "http://" + dispIPAddress + "/restart";
    http.useHTTP10(true);
    http.begin(serverPath);
    int response = http.POST("");
    if ((response >= 200) && (response < 300)) {
      //2xx codes means data sent/accepted
      dispResult = "<font color=#13400f><b>OK<b/></font>";
      //Provide delay (ping up to 5 tries) to give display controller a 'head start' on rebooting
      int delayCount = 5 - bootDelay;
      if (delayCount > 0) {
        http.end();
        serverPath = "http://" + dispIPAddress + "/api";
        String requestData = "ping=1";
        http.begin(serverPath);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        for (int i=0; i < delayCount; i++) {
          response = http.POST(requestData);
          if (response > 0) {
            //Connected
            i = 99;
            break;
          }
          delay(1000);
        }
      }
    } else {
      dispResult = "<font color=red>Error: " + String(response) + "</font>";
    }
    http.end();
  }

  page.replace("*BULBRESULT*", bulbResult);
  page.replace("*DISPRESULT*", dispResult);
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  page.replace("*IPADDR*", baseIPAddress);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
  delay(1000);
  //Turn off any lights before reboot
  if (useLEDs) {
    toggleLEDState(0);
  }
  if (bulbEnabled) {
    toggleBulbState(0);
  }
  ESP.restart();
}

/* -------------------------------
    Full Reset Page and Functions
   ------------------------------- */
void webResetPage() {
  String page;
  page.reserve(strlen(reset) + 300);
  page = reset;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
  delay(1000);
  LittleFS.begin();
  LittleFS.format();
  LittleFS.end();
  WiFi.disconnect(false, true);
  ESP.restart();
}

/* --------------------------------------
    Firmware Updgrade Page and Functions
   -------------------------------------- */
void webFirmwareUpdate() {
  String page;
  page.reserve(strlen(updateFirmware) + 300);
  page = updateFirmware;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

/* --------------------------------
    Config dump Page and Functions
   -------------------------------- */
void webConfigDump() {
  String page;
  page.reserve(strlen(configpage) + 300);
  page = configpage;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*IPADDR*", baseIPAddress);
  page.replace("*CONFIGJSON*", getCurrentConfig());
  page.replace("*DISCJSON*", getDiscoveryConfig());
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  //No footer on this page
  server.sendContent("");
}

/* --------------------------------
    Arudino OTA Page and Functions
   -------------------------------- */
void handleOTAUpdate() {
  String page;
  page.reserve(strlen(otaHtml) + 300);
  page = otaHtml;
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
  //Start OTA Window
  ota_flag = true;
  ota_time = ota_time_window;
  ota_time_elapsed = 0;
}

void displayOTA() {
  //Set display and/or Lights to indicate OTA mode
  //**For future use**
}

// ================================
//  LED functions
// ================================

bool currentLEDState() {
  if (useLEDs) {
    //Set LED variables based on current state
    activeLEDBrightness = FastLED.getBrightness();
    //Returns true if color is anything other than black (0, 0 ,0) AND brightness > 0
    uint8_t redValue = LEDs[0].r;
    uint8_t greenValue = LEDs[0].g;
    uint8_t blueValue = LEDs[0].b;
    
    //Base state on non-zero brightness and color other than black
    if (((redValue > 0) || (greenValue > 0) || (blueValue > 0)) && (activeLEDBrightness > 0)) {
      activeLEDState = true;
      activeLEDColor = rgbToHex(redValue, greenValue, blueValue);
    } else {
      activeLEDState = false;
      //Don't change active color if LEDs are off
    }
    return true;
  } else {
    return false;
  }
}

void toggleLEDState(bool state, String color, byte brightness) {
  //LEDs do not have a "state".  On/off handled via brightness, where 0 = Off and >0 = On
  if (useLEDs) {
    if (state) {
      //Set brightness to some value > 0
      if (brightness == 0) {
        if (activeLEDBrightness > 0) {
          brightness = activeLEDBrightness;
        } else if (lastLEDBrightness > 0) {
          brightness = lastLEDBrightness;
        } else if (defaultLEDBrightness > 0) {
          brightness = defaultLEDBrightness;
        } else {
          brightness = 64;  //~25% default to non-zero value so LEDs at least turn on
        }
      }
      CRGB newColor = hexToCRGB(color);
      FastLED.setBrightness(brightness);
      fill_solid(LEDs, numLEDs, newColor);
      FastLED.show();
      activeLEDState = true;
      activeLEDBrightness = brightness;
      activeLEDColor = color;
    } else {
      //Turn off LEDs
      //Store current brightness value for default toggle on
      if (activeLEDBrightness > 0) {
        lastLEDBrightness = activeLEDBrightness;
      } else if (defaultLEDBrightness > 0) {
        lastLEDBrightness = defaultLEDBrightness;
      } else {
        lastLEDBrightness = 64;
      }
      FastLED.setBrightness(0);
      FastLED.show();
      activeLEDState = false;
      activeLEDBrightness = 0;
      activeLEDColor = color;
    }
    if (mqttConnected) updateMQTT("ledall");
  }
}

void setLEDBrightness(byte brightness){ 
  if (useLEDs) {
    //Sets brightness.  If brightness = 0, LEDS will be considered 'OFF', or 'ON' otherwise
    //If setting to '0', the current activeBrightness is stored in lastBrightness for recall (and web slider)
    if (brightness == 0) {
      lastLEDBrightness = activeLEDBrightness;
      activeLEDBrightness = 0;
      activeLEDState = false;
    } else {
      activeLEDBrightness = brightness;
      lastLEDBrightness = activeLEDBrightness;
      activeLEDState = true;
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
    if (mqttConnected) updateMQTT("ledall");
  }
}

void setLEDColor(String hexColor, byte brightness) {
  //Setting color to non-black will turn on LEDs if off
  //Setting color to black (#000000) will turn off LEDs and set state to OFF.
  if (useLEDs) {
    if (brightness == 0) {
      if (activeLEDBrightness > 0) {
        brightness = activeLEDBrightness;
      } else if (lastLEDBrightness > 0) {
        brightness = lastLEDBrightness;
      } else if (defaultLEDBrightness > 0) {
        brightness = defaultLEDBrightness;
      } else {
        brightness = 64;  //~25% default to non-zero value so LEDs at least turn on
      }
    }
    CRGB newColor = hexToCRGB(hexColor);
    FastLED.setBrightness(brightness);
    fill_solid(LEDs, numLEDs, newColor);
    FastLED.show();
    activeLEDBrightness = brightness;
    lastLEDBrightness = activeLEDBrightness;
    activeLEDColor = hexColor;
    if (hexColor == "#000000") {
     activeLEDState = 0;
    } else {
     activeLEDState = 1;
    }
    if (mqttConnected) updateMQTT("ledall");

  }
}
// ================================
//  RGBWW Light Bulb Functions
// ================================
bool bulbIsConnected() {
  bool retVal = false;
  if (bulbEnabled) {
    IPAddress bulb_ip(bulbIPAddr_1, bulbIPAddr_2, bulbIPAddr_3, bulbIPAddr_4);
    bool pingResult =Ping.ping(bulb_ip);
    if (pingResult) {
      retVal = true;
    }
  }
  return retVal;
}

bool currentBulbState() {
  //Polls bulb to get its current state and set active vars
  if (!bulbEnabled) return false;
  http.useHTTP10(true);
  http.begin(bulbServerPath);
  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, http.getStream());
    if (!error) {
      activeBulbBrightness = doc["brightness"] | 0;
      activeBulbState = (doc["state"] == "ON") ? 1 :0;

      String colormode = doc["color_mode"] | "";
      if (colormode == "rgb") {
        activeBulbColorMode = 0;
        //Check for the existence of the color object to prevent null-ptr access
        JsonObject color = doc["color"];
        if (!color.isNull()) {
          activeBulbColor = rgbToHex(doc["color"]["r"] | 0, doc["color"]["g"] | 0, doc["color"]["b"] | 0);
        }
      } else {
        activeBulbColorMode = 1;
        activeBulbTemp = doc["color_temp"] | 0;
        activeBulbColor = "#ffffff";  //white
      }
      bulbConnected = true;
    } else {
      bulbConnected = false;
    }
  } else {
    bulbConnected = false;
  }
  http.end();
  return bulbConnected;
}

void toggleBulbState(bool state, String hexColor, byte brightness, uint16_t temperature) {
  if (bulbEnabled) {
    String serverPath;
    if (state) {
      //Turn on and set color, brightness, temp
      if (activeBulbColorMode == 1) {
        setBulbTemperature(temperature, brightness);
      } else {
        setBulbColor(hexColor, brightness);
      }
      activeBulbState = 1;
    } else {
      serverPath = bulbServerPath + "/turn_off";
      activeBulbState = 0;
      WiFiClient client;
      HTTPClient http;
      http.useHTTP10(true);
      http.setTimeout(500);
      http.begin(client, serverPath);
      int response = http.POST("");
      http.end();
    }
    if (mqttConnected) updateMQTT("bulball");
  }
}

void setBulbBrightness(byte newValue) {
  if (bulbEnabled) {
    String serverPath;
    serverPath = bulbServerPath + "/turn_on?brightness=" + String(newValue);
    WiFiClient client;         
    HTTPClient http;               
    http.useHTTP10(true);
    http.setTimeout(500);
    http.begin(client, serverPath);
    int response = http.POST("");
    if (response > 0) {
      activeBulbState = 1;
      activeBulbBrightness = newValue;
      if (mqttConnected) updateMQTT("bulball");
    }
    http.end();
  }
}

void setBulbBrightnessNoState(byte newValue) {
  if (bulbEnabled) {
    String serverPath;
    String jsonString;
    serverPath = bulbServerPath + "/turn_on";
    JsonDocument doc;
    doc["brightness"] = newValue;
    serializeJson(doc, jsonString);
    WiFiClient client;         
    HTTPClient http;               
    http.useHTTP10(true);
    http.setTimeout(500);
    http.begin(client, serverPath);
    int response = http.POST(jsonString);
    if (response > 0) {
      activeBulbBrightness = newValue;
      if (mqttConnected) updateMQTT("bulbbrightness");
    }
    http.end();
  }
}

void setBulbColor(String hexColor, byte brightness) {
  if (bulbEnabled) {
    String serverPath;
    byte red;
    byte green;
    byte blue;
    WiFiClient localClient;
    HTTPClient localHttp;
    //Convert hex to RGB values
    hexToRgb(hexColor, red, green, blue);
    serverPath = bulbServerPath + "/turn_on?color_mode=rgb";
    serverPath += "&r=" + String(red) + "&g=" + String(green) + "&b=" + String(blue);
    serverPath += "&brightness=" + String(brightness);
    localHttp.useHTTP10(true);
    localHttp.setTimeout(500);
    localHttp.begin(localClient, serverPath);
    int response = localHttp.POST("");
    if (response > 0) {
      activeBulbState = 1;
      activeBulbColor = hexColor;
      activeBulbColorMode = 0;  //rgb
      if (mqttConnected) updateMQTT("bulball");
    }
    localHttp.end();
  }
}

void setBulbTemperature(int newTemp, byte brightness) {
  if (bulbEnabled) {
    String serverPath;
    serverPath = bulbServerPath + "/turn_on?color_mode=color_temp&color_temp=" + String(newTemp) + "&brightness=" + String(brightness);
    WiFiClient client;         
    HTTPClient http;               
    http.useHTTP10(true);
    http.setTimeout(500);
    http.begin(client, serverPath);
    int response = http.POST("");
    if (response > 0) {
      activeBulbState = 1;
      activeBulbTemp = newTemp;
      activeBulbColorMode = 1;  //color_temp
      activeBulbColor = "#ffffff";
      activeBulbBrightness = brightness;
      if (mqttConnected) updateMQTT("bulball");
    }
    http.end();
  }
}

// ================================
//  TOUCH SENSOR functions
// ================================
void handleTouchPress(byte whichSensor) {
  byte action1 = 0;
  byte action2 = 0;
  if (whichSensor == 1) {
    touch1MqttLatched = true;
    action1 = touch1Control1;
    action2 = touch1Control2;
  } else {
    touch2MqttLatched = true;
    action1 = touch2Control1;
    action2 = touch2Control2;
  }
  if (alarmActive) {
    //Use touch sensor second function
    switch (action2) {
      case 1:
        //Snooze alarm
        if (sendAlarmAction("snooze")) {
          alarmActive = false;
        }
        break;
      case 2:
        //Stop or cancel next alarm
        if (sendAlarmAction("stop")) {
          alarmActive = false;
        }
        break;
      default:
        alarmActive = false;
        break;
    }
  } else {
    switch (action1) {
      case 1:
        //Toggle Bulb
        if (bulbEnabled) {
          toggleBulbState(!activeBulbState);
        }
        break;
      case 2:
        //Toggle LED
        if (useLEDs) {
          toggleLEDState(!activeLEDState);
        }
        break;
      case 3:
        //bulb bright+ (current state must be ON)
        if ((bulbEnabled) && (activeBulbState)) {
          uint16_t newBright;
          //Add ~10% to current brightness
          newBright = activeBulbBrightness + 25;  
          if (newBright > 255) newBright = 25;  //Wrap to dim
          setBulbBrightness(newBright);
        }
        break;
      case 4:
        //bulb bright- (current state must be ON)
        if ((bulbEnabled) && (activeBulbState)) {
          uint16_t newBright = 0;
          //Subtract ~10% from current brightness
          if (activeBulbBrightness > 25) {
            newBright = activeBulbBrightness - 25; 
          } 
          if (newBright == 0) newBright = 250;  //Wrap to bright
          setBulbBrightness(newBright);
        }
        break;
      case 5:
        //LED bright+ (current state must be ON)
        if ((useLEDs) && (activeLEDState)) {
          uint16_t newBright;
          //Add ~10% to current brightness
          newBright = activeLEDBrightness + 25;
          if (newBright > 255) newBright = 25;  //Wrap to dim
          setLEDBrightness(newBright);
        }
        break;
      case 6:
        //LED bright- (current state must be ON)
        if ((useLEDs) && (activeLEDState)) {
          uint16_t newBright = 0;
          //Subtract ~10% from current brightness
          if (activeLEDBrightness > 25) {
            newBright = activeLEDBrightness - 25;  
          }
          if (newBright == 0) newBright = 250;  //Wrap to bright
          setLEDBrightness(newBright);
        }
        break;
      case 7:
        //Display bright +
        if (dispConnected) {
          String serverPath = "http://" + dispIPAddress + "/api";
          String requestData = "dispbrightness=up";
          WiFiClient client;
          HTTPClient http;
          http.useHTTP10(true);
          http.setTimeout(2000);
          http.begin(serverPath);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          int response = http.POST(requestData);
        }
        break;
      case 8:
        //Display bright -
        if (dispConnected) {
          String serverPath = "http://" + dispIPAddress + "/api";
          String requestData = "dispbrightness=down";
          WiFiClient client;
          HTTPClient http;
          http.useHTTP10(true);
          http.setTimeout(2000);
          http.begin(serverPath);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");
          int response = http.POST(requestData);
        }
        break;
      default:
        //do nothing
        break;
    }
  }
  delay(200);  //allow short time for user to remove finger from touch sensor
}

//================================
// API Handler and Functions
//================================
//--------------------------
// Handle incoming requests
//--------------------------
void handleAPI() {

  bool recognized = false;
  bool alreadyProxied = server.hasArg("proxied");  //flag to prevent infinite loop between controllers
  String forwardedResponse = "";
  // ---- INTERNAL SYSTEM COMMANDS ----
  // These are used for internal communications
  // and should not be called directly via the API interface
  // Display server IP address
  if (server.argName(0) == "displayaddr") {
    if ((!dispEnabled) || (!dispConnected)) {
      String dispIPAddress = server.arg(0);
      if (reconnectDisplay(dispIPAddress)) {
        server.send(200, "text/plain", "OK");
      } else {
        server.send(200, "text/plain", "FAIL");
      }
      return;
    }
  }
  // Send all MQTT connection details to display
  if (server.argName(0) == "mqttdata") {
    JsonDocument doc;
    String jsonData;
    doc.clear();
    doc["mqtt_addr_1"] = mqttAddr_1;
    doc["mqtt_addr_2"] = mqttAddr_2;
    doc["mqtt_addr_3"] = mqttAddr_3;
    doc["mqtt_addr_4"] = mqttAddr_4;
    doc["mqtt_port"] = mqttPort;
    doc["mqtt_user"] = mqttUser;
    doc["mqtt_pw"] = mqttPW;
    doc["mqtt_sub"] = mqttTopicSub;
    doc["mqtt_pub"] = mqttTopicPub;
    doc["mqtt_period"] = mqttTelePeriod;
    serializeJson(doc, jsonData);
    server.send(200, "application/json", jsonData);
    return;
  }
  //Send all weather data connection info to display controller
  if (server.argName(0) == "weatherdata") {
    JsonDocument doc;
    String jsonData;
    doc.clear();
    doc["temp_units"] = defaultTempUnits;
    doc["weather_source"] = weatherSource;
    doc["owm_key"] = owmKey;
    doc["owm_lat"] = owmLat;
    doc["owm_long"] = owmLong;
    doc["temp_update_period"] = tempUpdatePeriod;
    serializeJson(doc, jsonData);
    server.send(200, "application/json", jsonData);
    return;
  }
  //Alarm State (from disp controller)
  if (server.argName(0) == "alarmstate") {
    if (server.arg(0) == "on") {
      alarmActive = ((server.arg(0) == "on") ? 1 : 0);
    }
    return;
  }

  // These commands are available for general API use
  //Ping
  if (server.argName(0) == "ping") {
    server.send(200, "text/plain", "OK");
    return;
  }  
  //Local IP Address
  if ((server.argName(0) == "ipaddress") || (server.argName(0) == "primaryipaddr")) {
    server.send(200, "text/plain", baseIPAddress);
    return;
  }  
  //Local MAC address
  if (server.argName(0) == "primarymac") {
    server.send(200, "text/plain", strMacAddr);
    return;
  }
  //Bulb IP Address
  if (server.argName(0) == "bulbipaddr") {
    server.send(200, "text/plain", bulbIPAddress);
    return;
  }
  //Restart Primary (this) Controller
  if (server.argName(0) == "primaryrestart") {
    server.send(200, "text/plain", "OK - Primary rebooting");
    delay(1000);
    ESP.restart();
  }
  //Restart bulb
  if (server.argName(0) == "bulbrestart") {
    server.send(200, "text/plain", "OK - Bulb rebooting");
    
  }
  //Reboot all controllers
  if (server.argName(0) == "restartall") {
    server.send(200, "text/plain", "OK - Attempting to reboot ALL controller");
    restartAllControllers();
    return;
  }

  //Multi-command Action Loop
  for (int i = 0; i < server.args(); i++) {
    String key = server.argName(i);
    String val = server.arg(i);
    if (key == "proxied") continue;
    // A. Alarm Passthrough (Direct to Display Controller)
    if (key == "alarmstop") {
      if (sendAlarmAction("stop")) recognized = true;
    } 
    else if (key == "alarmsnooze") {
      if (sendAlarmAction("snooze")) recognized = true;
    }
    // B. Everything else goes to the central command processor
    else {
      if (processCommand(key, val)) {
        recognized = true;
      }
    }
  }
  //Command not recognized, try forwarding to other controller
  if ((!recognized) && (!alreadyProxied) && (dispConnected)) {
    forwardedResponse = forwardAPIRequest(dispIPAddress);
    if (forwardedResponse != "") {
      recognized = true;
    }  
  }
  if (recognized) {
    if (forwardedResponse != "") {
      server.send(200, "text/plain", forwardedResponse);
    } else {
      server.send(200, "text/plain", "OK");
    }
  } else {
    server.send(404, "text/plain", "Unknown or Invalid API Command (primary)");
  }
}

String forwardAPIRequest(String targetIP) {
  String response = "";
  WiFiClient client;
  HTTPClient http;

  // Reconstruct the full query string (e.g., ledstate=on&brightness=100)
  String queryString = "";
  for (int i = 0; i < server.args(); i++) {
    if (i > 0) queryString += "&";
    queryString += server.argName(i) + "=" + server.arg(i);
  }
  // Add a "Circuit Breaker" to prevent infinite loops if forwarded request still not found
  if (queryString.length() > 0) queryString += "&";
  queryString += "proxied=1";
  String targetURL = "http://" + targetIP + "/api?" + queryString;

  http.setTimeout(3000);
  http.begin(client, targetURL);
  int httpCode = http.GET();

  // If the partner controller returns 200 OK, we consider it a success
  if (httpCode == HTTP_CODE_OK) {
    response = http.getString();
  }

  http.end();
  return response;
}
//---------------------------------------
// Posts to external APIs (e.g. Display)
//---------------------------------------
// =====================
//  ALARM Functions
// =====================
bool sendAlarmAction(String state) {
  bool retVal = false;
  String serverPath = "http://" + dispIPAddress + "/api";
  String requestData = "alarmupdate=" + state;
  http.begin(serverPath);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int response = http.POST(requestData);
  //There is an issue with getting a valid return so just return true for now
  retVal = true;

  http.end();
  return retVal;
}
// ================================
//  DISPLAY functions
// ================================
bool displayIsConnected() {
  bool retVal = false;
  String serverPath = "http://" + dispIPAddress + "/api"; 
  String requestData = "ping=1";
  http.begin(serverPath);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int response = http.POST(requestData);
  if (response > 0) {
    if (http.getString() = "OK") {
      retVal = true;
    }
  }
  http.end();
  return retVal;
}

bool exportToDisplay() {
  //This exports "shared" config info to the display's controller via API command
  //Display will receive this data, write to its own config and reboot
  bool retVal = false;
  if (dispConnected) {
    JsonDocument doc;
    String jsonPayload;
    String serverPath = "http://" + dispIPAddress + "/api?config=1";
    //MQTT
    doc["mqtt_addr_1"] = mqttAddr_1;
    doc["mqtt_addr_2"] = mqttAddr_2;
    doc["mqtt_addr_3"] = mqttAddr_3;
    doc["mqtt_addr_4"] = mqttAddr_4;
    doc["mqtt_port"] = mqttPort;
    doc["mqtt_tele_period"] = mqttTelePeriod;
    doc["mqtt_user"] = mqttUser;
    doc["mqtt_pw"] = mqttPW;
    doc["mqtt_topic_sub"] = mqttTopicSub;
    doc["mqtt_topic_pub"] = mqttTopicPub;
    //Weather/Temp
    doc["temp_units"] = defaultTempUnits;   //12=°C, 13=°F
    doc["weather_source"] = weatherSource;  //0 none (disabled), 1 OWM, 2 MQTT, 3 API
    doc["owm_key"] = owmKey;
    doc["own_lat"] = owmLat;
    doc["owm_long"] = owmLong;
    doc["temp_update_period"] = tempUpdatePeriod;
    serializeJson(doc, jsonPayload);
    http.begin(serverPath);
    http.addHeader("Content-Type", "application/json");
    int response = http.POST(jsonPayload);
    if (response > 0) {
      retVal = true;
    } 
    http.end();
  }
  return retVal;
}

bool reconnectDisplay(String dispAddr) {
  bool retVal = false;
  IPAddress dispIP;
  if (dispIP.fromString(dispAddr)) {
    byte dispIPAddr_1 = dispIP[0];
    byte dispIPAddr_2 = dispIP[1];
    byte dispIPAddr_3 = dispIP[2];
    byte dispIPAddr_4 = dispIP[3];

    if ((dispIPAddr_1 == 0) && (dispIPAddr_2 == 0) && (dispIPAddr_3 == 0) && (dispIPAddr_4 == 0)) {
      dispIPAddress = "0.0.0.0";
      dispEnabled = false;
      dispConnected = false;
    } else {
      dispIPAddress = String(dispIPAddr_1) + "." + String(dispIPAddr_2) + "." + String(dispIPAddr_3) + "." + String(dispIPAddr_4);
      dispEnabled = true;
      dispConnected = true;
      retVal = true;
    }
  }
  return retVal;
}
//---------------------------
// Other outbound API calls
//---------------------------
bool sendBulbState() {
  //sends state of Bulb to display controller. Called by toggleBulbState()
  bool retVal = false;
  String requestData = "";
  if (dispConnected) {
    String serverPath = "http://" + dispIPAddress + "/api"; // + "/api?ping=1";
    if (activeBulbState) {
      requestData = "bulbstate=1";
    } else {
      requestData = "bulbstate=0";
    }
    WiFiClient client;
    HTTPClient http;
    http.setTimeout(500);
    if (http.begin(client, serverPath)) {
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int response = http.POST(requestData);
      if (response > 0) {
        if (http.getString() = "OK") {
          retVal = true;
        }
      }
      http.end();
    }
  }
  return retVal;
}

bool sendLEDState() {
  //sends state of the LED strip to display controller. Called by toggleLEDState()
  bool retVal = false;
  String requestData = "";
  if (dispConnected) {
    String serverPath = "http://" + dispIPAddress + "/api"; // + "/api?ping=1";
    if (activeLEDState) {
      requestData = "ledstate=1";
    } else {
      requestData = "ledstate=0";
    }
    WiFiClient client;
    HTTPClient http;
    http.setTimeout(500);
    if (http.begin(serverPath)) {
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int response = http.POST(requestData);
      if (response > 0) {
        if (http.getString() = "OK") {
          retVal = true;
        }
      }
      http.end();
    }
  }
  return retVal;
}
// ============================
//  MQTT Update (pub) Functions
// ============================
bool mqttIsConnected() {
  bool retVal = false;
  if (client.connected()) {
    retVal = true;
  } else {
    //attempt a soft reconnect
    retVal = reconnect_soft();
  }
  return retVal;
}

void updateMQTT(String whichField) {
  //Sets flags for updating MQTT from main loop() - avoids thread-safe violations
  if (isBooting) return; // Shield during setup()

  if (whichField == "ledall") {
    mqttUpdateMask |= MSK_LEDSTATE;
    mqttUpdateMask |= MSK_LEDCOLOR;
    mqttUpdateMask |= MSK_LEDBRIGHT;
  } else if (whichField == "bulball") {
    mqttUpdateMask |= MSK_BULBSTATE;
    mqttUpdateMask |= MSK_BULBMODE;
    mqttUpdateMask |= MSK_BULBCOLOR;
    mqttUpdateMask |= MSK_BULBTEMP;
    mqttUpdateMask |= MSK_BULBBRIGHT;
  } else if (whichField == "touchall") {
    mqttUpdateMask |= MSK_TOUCH1_STATE;
    mqttUpdateMask |= MSK_TOUCH1_FUNC;
    mqttUpdateMask |= MSK_TOUCH1_FUNCA;
    mqttUpdateMask |= MSK_TOUCH2_STATE;
    mqttUpdateMask |= MSK_TOUCH2_FUNC;
    mqttUpdateMask |= MSK_TOUCH2_FUNCA;
  }
  else if (whichField == "bulbstate")      mqttUpdateMask |= MSK_BULBSTATE;
  else if (whichField == "bulbmode")       mqttUpdateMask |= MSK_BULBMODE;
  else if (whichField == "bulbcolor")      mqttUpdateMask |= MSK_BULBCOLOR;
  else if (whichField == "bulbtemp")       mqttUpdateMask |= MSK_BULBTEMP;
  else if (whichField == "bulbbrightness") mqttUpdateMask |= MSK_BULBBRIGHT;
  else if (whichField == "ledstate")       mqttUpdateMask |= MSK_LEDSTATE;
  else if (whichField == "ledcolor")       mqttUpdateMask |= MSK_LEDCOLOR;
  else if (whichField == "ledbrightness")  mqttUpdateMask |= MSK_LEDBRIGHT;
  else if (whichField == "touch1state")    mqttUpdateMask |= MSK_TOUCH1_STATE;
  else if (whichField == "touch1func")     mqttUpdateMask |= MSK_TOUCH1_FUNC;
  else if (whichField == "touch1funca")    mqttUpdateMask |= MSK_TOUCH1_FUNCA;
  else if (whichField == "touch2state")    mqttUpdateMask |= MSK_TOUCH2_STATE;
  else if (whichField == "touch2func")     mqttUpdateMask |= MSK_TOUCH2_FUNC;
  else if (whichField == "touch2funca")    mqttUpdateMask |= MSK_TOUCH2_FUNCA;
}

void handleQueuedMqttUpdates() {
  //Processes queue, with a delay between calls to prvent flooding wifi buffer 
  if (mqttUpdateMask == 0) return;
  
  //Priority 1 - touch sensors
  if (mqttUpdateMask & MSK_TOUCH1_STATE) { executeMqttUpdate("touch1state"); mqttUpdateMask &= ~MSK_TOUCH1_STATE; }
  else if (mqttUpdateMask & MSK_TOUCH2_STATE) { executeMqttUpdate("touch2state"); mqttUpdateMask &= ~MSK_TOUCH2_STATE; }
  //Priority 2 - state changes
  else if (mqttUpdateMask & MSK_BULBSTATE) { executeMqttUpdate("bulbstate"); mqttUpdateMask &= ~MSK_BULBSTATE; }
  else if (mqttUpdateMask & MSK_LEDSTATE) { executeMqttUpdate("ledstate"); mqttUpdateMask &= ~MSK_LEDSTATE; }
  //Priority 3 - color and brightness
  else if (mqttUpdateMask & MSK_LEDBRIGHT) { executeMqttUpdate("ledbrightness"); mqttUpdateMask &= ~MSK_LEDBRIGHT; }
  else if (mqttUpdateMask & MSK_BULBBRIGHT) { executeMqttUpdate("bulbbrightness"); mqttUpdateMask &= ~MSK_BULBBRIGHT; }
  else if (mqttUpdateMask & MSK_BULBCOLOR) { executeMqttUpdate("bulbcolor"); mqttUpdateMask &= ~MSK_BULBCOLOR; }
  else if (mqttUpdateMask & MSK_LEDCOLOR) { executeMqttUpdate("ledcolor"); mqttUpdateMask &= ~MSK_LEDCOLOR; }
  else if (mqttUpdateMask & MSK_BULBMODE) { executeMqttUpdate("bulbmode"); mqttUpdateMask &= ~MSK_BULBMODE; }
  else if (mqttUpdateMask & MSK_BULBTEMP) { executeMqttUpdate("bulbtemp"); mqttUpdateMask &= ~MSK_BULBTEMP; }
  //Priority 4 - Secondary settings 
  else if (mqttUpdateMask & MSK_TOUCH1_FUNC) { executeMqttUpdate("touch1func"); mqttUpdateMask &= ~MSK_TOUCH1_FUNC; }
  else if (mqttUpdateMask & MSK_TOUCH1_FUNCA) { executeMqttUpdate("touch1funca"); mqttUpdateMask &= ~MSK_TOUCH1_FUNCA; }
  else if (mqttUpdateMask & MSK_TOUCH2_FUNC) { executeMqttUpdate("touch2func"); mqttUpdateMask &= ~MSK_TOUCH2_FUNC; }
  else if (mqttUpdateMask & MSK_TOUCH2_FUNCA) { executeMqttUpdate("touch2funca"); mqttUpdateMask &= ~MSK_TOUCH2_FUNCA; }
  else {
    mqttUpdateMask = 0;
  }
}

void executeMqttUpdate(String whichField) {
  //Actually published message
  if (!mqttConnected) return;
  //Test connection and attempt reconnect
  if (mqttIsConnected()) {
    char topicBuf[128];   // Buffer for the full topic string
    char payloadBuf[32]; // Buffer for the converted numbers/bools
    // -- Main bulb --
    if (whichField == "bulbstate") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/bulbstate", mqttTopicPub.c_str());
      client.publish(topicBuf, activeBulbState ? "ON":"OFF", true); 
      return;
    }
    if (whichField == "bulbmode") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/bulbmode", mqttTopicPub.c_str());
      client.publish(topicBuf, (activeBulbColorMode == 0) ? "rgb":"color_temp", true); 
      return;
    }
    if (whichField == "bulbcolor") {
      byte r, g, b;
      hexToRgb(activeBulbColor, r, g, b);
      String mqttColor = String(r) + "," + String(g) + "," + String(b);
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/bulbcolor", mqttTopicPub.c_str());
      client.publish(topicBuf, mqttColor.c_str(), true);
      return;  
    }
    if (whichField == "bulbtemp") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/bulbtemp", mqttTopicPub.c_str());
      snprintf(payloadBuf, sizeof(payloadBuf), "%d", activeBulbTemp);
      client.publish(topicBuf, payloadBuf, true);
      return;
    }
    if (whichField == "bulbbrightness") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/bulbbrightness", mqttTopicPub.c_str());
      snprintf(payloadBuf, sizeof(payloadBuf), "%d", activeBulbBrightness);
      client.publish(topicBuf, payloadBuf, true);
      return;
    }
    // -- LED SECTION--
    if (whichField == "ledstate") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/ledstate", mqttTopicPub.c_str());
      client.publish(topicBuf, (activeLEDState == 1 ? "ON" : "OFF"), true);      
      return;
    }
    if (whichField == "ledcolor") {
      byte r, g, b;
      hexToRgb(activeLEDColor, r, g, b);
      String mqttColor = String(r) + "," + String(g) + "," + String(b);
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/ledcolor",mqttTopicPub.c_str());
      client.publish(topicBuf, mqttColor.c_str(), true);
      return;
    }
    if (whichField == "ledbrightness") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/ledbrightness", mqttTopicPub.c_str());
      snprintf(payloadBuf, sizeof(payloadBuf), "%d", activeLEDBrightness);  
      client.publish(topicBuf, payloadBuf, true);
      return;
    }
    // -- Touch sensors --
    if (whichField == "touch1state") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/touch1state", mqttTopicPub.c_str());
      if (touch1MqttLatched) {
        if (touch1Enabled) {
          client.publish(topicBuf, "ON", true);
        }
        touch1MqttLatched = false;
        updateMQTT("touch1state");
      } else {
        client.publish(topicBuf, "OFF", true);
      }
      return;
    }
    if (whichField == "touch1func") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/touch1func", mqttTopicPub.c_str());
      if (touch1Enabled) {client.publish(topicBuf, getIdleFuncStr(touch1Control1), true);}
      else {client.publish(topicBuf, "Disabled", true);}
      return;
    }
    if (whichField == "touch1funca") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/touch1funca", mqttTopicPub.c_str());
      if (touch1Enabled) {client.publish(topicBuf, getAlarmFuncStr(touch1Control2), true);}
      else {client.publish(topicBuf, "Disabled", true);}
      return;
    }
    if (whichField == "touch2state") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/touch2state", mqttTopicPub.c_str());
      if (touch2MqttLatched) {
        if (touch2Enabled) {
          client.publish(topicBuf, touch2State ? "ON":"OFF", true);
        }
        touch2MqttLatched = false;
        updateMQTT("touch2state");
      } else {
        client.publish(topicBuf, "OFF", true);
      }
      return;
    }
    if (whichField == "touch2func") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/touch2func", mqttTopicPub.c_str());
      if (touch2Enabled) {client.publish(topicBuf, getIdleFuncStr(touch2Control1), true);}
      else {client.publish(topicBuf, "Disabled", true);}
      return;
    }
    if (whichField == "touch2funca") {
      snprintf(topicBuf, sizeof(topicBuf), "stat/%s/touch2funca", mqttTopicPub.c_str());
      if (touch2Enabled) {client.publish(topicBuf, getAlarmFuncStr(touch2Control2), true);}
      else {client.publish(topicBuf, "Disabled", true);}
      return;
    }
  }
}

void handleInitMqttSync() {
  switch (syncStep) {
    case 0: executeMqttUpdate("bulbstate"); break;
    case 1: executeMqttUpdate("bulbmode"); break;
    case 2: executeMqttUpdate("bulbcolor"); break;
    case 3: executeMqttUpdate("bulbtemp"); break;
    case 4: executeMqttUpdate("bulbbrightness"); break;
    case 5: executeMqttUpdate("ledstate"); break;
    case 6: executeMqttUpdate("ledcolor"); break;
    case 7: executeMqttUpdate("ledbrightness"); break;
    case 8: executeMqttUpdate("touch1state"); break; // The big JSON payload
    case 9: executeMqttUpdate("touch1func"); break;
    case 10: executeMqttUpdate("touch1funca"); break;
    case 11: executeMqttUpdate("touch2state"); break;
    case 12: executeMqttUpdate("touch2func"); break;
    case 13: executeMqttUpdate("touch2funca"); break;
    case 14: 
      initialSyncRequired = false; // We are DONE!
      syncStep = -1;
      break;
  }
  syncStep++;  
}

// ============================
//  Touch Function Conversions
// ============================
const char* getIdleFuncStr(int func) {
  switch (func) {
    case 1: return "Toggle Bulb";
    case 2: return "Toggle LEDs";
    case 3: return "Bulb Brighter";
    case 4: return "Bulb Dimmer";
    case 5: return "LEDs Brighter";
    case 6: return "LEDs Dimmer";
    case 7: return "Display Brighter";
    case 8: return "Display Dimmer";
    default: return "None";
  }
}

const char* getAlarmFuncStr(int func) {
  switch (func) {
    case 1: return "Snooze";
    case 2: return "Stop";
    default: return "None";
  }
}

int getIdleFuncInt(char* payload) {
  if (strcmp(payload, "Toggle Bulb") == 0)      return 1;
  if (strcmp(payload, "Toggle LEDs") == 0)      return 2;
  if (strcmp(payload, "Bulb Brighter") == 0)    return 3;
  if (strcmp(payload, "Bulb Dimmer") == 0)      return 4;
  if (strcmp(payload, "LEDs Brighter") == 0)    return 5;
  if (strcmp(payload, "LEDs Dimmer") == 0)      return 6;
  if (strcmp(payload, "Display Brighter") == 0) return 7;
  if (strcmp(payload, "Display Dimmer") == 0)   return 8;
  return 0; // Default to None
}

int getAlarmFuncInt(char* payload) {
  if (strcmp(payload, "Snooze") == 0)      return 1;
  if (strcmp(payload, "Toggle LEDs") == 0) return 2;
  return 0;  //Default to None
}
// ===========================
//  WEATHER/TEMP Functions
// ===========================
bool owmIsConnected() {
  bool retVal = false;
  if (weatherSource == 1) {
    if ((owmKey == "NA") || (owmKey == "")) {
      retVal = false;
    } else {
      String jsonBuffer;
      String serverPath = "https://api.openweathermap.org/data/3.0/onecall?lat=" + owmLat + "&lon=" + owmLong + "&exclude=minutely,hourly,daily,alerts&appid=" + owmKey;
      http.useHTTP10(true);
      http.setTimeout(2000);
      http.begin(serverPath);
      int httpCode = http.GET();
      if (httpCode > 0) {
        retVal = true;
      }
      http.end();
    }
  }
  return retVal;
}
// ===========================
//  Color Conversions
// ===========================
bool isValidHex(String hex) {
  if (hex.startsWith("#")) hex = hex.substring(1);
  if (hex.length() != 6) return false;

  for (int i = 0; i < 6; i++) {
    char c = toupper(hex[i]);
    if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) {
      return false; // Found a non-hex character like 'Q' or 'W'
    }
  }
  return true;
}

CRGB hexToCRGB(String hexString) {
  // Takes hex value as string (ex. "#ff0000") and converts to CRGB color
  // Check if the string starts with '#' and remove it
  if (hexString.startsWith("#")) {
      hexString = hexString.substring(1);
  }

  // Convert the hex string to a long integer (base 16)
  unsigned long colorValue = strtol(hexString.c_str(), NULL, 16);

  // Create CRGB from a 32-bit integer (which is essentially the hex value)
  return CRGB(colorValue);
}

CRGB rgbToCRGB(byte red, byte green, byte blue) {
  //Accepts r, g and b values (0-255) and returns a FastLED CRGB color value
  return CRGB(red, green, blue);
}

String rgbToHex(byte red, byte green, byte blue) {
  //Accepts r, g and b values (0-255) and returns hex color code
  //e.g 255,0,0 will return #ff0000
  char hexColor[7];
  sprintf(hexColor, "%02x%02x%02x", red, green, blue);
  return "#" + String(hexColor);
}

void hexToRgb(String hexString, byte &r, byte &g, byte &b) {
  //Accepts hex color string (#ff00ff) and returns individual R, G and B values
  if (hexString.startsWith("#")) {
    hexString = hexString.substring(1);
  }
  char charBuf[7];
  hexString.toCharArray(charBuf, sizeof(charBuf));
  long hexValue = strtol(charBuf, NULL, 16);
  r = (hexValue >> 16) & 0xFF;
  g = (hexValue >> 8) & 0xFF;
  b = hexValue & 0xFF;
}
