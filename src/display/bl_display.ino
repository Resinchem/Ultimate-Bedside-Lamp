/* =================================================================
 * Bedside Lamp: CYD Touch Display
 * April, 2026
 * Version 0.52
 * Copyright ResinChemTech - released under the Apache 2.0 license
 * ================================================================= */

#include <WiFi.h>                 //Core WiFi services
#include <WiFiClient.h>           //Arduino ESP Core - creates a client that can connect to an IP address
#include <ESPping.h>              //v1.0.5 - ping remote IP to test connectivity (e.g bulb, display, etc.)
#include <ESPmDNS.h>              //mDNS service for Arduino OTA updates
#include <WebServer.h>            //Local web server (for settings pages)
#include <LittleFS.h>             //Read and write to flash storage
#include <Wire.h>                 //I2C
#include <SPI.h>                  //SPI
#include <SoftwareSerial.h>       //For DFPlayer Serial 
#include "DFRobotDFPlayerMini.h"  //MP3 Player: https://github.com/DFRobot/DFRobotDFPlayerMini (v.1.0.6)
#include "time.h"                 //Time functions
#include <HTTPClient.h>           //For sending HTTP requests
#include <WiFiClientSecure.h>     //for sending HTTPS requests
#include "esp_sntp.h"             //SNTP functions
#include <ESP32Time.h>            //For setting and getting RTC time from ESP32: https://github.com/fbiego/ESP32Time (v2.0.6)
#include <ArduinoJson.h>          //Reading and writing JSON files (e.g. config file)
#include <WiFiUdp.h>              //WiFi UDP functions
#include <ArduinoOTA.h>           //OTA Updates via Arduino IDE
#include <Update.h>               //OTA Updates via web page
#include <PubSubClient.h>         //https://github.com/knolleary/pubsubclient  Provides MQTT functions (v2.8)
#include "initGT911.h"            //Touch library https://github.com/milad-nikpendar/initGT911 (v1.0.1 - manually installed via .zip to fix rotation issue)
#include <TFT_eSPI.h>             //Main Display Library - See TFT_eSPI User_Setup_Select.h for using a different "CYD" version: https://github.com/Bodmer/TFT_eSPI
#include <TFT_eWidget.h>          //Additional features and widgets: https://github.com/Bodmer/TFT_eWidget (v0.0.5)
#include <ESP_Knob.h>             //Rotary Encoder (EC11) Library: https://github.com/esp-arduino-libs/ESP32_Knob (v0.0.1)
#include "html.h"                 //html code for the web settings pages
#include "icons20pt7b.h"          //Custom icon font for top of display
 
#define VERSION "v0.52"
#define APPNAME "LAMP DISPLAY"
#define WIFIMODE 2                // 0 = Only Soft Access Point, 1 = Only connect to local WiFi network with UN/PW, 2 = Both (both needed for onboarding)
#define SERIAL_DEBUG 0            // 0 = Disable (must be disabled if using RX/TX pins), 1 = enable

#define FORMAT_LITTLEFS_IF_FAILED true  // DO NOT CHANGE
#define GFXFF 1
// ==========================================================================
//  *** REVIEW AND UPDATE THESE VARIABLES TO MATCH YOUR ENVIRONMENT/BUILD ***
// ==========================================================================
//Pin Definitions - Update if your build is different
// Display & Touch pins and frequency
#define DISP_I2C_SDA 33
#define DISP_I2C_SCL 32
#define DISP_I2C_FREQ 400000
#define DISP_RST_PIN  25       // Display reset pin
#define DISP_INT_PIN -1        // Display interrupt - not used in polling mode (-1)
#define DISP_BL_PIN 27         // LED Backlight
#define DISP_LDR 35            // Displays built-in LDR
//Onboard LED
#define RED_LED_PIN 4          
#define GREEN_LED_PIN 16
#define BLUE_LED_PIN 17

//DFPlayer Serial GPIOs
#define DF_RX 22  //blue (old 19)
#define DF_TX 21  //yellow (old 23)

//Onboard LED freq and resoultion
#define LED_FREQ 5000
#define LED_RES 8                        //8 bit resolution (e.g. 0-255)
//Display Dimensions & Touch Address
//Only change if your display is different (you may also need to edit the TFT_eSPI setup and recompile)
#define DISP_H 480                       //Init horizontal resolution (in pixels) - can be changed via app after onboarding
#define DISP_V 320                       //Init vertical resoultion (in pixels) - can be changed via app after onboarding 
// GT911 I2C touch address
#define TOUCH_ADDR GT911_I2C_ADDR_BA     //Decimal 93 (Alternate addr: GT911_I2C_ADDR_28 or Decimal 20)
//Alarms & Sounds
#define MAX_ALARMS 5
#define MAX_SOUNDS 20
#define dfSerial Serial1
//MQTT Handling - needed to handle in main loop and to avoid thread conflicts
#define MSK_BRIGHTNESS   0x0001
#define MSK_AUTODIM      0x0002
#define MSK_COLOR        0x0004
#define MSK_GENTLE       0x0008
#define MSK_TRACK        0x0010
#define MSK_VOLUME       0x0020
#define MSK_SNOOZE       0x0040
#define MSK_ALARMS       0x0080
#define MSK_TEMP         0x0100
#define MSK_TIMESYNC     0x0200
// -------------------------------------------------------
//  The following values are default/starting values.
//  Most can be changed via the app after onboarding and
//  **do not** need to be modified directly here.
// -------------------------------------------------------
#define DISP_ROTATE 3                          //Init display rotation (in degrees 0: 0°, 1: 90°, 2: 180, 3: 270) - can be changed via app after onboarding
#define TIMEZONE "EST+5EDT,M3.2.0/2,M11.1.0/2" // Set your custom time zone from this list: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

/* ================ Default Starting Values ====================================
 * All of these following values can be modified and saved via
 * the web interface and the values below normally do not need to be modified.
 * Using the web settings is the preferred way to change these default values.  
 * Values listed here are just starting values before the config file exists or
 * if it can't be read.
 ===============================================================================*/
//----------------------------------
// Local Variables (wifi/onboarding)
//----------------------------------
String deviceName = "BL_Display01";  //Default Device Name - 16 chars max, no spaces.
String wifiHostName = deviceName;
String wifiSSID = "";
String wifiPW = "";
byte macAddr[6];                     //Array of device mac address as hex bytes (reversed)
String strMacAddr;                   //Formatted string of device mac address
String baseIPAddress;                //Device assigned IP Address
bool onboarding = false;             //Will be set to true if no config file or wifi cannot be joined

//----------------------------------
//Local Variables (Display/Touch)
//----------------------------------
uint16_t dispHeight = DISP_H;            //Local var for display horizontal resolution
uint16_t dispWidth = DISP_V;             //Local var for display vertical resolution
byte defaultDispRotate = DISP_ROTATE;    //Local var for setting/changing display rotation (0: 0°, 1: 90°, 2: 180° or 3: 270°)
byte activeDispRotate = DISP_ROTATE;
byte defaultBrightness = 128;            //Starting local brightness (will be overwritten by config file)
byte activeBrightness = 128;
byte holdBrightness = 128;
byte holdEditBrightness = 0;
bool overrideBrightness = false;         //Used on touch to temporarily increase brightness if less than default
uint32_t overrideTimer = 0;              //Timer for brightness override (will be 5 seconds by default)
bool useLED = false;                     //Use onboard LED
byte activePage = 0;                     //Tracks currently displayed page on display (0 = default clock page)
byte lastPage = 99;                      //For clearing/refreshing page when page changes
byte alarmEditNum = 0;                   //Alarm slot num being edited via touch screen (0 based array)
bool alarmEdited = false;                //Alarm has been edited via touch panel (but not yet saved)

//Touch
String touchAddr = "GT911_I2C_ADDR_BA";  //Touch address (either GT911_I2C_ADDR_BA or GT911_I2C_ADDR_28)
bool touchEnabled = false;               //Enable touch screen functions
bool touchEnabledTest = false;           //Temporarily enable touch for testing
unsigned long lastTouchScan = 0;         //For the 100ms polling interval
unsigned long touchCooldownTime = 0;     //To ignore touches briefly after state changes
unsigned long lastTouchTime = 0;         //Tracks last touch for auto-closing settings (no touch for >10 seconds)
//Global timing variables for touch scrolling
uint32_t lastScrollMillis = 0;
uint16_t scrollDelay = 400;

//Auto-Dimming
bool autoDim = false;                    //Enable or disable auto-dimming
bool autoDimActive = false;              //For tracking temp auto-dim changes
byte autoDimDebounce = 1;                //Auto dim debounce cycles (1 cycle ~ 1 sec). 0 min 10 max
byte ambLightLevel_1 = 0;
byte ambLightLevel_2 = 0;
byte ambLightLevel_3 = 0;
byte ambLightLevel_4 = 0;
byte dimBrightness_1 = 25;
byte dimBrightness_2 = 75;
byte dimBrightness_3 = 125;
byte dimBrightness_4 = 175;
byte dimBrightness_5 = 225;
uint32_t dimLastCheckMillis = 0;
byte activeThresholdMin = 0;
byte activeThresholdMax = 0;
byte crossedThresholdCount = 0;
//Testing vars (Holding values)
bool displayTestFlag = false;
byte displayTestRotate = 0;
byte displayTestBrightness = 0;
bool displayTestTouch = false;
bool holdAutoDim = false;
String whichData = "all";

//Display Settings Page
bool bulbState = false;
bool ledState = false;
bool lightStateChange = false;
bool dimStateChange = false;
bool brightChange = false;
//----------------------------------
//Local Variables (MQTT/Discovery)
//----------------------------------
//  MQTT will only be used if a server address other than '0.0.0.0' is entered via portal
byte mqttAddr_1 = 0;
byte mqttAddr_2 = 0;
byte mqttAddr_3 = 0;
byte mqttAddr_4 = 0;
int mqttPort = 0;
String mqttIPAddress = "";
String mqttClient = "bldisplay";
String mqttUser = "myusername";
String mqttPW = "mypassword";
uint16_t mqttTelePeriod = 60;          //seconds - how often MQTT is updated (outside of state changes)
uint32_t lastMqttRetryAttempt = 0;     //Non-blocking handler for MQTT reconnection
const int MQTT_RETRY_DELAY = 5000;     //Delay (ms) between reconnect attempts
String mqttTopicSub ="bldisplay";      //Can be changed via web app (preferred method)
String mqttTopicPub = "bldisplay";     //Can be changed via web app 
String mqttClockColor = "#ffffff";     //Color intent to handle 24-bit to 16-bit conversion for tft display

bool mqttEnabled = false;              //Will be enabled/disabled depending on whether a valid IP address is defined in Settings (0.0.0.0 disables MQTT)
bool mqttConnected = false;            //Will be enabled if defined and successful connnection made.  This var should be checked upon any MQTT action.
bool forceMQTTUpdate = false;          //Force MQTT update on state change

char globalMqttBuffer[1024];           //Buffer for publishing alarm data to MQTT
bool initialSyncRequired = false;      //Flag for initial publish of MQTT data after boot
int syncStep = 0;
bool rebootRequired = false;
unsigned long lastSyncStepTime = 0;
volatile uint32_t mqttUpdateMask = 0;  //Volatile so it can be updated by out-of-thread background tasks
const int INIT_SYNC_INTERVAL = 200;    //200ms between publishing
//----------------------------------
//Local Variables (Weather/Temp) 
//----------------------------------
byte defaultTempUnits = 13;            // Show temp in Celcius (12) or Fahrenheit (13)
byte activeTempUnits = 13;
byte weatherSource = 0;                // 0 None, 1 OpenWeatherMap (OWM), 2 MQTT, 3 API
String owmKey = "";                    // Open Weather Map API Key (https://openweathermap.org/api) 
String owmLat = "39.8083";             // Lat/Long defaults point to Lebanon, KS (geographic center of US)
String owmLong = "-98.555";
bool owmConnected = false;             // Boolean indicating if initial connection to OWM is valid (only used if weatherSource is set to OWM)
uint16_t tempUpdatePeriod = 15;        // How often in minutes to request temperature from OWM (minimum 15 min. to limit API calls)
uint32_t lastTempUpdate = 0;
int externalTemperature = 0;

//----------------------------------
// Local Variables (OTA Updates)
//----------------------------------
//OTA Variables
String otaHostName = deviceName + "_OTA";  // Will be updated by device name from onboarding + _OTA
bool ota_flag = true;                      // Must leave this as true for board to broadcast port to IDE upon boot
uint16_t ota_boot_time_window = 2500;      // minimum time on boot for IP address to show in IDE ports, in millisecs
uint16_t ota_time_window = 20000;          // time to start file upload when ota_flag set to true (after initial boot), in millsecs
uint16_t ota_time_elapsed = 0;             // Counter when OTA active
uint16_t ota_time = ota_boot_time_window;
uint8_t web_otaDone = 0;                   // Web OTA Update

//----------------------------------
//Local Variables (Time/Clock)
//----------------------------------
byte defaultClockStyle = 0;               // Time Display Style
byte activeClockStyle = 0;
byte defaultClockFont = 8;                // Time Font
byte activeClockFont = 8;
byte defaultClockSize = 1;                // Time Display Size
byte activeClockSize = 1;
byte holdClockStyle = 0;                  //Holding vars for clock testing
byte holdClockFont = 8;
byte holdClockSize = 1;
byte holdHourFormat = 12;
uint16_t holdClockColor = 0xFFFF;
uint16_t defaultClockColor = 0xFFFF;      // White
uint16_t activeClockColor = 0xFFFF;
byte defaultHourFormat = 12;              // Show time in 12 or 24 hour (military) format
byte activeHourFormat = 12; 
byte timeSource = 0;                      // 0 None, 1 NTP, 2 MQTT, 3 API
byte activeTimeSource = 0;                // Var for switching to manual
String ntpServer = "pool.ntp.org";        // Default server for syncing time (when using NTP as source)
String timeZone = TIMEZONE;
uint16_t ntpSyncInterval = 60;            // How often to sync to NTP server - in minutes (15 min - 1440 max)
String mqttTimeTopic = "cmnd/time";       // Topic to receive time updates
bool mqttLiveTime = false;                // Whether to use MQTT for live time (false means sync only)
bool apiLiveTime = false;                 // Whether to use API for live time (false means sync only)
unsigned long prevTime = 0;               // For updating time each minute
unsigned long lastSyncTime = 0;           // Last time a sync occurred (NTP source only)
int curMinutes = 0;
bool initSync = true;
bool lastTimeSyncOK = false;              // Holds result of last time sync
int liveLastYear = 2026;                  //These are needed for "live" (MQTT or API) date/time updates
int liveLastMonth = 1;
int liveLastDay = 1;
int liveLastHour = 0;
int liveLastMinute = 1;
int liveLastSecond = 0;
bool timeReceivedAtBoot = false;
struct TimeStyles {
  byte indexNum;
  String name;
  byte fontNum;
  byte fontSize;
};
TimeStyles clockStyles[4] = {
  {0, "Modern (Small)", 8, 1},
  {1, "Modern (Large)", 6, 3},
  {2, "Seven-Segment (Small)", 7, 2},
  {3, "Seven-Segment (Large)", 7, 3}
};

//----------------------------------
//Local Variables (Alarms & Sounds)
//----------------------------------
//Alarms
const char* ALARM_FILE_PATH = "/alarms.bin";
bool useSDCard = false;                // Use SD card for alarm sound. If false, no alarms sounds.
bool isPlayerOnline = false;
byte defaultSDAlarmTrack = 0;          // Which track to use for alarm sound
byte activeSDAlarmTrack = 0;
byte defaultAlarmVol = 10;             // 0-30 (0=mute, 30=max volume)
byte activeAlarmVol = 10;
byte holdAlarmVol = 10;                // Holder for sound testing
bool useGentleWake = false;            // Gradually increase volume from 1 to 'alarmVolume'
bool holdGentleWake = false;           // Holder for sound testing
byte gentleWakeActiveVol = 0;
byte gentleWakeIncrement = 0;          // activeVolume / 6 ... will increase over 30 seconds
byte snoozeTime = 0;                   // Snooze time (in minutes). 0 disables snooze.
bool activeAlarms = false;             // Whether any alarms are 'active' 
bool activeAlarmChange = false;        // Flag for updating display icon after active state change
bool alarmSounding = false;            // True when alarm is going off
bool alarmButtonsOn = false;
byte alarmBrightHold = 0;              // Variable for holding brightness prior to alarm
bool isPM = false;                     // Used with editing alarms via touch panel
byte editRepeatDraft = 0;              // Used with editing alarms via touch panel
unsigned long lastGentleTime = 0;      // For incrementing gentle wake volume increase
bool manualAlarmPlay = false;          // For MQTT/API manual alarm sounding
unsigned long manualAlarmStartTime = 0;  
unsigned long manualAlarmDuration = 0;
//Need "Alarm" data structure, array and/or class
typedef struct __attribute__((packed)) {
  byte active;                         // 1 byte (0 = inactive, 1 = active)
  char date[11];                       // 11 bytes (e.g., "YYYY-MM-DD\0") - Used only for non-repeating alarms
  char time[9];                        // 9 bytes (e.g., "HH:MM:SS\0")
  byte repeat;                         // 1 byte (0=None, 1=Sun, 2=Mon, ..., 8=Weekday, 9=Weekend)
} Alarm;
Alarm alarmArray[MAX_ALARMS];
Alarm stagingAlarm;                    // For editing via touch panel

//Individual month, day, year for edting alarms via touch panel
struct DateDraft {
  int year;
  int month;
  int day;
};
DateDraft editDateDraft;
//Parallel Array Time Cache (Optimization for Speed)
struct AlarmTimeElements {
  uint8_t h;
  uint8_t m;
  uint8_t s;
};
AlarmTimeElements alarmCache[MAX_ALARMS];
AlarmTimeElements editTimeDraft;

enum AlarmState {
  ALARM_IDLE,       // Default state (Clock running normally)
  ALARM_TRIGGERED,  // Time match found, ready to start audio
  ALARM_RINGING,    // Audio player is active, listening for buttons
  ALARM_SNOOZING    // Audio stopped, waiting for snooze timer to expire
};
AlarmState currentAlarmState = ALARM_IDLE;
int currentAlarmIndex = -1;       // Index of the alarm currently ringing
unsigned long lastAlarmCheck = 0; // Tracks the last time we checked the array (for 1-second interval)
unsigned long snoozeEndTime = 0;  // Stores the millis() time when the snooze period ends

//Sounds - sounds.json must be defined and saved in SPIFFS (LittleFS)
const char* SOUND_FILE_PATH = "/sounds.json";
//Type and Array of possible alarm sounds
typedef struct {
  byte index;       //1-20, order on SD card
  String filename;  //e.g. '0001.mp3'
  String title;     //e.g. 'Piano Sonata'
} Sound;
Sound soundLibrary[MAX_SOUNDS];
int currentSoundCount = 0;

//------------------------------------
//Local Variables (Settings page)
//------------------------------------
bool initSettingsSync = false;
unsigned long lastSettingsSync = 0;

//------------------------------------
//Local Variables (Primary Controller)
//------------------------------------
bool primConnected = false;
byte primIPAddr_1 = 0;
byte primIPAddr_2 = 0;
byte primIPAddr_3 = 0;
byte primIPAddr_4 = 0;
String primIPAddress = "0.0.0.0";
String primServerPath = "";           // Full URL for communicating with primary controller API

//----------------------------------
//Local Variables (Misc/Other)
//----------------------------------
bool isBooting = false;

/* ===============  RunTime Local Variables ====================
 * These are used for sketch functionality.  They should not 
 * be changed unless you have a very specific need and fully
 * understand what you are changing.  Any config/runtime
 * options can be changed either through the above #DEFINES or
 * thru the web application.
 ==============================================================*/

//---------------------------
// Instantiate objects
//---------------------------
WiFiClient espClient;
WebServer server(80);
PubSubClient client(espClient);
TFT_eSPI tft = TFT_eSPI(DISP_V, DISP_H);
initGT911 touch(&Wire, TOUCH_ADDR);   //Display touch
DFRobotDFPlayerMini sdPlayer;
//Define touch panel buttons
//Main Page (0)
ButtonWidget btnAlarmStop = ButtonWidget(&tft);
ButtonWidget btnAlarmSnooze = ButtonWidget(&tft);
ButtonWidget btnAlarmStopAlt = ButtonWidget(&tft);
ButtonWidget btnSettings = ButtonWidget(&tft);
//Settings Page (1)
ButtonWidget btnSettingsBack = ButtonWidget(&tft);
ButtonWidget btnSettingsBulbOff = ButtonWidget(&tft);
ButtonWidget btnSettingsBulbOn = ButtonWidget(&tft);
ButtonWidget btnSettingsLEDsOff = ButtonWidget(&tft);
ButtonWidget btnSettingsLEDsOn = ButtonWidget(&tft);
ButtonWidget btnSettingsAutoDimOn = ButtonWidget(&tft);
ButtonWidget btnSettingsAutoDimOff = ButtonWidget(&tft);
ButtonWidget btnSettingsAlarmSetup = ButtonWidget(&tft);
TFT_eSprite knob = TFT_eSprite(&tft);
SliderWidget sldBrightness(&tft, &knob);
slider_t param;
//Alarm List Page (2)
ButtonWidget btnAlarmListBack = ButtonWidget(&tft);
ButtonWidget btnAlarm1Active = ButtonWidget(&tft);
ButtonWidget btnAlarm1Inactive = ButtonWidget(&tft);
ButtonWidget btnAlarm1Edit = ButtonWidget(&tft);
ButtonWidget btnAlarm2Active = ButtonWidget(&tft);
ButtonWidget btnAlarm2Inactive = ButtonWidget(&tft);
ButtonWidget btnAlarm2Edit = ButtonWidget(&tft);
ButtonWidget btnAlarm3Active = ButtonWidget(&tft);
ButtonWidget btnAlarm3Inactive = ButtonWidget(&tft);
ButtonWidget btnAlarm3Edit = ButtonWidget(&tft);
ButtonWidget btnAlarm4Active = ButtonWidget(&tft);
ButtonWidget btnAlarm4Inactive = ButtonWidget(&tft);
ButtonWidget btnAlarm4Edit = ButtonWidget(&tft);
ButtonWidget btnAlarm5Active = ButtonWidget(&tft);
ButtonWidget btnAlarm5Inactive = ButtonWidget(&tft);
ButtonWidget btnAlarm5Edit = ButtonWidget(&tft);
ButtonWidget btnAlarmErrAckn = ButtonWidget(&tft);

ButtonWidget* activeBtns[] = {&btnAlarm1Active, &btnAlarm2Active, &btnAlarm3Active, &btnAlarm4Active, &btnAlarm5Active};
ButtonWidget* inactiveBtns[] = {&btnAlarm1Inactive, &btnAlarm2Inactive, &btnAlarm3Inactive, &btnAlarm4Inactive, &btnAlarm5Inactive};

//Alarm Edit Page (3)
ButtonWidget btnAlarmEditDate = ButtonWidget(&tft);
ButtonWidget btnAlarmEditTime = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeat = ButtonWidget(&tft);
ButtonWidget btnAlarmEditSave = ButtonWidget(&tft);
ButtonWidget btnAlarmEditCancel = ButtonWidget(&tft);
//Alarm Edit Date Page (4)
ButtonWidget btnAlarmEditMonth = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMonthPlus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMonthMinus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditDay = ButtonWidget(&tft);
ButtonWidget btnAlarmEditDayPlus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditDayMinus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditYear = ButtonWidget(&tft);
ButtonWidget btnAlarmEditYearPlus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditYearMinus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditDateTomorrow = ButtonWidget(&tft);
ButtonWidget btnAlarmEditDateUpdate = ButtonWidget(&tft);
ButtonWidget btnAlarmEditDateCancel = ButtonWidget(&tft);
//Alarm Edit Time Page (5)
ButtonWidget btnAlarmEditHour = ButtonWidget(&tft);
ButtonWidget btnAlarmEditHourPlus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditHourMinus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMinute = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMinutePlus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMinuteMinus = ButtonWidget(&tft);
ButtonWidget btnAlarmEditAM = ButtonWidget(&tft);
ButtonWidget btnAlarmEditPM = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMinute00 = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMinute15 = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMinute30 = ButtonWidget(&tft);
ButtonWidget btnAlarmEditMinute45 = ButtonWidget(&tft);
ButtonWidget btnAlarmEditTimeUpdate = ButtonWidget(&tft);
ButtonWidget btnAlarmEditTimeCancel = ButtonWidget(&tft);
//Alarm Edit Repeat Page (6)
ButtonWidget btnAlarmEditRepeatNone = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatMo = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatTu = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatWe = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatTh = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatFr = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatSa = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatSu = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatWD = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatWE = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatUpdate = ButtonWidget(&tft);
ButtonWidget btnAlarmEditRepeatCancel = ButtonWidget(&tft);
ButtonWidget* repeatBtns[] = {&btnAlarmEditRepeatNone, &btnAlarmEditRepeatSu, &btnAlarmEditRepeatMo, &btnAlarmEditRepeatTu, &btnAlarmEditRepeatWe,  
                              &btnAlarmEditRepeatTh, &btnAlarmEditRepeatFr, &btnAlarmEditRepeatSa, &btnAlarmEditRepeatWD, &btnAlarmEditRepeatWE};

//Initialize Objects
time_t now;
tm timeinfo;
WiFiUDP ntpUDP;
HTTPClient http;
ESP32Time rtc(0);  // offset handled via NTP time server

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
//MQTT
bool setup_mqtt();
void reconnect();
bool reconnect_soft();
bool mqttIsConnected();
void callback(char* topic, byte* payload, unsigned int length);
void bootCallback(char* topic, byte* payload, unsigned int length);
void processMqttTime(byte* payload, unsigned int length);
bool processAlarmCommand(JsonVariant doc);
bool processCommand(String key, String val);
int parsePayloadInt(byte* payload, unsigned int length);
float parsePayloadFloat(byte* payload, unsigned int length);
bool payloadMatch(byte* payload, unsigned int length, const char* target);
void updateMQTT(String whichField="all");
void handleInitMqttSync();
void handleQueuedMqttUpdates();
void executeMqttUpdate(String whichField);
bool updateAlarmActiveState(int num, bool active);
//Display Functions
void setBrightness(byte brightness, String testMsg="");
void setDisplayLED(bool state, byte red=0, byte green=0, byte blue=0);
byte getBrightnessRaw(byte pctValue);
byte getBrightnessPct(byte rawValue);
uint16_t getAmbientRaw(byte pctValue);
byte getAmbientPct(uint16_t rawValue);
//Display Touch
void processTouch();
void transformTouch(uint16_t &touchX, uint16_t &touchY);
void handleScrolling(ButtonWidget &btn, int x, int y, bool isPressed, void (*action)());
//Clock and Time
void syncTime();
void syncTimeNotifyCallback(struct timeval *tv);
void updateClock();
void updateIcons();
void manualTimeSet(int sec, int min, int hr, int day, int mon, int yr);
bool applyClockStyle(byte style);
byte getClockStyle(byte fontNum, byte fontSize);
String getManualDateString(int y, int m, int d, bool longWeekday=false, bool longMonth=false);
int getManualDow();
//Color and Conversions
uint16_t hexToTftColor(const char* hexString);
void tftColorToHex(uint16_t tftColor, char* buffer);
uint16_t rgbStringToTftColor(const char* rgbStr);
//Alarms and Sounds
bool saveAlarms();
bool loadAlarms();
void refreshAlarmCache();
String getCurrentAlarms();
bool activeAlarmsExist();
void checkAndTriggerAlarm();
void startAlarmSound(byte initVolume);
void stopAlarmSound();
void snoozeAlarmSound();
bool loadSoundsFromSD();
bool saveSoundsToSD();
String getCurrentSounds();
void initAlarmButtons();
void toggleAlarmButtons(bool show);
void alarmSnoozePress();
void alarmStopPress();
void settingsPagePress();
//Settings Page Functions (CYD)
void initSettingsButtons();
void drawSettingsPage();
void settingsBackPress(); 
void setAutoDimPress();
void showAlarmListPage();
void processSliderChange(uint16_t position);
//Alarm List Page Functions (CYD)
void initAlarmListButtons();
void drawAlarmListPage();
void alarmListBackPress();
void alarm1ActivePress();
void alarm2ActivePress();
void alarm3ActivePress();
void alarm4ActivePress();
void alarm5ActivePress();
void alarm1EditPress();
void alarm2EditPress();
void alarm3EditPress();
void alarm4EditPress();
void alarm5EditPress();
void alarmErrAcknPress();
void formatDateForDisplay(const char* input, char* output);
void formatTimeForDisplay(const char* input, char* output);
//Alarm Edit Page Functions (CYD)
void initAlarmEditButtons();
void drawAlarmEditPage();
void alarmEditSavePress();
void alarmEditCancelPress();
void alarmEditDatePress();
void alarmEditTimePress();
void alarmEditRepeatPress();
String getRepeatText(byte alarmNo);
//Alarm Edit Date Functions (CYD)
void initAlarmEditDateButtons();
void drawAlarmEditDatePage();
void bntAlarmEditMonthPlusPress();
void btnAlarmEditMonthMinusPress();
void btnAlarmEditDayPlusPress();
void btnAlarmEditDayMinusPress();
void btnAlarmEditYearPlusPress();
void btnAlarmEditYearMinusPress();
void btnAlarmEditDateTomorrowPress();
void btnAlarmEditDateUpdatePress();
void btnAlarmEditDateCancelPress();
int getDaysInMonth(int m, int y);
void validateDay();
//Alarm Edit Time Functions (CYD)
void initAlarmEditTimeButtons();
void drawAlarmEditTimePage();
void bntAlarmEditHourPlusPress();
void btnAlarmEditHourMinusPress();
void btnAlarmEditMinutePlusPress();
void btnAlarmEditMinuteMinusPress();
void btnAlarmEditAMPress();
void btnAlarmEditPMPress();
void btnAlarmEditMinute00Press();
void btnAlarmEditMinute15Press();
void btnAlarmEditMinute30Press();
void btnAlarmEditMinute45Press();
void btnAlarmEditTimeUpdatePress();
void btnAlarmEditTimeCancelPress();
//Alarm Edit Repeat Functions (CYD)
void initAlarmEditRepeatButtons();
void drawAlarmEditRepeatPage();
void btnAlarmEditRepeatNonePress();
void btnAlarmEditRepeatMoPress();
void btnAlarmEditRepeatTuPress();
void btnAlarmEditRepeatWePress();
void btnAlarmEditRepeatThPress();
void btnAlarmEditRepeatFrPress();
void btnAlarmEditRepeatSaPress();
void btnAlarmEditRepeatSuPress();
void btnAlarmEditRepeatWDPress();
void btnAlarmEditRepeatWEPress();
void btnAlarmEditRepeatUpdatePress();
void btnAlarmEditRepeatCancelPress();
//Weather and Temperature
bool owmIsConnected();
int getTemperature(bool refreshData);
//-- Web Pages & Handlers --
void setupWebHandlers();
//Onboarding and Main Page
void handleOnboard();
void webMainPage();
void webMainPageJson();
void handleLiveWebUpdate();
//System Integrations
void webSystemsPage();
void webSystemsPageJson();
void handleIntegrations();
//Display Settings
void webDisplayPage();
void webDisplayPageJson();
void handleDisplayTest();
void handleDisplayReset();
void handleDisplaySettings();
//Auto-Dimming
void handleAutoDimSettings(); 
void webDimCalibrate();
void handleCalibration();
void handleCalBrightChange();
void applyDimLevel();
//Clock and Time
void webClockPage();
void webClockPageJson();
void handleClockTest();
void handleClockReset();
void handleClockSettings();
void handleTimeSettings();
void handleSetTime();
//Alarms
void webAlarmPage();
void webAlarmPageJson();
void handleAlarmSettings();
void handleAlarmUpdate();
void handleSoundTest();
//Sounds
void webSoundPage();
void webSoundPageJson();
void handleSoundUpdate();
//API
void handleAPI();
bool updatePrimaryAlarmState(String state);
bool sendIPAddress(String ipAddr);
String forwardAPIRequest(String targetIP);
//Controller Commands
void webRestartPage();
void webFirmwareUpdate();
void handleOTAUpdate();
void webConfigDump();
void webResetPage();
//Primary Controller
bool primaryIsConnected(String ipAddr);
bool importMQTTData(String ipAddr);
bool importTempWeatherData(String ipAddr);
void setBulbState();
void setLEDState();

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
    //Connected to Wifi - Any "special" setup steps here
  }  
  //Setup hardware GPIO pins
  analogReadResolution(12);
  pinMode(DISP_BL_PIN, OUTPUT);   // Backlight/brightness control
  pinMode(DISP_LDR, INPUT);       // Light Dependent Resistor (built-in)

  //pinMode(DF_BUSY, INPUT_PULLUP);
  Serial1.begin(9600, SERIAL_8N1, DF_RX, DF_TX);
  delay(2000);
  //Onboard LED setup (inverted)
  ledcAttach(RED_LED_PIN, LED_FREQ, LED_RES);
  ledcAttach(BLUE_LED_PIN, LED_FREQ, LED_RES);
  ledcAttach(GREEN_LED_PIN, LED_FREQ, LED_RES);
  //Initially set dim red LED
  if (useLED) {
    setDisplayLED(1, 128, 0, 0);
    delay(250);
  } else {
    //just turn off all LEDs (since board defaults to dim red)
    setDisplayLED(0);
  }

  //Setup Display
  Wire.begin(DISP_I2C_SDA, DISP_I2C_SCL, DISP_I2C_FREQ);  //Initialize I2C
  tft.init();
  tft.setRotation(defaultDispRotate);
  if ((defaultDispRotate == 1) || (defaultDispRotate == 3)) {
    //flip horizontal and vertical dimensions
    dispWidth = DISP_H;
    dispHeight = DISP_V;
  }
  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  if (onboarding) {
    //Display "Join WiFi" on display so user knows onboarding is required
    tft.setTextColor(TFT_YELLOW);
    tft.drawCentreString("Join WiFi!",tft.width()/2, tft.height()/2, 6);

  } else {
    if (useLED) setDisplayLED(1, 96, 96, 0);   //Yellow
    setBrightness(196);  //set to ~75% for boot messages
    tft.setTextColor(TFT_BLACK, TFT_YELLOW, true);
    tft.drawCentreString("System Booting", tft.width()/2, 10, 4);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("IP Address:", 0, 65, 2);
    tft.setTextColor(TFT_GREEN);
    tft.drawString(baseIPAddress, tft.width()/2, 65, 2);  
    delay(1000);

    //-------------------------------------------
    // Apply Config file vars to starting states
    //-------------------------------------------
    //Set display active vars
    activeDispRotate = defaultDispRotate;
    activeBrightness = defaultBrightness;
    activeTempUnits = defaultTempUnits;
    activeClockStyle = defaultClockStyle;
    activeClockFont = defaultClockFont;
    activeClockSize = defaultClockSize;
    activeClockColor = defaultClockColor;
    activeHourFormat = defaultHourFormat;
    activeSDAlarmTrack = defaultSDAlarmTrack;
    activeAlarmVol = defaultAlarmVol;
    autoDimActive = autoDim;
    activeTimeSource = timeSource;

    //Attempt connect to primary controller - allow up to 30 seconds
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Pinging primary controller:", 0, 90, 2);
    if (primConnected) {
      bool pingOK = false;
      String primPath = "http://" + primIPAddress;
      String reqData = "api?ping=1";
      tft.setTextColor(TFT_YELLOW);
      tft.drawString("Attempt:", tft.width()/2, 90, 2);  
      //just means non-zero IP address at this point - attempt ping 30 times (once/second)
      for (int i = 1; i < 11; i++) {
        tft.fillRect(tft.width()/2 + 60, 90, 20, 20, TFT_BLACK);
        tft.drawString(String(i), (tft.width()/2) + 60, 90, 2);          
        http.begin(primPath);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        int response = http.POST(reqData);
        if (response > 0) {
          if (http.getString() = "OK") {
            pingOK = true;
            i = 99;        //Safety escape
            tft.fillRect((tft.width()/2) - 5, 85, tft.width()/2, 125, TFT_BLACK);
            tft.setTextColor(TFT_GREEN);
            tft.drawString("OK (@" + primIPAddress + ")", tft.width()/2, 90, 2);  //was 240
            delay(1000);
            break;
          }
        } else {
          delay(1000);  //Pause 1 second and try again
        }
        http.end();  
      }
      primConnected = pingOK;
    } else {
      if (useLED) setDisplayLED(1, 128, 0, 0);
      tft.setTextColor(TFT_BLACK);
      tft.fillRect((tft.width()/2) - 5, 85, tft.width()/2, 125, TFT_BLACK);
      tft.setTextColor(TFT_RED);
      tft.drawString("*FAILED*", tft.width()/2, 90, 2);   //was 240
      delay(1000);
    }
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Enabling OTA updates:", 0, 115, 2);
    delay(500);
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
    tft.setTextColor (TFT_GREEN);
    tft.drawString("OK", tft.width()/2, 115, 2);  
    delay(500);
    //Display Touch
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Touch Functionality:", 0, 140, 2);
    if (touchEnabled) {
      if (touch.begin(DISP_INT_PIN, DISP_RST_PIN, DISP_I2C_FREQ)) {
        rotation_t touchRotation;
        uint16_t dispH = tft.height();
        uint16_t dispW = tft.width();
        touchRotation = initGT911_ROTATION_0;
        touch.setupDisplay(dispH, dispW, touchRotation);  
        tft.setTextColor(TFT_GREEN);
        tft.drawString("OK", tft.width()/2, 140, 2);
      
      } else {
        if (useLED) setDisplayLED(1, 128, 0, 0);
        tft.setTextColor(TFT_RED);
        tft.drawString("**FAILED**", tft.width()/2, 140, 2);
      }
      initSettingsButtons();
      initAlarmListButtons();
      initAlarmEditButtons();
      initAlarmEditDateButtons();
      initAlarmEditTimeButtons();
      initAlarmEditRepeatButtons();
    } else {
      tft.setTextColor(TFT_YELLOW);
      tft.drawString("DISABLED", tft.width()/2, 140, 2);
    }
    delay(500);

    //Initialize Time
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Initial Time Sync:", 0, 165, 2);
    if (timeSource == 1) {
      //Connect to NTP server and set starting time
      sntp_set_sync_interval(ntpSyncInterval * 60000);             //convert to ms
      sntp_set_time_sync_notification_cb(syncTimeNotifyCallback);  //for MQTT update
      configTime(0, 0, ntpServer.c_str());                         //offset in seconds
      setenv("TZ", timeZone.c_str(), 1);
      tzset();
      //Get initial time
      syncTime();
      tft.setTextColor(TFT_GREEN);
      tft.drawString("OK", tft.width()/2, 165, 2);
    } else if (timeSource == 2) {
      tft.setTextColor(TFT_YELLOW);
      tft.drawString("MQTT - Pending", tft.width()/2, 165, 2);
      manualTimeSet(0, 0, 0, 1, 1, 2026);
    } else if (timeSource == 3) {
      tft.setTextColor(TFT_YELLOW);
      tft.drawString("API - Pending", tft.width()/2, 165, 2);
      manualTimeSet(0, 0, 0, 1, 1, 2026);
    } else {
      tft.setTextColor(TFT_YELLOW);
      tft.drawString("Disabled", tft.width()/2, 165, 2);
      //Manually set initial time to midnight, Jan 1, 2026
      manualTimeSet(0, 0, 0, 1, 1, 2026);
    } 
    delay(500);

    //MQTT Setup and Connection (if enabled by non zero IP address)
    tft.setTextColor(TFT_WHITE);
    tft.drawString("MQTT:", 0, 190, 2);
    if (mqttEnabled) {
      if (timeSource == 1) {
        //wait for ntp sync before setting up MQTT
        int retry = 0;
        while (time(nullptr) < 1000000000L && retry < 20) { 
          delay(500);
          retry++;
        }        
      }
      //Attempt to connect to MQTT broker - if it fails, disable MQTT
      if (!setup_mqtt()) {
        mqttConnected = false;
        tft.setTextColor(TFT_RED);
        tft.drawString("*FAILED*", tft.width()/2, 190, 2);
      } else {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("OK", tft.width()/2, 190, 2);
      }
    } else {
        tft.setTextColor(TFT_YELLOW);
        tft.drawString("Disabled", tft.width()/2, 190, 2);
    }
    delay(500);
    //If MQTT enabled and connected, it is available for any of the following at this point

    //Alarm File
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Alarms:", 0, 215, 2);
    if (loadAlarms()) {
      activeAlarms = activeAlarmsExist();
      initAlarmButtons();
      tft.setTextColor(TFT_GREEN);
      tft.drawString("OK", tft.width()/2, 215, 2);
    } else {
      tft.setTextColor(TFT_RED);
      tft.drawString("*FAILED*", tft.width()/2, 215, 2);
    }
    delay(1500);

    //DF Player Init
    tft.setTextColor(TFT_WHITE);
    tft.drawString("SD Card:", 0, 240, 2);
    if (useSDCard) {
      //Configure and start SD/MP3 Module;
      //Allow up to three attempts to start DFPlayer
      bool playerOnline = false;
      for (int i=0; i < 3; i++) {
        if (sdPlayer.begin(dfSerial, false, true)) {
          playerOnline = true;
          break;
        }
        delay(2000);
      }
      if (playerOnline) {          
        sdPlayer.volume(25);
        delay(200);
        tft.setTextColor(TFT_GREEN);
        tft.drawString("OK", tft.width()/2, 240, 2);
      } else {
        useSDCard = false;
        tft.setTextColor(TFT_RED);
        tft.drawString("*Failed!*", tft.width()/2, 240, 2);
      }
    } else {
      tft.setTextColor(TFT_YELLOW);
      tft.drawString("Disabled", tft.width()/2, 240, 2);
    }

    //Other setup step(s) here
    //Load sounds from SD
    if (!loadSoundsFromSD()) {
      //?? Take some sort of action if sound load fails
    }
    //Weather/Temperature
    if (weatherSource == 1) {
      #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("Get starting OWM temperature...");
      #endif

      //Get initial temperature reading.  Also serves to test connectivity
      externalTemperature = getTemperature(true);
      if (externalTemperature > -90) {
        owmConnected = true; 
      } else {
        owmConnected = false;
      }
    } else {
      owmConnected = false;
    }
 
    //Load icon fonts
    tft.setFreeFont(&icomoon20pt7b);   //Specify GFXFF as font to use
    //Boot message
    if (primConnected) {
      if (useLED) setDisplayLED(1, 0, 128, 0);
      tft.setTextColor(TFT_GREEN);
      tft.drawCentreString("BOOT COMPLETE: OK", tft.width()/2, 270, 4);
      //Send IP address to primary controller via API in case primary booted first
      if (sendIPAddress(primIPAddress)) {
        //nothing yet
      }
      delay(1000);
    } else {
      if (useLED) setDisplayLED(1, 128, 0, 0);
      tft.setTextColor(TFT_RED);
      tft.drawCentreString("See 'System' after boot!", tft.width()/2, 270, 4);
      delay(3000);
    }

    //Clear display
    if (useLED) setDisplayLED(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(defaultClockColor);
    setBrightness(defaultBrightness);
    activePage = 0;
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
      Serial.println("Setup complete. Entering main loop...");
    #endif
  }
  delay(500);
  //If MQTT enabled and connected, update all values.
  if (mqttConnected) {
    initialSyncRequired = true;
    syncStep = 0;
  }
  isBooting = false;
}

//===============================================
//    -------->   MAIN LOOP   <----------
//===============================================
void loop() {
  // When OTA flag set via HTML call, time to upload set at 20 sec. via server callback above.  Alter there if more time desired.
  if ((ota_flag) && (!onboarding)) {
    //displayOTA();
    uint16_t ota_time_start = millis();
    while (ota_time_elapsed < ota_time) {
      ArduinoOTA.handle();
      ota_time_elapsed = millis() - ota_time_start;
      delay(10);
    }
    ota_flag = false;
  }
  server.handleClient();
  
  if (!onboarding) {
    uint32_t curMillis = millis();

    if (mqttEnabled && (WIFIMODE > 0 && WiFi.status() == WL_CONNECTED)) {
      mqttConnected = client.connected();
    }

    processTouch();
    switch (activePage) {
      case 0:
        //Default Clock Page  
        if (activePage != lastPage) {
          //Page switch
          if (holdAutoDim) {
            autoDimActive = true;
            holdAutoDim = false;
            //Force brightness update
            activeThresholdMin = 0;
            activeThresholdMax = 0;
          }
          tft.fillScreen(TFT_BLACK);
          lastPage = activePage;
        }
        //Check and adjust brightness if current value overridden - only check once per second
        if ((curMillis - dimLastCheckMillis) > 1000) {
          if (overrideBrightness) {
            //Do not override brightness is alarm is actively sounding
            if (((currentAlarmState == ALARM_IDLE) || (currentAlarmState == ALARM_SNOOZING)) && (curMillis - overrideTimer >= 5000)) {  //Brighten for 5 seconds on override (main page touch)
              overrideBrightness = false;
              setBrightness(holdBrightness);
            }
          } else {
          //Check autoDim and set brightness if threshold crossed
            if (autoDimActive) {  //Only check once per second
              applyDimLevel();
            }
          }
          dimLastCheckMillis = curMillis;
        }

        //Show or update Time
        if (curMillis - prevTime >= 1000) {
          prevTime = curMillis;
          updateClock();
        }

        // Check for active alarms - Excute once per second
        if (curMillis - lastAlarmCheck >= 1000) {
          bool curActiveState = false;
          lastAlarmCheck = curMillis;
          checkAndTriggerAlarm();
          curActiveState = activeAlarmsExist();
          if (curActiveState != activeAlarms) {
            activeAlarmChange = true;
            activeAlarms = curActiveState;
          }
        }
        switch (currentAlarmState) {
          case ALARM_IDLE:
            //just skip
            break;
          case ALARM_TRIGGERED:
            //check for gentleWake and set init volume
            if (useGentleWake) {
              startAlarmSound(gentleWakeIncrement);
              lastGentleTime = curMillis;
            } else {
              startAlarmSound(activeAlarmVol);
            }
            break;
          case ALARM_RINGING:
            //check for gentleWake and increase volume
            if (useGentleWake) {
              if ((gentleWakeActiveVol < activeAlarmVol) && ((curMillis - lastGentleTime) > 5000)) {
                gentleWakeActiveVol = gentleWakeActiveVol + gentleWakeIncrement;
                if (gentleWakeActiveVol <= activeAlarmVol) {
                  sdPlayer.volume(gentleWakeActiveVol);
                } else {
                  sdPlayer.volume(activeAlarmVol);
                  gentleWakeActiveVol = activeAlarmVol;
                }
                lastGentleTime = curMillis;
              }
            }
            break;
          case ALARM_SNOOZING:
            if (millis() >= snoozeEndTime) {
              currentAlarmState = ALARM_TRIGGERED;
            }
            break;
        }
        break;
      case 1:
        //Main Settings Page
        if (activePage != lastPage) {
          //Page switch
          initSettingsSync = true;
          tft.fillScreen(TFT_BLACK);
          drawSettingsPage();
          lastPage = activePage;
        } else if (curMillis - prevTime >= 1000) {
          prevTime = curMillis;
          drawSettingsPage();
        }
        break;
      case 2:
        //Alarm List Page
        if (activePage != lastPage) {
          //Page switch
          initSettingsSync = true;
          tft.fillScreen(TFT_BLACK);
          drawAlarmListPage();
          lastPage = activePage;
        } else if (curMillis - prevTime >= 1000) {
          prevTime = curMillis;
          drawAlarmListPage();
        }
        break;
      case 3:
        //Alarm Edit Page
        if (activePage != lastPage) {
          //Page switch
          initSettingsSync = true;
          tft.fillScreen(TFT_BLACK);
          drawAlarmEditPage();
          lastPage = activePage;
        } else if (curMillis - prevTime >= 1000) {
          prevTime = curMillis;
          drawAlarmEditPage();
        }
        break;
      case 4:
        //Alarm Edit Date Page
        if (activePage != lastPage) {
          //Page switch
          initSettingsSync = true;
          tft.fillScreen(TFT_BLACK);
          drawAlarmEditDatePage();
          lastPage = activePage;
        } else if (curMillis - prevTime >= 1000) {
          prevTime = curMillis;
          drawAlarmEditDatePage();
        }
        break;
      case 5:
        //Alarm Edit Time Page
        if (activePage != lastPage) {
          //Page switch
          initSettingsSync = true;
          tft.fillScreen(TFT_BLACK);
          drawAlarmEditTimePage();
          lastPage = activePage;
        } else if (curMillis - prevTime >= 1000) {
          prevTime = curMillis;
          drawAlarmEditTimePage();
        }
        break;
      case 6:
        //Alarm Edit Repeat Page
        if (activePage != lastPage) {
          //Page switch
          initSettingsSync = true;
          tft.fillScreen(TFT_BLACK);
          drawAlarmEditRepeatPage();
          lastPage = activePage;
        } else if (curMillis - prevTime >= 1000) {
          prevTime = curMillis;
          drawAlarmEditRepeatPage();
        }
        break;

      case 10:
        //Display Test Page
        if ((touchEnabled) || (touchEnabledTest)) {
          uint8_t touchCount = touch.touched(GT911_MODE_POLLING);
          uint16_t x = 0, y = 0;
          if (touchCount > 0) {
            for (uint8_t i = 0; i < touchCount; i++) {
              GTPoint p = touch.getPoint(i);
              uint16_t mapX = p.x;
              uint16_t mapY = p.y;
              transformTouch(mapX, mapY);
              tft.fillCircle((mapX), (mapY), 2, TFT_WHITE);
            }
          }
          delay(20);
        }
        break;
      case 11:
        //Sound Test
        if ((alarmSounding) && (useGentleWake)) {
          if ((gentleWakeActiveVol < activeAlarmVol) && ((curMillis - lastGentleTime) > 5000)) {
            gentleWakeActiveVol = gentleWakeActiveVol + gentleWakeIncrement;
            if (gentleWakeActiveVol <= activeAlarmVol) {
              sdPlayer.volume(gentleWakeActiveVol);
            } else {
              sdPlayer.volume(activeAlarmVol);
              gentleWakeActiveVol = activeAlarmVol;
            }
            lastGentleTime = curMillis;
          }
        }
        break;
      default:
        //do nothing
        break;
    }
    //Handle any required MQTT state publishes (thread-safe)
    if (mqttEnabled && (WIFIMODE > 0 && WiFi.status() == WL_CONNECTED)) {
      if (!mqttConnected) {
        reconnect_soft(); 
      } else {
        client.loop(); 
      }
    }

    if (mqttConnected && !isBooting) {
      unsigned long currentMillis = millis();
      if (currentMillis - lastSyncStepTime > INIT_SYNC_INTERVAL) {
        if (initialSyncRequired) {
          handleInitMqttSync();
        }
        handleQueuedMqttUpdates();
        lastSyncStepTime = currentMillis; // Master clock update
      }
    }    
    //Handle any manual alarm play via MQTT/API
    if (manualAlarmPlay) {
      if ((millis() - manualAlarmStartTime) >= manualAlarmDuration) {
        sdPlayer.stop();
        manualAlarmPlay = false;
      }
    }
    //Save config and reboot if API command received
    if (rebootRequired) {
      sdPlayer.stop();  //In case any sounds are playing
      delay(1000);
      writeConfigFile(true);
    }
  }  
}

//========================================
// FILE FUNCTIONS (CONFIG, ALARMS, SOUNDS)
//========================================
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
        if (!deserializeError) {
          // Read values here from LittleFS (use defaults for all values in case they don't exist to avoid potential boot loop)
          //DON'T NEED TO STORE OR RECALL WIFI INFO - Written to flash automatically by library when successful connection occurs.
          deviceName = json["device_name"] | "BL_Display01";
          //Display and Touch
          dispHeight = json["disp_h"] | DISP_H;
          dispWidth = json["disp_v"] | DISP_V;
          defaultDispRotate = json["disp_rotate"] | DISP_ROTATE;
          touchEnabled = json["touch_enabled"] | 0;
          useLED = json["use_led"] | 0;
          defaultBrightness = json["brightness"] | 196;
          autoDim = json["auto_dim"] | 0;
          autoDimDebounce = json["dim_debounce"] |1;
          ambLightLevel_1 = json["amb_level_1"] | 0;
          ambLightLevel_2 = json["amb_level_2"] | 0;
          ambLightLevel_3 = json["amb_level_3"] | 0;
          ambLightLevel_4 = json["amb_level_4"] | 0;
          dimBrightness_1 = json["dim_bright_1"] | 25;
          dimBrightness_2 = json["dim_bright_2"] | 75;
          dimBrightness_3 = json["dim_bright_3"] | 125;
          dimBrightness_4 = json["dim_bright_4"] | 175;
          dimBrightness_5 = json["dim_bright_5"] | 225;
          //Clock & Time
          defaultClockStyle = json["clock_style"] | 0;
          defaultClockFont = json["clock_font"] | 8;
          defaultClockSize = json["clock_size"] | 1;
          defaultClockColor = json["clock_color"] | 0xFFFF;
          defaultHourFormat = json["hour_format"] | 12;
          timeSource = json["time_source"] | 0;
          timeZone = json["time_zone"] | TIMEZONE;
          ntpServer = json["ntp_server"] | "pool.ntp.org";
          ntpSyncInterval = json["ntp_sync_interval"] | 60;
          mqttTimeTopic = json["mqtt_time_topic"] | "cmnd/time";
          mqttLiveTime = json["mqtt_live_time"] | 0;
          apiLiveTime = json["api_live_time"] | 0;
          //Alarm Settings (actual alarms loaded during Setup() from different LittleFS file)
          useSDCard = json["alarm_sd_card"] | 0;
          defaultSDAlarmTrack = json["alarm_sd_track"] | 0;
          defaultAlarmVol = json["alarm_vol"] | 5;
          useGentleWake = json["alarm_gentle_wake"] | 0;
          snoozeTime = json["snooze_time"] | 0;
          //Primary Controller
          primIPAddr_1 = json["prim_ip_1"] | 0;
          primIPAddr_2 = json["prim_ip_2"] | 0;
          primIPAddr_3 = json["prim_ip_3"] | 0;
          primIPAddr_4 = json["prim_ip_4"] | 0;
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
          if ((mqttAddr_1 + mqttAddr_2 + mqttAddr_3 + mqttAddr_4) > 0) {
            //Special 24-bit color string for MQTT
            char hexBuf[8];
            tftColorToHex(defaultClockColor, hexBuf);
            mqttClockColor = String(hexBuf);
          }
          //Weather/Temp
          defaultTempUnits = json["temp_units"] | 13;         //12=°C, 13=°F
          weatherSource = json["weather_source"] | 0;         //0 none (disabled), 1 OWM, 2 MQTT, 3 API
          owmKey = json["owm_key"] | "N/A";
          owmLat = json["own_lat"] | "39.8083";               //Default Lat/Long is Lebanon, KS (center of cont. US)
          owmLong = json["owm_long"] | "-98.555";             //Stored as strings, since that's what the API call needs
          tempUpdatePeriod = json["temp_update_period"] | 15;

          //------------------------------------------------
          //Set or update local vars based on other settings
          //------------------------------------------------
          //Set IP address for primary controller
          primIPAddress = String(primIPAddr_1) + "." + String(primIPAddr_2) + "." + String(primIPAddr_3) + "." + String(primIPAddr_4);
          if (primIPAddress == "0.0.0.0") {
            primConnected = false;    //will attempt connection and set path in Setup()
          } else {
            primConnected = true;
          }
          //Disable or enable MQTT
          if ((mqttAddr_1 == 0) && (mqttAddr_2 == 0) && (mqttAddr_3 == 0) && (mqttAddr_4 == 0)) {
            mqttIPAddress = "0.0.0.0";
            mqttEnabled = false;
          } else {
            mqttIPAddress = String(mqttAddr_1) + "." + String(mqttAddr_2) + "." + String(mqttAddr_3) + "." + String(mqttAddr_4);
            mqttEnabled = true;
          }
          //Calculate volume increment if gentle wake is used
          if (useGentleWake) {
            if (defaultAlarmVol >= 15) {
              gentleWakeIncrement = round((defaultAlarmVol * 1.0) / 6.0);
            } else {
              gentleWakeIncrement = defaultAlarmVol;
            }
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

    LittleFS.end();  //End - need to prevent issue with OTA updates
  } else {
    //could not mount filesystem
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println("failed to mount FS");
        Serial.println("LittleFS Formatted. Restarting ESP.");
    #endif
    onboarding = true;
  }
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
    doc["device_name"] = deviceName;
    //Display, Touch and Dimming
    doc["disp_h"] = dispHeight;
    doc["disp_v"] = dispWidth;
    doc["disp_rotate"] = defaultDispRotate;
    doc["touch_enabled"] = ((touchEnabled) ? 1 : 0);
    doc["use_led"] = ((useLED) ? 1 : 0);
    doc["brightness"] = defaultBrightness;
    doc["auto_dim"] = ((autoDim) ? 1 :0);
    doc["dim_debounce"] = autoDimDebounce;
    doc["amb_level_1"] = ambLightLevel_1;
    doc["amb_level_2"] = ambLightLevel_2;
    doc["amb_level_3"] = ambLightLevel_3;
    doc["amb_level_4"] = ambLightLevel_4;
    doc["dim_bright_1"] = dimBrightness_1;
    doc["dim_bright_2"] = dimBrightness_2;
    doc["dim_bright_3"] = dimBrightness_3;
    doc["dim_bright_4"] = dimBrightness_4;
    doc["dim_bright_5"] = dimBrightness_5;
    //Clock & Time
    doc["clock_style"] = defaultClockStyle;
    doc["clock_size"] = defaultClockSize;
    doc["clock_font"] = defaultClockFont;
    doc["clock_color"] = defaultClockColor;
    doc["hour_format"] = defaultHourFormat;
    doc["time_source"] = timeSource;
    doc["time_zone"] = timeZone;
    doc["ntp_server"] = ntpServer;
    doc["ntp_sync_interval"] = ntpSyncInterval;
    doc["mqtt_time_topic"] = mqttTimeTopic;
    doc["mqtt_live_time"] = ((mqttLiveTime) ? 1 : 0);
    doc["api_live_time"] = ((apiLiveTime) ? 1 : 0);
    //Alarm Settings (actual alarms saved via saveAlarms() in different LittleFS file)
    doc["alarm_sd_card"] = ((useSDCard) ? 1 : 0);
    doc["alarm_sd_track"] = defaultSDAlarmTrack;
    doc["alarm_vol"] = defaultAlarmVol;
    doc["alarm_gentle_wake"] = ((useGentleWake) ? 1 : 0);
    doc["snooze_time"] = snoozeTime;
    //Primary Controller
    doc["prim_ip_1"] = primIPAddr_1;
    doc["prim_ip_2"] = primIPAddr_2;
    doc["prim_ip_3"] = primIPAddr_3;
    doc["prim_ip_4"] = primIPAddr_4;
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
      LittleFS.end();
      if (restart_ESP) {
        ESP.restart();
      }
    }
  } else {
    //could not mount filesystem
    #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
      Serial.println("failed to mount FS");
    #endif
  }
}

String getCurrentConfig() {
  //This function just reads and returns the raw contents of the config file in JSON format.
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
          if (json.containsKey("clock_color")) {
            uint16_t colorInt = json["clock_color"];
            char hexBuf[8];
            tftColorToHex(colorInt, hexBuf);
            String hybridOutput = String(colorInt) + " (" + String(hexBuf) +")";
            json["clock_color"] = hybridOutput;
          }     
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
//-----------------------------------
// Read Alarms from LittleFS (binary)
//-----------------------------------
bool loadAlarms() {
  bool retVal = false;
  if (!LittleFS.begin()) {
    goto use_dummy_data;
  }
  {
    File file = LittleFS.open(ALARM_FILE_PATH, "r");
    if (!file || file.size() != sizeof(alarmArray)) {
      //Dummy values for testing if file doesn't exist

      if (file) file.close();
      LittleFS.end();
      goto use_dummy_data;
    }
    size_t expectedSize = sizeof(alarmArray);
    size_t bytesRead = file.read((uint8_t*)alarmArray, expectedSize);
    file.close();
    if (bytesRead == expectedSize) {
      retVal = true;
    }
    LittleFS.end();
    refreshAlarmCache();
    return retVal;
  }
use_dummy_data:
  for (int i = 0; i < MAX_ALARMS; i++) {
    alarmArray[i].active = 0;
    strcpy(alarmArray[i].date, "2000-01-01");
    strcpy(alarmArray[i].time, "00:00:00");
    alarmArray[i].repeat = 0;
  }
  refreshAlarmCache();
  return false;
}

void refreshAlarmCache() {
  //Local cache for faster loop processing
  bool hasActive = false;
  for (int i = 0; i < MAX_ALARMS; i++) {
    const char* timeStr = alarmArray[i].time;
    int h, m, s = 0; // Initialize seconds to 0

    // Try to parse the full "HH:MM:SS" format
    if (sscanf(timeStr, "%d:%d:%d", &h, &m, &s) == 3) {
      // Full format parsed successfully
    } 
    // If not, try the shorter "HH:MM" format
    else if (sscanf(timeStr, "%d:%d", &h, &m) == 2) {
      // Hour and minute parsed; s remains 0
      s = 0; 
    }
    // Handle error or uninitialized time by defaulting to 00:00:00
    else {
      h = 0; m = 0; s = 0; 
    }

    // Store the parsed integers in the cache array
    alarmCache[i].h = h;
    alarmCache[i].m = m;
    alarmCache[i].s = s;
  }
}

//----------------------------------
// Save Alarms to LittleFS (binary)
//----------------------------------
bool saveAlarms() {
  bool retVal = false;
  if (!LittleFS.begin()) {
    return retVal;
  }
  File file = LittleFS.open(ALARM_FILE_PATH, "w");
  if (!file) return retVal;
  size_t expectedSize = sizeof(alarmArray);
  size_t bytesWritten = file.write((uint8_t*)alarmArray, sizeof(alarmArray));
  file.close();
  if (bytesWritten == expectedSize) {
    retVal = true;
  }
  LittleFS.end();
  refreshAlarmCache();
  if ((mqttConnected) && (!isBooting)) { 
    updateMQTT("alarms");
  }
  return retVal;
}
//-----------------------------------
// Get dump of current alarm.bin file
//-----------------------------------
String getCurrentAlarms() {
  String fileContents = "";
  if (LittleFS.begin()) {
    if (LittleFS.exists(ALARM_FILE_PATH)) {
      File file = LittleFS.open(ALARM_FILE_PATH, "r");
      if (file) {
        // Validate Size
        // Calculate expected size based on the Struct definition
        size_t expectedSize = sizeof(Alarm) * MAX_ALARMS;
        if (file.size() == expectedSize) {
          // Create a temporary array to hold the data read from disk
          Alarm tempAlarms[MAX_ALARMS];
          file.read((uint8_t*)tempAlarms, expectedSize);
          file.close();
          // Create JSON Document for the Output String
          JsonDocument doc; 
          char sysTimeStr[30];
          snprintf(sysTimeStr, sizeof(sysTimeStr), "%04d-%02d-%02d %02d:%02d:%02d", 
                   rtc.getYear(), rtc.getMonth() + 1, rtc.getDay(), 
                   rtc.getHour(true), rtc.getMinute(), rtc.getSecond());
          
          doc["system_time"] = sysTimeStr; // Add to top of JSON

          // --- Build Alarm Array ---
          JsonArray data = doc.createNestedArray("stored_alarms");

          for (int i = 0; i < MAX_ALARMS; i++) {
            JsonObject alm = data.createNestedObject();
            alm["index"] = i + 1;
            alm["active"] = tempAlarms[i].active;
            alm["date"] = tempAlarms[i].date;
            alm["time"] = tempAlarms[i].time;
            alm["repeat"] = tempAlarms[i].repeat;
          }
          // Serialize to String (Pretty Print)
          serializeJsonPretty(doc, fileContents);
        } else {
          fileContents = "ERROR: File size mismatch. Expected " + String(expectedSize) + " bytes, got " + String(file.size());
          file.close();
        }
      } else {
        fileContents = "ERROR: Could not open " + String(ALARM_FILE_PATH);
      }
    } else {
      fileContents = "ERROR: " + String(ALARM_FILE_PATH) + " does not exist.";
    }
    LittleFS.end();    
  } else {
    fileContents = "ERROR: Failed to mount LittleFS.";
  }
  return fileContents;
}
// -----------------------------------
//  Read Sounds from SD Card - binary
// -----------------------------------
bool loadSoundsFromSD() {
  bool retVal = false;
  if (!LittleFS.begin()) {
    goto use_dummy_data;
  }
  {
    File file = LittleFS.open(SOUND_FILE_PATH, "r");
    if (!file) {
      //Dummy values for testing if file doesn't exist
      if (file) file.close();
      LittleFS.end();
      goto use_dummy_data;
    }
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error) {
      retVal = false;
    } else {
      JsonArray sounds = doc.as<JsonArray>();
      int i = 0;
      for (JsonObject soundJson : sounds) {
        if (i < MAX_SOUNDS) {
          soundLibrary[i].index = soundJson["index"].as<byte>();
          soundLibrary[i].filename = soundJson["filename"].as<String>();
          soundLibrary[i].title = soundJson["title"].as<String>();
          i++;
        }
      }
    }
  }

  LittleFS.end();
  return retVal;

use_dummy_data:
  for (int i = 0; i < MAX_SOUNDS; i++) {
    soundLibrary[i].index = i + 1;
    soundLibrary[i].filename = "";
    soundLibrary[i].title = "empty";
  }
  //activeAlarms = false;
  return false;
}
// -----------------------------------
//  Save Sounds to SD Card - binary
// -----------------------------------
bool saveSoundsToSD() {
  bool retVal = false;
  JsonDocument doc;
  JsonArray sounds = doc.to<JsonArray>();

  for (int i=0; i < MAX_SOUNDS; i++) {
    JsonObject obj = sounds.add<JsonObject>();
    obj["index"] = soundLibrary[i].index;
    obj["filename"] = soundLibrary[i].filename;
    obj["title"] = soundLibrary[i].title;
  }
  if (!LittleFS.begin()) {
    return retVal;
  }
  File file = LittleFS.open(SOUND_FILE_PATH, "w");
  if (!file) {
    LittleFS.end();
    return retVal;
  } 

  if (serializeJson(doc, file) == 0) {
    retVal = false;
  } else {
    retVal = true;
  }
  file.close();
  LittleFS.end();
  return retVal;
}
// -----------------------------------
//  Dump Sound List to String (for config dump)
// -----------------------------------

String getCurrentSounds() {
  //Dump sound file to string for display
  String fileContents = "";
  if (LittleFS.begin(false)) {  //do not format on failure
    if (LittleFS.exists(SOUND_FILE_PATH)) {
      File soundFile = LittleFS.open(SOUND_FILE_PATH, "r");
      if (soundFile) {
        size_t size = soundFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        soundFile.readBytes(buf.get(), size);
        soundFile.close();
        JsonDocument json;
        auto deserializeError = deserializeJson(json, buf.get());
        if (!deserializeError) {        
          serializeJsonPretty(json, fileContents);
        } else {
          fileContents = "ERROR RETURNED: Could not deserialize sound file (does not appear to be JSON).";
        }
      } else {
        fileContents = "ERROR RETURNED: 'sound.json' could not be opened - See Troubleshooting.";
      }
    } else {
      fileContents = "ERROR RETURNED: 'sound.json' file does not exist.  Be sure you have setup your sounds at least one time.";
    }
    LittleFS.end();
  } else {
    //LittleFS.begin failed
    fileContents = "ERROR RETURNED: Failed to mount LittleFS - See Troubleshooting.";
  }
  return fileContents;
}

/* =====================================
    WIFI SETUP & Functions
   ===================================== */
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

// =====================================
//  SETUP MQTT, CALLBACKS and FUNCTIONS
// =====================================
bool setup_mqtt() {
  byte mcount = 0;
  
  IPAddress myserver = IPAddress(mqttAddr_1, mqttAddr_2, mqttAddr_3, mqttAddr_4);
  String statusTopic = "stat/" + mqttTopicPub + "/display/status";  
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
    if (mcount >= 60) {
      #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
        Serial.println();
        Serial.println("Could not connect to MQTT broker. MQTT disabled.");
      #endif
      // Could not connect to MQTT broker
      return false;
    }
    delay(500);
    mcount++;
  }

  #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
    Serial.println();
    Serial.println("Successfully connected to MQTT broker.");
  #endif
  //Special topic for MQTT time setting/syncing - topic defined in web app
  if ((timeSource == 2) && (mqttTimeTopic.length() > 0)) {
    client.subscribe(mqttTimeTopic.c_str());
    //Send command to MQTT to refresh time to the nearest second
    client.publish(("stat/" + mqttTopicPub + "/gettime").c_str(), "now", false);
    unsigned long startWait = millis();
    while (!timeReceivedAtBoot && millis() - startWait < 2000) {
      client.loop();
      delay(10);
    }
  }
  if (!mqttLiveTime) {
    //unsubscribe to save processing if not using live MQTT time updates
    client.unsubscribe(mqttTimeTopic.c_str());
  }
  client.setCallback(callback);
  String curVer = VERSION;

  //Publish LWT, current IP and MAC addresses
  client.publish((statusTopic).c_str(), "online", true);  //LWT
  client.publish(("stat/" + mqttTopicPub + "/display/ipaddr").c_str(), baseIPAddress.c_str(), true);
  client.publish(("stat/" + mqttTopicPub + "/display/macaddr").c_str(), strMacAddr.c_str(), true);
  client.publish(("stat/" + mqttTopicPub + "/display/version").c_str(), curVer.c_str(), true);
  if (rtc.getYear() > 2020) {
    char dt[17];
    sprintf(dt, "%04d-%02d-%02d %02d:%02d", rtc.getYear(), rtc.getMonth()+1, rtc.getDay(), rtc.getHour(true), rtc.getMinute());
    client.publish(("stat/" + mqttTopicPub + "/display/lastboot").c_str(), dt, true);
  } else {
    client.publish(("stat/" + mqttTopicPub + "/display/lastboot").c_str(), "N/A", true);
  }
  client.subscribe(("cmnd/" + mqttTopicSub + "/#").c_str());
  mqttConnected = true;
  return true;
}

void reconnect() {
  //Attempt to reconnect to MQTT broker. 
  if (!client.connected()) {
    String statusTopic = "stat/" + mqttTopicPub + "/display/status";
    // Attempt one connection
    if (client.connect(mqttClient.c_str(), mqttUser.c_str(), mqttPW.c_str(), statusTopic.c_str(), 1, true, "offline")) {
      client.publish((statusTopic).c_str(), "online", true);  //LWT
      client.subscribe(("cmnd/" + mqttTopicSub + "/#").c_str());
      mqttConnected = true;
    } else {
      // Set the flag and let the main loop continue.
      mqttConnected = false;
    }
  }
}

bool reconnect_soft() {
  //Attempt MQTT reconnect.  If fails, return false
  if (!client.connected()) {
    unsigned long now = millis();
    String statusTopic = "stat/" + mqttTopicPub + "/display/status";
    // ADDITION: Only try to connect every 5 seconds to avoid blocking the loop
    if (now - lastMqttRetryAttempt > MQTT_RETRY_DELAY) {
      lastMqttRetryAttempt = now;
      // Try to connect once
      if (client.connect(mqttClient.c_str(), mqttUser.c_str(), mqttPW.c_str(), statusTopic.c_str(), 1, true, "offline")) {
        client.publish((statusTopic).c_str(), "online", true);  //LWT
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

void callback(char* topic, byte* payload, unsigned int length) {
  if (isBooting) return;
  if (!mqttIsConnected()) return;
  //Special topic (as specifed in display setup->Clock) for syncing/updating time from MQTT - Will only run if timeSource = 2 (mqtt)
  if ((mqttTimeTopic.length() > 0) && (strcmp(topic, mqttTimeTopic.c_str()) == 0) && (timeSource == 2)) {
    //Payload must be in yyyy-mm-dd hh:mm:ss format
    processMqttTime(payload, length);
    return; // Stop processing once the time is set    
  }

  char* subTopic = strrchr(topic, '/');
  if (subTopic == NULL) return;
  subTopic++;

  char buf[length + 1];
  memcpy(buf, payload, length);
  buf[length] = '\0';
  String val = String(buf);

  //Force a state refresh (publish) of all MQTT topics
  if ((strcmp(subTopic, "displayrefresh") == 0) || (strcmp(subTopic, "refreshall") == 0)) {
    //payload ignored. Any payload can be passed
    syncStep = 0;               // Reset the counter to the first case
    initialSyncRequired = true; // Tell the loop to start the sequence
    lastSyncStepTime = millis(); // Initialize the timer    
    return;
  }

  //Reboot Display Controller (reboot all handled via primary controller)
  if (strcmp(subTopic, "displayrestart") == 0) {
    delay(500);
    ESP.restart();
    return;
  }

  //Save current settings as new boot defaults and reboot
  if (strcmp(subTopic, "dispsaveconfig") == 0) {
    delay(500); 
    writeConfigFile(true); // This saves and triggers ESP.restart()
    return;
  }

  //Set Date & Time
  if (strcmp(subTopic, "settime") == 0) {
    //payload must be in yyyy-mm-dd hh:mm:ss format
    int yr, mon, day, hr, min, sec;
    //Validate payload
    if (sscanf(buf, "%d-%d-%d %d:%d:%d", &yr, &mon, &day, &hr, &min, &sec) == 6) {
      manualTimeSet(sec, min, hr, day, mon, yr);
    }
    return;
  }

  //Alarm Update (JSON Payload)
  if (strcmp(subTopic, "setalarm") == 0) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, (char*)payload, length);
    if (!error) processAlarmCommand(doc);
    return;
  }

  processCommand(String(subTopic), val);
}

//Special callback for loading MQTT time at boot
void bootCallback(char* topic, byte* payload, unsigned int length) {
  if (mqttTimeTopic.length() > 0 && strcmp(topic, mqttTimeTopic.c_str()) == 0) {
    processMqttTime(payload, length);
  }
}

void processMqttTime(byte* payload, unsigned int length) {
  char buf[length + 1];
  memcpy(buf, payload, length);
  buf[length] = '\0'; 
  int yr, mon, day, hr, min, sec;
  int parsedCount = sscanf(buf, "%d-%d-%d %d:%d:%d", &yr, &mon, &day, &hr, &min, &sec);
  if (parsedCount == 6) {
    manualTimeSet(sec, min, hr, day, mon, yr);
    timeReceivedAtBoot = true; 
    lastTimeSyncOK = true;
  } else {
    lastTimeSyncOK = false;
  }
  if ((mqttConnected) && (!isBooting)) {
    updateMQTT("timesync");
  }
}
// ------------------------
// MQTT Payload Conversions
// ------------------------
//Convert payload to integer
int parsePayloadInt(byte* payload, unsigned int length) {
    char buf[length + 1];
    memcpy(buf, payload, length);
    buf[length] = '\0';
    //Verify numeric payload
    char* endptr;
    long val = strtol(buf, &endptr, 10);
    if (endptr == buf || *buf == '\0') {
        return 0; 
    }
    return (int)val;    
}
//Convert payload to float
float parsePayloadFloat(byte* payload, unsigned int length) {
    char buf[length + 1];
    memcpy(buf, payload, length);
    buf[length] = '\0';
    //verify numeric payload
    char* endptr;
    float val = strtof(buf, &endptr);
    if (endptr == buf || *buf == '\0') {
        return 0.0f;
    } 
    return val;
}
//Payload Text Compare
bool payloadMatch(byte* payload, unsigned int length, const char* target) {
    if (length != strlen(target)) return false;
    return memcmp(payload, target, length) == 0;
}

//------------------------------------------
// Process Commands - shared by MQTT and API
//------------------------------------------
bool processCommand(String key, String val) {
  key.trim();
  val.trim();
  bool recognized = true;
  //Display brightness
  if (key == "dispbrightness") {
    int newBright = activeBrightness;
    if (isNumeric(val)) {
      newBright = constrain(val.toInt(), 0, 255);
    } else if (val == "up") {
      newBright = constrain((activeBrightness + 25), 0, 255);
    } else if (val == "down") {
      newBright = constrain((activeBrightness - 25), 0, 255);
    }
    setBrightness(newBright);
  //Set Temperature
  } else if ((key == "settemperature") && ((weatherSource == 2) || (weatherSource ==3 ))) {
    //only process if weatherSource = MQTT (2) or API (3)
    externalTemperature = round(val.toFloat());
    if ((!isBooting) && (mqttConnected)) {
      updateMQTT("temperature");
    }
  //autoDim
  } else if (key == "autodim") {
    autoDimActive = (val.equalsIgnoreCase("on") || val == "1");
    if ((!isBooting) && (mqttConnected)) {
      updateMQTT("autodim");
    }
    
  //Clock color
  } else if (key == "clockcolor") {
    //Accept RGB or HEX Color string
    uint16_t newColor = 0;
    String normalizedHex = "";
    bool valid = false;
    int firstComma = val.indexOf(',');
    int secondComma = val.indexOf(',', firstComma + 1);


    if (firstComma != -1 && secondComma != -1) {
      // Parse and constrain for safety
      byte r = constrain(val.substring(0, firstComma).toInt(), 0, 255);
      byte g = constrain(val.substring(firstComma + 1, secondComma).toInt(), 0, 255);
      byte b = constrain(val.substring(secondComma + 1).toInt(), 0, 255);
      
      // Convert to 16-bit for the hardware
      newColor = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
      
      // Convert to 24-bit Hex for the MQTT cache
      char hexBuf[8];
      sprintf(hexBuf, "#%02x%02x%02x", r, g, b);
      normalizedHex = String(hexBuf);
      valid = true;
    } else if (isValidHex(val)) {
    // Handle Hex String (e.g. #ff0000)
      newColor = hexToTftColor(val.c_str());
      // Normalize to ensure it starts with # and is lowercase
      normalizedHex = val;
      if (!normalizedHex.startsWith("#")) normalizedHex = "#" + normalizedHex;
      normalizedHex.toLowerCase();
      valid = true;
    }
    if (valid) {
      activeClockColor = newColor;  // Hardware uses the 16-bit value
      mqttClockColor = normalizedHex; // Cache uses the 24-bit "intent"
      
      initSync = true; // Trigger display redraw
      if ((!isBooting) && (mqttConnected)) {
        updateMQTT("clockcolor");
      }
    }
  //gentleWake
  } else if (key == "gentlewake") {
    useGentleWake = (val.equalsIgnoreCase("on") || val == "1");
    if ((!isBooting) && (mqttConnected)) {
      updateMQTT("gentlewake");
    }
  //alarmTrack (alarm sound)  
  } else if (key == "alarmtrack") {
    int newTrack = val.toInt();
    if ((newTrack > 0) && (newTrack <=255)) {
      defaultSDAlarmTrack = (uint8_t)newTrack;
      activeSDAlarmTrack = defaultSDAlarmTrack;
      if ((!isBooting) && (mqttConnected)) {
        updateMQTT("alarmtrack");
      }
    }
  //Alarm Volume
  } else if (key == "alarmvolume") {
    int newVolume = val.toInt();
    if ((newVolume >= 0) && (newVolume <= 30)) {
      defaultAlarmVol = (uint8_t)newVolume;
      activeAlarmVol = defaultAlarmVol;
      if ((!isBooting) && (mqttConnected)) {
        updateMQTT("alarmvol");
      }
    }
  //Alarm Snooze Time  
  } else if (key == "snoozetime") {
    int newTime = val.toInt();
    if ((newTime >= 0) && (newTime <= 60)) {
      snoozeTime = newTime;
      if ((!isBooting) && (mqttConnected)) {
        updateMQTT("snoozetime");
      }
    }
  //Set active state of any alarm (active/inactive)
  } else if (key == "alarmactive") {

    int sep = val.indexOf(':');
    if (sep != -1) {
      int num = val.substring(0, sep).toInt();
      bool act = (val.substring(sep + 1).toInt() == 1);
      updateAlarmActiveState(num, act);
    }
  //Manually sound alarm for x seconds (0=stop)
  } else if (key =="playalarm") {
    // Do not allow manual play if an actual alarm is currently triggered or snoozing
    if (currentAlarmState == ALARM_IDLE) {
      int duration = constrain(val.toInt(), 0, 60);  //60-seconds max
      if (duration > 0) {
        sdPlayer.stop();
        delay(100);
        sdPlayer.volume(activeAlarmVol);
        delay(100);
        sdPlayer.loop(activeSDAlarmTrack);
        manualAlarmPlay = true;
        manualAlarmStartTime = millis();  
        manualAlarmDuration = (unsigned long)duration * 1000;
      } else {
        sdPlayer.stop();
        manualAlarmPlay = false;
      }
    }

  /* ===========================================================
     The following are primary for internal use and communcation 
     between the primary and display controllers.  While they 
     'technically' be used via API/MQTT, it is not recommended and has not
     been thorougly tested in all situations
     =========================================================== */
  //Active Alarm Actions - received from primary touch controllers
  } else if (key == "alarmupdate") {
    if (val == "stop") {
      alarmStopPress();
    } else if (val == "snooze") {
      if (snoozeTime > 0) {
        alarmSnoozePress();
      } else {
        alarmStopPress();
      }
    }
  //Bulb state - Internal use: received from primary for display on TFT
  } else if (key == "bulbstate") {
    bulbState = (val.toInt() == 1 ? 1 : 0);
    lightStateChange = true;
    recognized = false;    
  //LED state - Internal use: received from primary for display on TFT
  } else if (key == "ledstate") {
    ledState = (val.toInt() == 1 ? 1 : 0);
    lightStateChange = true;
    recognized = false;
  } else {
      recognized = false;
  }
  return recognized;
}

bool processAlarmCommand(JsonVariant doc) {
  // Create a copy of the existing alarm to modify
  int rawIndex = doc["alarmnum"] | 0;
  int i = rawIndex - 1;
  if (i < 0 || i > 4) return false; 

  Alarm tempAlarm = alarmArray[i]; 
  bool allValid = true;

  // Active flag
  if (doc.containsKey("active")) {
    if (doc["active"].is<bool>() || doc["active"].is<int>()) {
      tempAlarm.active = doc["active"];
    } else { allValid = false; }
  }

  // Repeat (0-9)
  if (allValid && doc.containsKey("repeat")) {
    int r = doc["repeat"] | -1;
    if (r >= 0 && r <= 9) { tempAlarm.repeat = (uint8_t)r; } 
    else { allValid = false; }
  }

  // Date (YYYY-MM-DD)
  if (allValid && doc.containsKey("date")) {
    const char* dStr = doc["date"] | "";
    int y, m, d;
    if (sscanf(dStr, "%4d-%2d-%2d", &y, &m, &d) == 3) {
      if (y >= 2025 && y <= 2099 && m >= 1 && m <= 12 && d >= 1 && d <= 31) {
        strncpy(tempAlarm.date, dStr, sizeof(tempAlarm.date) - 1);
      } else { allValid = false; }
    } else { allValid = false; }
  }

  // Time (HH:MM)
  if (allValid && doc.containsKey("time")) {
    const char* tStr = doc["time"] | "";
    int hr, mn;
    if (sscanf(tStr, "%2d:%2d", &hr, &mn) == 2) {
      if (hr >= 0 && hr <= 23 && mn >= 0 && mn <= 59) {
        strncpy(tempAlarm.time, tStr, sizeof(tempAlarm.time) - 1);
      } else { allValid = false; }
    } else { allValid = false; }
  }

  if (allValid) {
    alarmArray[i] = tempAlarm;
    saveAlarms(); // Your function to write to LittleFS
    return true;
  }
  return false;
}
//------------------------------------
// Process toggling alarm active state
//------------------------------------
bool updateAlarmActiveState(int num, bool active) {
  const int maxAlarms = sizeof(alarmArray) / sizeof(alarmArray[0]);
  int i = num - 1;

  if (i < 0 || i >= maxAlarms) return false;
  alarmArray[i].active = active;
  return saveAlarms(); 
}

//================================
// DISPLAY FUNCTIONS
//================================
void setBrightness(byte brightness, String testMsg) {
  //Valid analogWrite values: 0 - 255
  if (brightness > 255) brightness = 255;
  analogWrite(DISP_BL_PIN, brightness);
  activeBrightness = brightness;
  if (testMsg != "") {

    tft.fillScreen(TFT_BLACK);
    tft.drawCentreString(testMsg,dispWidth/2, dispHeight/4, 4);
  }
  if ((mqttConnected) && (!isBooting)) { 
    updateMQTT("dispbrightness");
  }

}

bool applyClockStyle(byte style) {
  //Extracts font and size from styles array and sets active font/size vars
  bool retVal = false;
  for (int i = 0; i < sizeof(clockStyles)/sizeof(clockStyles[0]); i++) {
    if (clockStyles[i].indexNum == style) {
      activeClockFont = clockStyles[i].fontNum;
      activeClockSize = clockStyles[i].fontSize;
      retVal = true;
    }
  }
  return retVal;
}

byte getClockStyle(byte fontNum, byte fontSize) {
  //Finds matching Style index num from font number and size
  byte retVal = 0;
  for (int i = 0; i < sizeof(clockStyles)/sizeof(clockStyles[0]); i++) {
    if ((clockStyles[i].fontNum == fontNum) && (clockStyles[i].fontSize == fontSize)) {
      retVal = clockStyles[i].indexNum;
      break;
    }
  }
  return retVal;
}

void updateIcons() {
  //if (!displayTestFlag) {
  if (activePage == 0) {
    //Updates icon indicators at top of display
    int curTemp = getTemperature(false);  //Will only update if OWM updatePeriod lapsed or if temperature changed (via MQTT or API)
    if ((curTemp != externalTemperature) || (initSync) || (activeAlarmChange)) {    
      externalTemperature = curTemp;      //Will only update if source = owm and updatePeriod lapsed, otherwise just returns current externalTemperature  
      activeAlarmChange = false;
      String curTemp = String(externalTemperature);
      if (activeTempUnits == 12) {
        curTemp += "dC";
      } else {
        curTemp += "dF";
      }
      tft.setFreeFont(&icomoon20pt7b);
      tft.setTextSize(1);
      tft.setTextDatum(TL_DATUM);
      tft.fillRect(0, 0, tft.width(), 45, TFT_BLACK);
      tft.setTextColor(activeClockColor, TFT_BLACK, true);
      tft.drawString(curTemp, 0, 0, GFXFF);
      if (WiFi.status() == WL_CONNECTED) {
        tft.drawString("w", 100 , 0, GFXFF);
      } else {
        tft.setTextColor(TFT_RED);
        tft.drawString("x", 100 , 0, GFXFF);
        tft.setTextColor(activeClockColor);
      }
      if (primConnected) {
        tft.drawString("l", 140, 0, GFXFF);
      } else {
        tft.setTextColor(TFT_RED);
        tft.drawString("m", 140 , 0, GFXFF);
        tft.setTextColor(activeClockColor);
      }
      if (activeAlarms) {
        btnSettings.drawButton(false, "ag");
      } else {
        btnSettings.drawButton(false, " g");
      }
    }
  }
}

//----------------------------------------
// Brightness & Ambient Light Conversions
//----------------------------------------
//These convert display brightness between raw (0-255) values to percent (0-100) values
//For ambient light, converts between LDR raw (0-4095) and inverted percentage (100-0) 
byte getBrightnessRaw(byte pctValue) {
  //Convert percent bright (0-100) to raw value (0-255)  
  int retVal = ((pctValue * 255) / 100);
  if (retVal > 255) retVal = 255;
  if (retVal < 0) retVal = 0;
  return retVal;
}

byte getBrightnessPct(byte rawValue) {
  //Convert raw brightness (0-255) to a percentage (0-100)
  int retVal = ((rawValue * 100) / 255);
  if (retVal > 100) retVal = 100;
  if (retVal < 0) retVal = 0;
  return retVal;
}

uint16_t getAmbientRaw(byte pctValue) {
  //Convert percent display (0-100) to inverse raw value (4095-0)
  if (pctValue > 100) pctValue = 100;
  float tmpValue = map(pctValue, 0, 100, 4095, 0);
  uint16_t retVal = round(tmpValue);
  return retVal;
}

byte getAmbientPct(uint16_t rawValue) {
  //Convert raw value (0-4096) to inverse percentage (100-0)
  if (rawValue > 4095) rawValue = 4095;
  float tmpValue = map(rawValue, 0, 4095, 100, 0);
  byte retVal = round(tmpValue);
  return retVal;
}

void applyDimLevel() {
  bool changeBrightness = false;
  byte curAmbientLevel = getAmbientPct(analogRead(DISP_LDR));

  if ((activeThresholdMin == 0) && (activeThresholdMax == 0)) {
    //first execution
    changeBrightness = true;
  } else if ((curAmbientLevel < activeThresholdMin) || (curAmbientLevel >= activeThresholdMax)) {
    crossedThresholdCount++;
    if (crossedThresholdCount > autoDimDebounce) {
      //Hold for at least two cycles (e.g. 'debounce')
      changeBrightness = true;
      crossedThresholdCount = 0;
    }
  }
  
  if (changeBrightness) {
    //Set to new brightness and set new thresholds
    if (curAmbientLevel < ambLightLevel_1) {
      setBrightness(dimBrightness_1);
      activeThresholdMin = 0;
      activeThresholdMax = ambLightLevel_1;
    } else if ((curAmbientLevel >= ambLightLevel_1) && (curAmbientLevel < ambLightLevel_2)) {
      setBrightness(dimBrightness_2);
      activeThresholdMin = ambLightLevel_1;
      activeThresholdMax = ambLightLevel_2;
    } else if ((curAmbientLevel >= ambLightLevel_2) && (curAmbientLevel < ambLightLevel_3)) {
      setBrightness(dimBrightness_3);
      activeThresholdMin = ambLightLevel_2;
      activeThresholdMax = ambLightLevel_3;
    } else if ((curAmbientLevel > ambLightLevel_3) && (curAmbientLevel < ambLightLevel_4)) {
      setBrightness(dimBrightness_4);
      activeThresholdMin = ambLightLevel_3;
      activeThresholdMax = ambLightLevel_4;
    } else if (curAmbientLevel >= ambLightLevel_4) {
      setBrightness(dimBrightness_5);
      activeThresholdMin = ambLightLevel_3;
      activeThresholdMax = ambLightLevel_4;
    }
  }
}

//-----------------------------
// Color and Other Conversions
//-----------------------------
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
// Conversion function that takes a 24-bit hex color string and returns a 16-bit RGB565 value (uint16_t)
uint16_t hexToTftColor(const char* hexString) {
  // Check if the string starts with a '#', and adjust the pointer if it does.
  // This allows the function to handle both "#RRGGBB" and "RRGGBB" formats.
  if (hexString[0] == '#') {
    hexString++;
  }

  uint32_t red24, green24, blue24;

  // Use sscanf to parse the three 2-digit hex components (RRGGBB) into 32-bit unsigned integers.
  // %2x reads exactly two hexadecimal characters.
  if (sscanf(hexString, "%2x%2x%2x", &red24, &green24, &blue24) != 3) {
    // If parsing fails, return a safe default color (e.g., black).
     return 0x0000;
  }

  // --- 24-bit (8-8-8) to 16-bit (5-6-5) Conversion ---
  // The 24-bit components (0-255) must be scaled down to the required bit depth.
  
  // Convert 8-bit (0-255) to 5-bit (0-31):
  // e.g: (red24 >> 3) effectively divides by 8, keeping the 5 most significant bits.
  uint16_t red5 = (uint16_t)(red24 >> 3);
  uint16_t green6 = (uint16_t)(green24 >> 2);
  uint16_t blue5 = (uint16_t)(blue24 >> 3);

  // --- Pack the components into a single uint16_t (RGB565) ---
  // The format is RRRRR GGGGGG BBBBB
  // Red is shifted 11 bits left: (R5 << 11)
  // Green is shifted 5 bits left: (G6 << 5)
  // Blue is left as is: (B5)
  return (red5 << 11) | (green6 << 5) | blue5;
}

// Conversion function that takes a 16-bit RGB565 value (uint16_t) and returns 24-bit hex color in 'buffer' 
void tftColorToHex(uint16_t tftColor, char* buffer) {
  uint8_t r5 = (tftColor >> 11) & 0x1F; // Top 5 bits
  uint8_t g6 = (tftColor >> 5) & 0x3F;  // Middle 6 bits
  uint8_t b5 = tftColor & 0x1F;         // Bottom 5 bits

  // Expand to 8-bit components (RGB888)
  // The expansion is done by copying the MSBs into the LSB positions.
  // 5-bit to 8-bit: (val << 3) | (val >> 2)
  // 6-bit to 8-bit: (val << 2) | (val >> 4)
  uint8_t r8 = (r5 << 3) | (r5 >> 2);
  uint8_t g8 = (g6 << 2) | (g6 >> 4);
  uint8_t b8 = (b5 << 3) | (b5 >> 2);

  // Format into the "#RRGGBB" string
  sprintf(buffer, "#%02x%02x%02x", r8, g8, b8);  
}

//Conversion function that takes a comma-delimted RGB string and returns a 16-bit color for the tft text color
uint16_t rgbStringToTftColor(const char* rgbStr) {
  int r = 0, g = 0, b = 0;

  // sscanf parses the string and fills our variables
  // It returns the number of successfully matched items
  if (sscanf(rgbStr, "%d,%d,%d", &r, &g, &b) == 3) {
    // Constrain to 0-255 just in case the input is wild
    r = (r < 0) ? 0 : (r > 255) ? 255 : r;
    g = (g < 0) ? 0 : (g > 255) ? 255 : g;
    b = (b < 0) ? 0 : (b > 255) ? 255 : b;

    // Convert to 565 format using bit shifting
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }

  return 0; // Return black if the string format was invalid
}

bool isNumeric(String str) {
  // Check for an empty string
  if (str.length() == 0) {
    return false;
  }

  int startIndex = 0;
  // Handle optional leading sign
  if (str.charAt(0) == '+' || str.charAt(0) == '-') {
    startIndex = 1;
    if (str.length() == 1) return false; // A string with only a sign is not a number
  }

  for (int i = startIndex; i < str.length(); i++) {
    if (!isDigit(str.charAt(i))) {
      return false;
    }
  }
  return true;  
}
//================================
// Touch Processing and Functions
//================================
void processTouch() {
  bool pressed = false;
  // If we just changed states (e.g. Alarm started), ignore inputs for a moment
  if (millis() < touchCooldownTime) {
    flushTouchBuffer();
    return;
  }

  // Polling Frequency Check (Run every 100ms)
  if (millis() - lastTouchScan >= 100) {
    lastTouchScan = millis();

    // 3. Read Hardware
    uint8_t touchCount = touch.touched(GT911_MODE_POLLING);
    if (touchCount > 0) {
      pressed = true;
      for (uint8_t i = 0; i < touchCount; i++) {
        GTPoint p = touch.getPoint(i);
        uint16_t mapX = p.x;
        uint16_t mapY = p.y;
        transformTouch(mapX, mapY); // Apply calibration
        lastTouchTime = millis();   // Capture touch time for auto-close of settings page(s)
        switch (activePage) {
          case 0:
            // --- CONTEXT AWARE BUTTON ROUTING ---
            if (currentAlarmState == ALARM_RINGING) {
              if (btnAlarmSnooze.contains(mapX, mapY)) {
                btnAlarmSnooze.pressAction();
                return; // Action taken, stop processing this touch
              } else if (btnAlarmStop.contains(mapX, mapY)) {
                btnAlarmStop.pressAction();
                return;
              }
            } else if (currentAlarmState == ALARM_SNOOZING) {
              if (btnAlarmStopAlt.contains(mapX, mapY)) {
                btnAlarmStopAlt.pressAction();
                return;
              } else if (activeBrightness < defaultBrightness) {
                overrideBrightness = true;
                holdBrightness = activeBrightness;
                if (autoDimActive) {
                  activeThresholdMin = 0;
                  activeThresholdMax = 0;
                }
                overrideTimer = millis();
                setBrightness(defaultBrightness);
                return;
              }
            
            } else if (currentAlarmState == ALARM_IDLE) {
              // Normal Operation (Idle / Snoozing)
              // Only check these buttons if the alarm is NOT ringing
              if (btnSettings.contains(mapX, mapY)) {
                btnSettings.pressAction();
                return;
              } else if (activeBrightness < defaultBrightness) {
                //Touch anywhere else: brighten screen for 5 seconds
                overrideBrightness = true;
                holdBrightness = activeBrightness;
                if (autoDimActive) {
                  activeThresholdMin = 0;
                  activeThresholdMax = 0;
                }
                overrideTimer = millis();
                setBrightness(defaultBrightness);
                return;
              }
            }
            break;

          case 1:
            if (btnSettingsBack.contains(mapX, mapY)) {
              btnSettingsBack.pressAction();
              return;
            }
            if (btnSettingsBulbOff.contains(mapX, mapY)) {
              btnSettingsBulbOff.pressAction();
              return;
            }
            if (btnSettingsBulbOn.contains(mapX, mapY)) {
              btnSettingsBulbOn.pressAction();
              return;
            }
            if (btnSettingsLEDsOff.contains(mapX, mapY)) {
              btnSettingsLEDsOff.pressAction();
              return;
            }
            if (btnSettingsLEDsOn.contains(mapX, mapY)) {
              btnSettingsLEDsOn.pressAction();
              return;
            }
            if (btnSettingsAutoDimOn.contains(mapX, mapY)) {
              btnSettingsAutoDimOn.pressAction();
              return;
            }
            if (btnSettingsAutoDimOff.contains(mapX, mapY)) {
              btnSettingsAutoDimOff.pressAction();
              return;
            }
            if (sldBrightness.checkTouch(mapX, mapY)) {
              processSliderChange(sldBrightness.getSliderPosition());
              return;
            }
            if (btnSettingsAlarmSetup.contains(mapX, mapY)) {
              btnSettingsAlarmSetup.pressAction();
              return;
            }
            break;
          case 2:
            if (btnAlarmListBack.contains(mapX, mapY)) {
              btnAlarmListBack.pressAction();
              return;
            }
            if ((btnAlarm1Active.contains(mapX, mapY)) || (btnAlarm1Inactive.contains(mapX, mapY))) {
              btnAlarm1Active.pressAction();
              return;
            }
            if ((btnAlarm2Active.contains(mapX, mapY)) || (btnAlarm2Inactive.contains(mapX, mapY))) {
              btnAlarm2Active.pressAction();
              return;
            }
            if ((btnAlarm3Active.contains(mapX, mapY)) || (btnAlarm3Inactive.contains(mapX, mapY))) {
              btnAlarm3Active.pressAction();
              return;
            }
            if ((btnAlarm4Active.contains(mapX, mapY)) || (btnAlarm4Inactive.contains(mapX, mapY))) {
              btnAlarm4Active.pressAction();
              return;
            }
            if ((btnAlarm5Active.contains(mapX, mapY)) || (btnAlarm5Inactive.contains(mapX, mapY))) {
              btnAlarm5Active.pressAction();
              return;
            }
            if (btnAlarm1Edit.contains(mapX, mapY)) {
              btnAlarm1Edit.pressAction();
              return;
            }
            if (btnAlarm2Edit.contains(mapX, mapY)) {
              btnAlarm2Edit.pressAction();
              return;
            }
            if (btnAlarm3Edit.contains(mapX, mapY)) {
              btnAlarm3Edit.pressAction();
              return;
            }
            if (btnAlarm4Edit.contains(mapX, mapY)) {
              btnAlarm4Edit.pressAction();
              return;
            }
            if (btnAlarm5Edit.contains(mapX, mapY)) {
              btnAlarm5Edit.pressAction();
              return;
            }
            if (btnAlarmErrAckn.contains(mapX, mapY)) {
              btnAlarmErrAckn.pressAction();
              return;
            }
            break;
          case 3:
            if (btnAlarmEditCancel.contains(mapX, mapY)) {
              btnAlarmEditCancel.pressAction();
              return;
            }
            if (btnAlarmEditDate.contains(mapX, mapY)) {
              btnAlarmEditDate.pressAction();
              return;
            }
            if (btnAlarmEditTime.contains(mapX, mapY)) {
              btnAlarmEditTime.pressAction();
              return;
            }
            if (btnAlarmEditRepeat.contains(mapX, mapY)) {
              btnAlarmEditRepeat.pressAction();
              return;
            }
            if (btnAlarmEditSave.contains(mapX, mapY)) {
              btnAlarmEditSave.pressAction();
              return;
            }
            break;
          case 4:
            if (btnAlarmEditDateCancel.contains(mapX, mapY)) {
              btnAlarmEditDateCancel.pressAction();
              return;
            } 
            if (btnAlarmEditMonthPlus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditMonthPlus, mapX, mapY, pressed, btnAlarmEditMonthPlusPress);
              return;
            }
            if (btnAlarmEditMonthMinus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditMonthMinus, mapX, mapY, pressed, btnAlarmEditMonthMinusPress);
               return;
            }
            if (btnAlarmEditDayPlus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditDayPlus, mapX, mapY, pressed, btnAlarmEditDayPlusPress);
              return;
            }
            if (btnAlarmEditDayMinus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditDayMinus, mapX, mapY, pressed, btnAlarmEditDayMinusPress);
               return;
            }
            if (btnAlarmEditYearPlus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditYearPlus, mapX, mapY, pressed, btnAlarmEditYearPlusPress);
              return;
            }
            if (btnAlarmEditYearMinus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditYearMinus, mapX, mapY, pressed, btnAlarmEditYearMinusPress);
              return;
            }
            if (btnAlarmEditDateTomorrow.contains(mapX, mapY)) {
              btnAlarmEditDateTomorrow.pressAction();
              return;
            }
            if (btnAlarmEditDateUpdate.contains(mapX, mapY)) {
              btnAlarmEditDateUpdate.pressAction();
              return;
            }
            break;
          case 5:
            if (btnAlarmEditTimeCancel.contains(mapX, mapY)) {
              btnAlarmEditTimeCancel.pressAction();
              return;
            }
            if (btnAlarmEditHourPlus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditHourPlus, mapX, mapY, pressed, btnAlarmEditHourPlusPress);
              return;
            }
            if (btnAlarmEditHourMinus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditHourMinus, mapX, mapY, pressed, btnAlarmEditHourMinusPress);
              return;
            }
            if (btnAlarmEditMinutePlus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditMinutePlus, mapX, mapY, pressed, btnAlarmEditMinutePlusPress);
              return;
            }
            if (btnAlarmEditMinuteMinus.contains(mapX, mapY)) {
              handleScrolling(btnAlarmEditMinuteMinus, mapX, mapY, pressed, btnAlarmEditMinuteMinusPress);
              return;
            }
            if (btnAlarmEditAM.contains(mapX, mapY)) {
              btnAlarmEditAM.pressAction();
              return;
            }
            if (btnAlarmEditPM.contains(mapX, mapY)) {
              btnAlarmEditPM.pressAction();
              return;
            }
            if (btnAlarmEditMinute00.contains(mapX, mapY)) {
              btnAlarmEditMinute00.pressAction();
              return;
            }
            if (btnAlarmEditMinute15.contains(mapX, mapY)) {
              btnAlarmEditMinute15.pressAction();
              return;
            }
            if (btnAlarmEditMinute30.contains(mapX, mapY)) {
              btnAlarmEditMinute30.pressAction();
              return;
            }
            if (btnAlarmEditMinute45.contains(mapX, mapY)) {
              btnAlarmEditMinute45.pressAction();
              return;
            }
            if (btnAlarmEditTimeUpdate.contains(mapX, mapY)) {
              btnAlarmEditTimeUpdate.pressAction();
              return;
            }
            break;
          case 6:
            if (btnAlarmEditRepeatNone.contains(mapX, mapY)) {
              btnAlarmEditRepeatNone.pressAction();
              return;
            }
            if (btnAlarmEditRepeatMo.contains(mapX, mapY)) {
              btnAlarmEditRepeatMo.pressAction();
              return;
            }
            if (btnAlarmEditRepeatTu.contains(mapX, mapY)) {
              btnAlarmEditRepeatTu.pressAction();
              return;
            }
            if (btnAlarmEditRepeatWe.contains(mapX, mapY)) {
              btnAlarmEditRepeatWe.pressAction();
              return;
            }
            if (btnAlarmEditRepeatTh.contains(mapX, mapY)) {
              btnAlarmEditRepeatTh.pressAction();
              return;
            }
            if (btnAlarmEditRepeatFr.contains(mapX, mapY)) {
              btnAlarmEditRepeatFr.pressAction();
              return;
            }
            if (btnAlarmEditRepeatSa.contains(mapX, mapY)) {
              btnAlarmEditRepeatSa.pressAction();
              return;
            }
            if (btnAlarmEditRepeatSu.contains(mapX, mapY)) {
              btnAlarmEditRepeatSu.pressAction();
              return;
            }
            if (btnAlarmEditRepeatWD.contains(mapX, mapY)) {
              btnAlarmEditRepeatWD.pressAction();
              return;
            }
            if (btnAlarmEditRepeatWE.contains(mapX, mapY)) {
              btnAlarmEditRepeatWE.pressAction();
              return;
            }
            if (btnAlarmEditRepeatUpdate.contains(mapX, mapY)) {
              btnAlarmEditRepeatUpdate.pressAction();
              return;
            }
            if (btnAlarmEditRepeatCancel.contains(mapX, mapY)) {
              btnAlarmEditRepeatCancel.pressAction();
              return;
            }
          default:
            break;
        }
      } // End For Loop
      
      // Optional: Reset scan timer to prevent rapid-fire triggers
      lastTouchScan = millis(); 
    } else if ((activePage > 0) && (millis() > lastTouchTime + 10000)) {
      pressed = false;
      //Auto-close page and return to main page (0) if no touch for 10 seconds
      if (activePage == 1) {
        btnSettingsBack.pressAction();
      }
    }
  }
}

void transformTouch(uint16_t &touchX, uint16_t &touchY) {
  // Store original values temporarily before modification
  uint16_t originalX = touchX;
  uint16_t originalY = touchY;
  //Get current display dimensions (based on active rotation)
  uint16_t curDispWidth = tft.width();
  uint16_t curDispHeight = tft.height();

  switch (activeDispRotate) {
    case 0:  
      //Display Rotation = 0°
      //Default: Don't do anything
      touchX = originalX;
      touchY = originalY;
      break;
    case 1:
      //Display Rotation = 90°
      touchX = originalY;
      touchY = curDispHeight - originalX;
      break;
      
    case 2:
      //Display Rotation = 180°
      touchX = curDispWidth - originalX;
      touchY = curDispHeight - originalY;
      break;
      
    case 3:
      //Display Rotation = 270°
      touchX = curDispWidth - originalY;
      touchY = originalX;
      break;
      
    default:
      // Handle invalid input: coordinates are left as they were
      break;
  }
}

void handleScrolling(ButtonWidget &btn, int x, int y, bool isPressed, void (*action)()) {
if (isPressed && btn.contains(x, y)) {
    btn.press(true); // Visual feedback

    // Check if it's time for the next increment
    if (millis() - lastScrollMillis > scrollDelay) {
      action(); // Execute the + or - function passed in
      
      // Acceleration Logic
      if (scrollDelay > 100) {
        scrollDelay -= 50; // Speed up each step
      } else {
        scrollDelay = 80;  // Top speed cap
      }
      
      lastScrollMillis = millis();
    }
  } else {
    btn.press(false);
    // When the specific button is released, reset the speed
    if (btn.justReleased()) {
      scrollDelay = 400;
    }
  }
}

//=========================
// Time-Related Functions
//=========================
void syncTime() {
  //When using NTP as time source (MQTT/API time handled in MQTT and API callbacks)
  time(&now);                    //Get current time
  localtime_r(&now, &timeinfo);  //convert to local time
  lastTimeSyncOK = true;
  if ((!isBooting) && (mqttConnected)) {
    updateMQTT("timesync");
  }
}

void syncTimeNotifyCallback(struct timeval *tv) {
  if (mqttConnected) {
    lastTimeSyncOK = true;
    updateMQTT("timesync");
  }
}

void updateClock() {
  int hour, mins, secs;
  String today = "";
  if (((timeSource == 2) && (mqttLiveTime)) || ((timeSource == 3) && (apiLiveTime))) {
    //MQTT or API Time Source
    hour = liveLastHour;
    mins = liveLastMinute;
    secs = liveLastSecond;
    //Get date string
    today = getManualDateString(liveLastYear, liveLastMonth, liveLastDay, true, true);
  } else {
    //NTP or Manual Time Source
    hour = rtc.getHour(true);
    mins = rtc.getMinute();
    secs = rtc.getSecond();
    today = rtc.getDate(true);
  }
  
  int rawHour = hour;  //need for date update
  if (((curMinutes != mins) || (initSync)) && (activePage == 0)) {  
    String curTime;
    if (activeHourFormat == 12 && hour > 12) {
      hour = hour - 12;
    } else if (activeHourFormat == 12 && hour == 0) {  
      hour = 12;
    }
    byte h1 = hour / 10;
    byte h2 = hour % 10;
    byte m1 = mins / 10;
    byte m2 = mins % 10;
    byte s1 = secs / 10;
    byte s2 = secs % 10;

    if (h1 > 0 || (activeHourFormat == 24 && hour == 0)) { //display leading zero for midnight when 24-hour display
      curTime = String(h1);
    } else {
      curTime = " ";  // Blank
    }
    curTime += String(h2) + ":" + String(m1) + String(m2);
    tft.fillRect(0, 75, tft.width(), tft.height()-145, TFT_BLACK);  //Just overwrite time area of screen. Prevents flashing of icons and date.
    tft.setTextColor(activeClockColor);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(activeClockSize);
    tft.drawString(curTime, tft.width()/2, (tft.height()/2), activeClockFont);

    if (alarmButtonsOn) {
      //Redraw buttons to overlay time update above
      if ((currentAlarmState == ALARM_SNOOZING) || (snoozeTime == 0)) {
        btnAlarmStopAlt.drawButton();
      } else {
        btnAlarmSnooze.drawButton();
        btnAlarmStop.drawButton();
      }
    } else {
      if ((((rawHour == 0) && (mins == 0)) || ((rawHour == 0) && (mins == 1))) || (initSync)) {
        //Only update date at midnight - also run again at 12:01 just to assure date change is captured
        tft.setTextSize(1);
        tft.fillRect(0, 280, tft.width(), tft.height() - 280, TFT_BLACK);
        tft.drawCentreString(today, tft.width()/2, 280, 4);
      }
    }
    updateIcons();
  } else if (activeAlarmChange) {
    updateIcons();
  }
  initSync = false;
  curMinutes = mins;
}

void manualTimeSet(int sec, int min, int hr, int day, int mon, int yr) {
  rtc.setTime(sec, min, hr, day, mon, yr);
  if ((timeSource == 2) || (timeSource == 3)) {
    if ((yr != liveLastYear) || (mon != liveLastMonth) || (day != liveLastDay)) {
      initSync = true;
    }
    liveLastYear = yr;
    liveLastMonth = mon;
    liveLastDay = day;
    liveLastHour = hr;
    liveLastMinute = min;
    liveLastSecond = sec;
  }
}
int getManualDow() {
  //Used for MQTT/API time source to get day of week
  int y = liveLastYear;
  int m = liveLastMonth;
  int d = liveLastDay;
  int dayNum = 0;
  if (m < 3) {
    m += 12;
    y--;
  }  
  // Algorithm for proleptic Gregorian calendar
  int h = (d + (13 * (m + 1) / 5) + y + (y / 4) - (y / 100) + (y / 400)) % 7;
  // To get 0 = Saturday, 1 = Monday:
  dayNum = (h + 6) % 7; 
  return dayNum;
}

String getManualDateString(int y, int m, int d, bool longWeekday, bool longMonth) {
  //Get date string when using manual time (no RTC)
  String dow = "";
  String monthName = "";
  //String retVal = "";
  //int dayNum = 0;
  int dowNum = getManualDow();

  switch (dowNum) {
    case 0: dow = (longWeekday) ? "Sunday" : "Sun"; break;
    case 1: dow = (longWeekday) ? "Monday" : "Mon"; break;
    case 2: dow = (longWeekday) ? "Tuesday" : "Tue"; break;
    case 3: dow = (longWeekday) ? "Wednesday" : "Wed"; break;
    case 4: dow = (longWeekday) ? "Thursday" : "Thu"; break;
    case 5: dow = (longWeekday) ? "Friday" : "Fri"; break;
    case 6: dow = (longWeekday) ? "Saturday" : "Sat"; break;
  }
  switch (m) {
    case 1: monthName = (longMonth) ? "January" : "Jan"; break;
    case 2: monthName = (longMonth) ? "February" : "Feb"; break;
    case 3: monthName = (longMonth) ? "March" : "Mar"; break;
    case 4: monthName = (longMonth) ? "April" : "Apr"; break;
    case 5: monthName = "May"; break;
    case 6: monthName = (longMonth) ? "June" : "Jun"; break;
    case 7: monthName = (longMonth) ? "July" : "Jul"; break;
    case 8: monthName = (longMonth) ? "August" : "Aug"; break;
    case 9: monthName = (longMonth) ? "September" : "Sep"; break;
    case 10: monthName = (longMonth) ? "October" : "Oct"; break;
    case 11: monthName = (longMonth) ? "November" : "Nov"; break;
    case 12: monthName = (longMonth) ? "December" : "Dec"; break;
  }
  return dow + ", " + monthName + " " + String(d) + " " + String(y);
}
//========================
// Onboard LED Functions
//========================
void setDisplayLED(bool state, byte red, byte green, byte blue) {
  //LED is 'inverted':  255 = 0ff, 0 = 100%
  if (state) {
    ledcWrite(RED_LED_PIN, (255-red));
    ledcWrite(GREEN_LED_PIN, (255-green));
    ledcWrite(BLUE_LED_PIN, (255-blue));
  } else {
    ledcWrite(RED_LED_PIN, 255);
    ledcWrite(GREEN_LED_PIN, 255);
    ledcWrite(BLUE_LED_PIN, 255);
  }
}

// ===========================
//  ALARM AND SOUND Functions
// ===========================
bool activeAlarmsExist() {
  //Checks loaded array for any alarms that are active
  //Alarms without a repeat, but a date in the past should be considered "inactive", regardless of flag
  int currentYear = rtc.getYear();
  int currentMonth = rtc.getMonth() + 1; // Library returns 0-11, so we add 1
  int currentDay = rtc.getDay();
  
  int currentHour = rtc.getHour(true);   // 'true' forces 24-hour format (0-23)
  int currentMin = rtc.getMinute();
  int currentSec = rtc.getSecond();

  // Format them into ISO 8601 buffers (YYYY-MM-DD and HH:MM:SS)
  // This ensures leading zeros (e.g., "2025-01-09") for correct string comparison
  char currentDate[11]; 
  char currentTime[9];  

  snprintf(currentDate, sizeof(currentDate), "%04d-%02d-%02d", currentYear, currentMonth, currentDay);
  snprintf(currentTime, sizeof(currentTime), "%02d:%02d:%02d", currentHour, currentMin, currentSec);

  for (int i=0; i < MAX_ALARMS; i++) {
    if (alarmArray[i].active == 0) {
      continue;
    }
    int repeatCheckValue = 0;
    memcpy(&repeatCheckValue, &alarmArray[i].repeat, sizeof(byte));
    if (repeatCheckValue != 0) {
      return true; 
    }
    int dateCompare = strcmp(alarmArray[i].date, currentDate);
    if (dateCompare > 0) {
      // Alarm is in the future (so it's 'active')
      return true;
    } else if (dateCompare == 0) {
      // Alarm is TODAY. Check the time.
      int timeCompare = strcmp(alarmArray[i].time, currentTime);
      
      if (timeCompare > 0) {
        // Time is in the future (e.g. Alarm is 8:00, Now is 7:00)
        return true;
      }      
    }   
  }
  //No alarms meet criteria for 'Active
  return false;
}

void checkAndTriggerAlarm() {
  
  if (currentAlarmState != ALARM_IDLE) {
    return; // Already handling an alarm
  }
  int curH, curM, curS, currentDayOfWeek;
  char currentDateStr[11];  
  if (((timeSource == 2) && (mqttLiveTime)) || ((timeSource == 3) && (apiLiveTime))) {
    curH = liveLastHour;
    curM = liveLastMinute;
    curS = liveLastSecond;
    currentDayOfWeek = getManualDow();
    snprintf(currentDateStr, sizeof(currentDateStr), "%04d-%02d-%02d", liveLastYear, liveLastMonth, liveLastDay);
  } else {
    curH = rtc.getHour(true);
    curM = rtc.getMinute();
    curS = rtc.getSecond();
    currentDayOfWeek = rtc.getDayofWeek(); 
    snprintf(currentDateStr, sizeof(currentDateStr), "%04d-%02d-%02d", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay());
  }
  // Create comparable time value (HHMMSS format)
  long currentTimeValue = (long)curH * 10000 + (long)curM * 100 + (long)curS;

  for (int i = 0; i < MAX_ALARMS; i++) {
    
    if (alarmArray[i].active == 0) continue;
    
    // Get time from the FAST CACHE
    long alarmTimeValue = (long)alarmCache[i].h * 10000 + (long)alarmCache[i].m * 100 + (long)alarmCache[i].s;
    
    // Time Window Match
    // If current time >= alarm time, AND we are within a 5-second trigger window:
    if (currentTimeValue >= alarmTimeValue && (currentTimeValue - alarmTimeValue) < 5) {
      
      // Date/Repeat Match 
      bool dateMatch = false;
      
      // Case A: Repeating Alarm
      if (alarmArray[i].repeat != 0) {
        if (alarmArray[i].repeat == (currentDayOfWeek + 1)) {   //Day of week is zero-based
          //Single day match (repeat = 1-7)
          dateMatch = true;
        } else if (alarmArray[i].repeat == 8) {
          //Weekday (Mon/Fri) match
          if (currentDayOfWeek >= 1 && currentDayOfWeek <= 5) {
            dateMatch = true;
          }
        } else if (alarmArray[i].repeat == 9) {
          //Weekend (Sat/Sun) match
          if (currentDayOfWeek == 6 || currentDayOfWeek == 0) {
              dateMatch = true;
          }
        }
      }
      // Case B: Non-Repeating Alarm (repeat == 0)
      else if (strcmp(alarmArray[i].date, currentDateStr) == 0) {
        dateMatch = true;
      }
      
      if (dateMatch) {
        // --- Alarm Triggered ---
        currentAlarmIndex = i;
        currentAlarmState = ALARM_TRIGGERED;
        
        // Disable non-repeating alarms immediately
        if (alarmArray[i].repeat == 0) {
          alarmArray[i].active = 0;
          saveAlarms(); 
        }
        return; 
      }
    }
  }
}

void startAlarmSound(byte initVolume) {
    if (manualAlarmPlay) {
      //Override and stop any manual alarm sounds from MQTT/API
      manualAlarmPlay = false;
      manualAlarmStartTime = 0;
      manualAlarmDuration = 0;
    } else {
      //Capture current display brightness and set to max bright variable
      overrideBrightness = true;
      holdBrightness = activeBrightness;
      overrideTimer = millis();  //brighten screen for 30 seconds (25 + default 5 seconds)
      setBrightness(defaultBrightness);
    }
    currentAlarmState = ALARM_RINGING;
    flushTouchBuffer();                
    touchCooldownTime = millis() + 500;  
    toggleAlarmButtons(true);
    sdPlayer.volume(initVolume);
    delay(500);
    sdPlayer.loop(activeSDAlarmTrack);
    delay(500);
    bool dummy = updatePrimaryAlarmState("on");
}

void stopAlarmSound() {
  sdPlayer.stop();
  if ((overrideBrightness) && (!manualAlarmPlay)) {
    //Set override time to 2 seconds
    overrideTimer = millis();// + 2000; 
  }
  manualAlarmPlay = false;
  manualAlarmStartTime = 0;
  manualAlarmDuration = 0;
  toggleAlarmButtons(false);
  bool dummy = updatePrimaryAlarmState("off");
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
      WiFiClientSecure https;   //must use https
      https.setInsecure();
      HTTPClient http;
      String serverPath = "https://api.openweathermap.org/data/3.0/onecall?lat=" + owmLat + "&lon=" + owmLong + "&exclude=minutely,hourly,daily,alerts&appid=" + owmKey;
      http.setTimeout(5000);
      http.useHTTP10(true);
      http.begin(https, serverPath);
      int httpCode = http.GET();
      if (httpCode == 200) {
        retVal = true;
      }
      http.end();
    }
  }
  return retVal;
}

int getTemperature(bool refreshData) {
  uint32_t tmpPeriod = (tempUpdatePeriod * 60000);  //Convert minutes to milliseconds
  uint32_t curTime = millis();
  int initTemp = externalTemperature;
  int retVal = initTemp;
  if (((curTime - lastTempUpdate) >= tmpPeriod) || (refreshData)) {
    lastTempUpdate = curTime;
    float rtemp = -99.9;

    if ((weatherSource == 1) && (owmKey != "NA") && (owmKey != "")) {
      String serverPath = "https://api.openweathermap.org/data/3.0/onecall?lat=" + owmLat + "&lon=" + owmLong + "&exclude=minutely,hourly,daily,alerts&appid=" + owmKey + "&units=";
      if (activeTempUnits == 12) {
        serverPath += "metric";
      } else {
        serverPath += "imperial";
      }
      WiFiClientSecure https;
      https.setInsecure();
      HTTPClient http;
      http.setTimeout(5000);
      http.useHTTP10(true);
      if (http.begin(https, serverPath)) {
        int httpCode = http.GET();
        #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
          Serial.println("OWM API return code: " + String(httpCode));
        #endif
        if (httpCode > 299) {
          Stream& responseStream = http.getStream();
          JsonDocument doc;
          DeserializationError jsonErr = deserializeJson(doc, responseStream);
          if (jsonErr) {
            #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
              Serial.print("OWM JSON Error: ");
              Serial.println(jsonErr.c_str());
            #endif
          } else {
            //String errMsg = doc["message"];
            #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
              Serial.println("OWM API Error: " + errMsg);
            #endif
          }
          retVal = 0;
        } else if (httpCode > 0) {
          Stream& responseStream = http.getStream();
          JsonDocument doc;
          DeserializationError jsonErr = deserializeJson(doc, responseStream);
          if (jsonErr) {
            #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
              Serial.print("OWM JSON Error: ");
              Serial.println(jsonErr.c_str());
            #endif
          } else {
            rtemp = doc["current"]["temp"] | 0.0;
            #if defined(SERIAL_DEBUG) && (SERIAL_DEBUG == 1)
              Serial.print("OWM Temperature Received: ");
              Serial.println(rtemp);
            #endif
          }
        } else {
          rtemp = 0.0;
        }
        retVal = round(rtemp);
        http.end();
      } else {
        retVal = 0;
      }
    } else {
      //Assume API or MQTT updated temperature and just return set value
      retVal = externalTemperature;
    }
    if ((retVal != initTemp) && (mqttConnected) && (!isBooting)) { 
      updateMQTT("temperature");
    }
  }
  return retVal;
}

//==============================
// BUTTON AND WIDGET FUNCTIONS
//==============================
//Main page (0) - Alarm and Settings buttons
void initAlarmButtons() {
  if (snoozeTime > 0) {
    //Snooze
    btnAlarmSnooze.initButtonUL(10, tft.height()-90, tft.width()/2 - 20, 80, TFT_RED, TFT_GREEN, TFT_WHITE, "SNOOZE", 1);
    btnAlarmSnooze.setPressAction(alarmSnoozePress);
    //Stop
    btnAlarmStop.initButtonUL(tft.width()/2 + 10, tft.height()-90, tft.width()/2 - 20, 80, TFT_RED, TFT_RED, TFT_WHITE, " STOP ", 1);
    btnAlarmStop.setPressAction(alarmStopPress);
    //Stop Alt - shown during snooze
    btnAlarmStopAlt.initButton(tft.width()/2, tft.height()-50, tft.width()/2 - 20, 80, TFT_RED, TFT_RED, TFT_WHITE, " STOP ", 1);
    btnAlarmStopAlt.setPressAction(alarmStopPress);
  } else {
    //Only show stop button
    btnAlarmStop.initButton(tft.width()/2, tft.height()-50, tft.width()/2 - 20, 80, TFT_RED, TFT_RED, TFT_WHITE, " STOP ", 1);
    btnAlarmStop.setPressAction(alarmStopPress);
  }
  //Settings button - shown on main page with alarm buttons
  btnSettings.initButtonUL(tft.width() - 80, 0, 80, 40, TFT_BLACK, TFT_BLACK, TFT_WHITE, " g", 1);
  btnSettings.setPressAction(settingsPagePress);
}
void toggleAlarmButtons(bool show) {
  if (show) {
    if (alarmButtonsOn) {
      //Alt Stop button (snoozing) may be shown, so blank out area
      tft.fillRect(0, tft.height()-90, tft.width(), 80, TFT_BLACK);
    }
    alarmButtonsOn = true;
    if ((currentAlarmState == ALARM_SNOOZING) || (snoozeTime == 0)) {
      btnAlarmStopAlt.drawButton();
    } else {
      btnAlarmSnooze.drawButton();
      btnAlarmStop.drawButton();
    }
  } else {
    alarmButtonsOn = false;
    tft.fillScreen(TFT_BLACK);
    //Force date redraw
    initSync = true;
  }
}

void alarmSnoozePress() {
  sdPlayer.stop();
  flushTouchBuffer();
  touchCooldownTime = millis() + 500;
  if (snoozeTime > 0) {
    snoozeEndTime = millis() + ((unsigned long)snoozeTime * 60000UL);
    currentAlarmState = ALARM_SNOOZING;
    if (useGentleWake) gentleWakeActiveVol = 0;
    toggleAlarmButtons(true);
    bool dummy = updatePrimaryAlarmState("off");
  } else {
    alarmStopPress();
  }
  if (overrideBrightness) {
    overrideTimer = millis();
  }
}

void alarmStopPress() {
  sdPlayer.stop();
  flushTouchBuffer();
  touchCooldownTime = millis() + 500;
  currentAlarmState = ALARM_IDLE;
  currentAlarmIndex = -1;
  if (useGentleWake) gentleWakeActiveVol = 0;
  toggleAlarmButtons(false);
  bool dummy = updatePrimaryAlarmState("off");
  if (overrideBrightness) {
    overrideTimer = millis();
  }
}

void settingsPagePress() {
  flushTouchBuffer();
  touchCooldownTime = millis() + 500;
  activePage = 1;
}
void flushTouchBuffer() {
  // Read repeatedly until no touches are reported
  // Use a safety counter to prevent an infinite loop if hardware is stuck
  int safetyCount = 0;
  while (touch.touched(GT911_MODE_POLLING) > 0 && safetyCount < 10) {
    // We just read the status to clear the register flags.
    GTPoint p = touch.getPoint(0); 
    delay(20); // Small delay to let I2C clear
    safetyCount++;
  }
}
//-----------------------------------
//  Settings Page (1) Widgets/buttons
//-----------------------------------
void initSettingsButtons() {
  btnSettingsBack.initButtonUL(tft.width() - 80, 0, 80, 40, TFT_BLACK, TFT_BLACK, TFT_RED, " i", 1);
  btnSettingsBack.setPressAction(settingsBackPress);
  //Bulb Switches
  btnSettingsBulbOff.initButton(tft.width()/4 + 10, 70, 150, 60, TFT_BLACK, TFT_LIGHTGREY, TFT_BLACK, "OFF", 1);
  btnSettingsBulbOff.setPressAction(setBulbState);
  btnSettingsBulbOn.initButton(tft.width()/4 + 10, 70, 150, 60, TFT_BLACK, TFT_GREENYELLOW, TFT_BLACK, "ON", 1);
  btnSettingsBulbOn.setPressAction(setBulbState);
  //LED Switches
  btnSettingsLEDsOff.initButton(tft.width() - ((tft.width()/4) + 10), 70, 150, 60, TFT_BLACK, TFT_LIGHTGREY, TFT_BLACK, "OFF", 1);
  btnSettingsLEDsOff.setPressAction(setLEDState);
  btnSettingsLEDsOn.initButton(tft.width() - ((tft.width()/4) + 10), 70, 150, 60, TFT_BLACK, TFT_GREENYELLOW, TFT_BLACK, "ON", 1);
  btnSettingsLEDsOn.setPressAction(setLEDState);
  //Display Brightness & Auto-Dim
  btnSettingsAutoDimOn.initButton(60, 190, 40, 45, TFT_GREEN, TFT_BLACK, TFT_GREEN, "j", 1);
  btnSettingsAutoDimOn.setPressAction(setAutoDimPress);
  btnSettingsAutoDimOff.initButton(60, 190, 40, 45, TFT_BLACK, TFT_BLACK, TFT_RED, "i", 1);
  btnSettingsAutoDimOff.setPressAction(setAutoDimPress);
  //Alarm Settings Button
  btnSettingsAlarmSetup.initButtonUL((tft.width()/2) - 90, tft.height() - 70, 180, 60, TFT_BLACK, TFT_DARKCYAN, TFT_BLACK, "ALARMS", 1);
  btnSettingsAlarmSetup.setPressAction(showAlarmListPage);
  //Brightness Slider
  param.slotWidth = 9;           // Note: ends of slot will be rounded and anti-aliased
  param.slotLength = 250;        // Length includes rounded ends
  param.slotColor = TFT_BLUE;    // Slot colour
  param.slotBgColor = TFT_BLACK; // Slot background colour for anti-aliasing
  param.orientation = H_SLIDER;  // sets it "true" for horizontal

  // Slider control knob parameters (smooth rounded rectangle)
  param.knobWidth = 15;          // Always along x axis
  param.knobHeight = 25;         // Always along y axis
  param.knobRadius = 5;          // Corner radius
  param.knobColor = TFT_WHITE;   // Anti-aliased with slot backgound colour
  param.knobLineColor = TFT_RED; // Colour of marker line (set to same as knobColor for no line)

  // Slider range and movement speed
  param.sliderLT = 0;            // Left side for horizontal, top for vertical slider
  param.sliderRB = 255;          // Right side for horizontal, bottom for vertical slider
  param.startPosition = 0;       // Start position for control knob
  param.sliderDelay = 0;         // Microseconds per pixel movement delay (0 = no delay)
}

void drawSettingsPage() {
  //Set default brightness, but save autoDim if set
  if ((millis() >= lastSettingsSync) || (initSettingsSync)) {
    lastSettingsSync = millis() + 1000;
    holdEditBrightness = activeBrightness;
    if (initSettingsSync) {
      if ((autoDimActive) || (holdAutoDim)) {
        holdAutoDim = true;
        autoDimActive = false;
        setBrightness(defaultBrightness);
      } else {
        holdAutoDim = false;
        setBrightness(defaultBrightness);
      }
      //Draw initial screen fixed text
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
      tft.drawCentreString("BULB", tft.width()/4, 15, 4);
      tft.drawCentreString("LEDS", tft.width() - (tft.width()/4), 15, 4);
      tft.drawCentreString("Display Brightness", tft.width()/2, 120, 4);
      tft.drawCentreString("Auto-Dim", 55, 140, 4);

      //set GFXFF font
      tft.setFreeFont(&icomoon20pt7b);
      btnSettingsBack.drawButton(false, " i");
      btnSettingsAlarmSetup.drawButton();
      //Draw controls
      if (bulbState) {
        btnSettingsBulbOn.drawButton();
      } else {
        btnSettingsBulbOff.drawButton();
      }
      if (ledState) {
        btnSettingsLEDsOn.drawButton();
      } else {
        btnSettingsLEDsOff.drawButton();
      }
      if (holdAutoDim) {
        btnSettingsAutoDimOn.drawButton();
      } else {
        btnSettingsAutoDimOff.drawButton();
      }
      sldBrightness.drawSlider(130, 175, param);
      sldBrightness.setSliderPosition(holdEditBrightness);
      initSettingsSync = false;
      brightChange = false;
    } else if (lightStateChange) {
      tft.drawString(" ", 0, 0, GFXFF);
      if (bulbState) {
        btnSettingsBulbOn.drawButton();
      } else {
        btnSettingsBulbOff.drawButton();
      }
      if (ledState) {
        btnSettingsLEDsOn.drawButton();
      } else {
        btnSettingsLEDsOff.drawButton();
      }
      lightStateChange = false;
    } else if (dimStateChange) {
      tft.fillRect(60, 210, 40, 45, TFT_BLACK);
      if (holdAutoDim) {
        btnSettingsAutoDimOn.drawButton();
      } else {
        btnSettingsAutoDimOff.drawButton();
      }
      dimStateChange = false;
    }
  }
}

void settingsBackPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  //Allow longer time so that main setting press doesn't get registered.
  lastPage = activePage;
  activePage = 0;
  initSync = true;  //Force full page refresh
  if (holdAutoDim) {
    //Restore autoDim
    autoDimActive = true;
    holdAutoDim = false;
    //force refresh
    activeThresholdMin = 0;
    activeThresholdMax = 0;
  } else {
    activeBrightness = sldBrightness.getSliderPosition();
    setBrightness(activeBrightness);
  } 
}

void setBulbState() {
  //Calls primary controller API
  bool retVal = false;
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  
  if (primConnected) {
    //send command to toggle bulb state
    String serverPath = "http://" + primIPAddress + "/api";
    String requestData = "";
    if (bulbState) {
      requestData = "bulbstate=0";
    } else {
      requestData = "bulbstate=1";
    }
    WiFiClient client;
    HTTPClient http;
    http.useHTTP10(true);
    http.setTimeout(2000);
    http.begin(client, serverPath);
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

void setLEDState() {
  //Calls primary controller API
  bool retVal = false;
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  
  if (primConnected) {
    //send command to toggle LEDs state
    String serverPath = "http://" + primIPAddress + "/api";
    String requestData = "";
    if (ledState) {
      requestData = "ledstate=0";
    } else {
      requestData = "ledstate=1";
    }
    WiFiClient client;
    HTTPClient http;
    http.useHTTP10(true);
    http.setTimeout(2000);
    http.begin(serverPath);
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

void setAutoDimPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  
  //Check state of holding var, since autoDimming disabled for settings page
  if (holdAutoDim) {
    //Auto Dimming enabled.  Toggle off by setting both hold and active vars
    holdAutoDim = false;
    autoDimActive = false;
  } else {
    //Toggle on, but only set holding var.  Active var will be set when returning to main page
    holdAutoDim = true;
  }
  dimStateChange = true;
}

void processSliderChange(uint16_t position) {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  //Shorter time so slider is more "smooth".
  if (position > 255) position = 255;
  setBrightness(position);
  brightChange = true;
}

void showAlarmListPage() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  activePage = 2;
}

//-----------------------------------
//  Alarms Page (2) Widgets/buttons
//-----------------------------------
void initAlarmListButtons() {
  btnAlarmListBack.initButtonUL(tft.width() - 80, 0, 80, 40, TFT_BLACK, TFT_BLACK, TFT_RED, " i", 1);
  btnAlarmListBack.setPressAction(alarmListBackPress);

  btnAlarm1Active.initButtonUL(20, 60, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREEN, " j", 1);
  btnAlarm1Active.setPressAction(alarm1ActivePress);
  btnAlarm1Inactive.initButtonUL(20, 60, 40, 40, TFT_BLACK, TFT_BLACK, TFT_RED, " X", 1);
  btnAlarm1Inactive.setPressAction(alarm1ActivePress);
  btnAlarm1Edit.initButtonUL(420, 60, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarm1Edit.setPressAction(alarm1EditPress);

  btnAlarm2Active.initButtonUL(20, 105, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREEN, " j", 1);
  btnAlarm2Active.setPressAction(alarm2ActivePress);
  btnAlarm2Inactive.initButtonUL(20, 105, 40, 40, TFT_BLACK, TFT_BLACK, TFT_RED, " X", 1);
  btnAlarm2Inactive.setPressAction(alarm2ActivePress);
  btnAlarm2Edit.initButtonUL(420, 105, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarm2Edit.setPressAction(alarm2EditPress);

  btnAlarm3Active.initButtonUL(20, 150, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREEN, " j", 1);
  btnAlarm3Active.setPressAction(alarm3ActivePress);
  btnAlarm3Inactive.initButtonUL(20, 150, 40, 40, TFT_BLACK, TFT_BLACK, TFT_RED, " X", 1);
  btnAlarm3Inactive.setPressAction(alarm3ActivePress);
  btnAlarm3Edit.initButtonUL(420, 150, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarm3Edit.setPressAction(alarm3EditPress);

  btnAlarm4Active.initButtonUL(20, 195, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREEN, " j", 1);
  btnAlarm4Active.setPressAction(alarm4ActivePress);
  btnAlarm4Inactive.initButtonUL(20, 195, 40, 40, TFT_BLACK, TFT_BLACK, TFT_RED, " X", 1);
  btnAlarm4Inactive.setPressAction(alarm4ActivePress);
  btnAlarm4Edit.initButtonUL(420, 195, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarm4Edit.setPressAction(alarm4EditPress);

  btnAlarm5Active.initButtonUL(20, 240, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREEN, " j", 1);
  btnAlarm5Active.setPressAction(alarm5ActivePress);
  btnAlarm5Inactive.initButtonUL(20, 240, 40, 40, TFT_BLACK, TFT_BLACK, TFT_RED, " X", 1);
  btnAlarm5Inactive.setPressAction(alarm5ActivePress);
  btnAlarm5Edit.initButtonUL(420, 240, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarm5Edit.setPressAction(alarm5EditPress);

  btnAlarmErrAckn.initButton(tft.width()/2, tft.height()/2 + 100, 220, 60, TFT_BLACK, TFT_BLACK, TFT_RED, "CONFIRM", 1);
  btnAlarmErrAckn.setPressAction(alarmErrAcknPress);
}

void drawAlarmListPage() {
  if ((millis() >= lastSettingsSync) || (initSettingsSync)) {
    lastSettingsSync = millis() + 1000;
    if (initSettingsSync) {
      initSettingsSync = false;
      //Draw initial screen fixed text
      tft.setCursor(0, 0, 4);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
      tft.drawCentreString("ACTV", 50, 20, 4);
      tft.drawCentreString("DATE", 135, 20, 4);
      tft.drawCentreString("TIME", 260, 20, 4);
      tft.drawCentreString("RPT", 355, 20, 4);

      //edit buttons
      tft.setFreeFont(&icomoon20pt7b);
      btnAlarm1Edit.drawButton(false);
      btnAlarm2Edit.drawButton(false);
      btnAlarm3Edit.drawButton(false);
      btnAlarm4Edit.drawButton(false);
      btnAlarm5Edit.drawButton(false);
      tft.setTextFont(4);
      tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
      
      for (int i=0; i < MAX_ALARMS; i++) {
        char shortDate[9];
        char shortTime[8];
        formatDateForDisplay(alarmArray[i].date, shortDate); 
        if (activeHourFormat == 12) {
          formatTimeForDisplay(alarmArray[i].time, shortTime);
          tft.drawString(shortTime, 235, ((i+1) * 45) + 20, 4);
        } else {
          tft.drawString(alarmArray[i].time, 235, ((i+1) * 45) + 20, 4);
        }
        if (alarmArray[i].repeat > 0) {
          String repeatDesc = "";
          tft.drawString("  ---  ", 100, ((i+1) * 45) + 20, 4);
          switch (alarmArray[i].repeat) {
            case 1:
              repeatDesc = "Su";
              break;
            case 2:
              repeatDesc = "Mo";
              break;
            case 3:
              repeatDesc = "Tu";
              break;
            case 4:
              repeatDesc = "We";
              break;
            case 5:
              repeatDesc = "Th";
              break;
            case 6:
              repeatDesc = "Fr";
              break;
            case 7:
              repeatDesc = "Sa";
              break;
            case 8:
              repeatDesc = "WD";
              break;
            case 9:
              repeatDesc = "WE";
              break;
            default:
              repeatDesc = "";
              break;
          } 
          tft.drawString(repeatDesc, 340, ((i+1) * 45) + 20, 4);
        } else {
          tft.drawString(shortDate, 100, ((i+1) * 45) + 20, 4);
        }
      }
      //set GFXFF font and draw buttons
      tft.setFreeFont(&icomoon20pt7b);
      btnAlarmListBack.drawButton(false, " i");
      for (int i=0; i < MAX_ALARMS; i++) {
        if (alarmArray[i].active) {
          activeBtns[i]->drawButton(false, " j");
        } else {
          inactiveBtns[i]->drawButton(false, " X");
        }
      }
    }
  }
}

void alarmListBackPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  //Allow longer time so that main setting press doesn't get registered.
  //Save current alarm array to LittleFS
  if (!saveAlarms()) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(4);
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW);
    tft.drawCentreString("WARNING!", tft.width()/2, 60, 4);
    tft.setTextSize(1);
    tft.drawCentreString("Alarms could not be saved.", tft.width()/2, 120, 4);
    tft.drawCentreString("Check web application.", tft.width()/2, 155, 4);
    tft.setFreeFont(&icomoon20pt7b);
    btnAlarmErrAckn.drawButton(true);
    tft.setTextFont(4);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);

  } else {
    lastPage = activePage;
    activePage = 1;
    initSync = true;  //Force full page refresh
  }

}

void alarm1ActivePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  
  if (alarmArray[0].active) {
    alarmArray[0].active = 0;
    inactiveBtns[0]->drawButton(false, " X");
  } else {
    alarmArray[0].active = 1;
    activeBtns[0]->drawButton(false, " j");
  }
}

void alarm2ActivePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  
  if (alarmArray[1].active) {
    alarmArray[1].active = 0;
    inactiveBtns[1]->drawButton(false, " X");
  } else {
    alarmArray[1].active = 1;
    activeBtns[1]->drawButton(false, " j");
  }
}

void alarm3ActivePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  
  if (alarmArray[2].active) {
    alarmArray[2].active = 0;
    inactiveBtns[2]->drawButton(false, " X");
  } else {
    alarmArray[2].active = 1;
    activeBtns[2]->drawButton(false, " j");
  }
}

void alarm4ActivePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  
  if (alarmArray[3].active) {
    alarmArray[3].active = 0;
    inactiveBtns[3]->drawButton(false, " X");
  } else {
    alarmArray[3].active = 1;
    activeBtns[3]->drawButton(false, " j");
  }
}

void alarm5ActivePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  
  if (alarmArray[4].active) {
    alarmArray[4].active = 0;
    inactiveBtns[4]->drawButton(false, " X");
  } else {
    alarmArray[4].active = 1;
    activeBtns[4]->drawButton(false, " j");
  }
}

void alarm1EditPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  alarmEditNum = 0;
  activePage = 3;
}
void alarm2EditPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  alarmEditNum = 1;
  activePage = 3;
}
void alarm3EditPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  alarmEditNum = 2;
  activePage = 3;
}
void alarm4EditPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  alarmEditNum = 3;
  activePage = 3;
}
void alarm5EditPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  alarmEditNum = 4;
  activePage = 3;
}

void alarmErrAcknPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  //Allow longer time so that main setting press doesn't get registered.
  lastPage = activePage;
  activePage = 1;
  initSync = true;  //Force full page refresh

}

void formatDateForDisplay(const char* input, char* output) {
  int y, m, d;
  if (sscanf(input, "%d-%d-%d", &y, &m, &d) == 3) {
    // Format into mm/dd/yyyy (using %02d to ensure leading zeros)
      sprintf(output, "%02d/%02d/%02d", m, d, (y % 100));
  }
}

void formatTimeForDisplay(const char* input, char* output) {
  int h, m;
  //Convert 24-hour to 12-hour and add a/p
  if (sscanf(input, "%d:%d", &h, &m) == 2) {
    if (h > 12) {
      sprintf(output, "%02d:%02d p", (h-12), m);
    } else if (h == 12) {
      sprintf(output, "%02d:%02d p", h, m);
    } else {
      sprintf(output, "%02d:%02d a", h, m);
    }
  }
}

//-------------------------------------
//  Alarm Edit Page (3) Widgets/buttons
//-------------------------------------
void initAlarmEditButtons() {
  btnAlarmEditDate.initButtonUL(400, 35, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarmEditDate.setPressAction(alarmEditDatePress);
  btnAlarmEditTime.initButtonUL(400, 105, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarmEditTime.setPressAction(alarmEditTimePress);
  btnAlarmEditRepeat.initButtonUL(400, 175, 40, 40, TFT_BLACK, TFT_BLACK, TFT_GREENYELLOW, " g", 1);
  btnAlarmEditRepeat.setPressAction(alarmEditRepeatPress);
  btnAlarmEditSave.initButtonUL(tft.width()/2 - 190, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_GREEN, "SAVE", 1);
  btnAlarmEditSave.setPressAction(alarmEditSavePress);
  btnAlarmEditCancel.initButtonUL(tft.width()/2 + 10, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_RED, "CANCEL", 1);
  btnAlarmEditCancel.setPressAction(alarmEditCancelPress);
}

void drawAlarmEditPage() {
  if ((millis() >= lastSettingsSync) || (initSettingsSync)) {
    lastSettingsSync = millis() + 1000;

    if (initSettingsSync) {
      char shortDate[9];
      char shortTime[8];
      initSettingsSync = false;
      String repeatVal="None";
      //Copy current alarm into staging bucket if not from edit page
      if (!alarmEdited) {
        stagingAlarm = alarmArray[alarmEditNum];
      } else {
        alarmEdited = false;
      }

      //Draw initial screen fixed text
      tft.setCursor(0, 0, 4);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
      tft.drawString("DATE:", 10, 40, 4);
      tft.drawString("TIME:", 10, 110, 4);
      tft.drawString("REPEAT:", 10, 180, 4);
      tft.setTextSize(2);
      if (stagingAlarm.repeat > 0) {
        tft.setTextSize(1);
        tft.drawString("Repeats", 130, 42, 4);
        tft.setTextSize(2);
      } else {
        formatDateForDisplay(stagingAlarm.date, shortDate);
        tft.drawString(shortDate, 130, 30, 4);
      }
      if (activeHourFormat == 12) {
        formatTimeForDisplay(stagingAlarm.time, shortTime);
        tft.drawString(shortTime, 130, 100, 4);  
      } else {
        tft.drawString(stagingAlarm.time, 130, 100, 4);  
      }
      tft.setTextSize(1);
      tft.drawString(getRepeatText(stagingAlarm.repeat), 130, 180, 4);
      //Draw buttons
      tft.setFreeFont(&icomoon20pt7b);
      btnAlarmEditDate.drawButton(false);
      btnAlarmEditTime.drawButton(false);
      btnAlarmEditRepeat.drawButton(false);
      btnAlarmEditSave.drawButton(true);
      btnAlarmEditCancel.drawButton(true);
      tft.setTextFont(4);
      tft.setTextColor(TFT_WHITE, TFT_BLACK, true);      
    }
  }
}

void alarmEditDatePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  //alarmEditNum = 0;
  activePage = 4;
}

void alarmEditTimePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  //alarmEditNum = 0;
  activePage = 5;
}

void alarmEditRepeatPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  lastPage = activePage;
  //alarmEditNum = 0;
  activePage = 6;
}

void alarmEditSavePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;
  if (stagingAlarm.repeat > 0) {
    //Default date to today for a repeating alarm (month is zero-based, so add 1)
    sprintf(stagingAlarm.date, "%04d-%02d-%02d", rtc.getYear(), rtc.getMonth()+1, rtc.getDay());
  }
  alarmArray[alarmEditNum] = stagingAlarm;
  alarmArray[alarmEditNum].active = 1;  //Default to active
  lastPage = activePage;
  initSync = true;
  activePage = 2;
}

void alarmEditCancelPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  //Allow longer time so that main setting press doesn't get registered.
  lastPage = activePage;
  activePage = 2;
  initSync = true;  //Force full page refresh

}

String getRepeatText(byte alarmNo) {
  String repeatVal;
  switch (alarmNo) {
    case 0:
      repeatVal = "None";
      break;
    case 1:
      repeatVal = "Every Sunday";
      break;
    case 2:
      repeatVal = "Every Monday";
      break;
    case 3:
      repeatVal = "Every Tuesday";
      break;
    case 4:
      repeatVal = "Every Wednesday";
      break;
    case 5:
      repeatVal = "Every Thursday";
      break;
    case 6:
      repeatVal = "Every Friday";
      break;
    case 7:
      repeatVal = "Every Saturday";
      break;
    case 8:
      repeatVal = "Weekdays (M-F)";
      break;
    case 9:
      repeatVal = "Weekends (Sa/Sun)";
      break;
    default:
      repeatVal = "?";
      break;
  } 
  return repeatVal;
}

//-------------------------------------
//  Alarm Edit Date (4) Widgets/buttons
//-------------------------------------
void initAlarmEditDateButtons() {
  tft.setCursor(0, 0);
  btnAlarmEditMonthPlus.initButton(170, 40, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "+", 1);
  btnAlarmEditMonthPlus.setLabelDatum(-2, 2, MC_DATUM);
  btnAlarmEditMonthPlus.setPressAction(btnAlarmEditMonthPlusPress);
  btnAlarmEditMonth.initButton(170, 95, 45, 45, TFT_BLACK, TFT_BLACK, TFT_WHITE, "02", 1);
  btnAlarmEditMonth.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMonthMinus.initButton(170, 140, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "-", 1);
  btnAlarmEditMonthMinus.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMonthMinus.setPressAction(btnAlarmEditMonthMinusPress);
  btnAlarmEditDayPlus.initButton(240, 40, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "+", 1);
  btnAlarmEditDayPlus.setLabelDatum(-2, 2, MC_DATUM);
  btnAlarmEditDayPlus.setPressAction(btnAlarmEditDayPlusPress);
  btnAlarmEditDay.initButton(240, 95, 45, 45, TFT_BLACK, TFT_BLACK, TFT_WHITE, "08", 1);
  btnAlarmEditDay.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditDayMinus.initButton(240, 140, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "-", 1);
  btnAlarmEditDayMinus.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditDayMinus.setPressAction(btnAlarmEditDayMinusPress);
  btnAlarmEditYearPlus.initButton(310, 40, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "+", 1);
  btnAlarmEditYearPlus.setLabelDatum(-2, 2, MC_DATUM);
  btnAlarmEditYearPlus.setPressAction(btnAlarmEditYearPlusPress);
  btnAlarmEditYear.initButton(310, 95, 45, 45, TFT_BLACK, TFT_BLACK, TFT_WHITE, "26", 1);
  btnAlarmEditYear.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditYearMinus.initButton(310, 140, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "-", 1);
  btnAlarmEditYearMinus.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditYearMinus.setPressAction(btnAlarmEditYearMinusPress);
  btnAlarmEditDateTomorrow.initButtonUL(tft.width()/2 - 125, 185, 250, 50, TFT_BLACK, TFT_BLACK, TFT_SKYBLUE, "TOMORROW", 1);
  btnAlarmEditDateTomorrow.setPressAction(btnAlarmEditDateTomorrowPress);
  btnAlarmEditDateUpdate.initButtonUL(tft.width()/2 - 190, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_GREEN, "UPDATE", 1);
  btnAlarmEditDateUpdate.setPressAction(btnAlarmEditDateUpdatePress);
  btnAlarmEditDateCancel.initButtonUL(tft.width()/2 + 10, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_RED, "CANCEL", 1);
  btnAlarmEditDateCancel.setPressAction(btnAlarmEditDateCancelPress);
}

void drawAlarmEditDatePage() {
  if ((millis() >= lastSettingsSync) || (initSettingsSync)) {
    lastSettingsSync = millis() + 1000;
    if (initSettingsSync) {
      initSettingsSync = false;
      char b[5];  //temp var for converting ints to strings for drawButton() functions
      sscanf(stagingAlarm.date, "%d-%d-%d", &editDateDraft.year, &editDateDraft.month, &editDateDraft.day);
      //Draw static text
      tft.setCursor(0, 0, 4);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE);
      tft.drawString("DATE:", 30, 80, 4);
      tft.drawString("/", 205, 80, 4);
      tft.drawString("/", 275, 80, 4);
      //Draw button widgets
      tft.setFreeFont(&icomoon20pt7b);
      sprintf(b, "%02d", editDateDraft.month);
      btnAlarmEditMonthPlus.drawButton(false);
      btnAlarmEditMonth.drawButton(false, b);
      btnAlarmEditMonthMinus.drawButton(false);
      sprintf(b, "%02d", editDateDraft.day);
      btnAlarmEditDayPlus.drawButton(false);
      btnAlarmEditDay.drawButton(false, b);
      btnAlarmEditDayMinus.drawButton(false);
      sprintf(b, "%02d", (editDateDraft.year % 100));
      btnAlarmEditYearPlus.drawButton(false);
      btnAlarmEditYear.drawButton(false, b);
      btnAlarmEditYearMinus.drawButton(false);
      btnAlarmEditDateTomorrow.drawButton(true);
      btnAlarmEditDateUpdate.drawButton(true);
      btnAlarmEditDateCancel.drawButton(true);
    }
  }
}

void btnAlarmEditMonthPlusPress() {
  char b[3];
  flushTouchBuffer();

  if (editDateDraft.month < 12) { 
    editDateDraft.month++;  
  } else {
    editDateDraft.month = 1;
  }
  validateDay();
  sprintf(b, "%02d", editDateDraft.month);
  btnAlarmEditMonth.drawButton(false, b);
}

void btnAlarmEditMonthMinusPress() {
  char b[3];
  flushTouchBuffer();

  if (editDateDraft.month > 1) { 
    editDateDraft.month--;  
  } else {
    editDateDraft.month = 12;
  }
  validateDay();
  sprintf(b, "%02d", editDateDraft.month);
  btnAlarmEditMonth.drawButton(false, b);
}

void btnAlarmEditDayPlusPress() {
  char b[3];
  flushTouchBuffer();
  editDateDraft.day++;
  if (editDateDraft.day > getDaysInMonth(editDateDraft.month, editDateDraft.year)) {
    editDateDraft.day = 1;
  }
  sprintf(b, "%02d", editDateDraft.day);
  btnAlarmEditDay.drawButton(false, b);

}

void btnAlarmEditDayMinusPress() {
  char b[3];
  flushTouchBuffer();
  editDateDraft.day--;
  if (editDateDraft.day < 1) {
    editDateDraft.day = getDaysInMonth(editDateDraft.month, editDateDraft.year); 
  }
  sprintf(b, "%02d", editDateDraft.day);
  btnAlarmEditDay.drawButton(false, b);
}

void btnAlarmEditYearPlusPress() {
  char b[5];
  flushTouchBuffer();
  if (editDateDraft.year < 2099) {
    editDateDraft.year++;
  }
  validateDay();
  sprintf(b, "%02d", (editDateDraft.year % 100));
  btnAlarmEditYear.drawButton(false, b);
}

void btnAlarmEditYearMinusPress() {
  char b[5];
  flushTouchBuffer();
  if (editDateDraft.year > 2000) {
    editDateDraft.year--;
  }
  validateDay();
  sprintf(b, "%02d", (editDateDraft.year % 100));
  btnAlarmEditYear.drawButton(false, b);

}

void btnAlarmEditDateTomorrowPress() {
  int m, d, y, ldom;
  char b[5];
  m = rtc.getMonth() + 1;  //0-based
  d = rtc.getDay();
  y = rtc.getYear();
  ldom = getDaysInMonth(m, y);
  //Add one day, rolling over month or year if needed
  if (d < ldom) {
    d++;
  } else {
    d = 1;
    if (m < 12) {
      m++;
    } else {
      m = 1;
      y++;
    }
  }
  editDateDraft.day = d;
  editDateDraft.month = m;
  editDateDraft.year = y;
  sprintf(b, "%02d", m);
  btnAlarmEditMonth.drawButton(false, b);
  sprintf(b, "%02d", d);
  btnAlarmEditDay.drawButton(false, b);
  sprintf(b, "%02d", (y % 100));
  btnAlarmEditYear.drawButton(false, b);
}

void btnAlarmEditDateUpdatePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  //Allow longer time so that prior page cancel press doesn't get registered.
  sprintf(stagingAlarm.date, "%04d-%02d-%02d", editDateDraft.year, editDateDraft.month, editDateDraft.day);
  alarmEdited = true;
  lastPage = activePage;
  activePage = 3;
  initSync = true;  //Force full page refresh

}

void btnAlarmEditDateCancelPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 1000;  //Allow longer time so that main setting press doesn't get registered.
  lastPage = activePage;
  activePage = 3;
  initSync = true;  //Force full page refresh
}

int getDaysInMonth(int m, int y) {
  if (m == 2) {
    // Leap year logic: divisible by 4, but not 100 (unless also divisible by 400)
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) return 29;
    return 28;
  }
  // April, June, September, November have 30 days
  if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
  return 31;
}

void validateDay() {
  char b[3];
  int maxDays = getDaysInMonth(editDateDraft.month, editDateDraft.year);
  if (editDateDraft.day > maxDays) {
    editDateDraft.day = maxDays; // Snap to 28, 29, or 30
    sprintf(b, "%02d", editDateDraft.day);
    btnAlarmEditDay.drawButton(false, b);
  }
}

//-------------------------------------
//  Alarm Edit Time (5) Widgets/buttons
//-------------------------------------
void initAlarmEditTimeButtons() {
  tft.setCursor(0, 0);
  btnAlarmEditHourPlus.initButton(150, 40, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "+", 1);
  btnAlarmEditHourPlus.setLabelDatum(-2, 2, MC_DATUM);
  btnAlarmEditHourPlus.setPressAction(btnAlarmEditHourPlusPress);
  btnAlarmEditHour.initButton(150, 95, 45, 45, TFT_BLACK, TFT_BLACK, TFT_WHITE, "02", 1);
  btnAlarmEditHour.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditHourMinus.initButton(150, 140, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "-", 1);
  btnAlarmEditHourMinus.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditHourMinus.setPressAction(btnAlarmEditHourMinusPress);
  btnAlarmEditMinutePlus.initButton(220, 40, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "+", 1);
  btnAlarmEditMinutePlus.setLabelDatum(-2, 2, MC_DATUM);
  btnAlarmEditMinutePlus.setPressAction(btnAlarmEditMinutePlusPress);
  btnAlarmEditMinute.initButton(220, 95, 45, 45, TFT_BLACK, TFT_BLACK, TFT_WHITE, "08", 1);
  btnAlarmEditMinute.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMinuteMinus.initButton(220, 140, 45, 45, TFT_WHITE, TFT_BLACK, TFT_WHITE, "-", 1);
  btnAlarmEditMinuteMinus.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMinuteMinus.setPressAction(btnAlarmEditMinuteMinusPress);
  btnAlarmEditAM.initButtonUL(290, 72, 45, 45, TFT_WHITE, TFT_BLACK, TFT_GREEN, "O", 1);
  btnAlarmEditAM.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditAM.setPressAction(btnAlarmEditAMPress);
  btnAlarmEditPM.initButtonUL(370, 72, 45, 45, TFT_WHITE, TFT_BLACK, TFT_GREEN, "O", 1);
  btnAlarmEditPM.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditPM.setPressAction(btnAlarmEditPMPress);
  btnAlarmEditMinute00.initButtonUL(36, 180, 75, 50, TFT_WHITE, TFT_BLACK, TFT_RED, ":00", 1);
  btnAlarmEditMinute00.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMinute00.setPressAction(btnAlarmEditMinute00Press);
  btnAlarmEditMinute15.initButtonUL(147, 180, 75, 50, TFT_WHITE, TFT_BLACK, TFT_BLUE, ":15", 1);
  btnAlarmEditMinute15.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMinute15.setPressAction(btnAlarmEditMinute15Press);
  btnAlarmEditMinute30.initButtonUL(258, 180, 75, 50, TFT_WHITE, TFT_BLACK, TFT_GREEN, ":30", 1);
  btnAlarmEditMinute30.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMinute30.setPressAction(btnAlarmEditMinute30Press);
  btnAlarmEditMinute45.initButtonUL(369, 180, 75, 50, TFT_WHITE, TFT_BLACK, TFT_ORANGE, ":45", 1);
  btnAlarmEditMinute45.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditMinute45.setPressAction(btnAlarmEditMinute45Press);
  btnAlarmEditTimeUpdate.initButtonUL(tft.width()/2 - 190, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_GREEN, "UPDATE", 1);
  btnAlarmEditTimeUpdate.setPressAction(btnAlarmEditTimeUpdatePress);
  btnAlarmEditTimeCancel.initButtonUL(tft.width()/2 + 10, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_RED, "CANCEL", 1);
  btnAlarmEditTimeCancel.setPressAction(btnAlarmEditTimeCancelPress);
}

void drawAlarmEditTimePage() {
  if ((millis() >= lastSettingsSync) || (initSettingsSync)) {
    lastSettingsSync = millis() + 1000;
    if (initSettingsSync) {
      initSettingsSync = false;
      char b[3];  //temp var for converting ints to strings for drawButton() functions
      sscanf(stagingAlarm.time, "%d:%d:%d", &editTimeDraft.h, &editTimeDraft.m, &editTimeDraft.s);
      if (editTimeDraft.h > 11) {
        isPM = true;
        if (editTimeDraft.h > 12) {
          editTimeDraft.h = (editTimeDraft.h - 12);
        }
      }
      //Draw static text
      tft.setCursor(0, 0, 4);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE);
      tft.drawString("Time:", 30, 80, 4);
      tft.drawString(":", 185, 80, 4);
      tft.drawString("AM", 290, 35, 4);
      tft.drawString("PM", 370, 35, 4);
      //Draw button widgets
      tft.setFreeFont(&icomoon20pt7b);
      sprintf(b, "%02d", editTimeDraft.h);
      btnAlarmEditHourPlus.drawButton(false);
      btnAlarmEditHour.drawButton(false, b);
      btnAlarmEditHourMinus.drawButton(false);
      sprintf(b, "%02d", editTimeDraft.m);
      btnAlarmEditMinutePlus.drawButton(false);
      btnAlarmEditMinute.drawButton(false, b);
      btnAlarmEditMinuteMinus.drawButton(false);
      if (isPM) {
        btnAlarmEditAM.drawButton(false, " ");
        btnAlarmEditPM.drawButton(false, "j");
      } else {
        btnAlarmEditAM.drawButton(false, "j");
        btnAlarmEditPM.drawButton(false, " ");
      }
      btnAlarmEditMinute00.drawButton(false);
      btnAlarmEditMinute15.drawButton(false);
      btnAlarmEditMinute30.drawButton(false);
      btnAlarmEditMinute45.drawButton(false);
      btnAlarmEditTimeUpdate.drawButton(true);
      btnAlarmEditTimeCancel.drawButton(true);
    }
  }
}

void btnAlarmEditHourPlusPress() {
  char b[3];
  flushTouchBuffer();
  if (editTimeDraft.h == 12) {
    editTimeDraft.h = 1;
  } else {
    editTimeDraft.h++;
  }
  sprintf(b, "%02d", editTimeDraft.h);
  btnAlarmEditHour.drawButton(false, b);
}

void btnAlarmEditHourMinusPress() {
  char b[3];
  flushTouchBuffer();
  if (editTimeDraft.h == 1) {
    editTimeDraft.h = 12;
  } else {
    editTimeDraft.h--;
  }
  sprintf(b, "%02d", editTimeDraft.h);
  btnAlarmEditHour.drawButton(false, b);
}

void btnAlarmEditMinutePlusPress() {
  char b[3];
  flushTouchBuffer();
  if (editTimeDraft.m == 59) {
    editTimeDraft.m = 0;
  } else {
    editTimeDraft.m++;
  }
  sprintf(b, "%02d", editTimeDraft.m);
  btnAlarmEditMinute.drawButton(false, b);
}

void btnAlarmEditMinuteMinusPress() {
  char b[3];
  flushTouchBuffer();
  if (editTimeDraft.m == 0) {
    editTimeDraft.m = 59;
  } else {
    editTimeDraft.m--;
  }
  sprintf(b, "%02d", editTimeDraft.m);
  btnAlarmEditMinute.drawButton(false, b);
}

void btnAlarmEditAMPress() {
  btnAlarmEditAM.drawButton(false, "j");
  btnAlarmEditPM.drawButton(false, " ");
  isPM = false;
}

void btnAlarmEditPMPress() {
  btnAlarmEditPM.drawButton(false, "j");
  btnAlarmEditAM.drawButton(false, " ");
  isPM = true;

}

void btnAlarmEditMinute00Press() {
  editTimeDraft.m = 0;
  btnAlarmEditMinute.drawButton(false, "00");
}

void btnAlarmEditMinute15Press() {
  editTimeDraft.m = 15;
  btnAlarmEditMinute.drawButton(false, "15");
}

void btnAlarmEditMinute30Press() {
  editTimeDraft.m = 30;
  btnAlarmEditMinute.drawButton(false, "30");  
}

void btnAlarmEditMinute45Press() {
  editTimeDraft.m = 45;
  btnAlarmEditMinute.drawButton(false, "45");
}

void btnAlarmEditTimeCancelPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  //Allow longer time so that main setting press doesn't get registered.
  lastPage = activePage;
  activePage = 3;
  initSync = true;  //Force full page refresh
}

void btnAlarmEditTimeUpdatePress() {
  int newHour = 0;
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  //Allow longer time so that prior page cancel press doesn't get registered.
  //Convert hours back to 24-hour
  if ((isPM) && (editTimeDraft.h < 12)) {
    newHour = editTimeDraft.h + 12; 
  } else {
    newHour = editTimeDraft.h;
  }
  sprintf(stagingAlarm.time, "%02d:%02d:%02d", newHour, editTimeDraft.m, 0);
  alarmEdited = true;
  lastPage = activePage;
  activePage = 3;
  initSync = true;  //Force full page refresh
}

//----------------------------------------
//  Alarm Edit Repeat (6) Widgets/buttons
//----------------------------------------
void initAlarmEditRepeatButtons() {
  tft.setCursor(0, 0);
  btnAlarmEditRepeatNone.initButton(tft.width()/2, 40, 125, 75, TFT_WHITE, TFT_BLACK, TFT_WHITE, "NONE", 1);
  btnAlarmEditRepeatNone.setLabelDatum(-2, 2, MC_DATUM);
  btnAlarmEditRepeatNone.setPressAction(btnAlarmEditRepeatNonePress);

  btnAlarmEditRepeatMo.initButtonUL(10, 100, 80, 50, TFT_WHITE, TFT_BLACK, TFT_WHITE, "MO", 1);
  btnAlarmEditRepeatMo.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatMo.setPressAction(btnAlarmEditRepeatMoPress);
  btnAlarmEditRepeatTu.initButtonUL(100, 100, 80, 50, TFT_WHITE, TFT_BLACK, TFT_WHITE, "TU", 1);
  btnAlarmEditRepeatTu.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatTu.setPressAction(btnAlarmEditRepeatTuPress);
  btnAlarmEditRepeatWe.initButtonUL(190, 100, 80, 50, TFT_WHITE, TFT_BLACK, TFT_WHITE, "WE", 1);
  btnAlarmEditRepeatWe.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatWe.setPressAction(btnAlarmEditRepeatWePress);
  btnAlarmEditRepeatTh.initButtonUL(280, 100, 80, 50, TFT_WHITE, TFT_BLACK, TFT_WHITE, "TH", 1);
  btnAlarmEditRepeatTh.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatTh.setPressAction(btnAlarmEditRepeatThPress);
  btnAlarmEditRepeatFr.initButtonUL(370, 100, 80, 50, TFT_WHITE, TFT_BLACK, TFT_WHITE, "FR", 1);
  btnAlarmEditRepeatFr.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatFr.setPressAction(btnAlarmEditRepeatFrPress);
  btnAlarmEditRepeatSa.initButtonUL(10, 180, 80, 50, TFT_WHITE, TFT_BLACK, TFT_WHITE, "SA", 1);
  btnAlarmEditRepeatSa.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatSa.setPressAction(btnAlarmEditRepeatSaPress);
  btnAlarmEditRepeatSu.initButtonUL(100, 180, 80, 50, TFT_WHITE, TFT_BLACK, TFT_WHITE, "SU", 1);
  btnAlarmEditRepeatSu.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatSu.setPressAction(btnAlarmEditRepeatSuPress);
  btnAlarmEditRepeatWD.initButtonUL(190, 180, 100, 50, TFT_WHITE, TFT_BLACK, TFT_CYAN, "M-F", 1);
  btnAlarmEditRepeatWD.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatWD.setPressAction(btnAlarmEditRepeatWDPress);
  btnAlarmEditRepeatWE.initButtonUL(300, 180, 100, 50, TFT_WHITE, TFT_BLACK, TFT_PINK, "S-S", 1);
  btnAlarmEditRepeatWE.setLabelDatum(0, 0, MC_DATUM);
  btnAlarmEditRepeatWE.setPressAction(btnAlarmEditRepeatWEPress);
  btnAlarmEditRepeatUpdate.initButtonUL(tft.width()/2 - 190, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_GREEN, "UPDATE", 1);
  btnAlarmEditRepeatUpdate.setPressAction(btnAlarmEditRepeatUpdatePress);
  btnAlarmEditRepeatCancel.initButtonUL(tft.width()/2 + 10, tft.height()-70, 180, 50, TFT_BLACK, TFT_BLACK, TFT_RED, "CANCEL", 1);
  btnAlarmEditRepeatCancel.setPressAction(btnAlarmEditRepeatCancelPress);
}

void drawAlarmEditRepeatPage() {
  if ((millis() >= lastSettingsSync) || (initSettingsSync)) {
    lastSettingsSync = millis() + 1000;
    if (initSettingsSync) {
      initSettingsSync = false;
      editRepeatDraft = stagingAlarm.repeat;
      //Draw static text
      tft.setCursor(0, 0, 4);
      tft.setTextSize(1);
      tft.setTextColor(TFT_WHITE);
      tft.drawString("Repeat", 15, 15, 4);
      tft.drawString("Alarm:", 15, 40, 4);
      //Draw button widgets
      tft.setFreeFont(&icomoon20pt7b);
      updateAlarmEditRepeatButtons();
      btnAlarmEditRepeatUpdate.drawButton(true);
      btnAlarmEditRepeatCancel.drawButton(true);
    }
  }  
}

void updateAlarmEditRepeatButtons() {
  for (int i=0; i < 10; i++) {
    if (i == editRepeatDraft) {
      repeatBtns[i]->drawButton(true);
    } else {
      repeatBtns[i]->drawButton(false);
    }
  }
}

void btnAlarmEditRepeatNonePress() {
  editRepeatDraft = 0;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatMoPress() {
  editRepeatDraft = 2;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatTuPress() {
  editRepeatDraft = 3;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatWePress() {
  editRepeatDraft = 4;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatThPress() {
  editRepeatDraft = 5;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatFrPress() {
  editRepeatDraft = 6;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatSaPress() {
  editRepeatDraft = 7;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatSuPress() {
  editRepeatDraft = 1;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatWDPress() {
  editRepeatDraft = 8;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatWEPress() {
  editRepeatDraft = 9;
  updateAlarmEditRepeatButtons();
}

void btnAlarmEditRepeatUpdatePress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  //Allow longer time so that prior page cancel press doesn't get registered.
  stagingAlarm.repeat = editRepeatDraft;
  alarmEdited = true;
  lastPage = activePage;
  activePage = 3;
  initSync = true;  //Force full page refresh
}

void btnAlarmEditRepeatCancelPress() {
  flushTouchBuffer();
  delay(100);
  touchCooldownTime = millis() + 500;  //Allow longer time so that main setting press doesn't get registered.
  alarmEdited = false;
  lastPage = activePage;
  activePage = 3;
  initSync = true;  //Force full page refresh
}
//================================
// API FUNCTIONS
//================================
//--------------------------
// Handle incoming requests
//--------------------------
void handleAPI() {

  bool actionTaken = false;
  bool alreadyProxied = server.hasArg("proxied");  //flag to prevent infinite loop between controllers
  String forwardedResponse = "";

  if (server.argName(0) == "config") {
    //Config data from primary controller - for INTERNAL USE ONLY - do NOT call via API
    if (server.hasArg("plain")) {
      String payload = server.arg("plain");
      JsonDocument doc;
      DeserializationError dsError = deserializeJson(doc, payload);
      if (!dsError) {
        //explicitely set String type to avoid compiler errors
        mqttAddr_1 = doc["mqtt_addr_1"];
        mqttAddr_2 = doc["mqtt_addr_2"];
        mqttAddr_3 = doc["mqtt_addr_3"];
        mqttAddr_4 = doc["mqtt_addr_4"];
        mqttPort = doc["mqtt_port"];
        mqttUser = String(doc["mqtt_user"]);
        mqttPW = String(doc["mqtt_pw"]);
        mqttTopicSub = String(doc["mqtt_topic_sub"]);
        mqttTopicPub = String(doc["mqtt_topic_pub"]);
        mqttTelePeriod = doc["mqtt_tele_period"];
        defaultTempUnits = doc["temp_units"];
        weatherSource = doc["weather_source"];
        owmKey = String(doc["owm_key"]);
        owmLat = String(doc["own_lat"]);
        owmLong = String(doc["owm_long"]);
        tempUpdatePeriod = doc["temp_update_period"];
        server.send(200, "text/plain", "OK");
        writeConfigFile(true);
        return;
      }
    }
  }
  //--------------------------------------
  //These API commands can be safely used
  //--------------------------------------
  //Ping - just test to see if controller is alive - can only be used standalone and not combined with other params
  if (server.argName(0) == "ping") {
    server.send(200, "text/plain", "OK"); 
    return;
  }
  //Get IP address - returns controller IP as string.  Standalone only, cannot be combined with other params
  if ((server.argName(0) == "ipaddr") || (server.argName(0) == "displayipaddr")) {
    server.send(200, "text/plain", baseIPAddress);
    return;
  }
  //Get MAC address - returns controller MAC as string.  Standalone only, cannot be combined with other params
  if ((server.argName(0) == "macaddr") || (server.argName(0) == "displaymac")) {
    server.send(200, "text/plain", strMacAddr);
    return;
  }
  //Manually set the time - can only be used standalone.  All params must be included and be valid
  if (server.argName(0) == "settime") {
    if (server.hasArg("yr") && server.hasArg("mon") && server.hasArg("day") && 
        server.hasArg("hr") && server.hasArg("min") && server.hasArg("sec")) {
        
      int yr  = server.arg("yr").toInt();
      int mon = server.arg("mon").toInt();
      int day = server.arg("day").toInt();
      int hr  = server.arg("hr").toInt();
      int min = server.arg("min").toInt();
      int sec = server.arg("sec").toInt();

      // Range validation before calling the setter
      if (yr >= 2025 && yr < 2100 && mon >= 1 && mon <= 12 && day >= 1 && day <= 31 && 
          hr >= 0 && hr <= 23 && min >= 0 && min <= 59 && sec >= 0 && sec <= 59) {
          
        manualTimeSet(sec, min, hr, day, mon, yr);
        server.send(200, "text/plain", "OK - Time Set");
      } else {
        server.send(400, "text/plain", "Validation Error - Invalid Date/Time Range");
      }
    } else {
      server.send(400, "text/plain", "Incomplete Time Args");
    }
    return;
  }

  //Set an alarm - can only be used standalone and not combined with other params
  if (server.argName(0) == "setalarm") {
    if (server.hasArg("alarmnum") && server.hasArg("time") && server.hasArg("date") && server.hasArg("repeat")) {
      JsonDocument tempDoc;
      tempDoc["active"] = (server.arg("setalarm").toInt() == 1);
      tempDoc["alarmnum"] = server.arg("alarmnum").toInt();
      tempDoc["time"] = server.arg("time");
      tempDoc["date"] = server.arg("date");
      tempDoc["repeat"] = server.arg("repeat").toInt();
      if (processAlarmCommand(tempDoc)) {
        server.send(200, "text/plain", "OK");
      } else {
        server.send(400, "text/plain", "Validation Error");
      }
    } else {
      server.send(400, "text/plain", "Incomplete Alarm Args");
    }
    return;      
  }

  // Alarm Active Toggle - can only be used standalone and not combined with other params
  if (server.argName(0) == "alarmactive") {
    if (server.hasArg("active")) {
      if (updateAlarmActiveState(server.arg("alarmactive").toInt(), ((server.arg("active").toInt() == 1) || (server.arg("active").equalsIgnoreCase("on"))))) {
        server.send(200, "text/plain", "OK");
      } else server.send(400, "text/plain", "Invalid Alarm Index");
    } else server.send(400, "text/plain", "Missing or invalid Active Arg");
    return;
  }

  //Reboot display (this) controller (all other reboots, include restartall, handled by primary controller)
  if (server.argName(0) == "displayrestart") {
      server.send(200, "text/plain", "OK - Display rebooting");
      delay(500); 
      ESP.restart();
      return; // Important to exit here
  }  

  //All other commands can be combined (e.g. /api?autodim=0&dispbrightness=128...)
  for (int i = 0; i < server.args(); i++) {
    String k = server.argName(i);
    String v = server.arg(i);
    if (k == "proxied") continue;
    if (k == "dispsaveconfig") { rebootRequired = true; actionTaken = true; }
    else if (processCommand(k, v)) actionTaken = true;
  }

  if ((!actionTaken) && (!alreadyProxied) && (primConnected)) {
    forwardedResponse = forwardAPIRequest(primIPAddress);
    if (forwardedResponse != "") {
      actionTaken = true;
    }
  }

  if (actionTaken) {
    if (forwardedResponse != "") {
      server.send(200, "text/plain", forwardedResponse);
    } else {
      server.send(200, "text/plain", rebootRequired ? "OK - Rebooting" : "OK");
    }
  } else {
    server.send(400, "text/plain", "Unknown Command (display)");
  }
}

String forwardAPIRequest(String targetIP) {
  String response = "";
  WiFiClient client;
  HTTPClient http;

  // Rebuild the query string from the current request
  String queryString = "";
  for (int i = 0; i < server.args(); i++) {
    if (i > 0) queryString += "&";
    queryString += server.argName(i) + "=" + server.arg(i);
  }

  // Add the circuit breaker flag
  if (queryString.length() > 0) queryString += "&";
  queryString += "proxied=1";

  String targetURL = "http://" + targetIP + "/api?" + queryString;

  http.setTimeout(3000); // 3-second safety timeout
  http.begin(client, targetURL);
  int httpCode = http.GET();

  // If the Primary Controller returns 200 OK, we consider it a success
  if (httpCode == 200) {
    response = http.getString();
  }

  http.end();
  return response;
}

//-------------------------------------
// Outgoing (Primary Controller) Calls
//-------------------------------------
bool primaryIsConnected(String ipAddr) {
  bool retVal = false;
  String serverPath = "http://" + ipAddr + "/api";
  String requestData = "ping=1";
  WiFiClient client;
  HTTPClient http;
  http.useHTTP10(true);
  http.setTimeout(2000);
  http.begin(client, serverPath);
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

bool sendIPAddress(String primAddr) {
  //Sends display IP to primary controller - will set primary's "dispConnected" to true
  bool retVal = false;
  String serverPath = "http://" + primAddr + "/api";
  String requestData = "displayaddr=" + baseIPAddress;
  WiFiClient client;
  HTTPClient http;
  http.useHTTP10(true);
  http.setTimeout(2000);
  http.begin(client, serverPath);
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

bool updatePrimaryAlarmState(String state) {
  bool retVal = false;
  if (primConnected) {
    String serverPath = "http://" + primIPAddress + "/api";
    String requestData = "alarmstate=" + state;
    WiFiClient client;
    HTTPClient http;
    http.useHTTP10(true);
    http.setTimeout(2000);
    http.begin(serverPath);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int response = http.POST(requestData);
    if (response > 0) {
      if (http.getString() = "OK") {
        retVal = true;
      }
    }
    http.end();
  }
  return retVal;  
}


bool importMQTTData(String ipAddr) {
  bool retVal = false;
  String serverPath = "http://" + ipAddr + "/api";
  String requestData = "mqttdata=1";
  WiFiClient client;
  HTTPClient http;
  http.useHTTP10(true);
  http.setTimeout(3000);
  http.begin(client, serverPath);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int response = http.POST(requestData);
  if (response > 0) {
    JsonDocument doc;
    String payload = http.getString();
    auto deserializeError = deserializeJson(doc, payload);
    if (deserializeError) {
      retVal = false;
    } else {
      mqttAddr_1 = doc["mqtt_addr_1"];
      mqttAddr_2 = doc["mqtt_addr_2"];
      mqttAddr_3 = doc["mqtt_addr_3"];
      mqttAddr_4 = doc["mqtt_addr_4"];
      mqttPort = doc["mqtt_port"];
      mqttUser = String(doc["mqtt_user"] | "");
      mqttPW = String(doc["mqtt_pw"] | "");
      mqttTopicSub = String(doc["mqtt_topic_sub"] | "");
      mqttTopicPub = String(doc["mqtt_topic_pub"] | "");
      mqttTelePeriod = doc["mqtt_tele_period"];      //in seconds
      retVal = true;
    }
  } else {
    retVal = false;
  }
  http.end();
  return retVal;
}

bool importTempWeatherData(String ipAddr) {
  bool retVal = false;
  String serverPath = "http://" + ipAddr + "/api";
  String requestData = "weatherdata=1";
  WiFiClient client;
  HTTPClient http;
  http.useHTTP10(true);
  http.setTimeout(3000);
  http.begin(serverPath);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int response = http.POST(requestData);
  if (response > 0) {
    JsonDocument doc;
    String payload = http.getString();
    auto deserializeError = deserializeJson(doc, payload);
    if (deserializeError) {
      retVal = false;
    } else {
      defaultTempUnits = doc["temp_units"];
      weatherSource = doc["weather_source"];
      owmKey = String(doc["owm_key"] | "");
      owmLat = String(doc["owm_lat"] | "");
      owmLong = String(doc["owm_long"] | "");
      tempUpdatePeriod = doc["temp_update_period"];
      retVal = true;
    }
  } else {
    retVal = false;
  }
  http.end();
  return retVal;
}

//================================
//  WEB PAGES AND HANDLERS
//================================
// Most HTML / code found in html.h

//-----------------------
// Web Page Handlers Def
//-----------------------
void setupWebHandlers() {
  //Main page and onboarding
  server.on("/", webMainPage);
  server.on("/onboard", handleOnboard);
  server.on("/jsonmain", webMainPageJson);           //Get main settings page live data
  server.on("/webliveupdate", handleLiveWebUpdate);  //Process immediate updates for "live controls"
  //System Integrations
  server.on("/system", webSystemsPage);
  server.on("/jsonsystems", webSystemsPageJson);
  server.on("/applyintegrate", handleIntegrations);  
  //Display Settings
  server.on("/display", webDisplayPage);
  server.on("/jsondisplay", webDisplayPageJson);
  server.on("/testdisplay", handleDisplayTest);
  server.on("/resetdisplay", handleDisplayReset);
  server.on("/applydisplay", handleDisplaySettings);
  server.on("/applydim",  handleAutoDimSettings);
  server.on("/calibratedim", webDimCalibrate);
  server.on("/setcalbright", handleCalBrightChange);
  server.on("/calibrateupdate", handleCalibration);
  //Clock and Time
  server.on("/clock", webClockPage);
  server.on("/jsonclock", webClockPageJson);
  server.on("/testclock", handleClockTest);
  server.on("/resetclock", handleClockReset);
  server.on("/applyclock", handleClockSettings);
  server.on("/applytime", handleTimeSettings);
  server.on("/settime", handleSetTime);
  //Alarms
  server.on("/alarms", webAlarmPage);
  server.on("/jsonalarms", webAlarmPageJson);
  server.on("/alarmsettings", handleAlarmSettings);
  server.on("/alarmupdate", handleAlarmUpdate);
  server.on("/soundtest", handleSoundTest);
  //Sounds
  server.on("/sounds", webSoundPage);
  server.on("/jsonsounds", webSoundPageJson);
  server.on("/soundupdate", handleSoundUpdate);
  //Controller Functions
  server.on("/restart", webRestartPage);
  server.on("/firmwareupdate", webFirmwareUpdate);
  server.on("/otaupdate", handleOTAUpdate); //OTAUpdate via Arduino IDE
  server.on("/configdump", webConfigDump);
  server.on("/reset", webResetPage);
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

void webMainPageJson() {
  String jsonString;
  JsonDocument doc;
  doc.clear();
  doc["cur_brightness"] = getBrightnessPct(activeBrightness); 
  doc["auto_dim"] = ((autoDim) ? 1 : 0);
  doc["auto_dim_hold"] = ((autoDimActive) ? 1 : 0);
  if (primConnected) {
     doc["prim_ctrl_enabled"] = 1;
     doc["prim_address"] = primIPAddress;
  } else {
     doc["prim_ctrl_enabled"] = 0;
     doc["prim_address"] = "0";
  }

  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleLiveWebUpdate() {
  //Get which control from web hidden field
  String whichControl = server.arg("updatefield");
  //Update values based on which control changed
  if (whichControl == "autodim") {
    autoDimActive = server.arg("autodimval").toInt();
    if (activePage == 0) {
      if (autoDimActive) {
        tft.fillRect(0, 270, tft.width(), tft.height() - 270, TFT_BLACK);
        tft.drawCentreString("Auto-Dim: ENABLED", tft.width()/2, 270, 4);
        delay(1000);
        tft.fillScreen(TFT_BLACK);
        //force update
        activeThresholdMin = 0;
        activeThresholdMax = 0;
        applyDimLevel();
      } else {
        tft.fillRect(0, 270, tft.width(), tft.height() - 270, TFT_BLACK);
        tft.drawCentreString("Auto-Dim: DISABLED", tft.width()/2, 270, 4);
        delay(1000);
        tft.fillScreen(TFT_BLACK);
      }
      initSync = true;
    } else if (activePage == 1) {
      //Settings page on display.  Set holding var and force update.
      holdAutoDim = autoDimActive;
      dimStateChange = true;
    }
    if ((mqttConnected) && (!isBooting)) {
      updateMQTT("autodim");
    }
  } else if (whichControl == "brightness") {
    int newValue = server.arg("brightness").toInt();
    String dispValue = String(newValue);
    newValue = getBrightnessRaw(newValue);  //round((newBrightness / 100) * 255);
    if (newValue > 255) newValue = 255;
    setBrightness(newValue);
  }
  //Refresh page
  webMainPage();
}

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

/* -------------------------------------
    Web Integrations Page and Functions
   ------------------------------------- */
void webSystemsPage() {
    String page;
    page.reserve(strlen(systempage) + 3000);
    page = systempage;
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*IPADDR*", baseIPAddress);
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");  //sends headers
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
}

void webSystemsPageJson() {
  String jsonString;
  JsonDocument doc;
  doc.clear();
  //Primary Controller
  doc["prim_ip_1"] = primIPAddr_1;
  doc["prim_ip_2"] = primIPAddr_2;
  doc["prim_ip_3"] = primIPAddr_3;
  doc["prim_ip_4"] = primIPAddr_4;
  doc["prim_connected"] = ((primConnected) ? 1 : 0);  //connection tested/verified in Setup()
  //Temperature
  doc["temp_units"] = defaultTempUnits;
  //Weather Source
  switch (weatherSource) {
    case 1:
      doc["weather_source"] = "Open Weather Map (OWM)";
      break;
    case 2:
      doc["weather_source"] = "MQTT";
      break;
    case 3:
      doc["weather_source"] = "API";
      break;
    default:
      doc["weather_source"] = "None/Disabled";
      break;
  }
  doc["owm_key"] = owmKey;
  doc["owm_lat"] = owmLat;
  doc["owm_long"] = owmLong;
  doc["owm_connected"] = owmConnected;
  doc["temp_upd_period"] = tempUpdatePeriod;
  //MQTT
  doc["mqtt_addr"] = String(mqttAddr_1) + "." + String(mqttAddr_2) + "." + String(mqttAddr_3) + "." + String(mqttAddr_4);
  doc["mqtt_port"] = mqttPort;
  doc["mqtt_user"] = mqttUser;
  doc["mqtt_pw"] = mqttPW;
  doc["mqtt_pub"] = mqttTopicPub;
  doc["mqtt_sub"] = mqttTopicSub;
  doc["mqtt_period"] = mqttTelePeriod;
  doc["mqtt_enabled"] = ((mqttEnabled) ? 1 : 0);
  if (mqttEnabled) {
      doc["mqtt_connected"] = 1;
  } else {
    doc["mqtt_connected"] = 0;
    mqttConnected = false;
  }
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleIntegrations() {
  String page;
  page.reserve(strlen(postIntegrations) + 500);
  page = postIntegrations;
  bool primSuccess = false;
  bool mqttSuccess = false;
  bool weatherSuccess = false;
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    primIPAddr_1 = server.arg("primip1").toInt();
    primIPAddr_2 = server.arg("primip2").toInt();
    primIPAddr_3 = server.arg("primip3").toInt();
    primIPAddr_4 = server.arg("primip4").toInt();
    if ((primIPAddr_1 == 0) && (primIPAddr_2 == 0) && (primIPAddr_3 == 0) & (primIPAddr_4 == 0)) {
      primSuccess = false;
    } else {
      //Attempt to ping primary controller via API
      primIPAddress = String(primIPAddr_1) + "." + String(primIPAddr_2) + "." + String(primIPAddr_3) + "." + String(primIPAddr_4);
      primSuccess = primaryIsConnected(primIPAddress); 
    }
    //If successfully integrated, attempt to import MQTT, Temp and Weather settings
    if (primSuccess) {
      //import MQTT
      mqttSuccess = importMQTTData(primIPAddress);
      //Load Temp/Weather
      weatherSuccess = importTempWeatherData(primIPAddress);
    }
    //return web page
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    if (primSuccess) page.replace("*PRIMRESULT*", "1");
    if (mqttSuccess) page.replace("*MQTTRESULT*", "1");
    if (weatherSuccess) page.replace("*WEATHERRESULT*", "1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
    delay(1000);
    //Write config to LittleFS and reboot
    writeConfigFile(true);
  }
}

/* -------------------------------------
    Display Settings Page and Functions
   ------------------------------------- */
void webDisplayPage() {
  String page;
  page.reserve(strlen(displaypage) + 3000);
  page = displaypage;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");  //tells browser send is complete

}

void webDisplayPageJson() {
  String jsonString;
  JsonDocument doc;
  doc.clear();
  doc["which_data"] = String(whichData);  //Current data request
  whichData = "all";                      //reset for next request (all is default)
  if (dispHeight > dispWidth) {
    doc["disp_resolution"] = String(dispHeight) + "x" + String(dispWidth);
  } else {
    doc["disp_resolution"] = String(dispWidth) + "x" + String(dispHeight);
  }
  doc["touch_addr"] = touchAddr;
  doc["disp_rotate"] = defaultDispRotate;
  doc["brightness"] = getBrightnessPct(defaultBrightness); 
  doc["touch_enabled"] = ((touchEnabled) ? 1 : 0);
  doc["use_led"] = ((useLED) ? 1 : 0);
  doc["auto_dim"] = ((autoDim) ? 1 : 0);
  doc["dim_debounce"] = autoDimDebounce;

  //Convert global var range (0-255) to a percentage (0-100) for web controls
  doc["amb_level_1"] = ambLightLevel_1; 
  doc["amb_level_2"] = ambLightLevel_2; 
  doc["amb_level_3"] = ambLightLevel_3; 
  doc["amb_level_4"] = ambLightLevel_4; 
  doc["dim_bright_1"] = getBrightnessPct(dimBrightness_1); 
  doc["dim_bright_2"] = getBrightnessPct(dimBrightness_2); 
  doc["dim_bright_3"] = getBrightnessPct(dimBrightness_3); 
  doc["dim_bright_4"] = getBrightnessPct(dimBrightness_4); 
  doc["dim_bright_5"] = getBrightnessPct(dimBrightness_5); 

  doc["disp_test_active"] = ((displayTestFlag) ? 1 : 0);

  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleDisplayTest() {
  if (server.hasArg("plain")) {
    String payload = server.arg("plain");
    JsonDocument doc;
    DeserializationError dsError = deserializeJson(doc, payload);
    if (!dsError) {
      bool activeTest = doc["disp_test_active"];
      byte newRotate = doc["disp_rotate"];
      bool enableTouch = doc["touch_enabled"];
      bool enableLED = doc["use_led"];
      byte rawBright = doc["brightness"];
      uint16_t newBright = getBrightnessRaw(rawBright); 
      if (activeTest) {
        touchEnabledTest = enableTouch;
        displayTestFlag = true;
        activePage = 10;
        delay(500);
        if (enableLED) {
          setDisplayLED(1, 64, 0, 0);
        } else {
          setDisplayLED(0);
        }
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(1);
        analogWrite(DISP_BL_PIN, newBright);  //don't use function as it also set active value
        tft.setRotation(newRotate);
        tft.setTextColor(TFT_WHITE);
        tft.drawCentreString("TOP", tft.width()/2, 20, 4);
        tft.setTextDatum(TR_DATUM);
        tft.drawString("RIGHT", tft.width()-10, tft.height()/2, 4);
        tft.setTextDatum(TL_DATUM);
        tft.drawCentreString("BOTTOM", tft.width()/2, tft.height()-30, 4);
        tft.drawString("LEFT", 20, tft.height()/2, 4);

        //Add pixel pos indicators
        for (int x=10; x < tft.width(); x=x+20) {
          int pos = x;
          if (x < 100) {
            tft.drawString(String(pos), x, 0, 2);
          } else if ((x == 110) || (x == 210) || (x == 310) || (x == 410)) {
            tft.drawString(String(pos), x, 0, 2);
            //Skip next number for spacing
            x = x + 20;
            continue;
          } else {
            if (x > 410) {
              pos = x - 400;
            } else if (x > 310) {
              pos = x - 300;
            } else if (x > 210) {
              pos = x - 200;
            } else if (x > 110) {
              pos = x - 100;
            }
          }
          tft.drawString(String(pos), x, 0, 2);
        }
        for (int y=10; y < tft.height(); y=y+20) {
          tft.drawString(String(y), 0, y, 2);
        }
        activeDispRotate = newRotate;
      } else {
        displayTestFlag = false;
        activePage = 0;
        initSync = true;
        touchEnabledTest = false;
        setDisplayLED(0);
        tft.fillScreen(TFT_BLACK);
        tft.setRotation(defaultDispRotate);
        setBrightness(activeBrightness);
        //Need to reset color back to default here
        activeDispRotate = defaultDispRotate;
      }
      server.send(200, "text/html", "OK");
    } else {
      server.send(500,"text/html", "Could not parse data");
    }
  }
}

void handleDisplayReset() {

  whichData = "disp";
  if (server.hasArg("plain")) {
    String payload = server.arg("plain");
    JsonDocument doc;
    DeserializationError dsError = deserializeJson(doc, payload);
    if (!dsError) {
      setBrightness(activeBrightness);
      tft.fillScreen(TFT_BLACK);
      tft.setRotation(defaultDispRotate);
      tft.setTextColor(activeClockColor);
      server.send(200, "text/html", "OK");
    } else {
      server.send(500,"text/html", "Could not parse data");
    }
  }
  //Force clock refresh
  initSync = true;
  displayTestFlag = false;
  activePage = 0;
}

void handleDisplaySettings() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    String page;
    page.reserve(strlen(postdisplaysettings) + 300);
    page = postdisplaysettings;
    int brightWeb = server.arg("brightness").toInt();
    int newBright = getBrightnessRaw(brightWeb);  
    defaultBrightness = newBright;
    defaultDispRotate = server.arg("disprotate").toInt();
    touchEnabled = server.arg("touchactive").toInt();
    useLED = server.arg("ledactive").toInt();

    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    page.replace("*WHICHONE*", "Display");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");    
    delay(1000);
    //Save config and reboot
    writeConfigFile(true);
  }
}

void handleAutoDimSettings() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    //Convert all brightness levels from 0-100% to 0-255

    //Update global vars here
    autoDim = server.arg("dimactive").toInt();
    autoDimDebounce = server.arg("dimdebounce").toInt();
    ambLightLevel_1 = server.arg("amblevel1").toInt();
    ambLightLevel_2 = server.arg("amblevel2").toInt();
    ambLightLevel_3 = server.arg("amblevel3").toInt();
    ambLightLevel_4 = server.arg("amblevel4").toInt();
    dimBrightness_1 = getBrightnessRaw(server.arg("dimbright1").toInt());
    dimBrightness_2 = getBrightnessRaw(server.arg("dimbright2").toInt());
    dimBrightness_3 = getBrightnessRaw(server.arg("dimbright3").toInt());
    dimBrightness_4 = getBrightnessRaw(server.arg("dimbright4").toInt());
    dimBrightness_5 = getBrightnessRaw(server.arg("dimbright5").toInt());

    String page;
    page.reserve(strlen(postdisplaysettings) + 300);    
    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    page.replace("*WHICHONE*", "Auto-Dim");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");  //sends headers
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
    delay(1000);
    //Save to LittleFS and reboot  
    writeConfigFile(true);
  }
}

void webDimCalibrate() {
  String page;
  page.reserve(strlen(calibrate) + 500);
  page = calibrate;
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

void handleCalBrightChange() {
  byte newBright = server.arg("dispbrighttest").toInt();
  byte rawBright = getBrightnessRaw(newBright);
  setBrightness(rawBright);
  webDimCalibrate();  
}

void handleCalibration() {
    int lightLevel = analogRead(DISP_LDR);
    int curLevel = getAmbientPct(lightLevel); 
    server.send(200, "text/plain", String(curLevel) + "|" + String(getBrightnessPct(activeBrightness)));
}
/* -----------------------------------
    Clock and Time Page and Functions
   ----------------------------------- */
void webClockPage() {
  String page;
  page.reserve(strlen(clockpage) + 3000);
  page = clockpage;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

void webClockPageJson() {
  String jsonString;
  JsonDocument doc;
  doc.clear();
  doc["which_data"] = String(whichData);  //Current data request
  whichData = "all";                      //reset for next request (all is default)
  char webColor[8];
  tftColorToHex(defaultClockColor, webColor);
  doc["clock_style"] = defaultClockStyle;
  doc["clock_color"] = String(webColor); //defaultClockColor;
  doc["hour_format"] = defaultHourFormat;
  doc["time_source"] = timeSource;
  doc["ntp_server"] = ntpServer;
  doc["time_zone"] = timeZone;
  doc["ntp_sync_int"] = ntpSyncInterval;
  doc["mqtt_connected"] = ((mqttConnected) ? 1: 0);
  doc["mqtt_time_topic"] = mqttTimeTopic;
  doc["mqtt_use"] = ((mqttLiveTime) ? 1 : 0);
  doc["api_use"] = ((apiLiveTime) ? 1 :0);
  JsonArray styleArray = doc.createNestedArray("styles");
  for (int i = 0; i < sizeof(clockStyles)/sizeof(clockStyles[0]); i++) {
    JsonObject styleObj = styleArray.createNestedObject();
    styleObj["index_val"] = clockStyles[i].indexNum;
    styleObj["name"] = clockStyles[i].name;
  }
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleClockTest() {
 
  if (server.hasArg("plain")) {
    String requestBody = server.arg("plain");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);
    if (error) {
      server.send(400, "text/plain", "Bad Request: Invalid JSON format");
        return;
    }
    //Set test values
    int rawStyle = doc["new_style"].as<int>();
    String newColor = doc["new_color"];
    uint16_t newTftColor = hexToTftColor(newColor.c_str());
    int newHourFormat = doc["hour_format"].as<int>();
    bool result = applyClockStyle(rawStyle);
    if  (!result) {
      server.send(500, "Issue with getting style");
      return;
    }
    tft.setTextColor(newTftColor);
    activeClockColor = newTftColor;
    activeHourFormat = doc["hour_format"].as<int>();
    initSync = true;  //Force clock update (otherwise, it won't update until next minute change)
    if ((mqttConnected) && (!isBooting)) {
      mqttClockColor = newColor;
      updateMQTT("clockcolor");
    }
    server.send(200, "text/plain", "OK");
  } else {
    server.send(418, "text/plain", "Teapot");  
  }
}
void handleClockReset() {
  activeClockColor = defaultClockColor;
  activeClockFont = defaultClockFont;
  activeClockSize = defaultClockSize;
  activeHourFormat = defaultHourFormat;  
  if ((mqttConnected) && (!isBooting)) {
    mqttClockColor = server.arg("clockcolor");  //24-bit hex for MQTT
    updateMQTT("clockcolor");
  }
  tft.setTextColor(activeClockColor);
  initSync = true;
  server.send(200, "text/plain", "OK");
}
void handleClockSettings() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    String page;
    page.reserve(strlen(postclocksettings) + 300);
    page = postclocksettings;
    defaultClockStyle = server.arg("clockstyle").toInt();
    defaultClockFont = clockStyles[defaultClockStyle].fontNum;
    defaultClockSize =  clockStyles[defaultClockStyle].fontSize;
    defaultClockColor = hexToTftColor(server.arg("clockcolor").c_str());
    defaultHourFormat = server.arg("hourformat").toInt();

    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    page.replace("*WHICHONE*", "Clock");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
    delay(1000);
    //Save to LittleFS and reboot
    writeConfigFile(true);
  }

}

void handleTimeSettings() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    String page;
    page.reserve(strlen(postclocksettings) + 300);
    page = postclocksettings;

    timeSource = server.arg("timesource").toInt();
    ntpServer = server.arg("ntpserver");
    timeZone = server.arg("timezone");
    ntpSyncInterval = server.arg("syncint").toInt();
    mqttTimeTopic = server.arg("mqtttopic");
    mqttLiveTime = ((server.arg("mqttuse").toInt()) == 1);
    apiLiveTime = ((server.arg("apiuse").toInt()) == 1);

    page.replace("*VAR_APP_NAME*", APPNAME);
    page.replace("*DEVICENAME*", deviceName);
    page.replace("*VAR_CURRENT_VER*", VERSION);
    page.replace("*IPADDR*", baseIPAddress);
    page.replace("*WHICHONE*", "Time");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(page);
    server.sendContent(html_footer);
    server.sendContent("");
    delay(1000);
    //Save to LittleFS and reboot
    writeConfigFile(true);
  }
}

void handleSetTime() {

  if (server.hasArg("plain")) {
    String payload = server.arg("plain");
    JsonDocument doc;
    DeserializationError dsError = deserializeJson(doc, payload);
    if (!dsError) {
      int newYear = doc["newyear"] | 2026;
      int newMonth = doc["newmonth"] | 0;
      int newDay = doc["newday"] | 1;
      int newHour = doc["newhour"] | 0;
      int newMinute = doc["newminute"] | 0;
      //Month from web is zero-based, so add 1
      newMonth++;
      if (activeTimeSource == 1) {
        //disable snpt sync if running
        esp_sntp_stop();
      }
      activeTimeSource == 0;
      manualTimeSet(0, newMinute, newHour, newDay, newMonth, newYear);
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  }
}

/* -----------------------------------
    Alarm Setup Page and Functions
   ----------------------------------- */
void webAlarmPage() {
  String page;
  page.reserve(strlen(alarmpage) + 3000);
  page = alarmpage;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

void webAlarmPageJson() {
  String jsonString;
  JsonDocument doc;
  doc.clear();
  doc["which_data"] = String(whichData);  //Current data request
  whichData = "all";                      //reset for next request (all is default)
  doc["use_sd"] = ((useSDCard) ? 1 : 0);
  doc["alarm_track"] = defaultSDAlarmTrack;
  doc["snooze_time"] = snoozeTime;
  doc["alarm_vol"] = defaultAlarmVol;
  doc["gentle_wake"] = ((useGentleWake) ? 1 : 0);
  //Array for dropdown select
  JsonArray trackArray = doc.createNestedArray("tracks");
  for (int i = 0; i < MAX_SOUNDS; i++) {
    JsonObject trackObj = trackArray.createNestedObject();
    trackObj["index"] = soundLibrary[i].index;
    trackObj["title"] = soundLibrary[i].title;
  }
  //Array of current alarms (read from /alarms.bin during Setup())
  JsonArray curAlarms = doc.createNestedArray("alarms");
  for (int i = 0; i < sizeof(alarmArray)/sizeof(alarmArray[0]); i++) { 
    JsonObject alarmObj = curAlarms.createNestedObject();
    alarmObj["index"] = i + 1;  //make 1 based instead of 0
    alarmObj["active"] = alarmArray[i].active;
    alarmObj["date"] = alarmArray[i].date;
    alarmObj["time"] = alarmArray[i].time;
    alarmObj["repeat"] = alarmArray[i].repeat;
  }
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);
}

void handleAlarmSettings() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method not allowed");
  } else {
    String page;
    page.reserve(strlen(postalarmsettings) + 300);
    page = postalarmsettings;
    useSDCard = server.arg("usesdval").toInt();
    defaultSDAlarmTrack = server.arg("alarmsound").toInt();
    defaultAlarmVol = server.arg("volume").toInt();
    useGentleWake = server.arg("gentlewakeval").toInt();
    snoozeTime = server.arg("snoozetime").toInt();
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
    //Save to LittleFS and reboot
    writeConfigFile(true);
  }
}

void handleAlarmUpdate() {
  if (server.method() != HTTP_POST || !server.hasArg("plain")) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  } 
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    // Send a 400 status back to the client
    server.send(400, "text/plain", "Invalid JSON format.");
    return;
  }  
  //Get the JSON array
  JsonArray alarmsArray = doc.as<JsonArray>();

  if (alarmsArray.size() != MAX_ALARMS) {
    server.send(400, "text/plain", "Alarm count mismatch.");
    return;
  }
  //Process the JSON array into the C++ struct array (alarmArray)
  int i = 0;
  for (JsonObject alarmJson : alarmsArray) {
    
    // Note: alarmJson["active"] will be 1 or 0, which fits into a 'byte'
    alarmArray[i].active = alarmJson["active"].as<byte>();
    alarmArray[i].repeat = alarmJson["repeat"].as<byte>();
   
    // DATE (11 bytes: YYYY-MM-DD\0)
    strncpy(alarmArray[i].date, alarmJson["date"].as<const char*>(), sizeof(alarmArray[i].date) - 1);
    alarmArray[i].date[sizeof(alarmArray[i].date) - 1] = '\0'; // Ensure null termination

    // TIME (9 bytes: HH:MM:SS\0)
    strncpy(alarmArray[i].time, alarmJson["time"].as<const char*>(), sizeof(alarmArray[i].time) - 1);
    alarmArray[i].time[sizeof(alarmArray[i].time) - 1] = '\0'; // Ensure null termination
    
    i++;
  }
  if (saveAlarms()) {
    server.send(200, "text/plain", "Alarms successfully saved.");
  } else {
    server.send(500, "text/plain", "Failed to update saved alarms!");
  }
}

/*----------------------------------
   Sound Library Page and Functions
  ---------------------------------- */
void webSoundPage() {
  String page;
  page.reserve(strlen(soundpage) + 3000);
  page = soundpage; 
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*VAR_CURRENT_VER*", VERSION);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
}

void webSoundPageJson() {
  String jsonString;
  JsonDocument doc;
  JsonArray rows = doc.createNestedArray("tracks");
  for (int i = 0; i < MAX_SOUNDS; i++) {
    JsonObject row = rows.createNestedObject();
    row["index"] = soundLibrary[i].index;
    row["filename"] = soundLibrary[i].filename;
    row["title"] = soundLibrary[i].title;
  }
  serializeJson(doc, jsonString);
  server.send(200, "application/json", jsonString);

}

void handleSoundUpdate() {
  if (server.method() != HTTP_POST || !server.hasArg("plain")) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  }  
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));
  if (error) {
    // Send a 400 status back to the client
    server.send(400, "text/plain", "Invalid JSON format.");
    return;
  }  
  //Get the JSON array
  JsonArray sounds = doc.as<JsonArray>();
  int i = 0;
  for (JsonObject soundJson : sounds) {
    if (i < MAX_SOUNDS) {
      // Note: soundJson["index"] will be 1 - 20, which fits into a 'byte'
      soundLibrary[i].index = soundJson["index"].as<byte>();
      soundLibrary[i].filename = soundJson["filename"].as<String>();
      soundLibrary[i].title = soundJson["title"].as<String>();
      i++;
    }
  }
  if (saveSoundsToSD()) {
    server.send(200, "text/plain", "Sounds successfully saved.");
  } else {
    server.send(500, "text/plain", "Failed to update sound file!");
  }
}

void handleSoundTest() {
  if (server.method() != HTTP_POST || !server.hasArg("plain")) {
    server.send(400, "text/plain", "Invalid Request");
    return;
  }  
  String payload = server.arg("plain");
  JsonDocument doc;
  DeserializationError dsError = deserializeJson(doc, payload);
  if (!dsError) {
    bool startTest = doc["mode"];
    byte testVol = doc["volume"];
    byte testTrack = doc["tracknum"];
    bool testGentleWake = doc["gentlewake"];

    if (startTest) {
      holdAlarmVol = activeAlarmVol;
      holdGentleWake = useGentleWake;
      activeAlarmVol = testVol;
      activePage = 11;
      //Update screen
      tft.fillRect(0, 80, tft.width(), tft.height()-160, TFT_BLACK);
      tft.drawCentreString("Sound Test", tft.width()/2, tft.height()/2, 4);
      if (testGentleWake) {
        
        //Recalc interval
        gentleWakeIncrement = round((testVol * 1.0) / 6.0);
        if (testVol >= 15) {
          sdPlayer.volume(gentleWakeIncrement);
          gentleWakeActiveVol = gentleWakeIncrement;
          delay(500);
          useGentleWake = true;
          lastGentleTime = millis();
        } else {
          useGentleWake = false;
          sdPlayer.volume(testVol);
          delay(500);
        }
      } else {
        useGentleWake = false;
        sdPlayer.volume(testVol);
        delay(500);
      }
      sdPlayer.loop(testTrack);
      alarmSounding = true;
      server.send(200, "text/plain", "Sounds test started.");
    } else {
      tft.fillRect(0, 80, tft.width(), tft.height()-160, TFT_BLACK);
      //testMode = false;
      activePage = 0;
      initSync = true;
      sdPlayer.stop();
      alarmSounding = false;
      //Reset any values changed during testing
      useGentleWake = holdGentleWake;
      activeAlarmVol = holdAlarmVol;
      server.send(200, "text/plain", "Sounds test stopped.");
    }

  } else {
    server.send(500, "text/plain", "Failed to start sound test!");
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
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
  delay(1000);
  ESP.restart();
}

/* --------------------------------------
    Firmware Updgrade Page and Functions
   -------------------------------------- */
void webFirmwareUpdate() {
  String page;
  page.reserve(strlen(updateFirmware) + 500);
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

/* --------------------------------
    Config dump Page and Functions
   -------------------------------- */
void webConfigDump() {
  String page;
  page.reserve(strlen(configpage) + 4000);
  page = configpage;
  page.replace("*DEVICENAME*", deviceName);
  page.replace("*VAR_APP_NAME*", APPNAME);
  page.replace("*IPADDR*", baseIPAddress);
  page.replace("*CONFIGJSON*", getCurrentConfig());
  page.replace("*CONFIGALARM*", getCurrentAlarms());
  page.replace("*CONFIGSOUND*", getCurrentSounds());
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(page);
  server.sendContent(html_footer);
  server.sendContent("");
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
  //Format LittleFS, wipe WiFi and reboot
  LittleFS.begin();
  LittleFS.format();
  LittleFS.end();
  WiFi.disconnect(false, true);
  ESP.restart();
}

//=============================
// MQTT Update (pub) Functions
//=============================
void updateMQTT(String whichField) {
  //Sets flags for updating MQTT from main loop() - avoids thread-safe violations
  if (isBooting) return; // Shield during setup()

  if (whichField == "dispbrightness") mqttUpdateMask |= MSK_BRIGHTNESS;
  else if (whichField == "autodim")    mqttUpdateMask |= MSK_AUTODIM;
  else if (whichField == "clockcolor") mqttUpdateMask |= MSK_COLOR;
  else if (whichField == "alarms")      mqttUpdateMask |= MSK_ALARMS;
  else if (whichField == "temperature") mqttUpdateMask |= MSK_TEMP;
  else if (whichField == "timesync")    mqttUpdateMask |= MSK_TIMESYNC;
  else if (whichField == "gentlewake")  mqttUpdateMask |= MSK_GENTLE;
  else if (whichField == "alarmtrack")  mqttUpdateMask |= MSK_TRACK;
  else if (whichField == "alarmvolume") mqttUpdateMask |= MSK_VOLUME;
  else if (whichField == "snoozetime")  mqttUpdateMask |= MSK_SNOOZE;
}

void handleQueuedMqttUpdates() {
  //Processes queue, with a delay between calls to prvent flooding wifi buffer 
  if (mqttUpdateMask == 0) return;
  
  if (mqttUpdateMask & MSK_TIMESYNC) { executeMqttUpdate("timesync"); mqttUpdateMask &= ~MSK_TIMESYNC; }
  else if (mqttUpdateMask & MSK_TEMP) { executeMqttUpdate("temperature"); mqttUpdateMask &= ~MSK_TEMP; }
  else if (mqttUpdateMask & MSK_ALARMS) { executeMqttUpdate("alarms"); mqttUpdateMask &= ~MSK_ALARMS; }
  else if (mqttUpdateMask & MSK_BRIGHTNESS) { executeMqttUpdate("dispbrightness"); mqttUpdateMask &= ~MSK_BRIGHTNESS; }
  else if (mqttUpdateMask & MSK_AUTODIM) { executeMqttUpdate("autodim"); mqttUpdateMask &= ~MSK_AUTODIM; }
  else if (mqttUpdateMask & MSK_COLOR) { executeMqttUpdate("clockcolor"); mqttUpdateMask &= ~MSK_COLOR; }
  else if (mqttUpdateMask & MSK_GENTLE) { executeMqttUpdate("gentlewake"); mqttUpdateMask &= ~MSK_GENTLE; }
  else if (mqttUpdateMask & MSK_TRACK) { executeMqttUpdate("alarmtrack"); mqttUpdateMask &= ~MSK_TRACK; }
  else if (mqttUpdateMask & MSK_VOLUME) { executeMqttUpdate("alarmvolume"); mqttUpdateMask &= ~MSK_VOLUME; }
  else if (mqttUpdateMask & MSK_SNOOZE) { executeMqttUpdate("snoozetime"); mqttUpdateMask &= ~MSK_SNOOZE; }
  else {
    mqttUpdateMask = 0;
  }
}

void executeMqttUpdate(String whichField) {
  if (mqttConnected) {
    //Test connection and attempt reconnect
    if (mqttIsConnected()) {
      char topicBuf[128];   // Buffer for the full topic string
      char payloadBuf[32]; // Buffer for the converted numbers/bools
      if (whichField == "dispbrightness") {
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/dispbrightness", mqttTopicPub.c_str());
        itoa(activeBrightness, payloadBuf, 10); // Converts integer to string (10 = base 10)
        client.publish(topicBuf, payloadBuf, true); 
        //Include autoDim setting with brightness change
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/autodim", mqttTopicPub.c_str()); 
        client.publish(topicBuf, autoDimActive ? "ON":"OFF", true);
        return;
      }
      if (whichField == "autodim") {  
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/autodim", mqttTopicPub.c_str());
        client.publish(topicBuf, autoDimActive ? "ON":"OFF", true);
        return;
      }
      if (whichField == "timesync") {
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/timesync", mqttTopicPub.c_str());
        client.publish(topicBuf, lastTimeSyncOK ? "OK":"FAIL", true);
        //reset flag
        lastTimeSyncOK = false;
        return;
      }
      if (whichField == "temperature") {
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/temperature", mqttTopicPub.c_str());
        itoa(externalTemperature, payloadBuf, 10);
        client.publish(topicBuf, payloadBuf, true);
        return;
      }
      if (whichField == "clockcolor") {
        // Output as hex string to match other MQTT colors
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/clockcolor", mqttTopicPub.c_str());
        client.publish(topicBuf, mqttClockColor.c_str(), true);
        return;
      }
      if (whichField == "gentlewake") {
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/gentlewake", mqttTopicPub.c_str());
        client.publish(topicBuf, useGentleWake ? "ON":"OFF", true);
        return;
      }
      if (whichField == "alarmtrack") {
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/alarmtrack", mqttTopicPub.c_str());
        itoa(activeSDAlarmTrack, payloadBuf, 10);
        client.publish(topicBuf, payloadBuf, true);
        return;
      }
      if (whichField == "alarmvolume") {
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/alarmvolume", mqttTopicPub.c_str());
        itoa(activeAlarmVol, payloadBuf, 10);
        client.publish(topicBuf, payloadBuf, true);
        return;
      }
      if (whichField == "snoozetime") {
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/snoozetime", mqttTopicPub.c_str());
        itoa(snoozeTime, payloadBuf, 10);
        client.publish(topicBuf, payloadBuf, true);
        return;
      }
      //Alarm JSON array
      if (whichField == "alarms") {
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();
        snprintf(topicBuf, sizeof(topicBuf), "stat/%s/alarms", mqttTopicPub.c_str());
        for (int i = 0; i < MAX_ALARMS; i++) {
          JsonObject obj = array.add<JsonObject>();
          obj["alarmnum"] = i + 1;             // 1-5 index for Home Assistant
          obj["active"] = alarmArray[i].active;
          obj["date"] = alarmArray[i].date; // char[] automatically treated as string
          obj["time"] = alarmArray[i].time;
          obj["repeat"] = alarmArray[i].repeat;
        }
        size_t n = serializeJson(doc, globalMqttBuffer, sizeof(globalMqttBuffer));
        client.publish(topicBuf, (const uint8_t*)globalMqttBuffer, n, true);
        return;
      }
    }
  }
}

void handleInitMqttSync() {
  switch (syncStep) {
    case 0: executeMqttUpdate("dispbrightness"); break;
    case 1: executeMqttUpdate("autodim"); break;
    case 2: executeMqttUpdate("clockcolor"); break;
    case 3: executeMqttUpdate("gentlewake"); break;
    case 4: executeMqttUpdate("alarmtrack"); break;
    case 5: executeMqttUpdate("alarmvolume"); break;
    case 6: executeMqttUpdate("snoozetime"); break;
    case 7: executeMqttUpdate("temperature"); break;
    case 8: executeMqttUpdate("timesync"); break; 
    case 9: executeMqttUpdate("alarms"); break;
    case 10: 
      initialSyncRequired = false; 
      syncStep = -1;
      break;
  }
  syncStep++;  
}
//===============================
//  SD CARD AND FILE FUNCTIONS
//===============================
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
/*
  //This function works to read files from CYD SD card
  //Unfortunately, the DFPlayer does not off this functionality  
  File root = fs.open(dirname);
  if(!root){
    //tft.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    //tft.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  uint16_t vPos = 40;
  while(file){
    if(file.isDirectory()){
      //tft.print("  DIR : ");
      //tft.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      
      tft.print("  FILE: ");
      tft.print(file.name());
      tft.print("  SIZE: ");
      tft.println(file.size());
      
      //tft.drawString("  FILE:  ", 50, vPos, 2);
      tft.drawString("FILE: " + String(file.name()), 100, vPos, 2);
      vPos = vPos + 15;
    }
    file = root.openNextFile();
  }
*/
}


