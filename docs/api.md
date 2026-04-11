# HTTP API Command List
<div align="center">

![api_01](images/api_01.jpg)
</div>

The API command list is pretty similar to the commands available via MQTT.  However, the API does not require any third party components (like a broker) nor does it need to be configured.  You simply issue the command as a URL in your browser:

`http://[your-controller-ip]/api?...`

But which controller IP address do you use?  For most commands, it doesn't matter.  You can use the primary's or display's IP address and the command will be handled properly.  There is currently one commands (PING), that must be sent to a particular controller's IP address.  This is listed under the command's notes.

Also note that most commands can be combined in a single HTTP call.  Additional commands can be concatenated together with the ampersand (&).  For example, to set both the brightness and color of the LED strip using a single API call, combine both commands into a single URL:

`http://[primary-ip-address]?ledbrightness=128&ledcolor=ff0000`

There are some commands that cannot be combined with others.  In addition, most of the following are commands *sent* to the system.  Most commands simply return OK or the current setting value.  Unlike MQTT, the system does not 'send' data back to the browser when a state changes.  However, there are a few commands specifically designed to GET data instead of changing some parameter of the lamp.  These are broken out separately below. 

### SET Commands
These commands are sent to make a 'change' to the system. If the Multi column lists **NO**, then that command cannot be combined with others and must be sent alone as the only API command. If used by the external system, these should be sent as POST commands.

`http://[controller-ip-address]/api?...`

|Command | Parameter(s) | Returns | Multi | Example/Notes
|--------|:------------:|:---------:|:----:|-----
|`alarmactive`|alarmnum&active=n|OK|**NO**|/api?alarmactive=2&active=1<br>Sets active state of specified alarm number
|`alarmtrack`| number (1-20) | OK | Yes |/api?alarmtrack=13<br>Set the active track for alarm sound
|`alarmupdate`|snooze _or_ stop | OK | Yes | /api?alarmupdate=snooze<br>Snoozes or stops active alarm.<br>Ignored if alarm not sounding
|`alarmvolume` | number (0-30) | OK |Yes | /api?alarmvolume=15<br>Set alarm volume or max vol if gentle wake used
|`snoozetime` | number (0-60) | OK | Yes | /api?snoozetime=10<br>Sets snooze time, in minutes.
|`playalarm` | length (0-60) | OK | Yes | /api?playalarm=15<br>Plays current alarm track for specified number of seconds.
|`setalarm` | _see notes below_| OK if set | **NO** | Must be only command sent. See below for stucture and example.
|`bulbbrightness`| number (0-255) | OK | Yes | /api?bulbbrightness=128<br>Sets bulb brightness.  Will also toggle bulb ON.
|`bulbcolor` | RGB _or_ Hex color string | OK | Yes | /api?bulbcolor=0,255,0<br>/api?bulbcolor=00ff00<br>Sets color of the light bulb.  Will also toggle bulb ON and sets color mode to [rgb]
|`bulbstate`| OFF _or_ ON<br>0 _or_ 1| OK | Yes | /api?bulbstate=ON<br>Sets bulb state. Can use OFF/ON or 0/1.
|`ledbrightness`| number (0-255) | OK | Yes | /api?ledbrightness=128<br>Sets LED Strip brightness. Will also toggle LEDs ON.
|`ledcolor` | RGB _or_ Hex color string | OK | Yes | /api?ledcolor=255,0,255<br>/api?ledcolor=ff00ff<br>Set the color of the LEDs.  If using hex string **omit** leading '#' sign.  Will also toggle LEDs ON.
|`ledstate` | OFF _or_ ON<br>0 _or_ 1| OK | Yes | /api?ledstate=ON<br>Sets LED strip state. Can use OFF/ON or 0/1.
|`clockcolor`| RGB _or_ Hex color string | OK | Yes | /api?clockcolor=255,255,255<br>/api?clockcolor=ffffff<br>Set the color of the clock on the touch display.  If using hex string **omit** leading '#' sign.
|`dispbrightness`| number (0-255)<br>_up_<br>_down_ | OK | Yes | /api?dispbrightness=128<br>/api?dispbrightness=up<br>Sets the brightness to a set value if number used.  Use "up" to increase brightness by approximately 10% or "down" to decrease brightness by approximately 10%.<br> _Auto-dim must be disable or brightness will be overwritten._
|`autodim` | OFF _or_ ON<br>0 _or_ 1| OK | Yes | /api?autodim=OFF<br>Sets the active state of the display auto-dim feature.<br>May use ON/OFF or 0/1.
|`settemperature`| number | OK | Yes |api?settemperature=72<br>Temperature source must be MQTT or API. Ignored otherwise.<br>Uses raw value and does not perform any conversion.
| `settime` | _See notes below_ | OK | **NO** | Only applicable if time source is manual. Otherwise time will be overwritten by next sync.<br>_See additional notes below._
|`touch1func` | number (0-8) | OK | Yes | /api?touch1func=1<br>Sets the active primary function of Touch Sensor 1<br>0 = None<br>1=Toggle Light Bulb<br>2 = Toggle LED Strip<br>3 = Increase Bulb Brightness<br>4 = Decrease Bulb Brightness<br>5 = Increase LED Strip Brightness<br>6 = Decrease LED Strip Brightness<br>7 = Increase Display Brightness<br>8 = Decrease Display Brightness
|`touch1funca` | number (0-2) | OK | Yes | /api?touch1funca=2<br>Sets the active alarm function of Touch Sensor 1<br>0 = None<br>1 = Snooze<br>2 = Stop
|`touch2func` | number (0-8) | OK | Yes | /api?touch2func=1<br>Sets the active primary function of Touch Sensor 2<br>Options the same as _touch1func_ above.
|`touch2funca` | number (0-2) | OK | Yes | /api?touch2funca=2<br>Sets the active alarm function of Touch Sensor 2<br>Options the same as _touch1funca_ above.
| `bulbrestart` | 1 | OK | **NO** | /api?bulbrestart=1<br>Restarts the RGBW Light bulb<br>_Must be the only command sent._  Any other params ignored.
| `displayrestart` | 1 | OK | **NO** | /api?displayrestart=1<br>Restarts the display controller<br>_Must be the only command sent._  Any other params ignored.
| `primaryrestart` | 1 | OK | **NO** | /api?rprimaryrestart=1<br>Restarts the primary controller<br>_Must be the only command sent._  Any other params ignored.
| `restartall` | 1 | OK | **NO** | /api?restartall=1<br>Restarts all three controllers (bulb, primary, display)<br>_Must be the only command sent._  Any other params ignored.
| `dispsaveconfig` | 1 | OK | **NO** |**USE WITH EXTREME CAUTION!!!** This command will save ALL applicable _active_ values as the new _default_ boot values.  If the display has auto-dim disable, display color is green at 50% brightness, then auto-dim OFF, and a display with a green color at 50% brightness will become the new boot defaults and will be the state the display is set to whenever the system restarts.  This may lead to unforeseen or undesirable results.  Be absolutely sure all states are what you want for the new startup/boot values before issuing this command.  The controller will also reboot as part of this process.
| `primsaveconfig` | 1 | OK | **NO** |**USE WITH EXTREME CAUTION!!!**  This command saves all primary-related active settings as the new boot defaults.  See the notes for the /dispsaveconfi above as the same warnings apply here.  If executed, note that _both_ the primary and display controllers will reboot.

#### Additional Notes:

***Setting an Alarm***<br>
Setting an alarm via the HTTP API requires a very specific set of parameters:<br>
`/api?setalarm=1&alarmnum=3&date=2026-04-01&time=14:30:00&repeat=7`

- ***setalarm***: 0 - Create alarm but set to inactive;  1 - Create alarm as active
- ***alarmnum***: Specify the alarm slot (1-5) that you wish to create/modify
- ***date***: Specify the date for the alarm.  Must be in YYYY-MM-DD format.
- ***time***: Specify alarm time.  Must be in hh:mm:ss format (_yes, seconds must be included_).
- ***repeat***: Alarm repeat setting.  Valid values are 0-9:
  - 0 = No repeat (none)
  - 1 - 7 = Single day (Sundays=1, Tuesdays=2... Saturdays= 7)
  - 8 = Weekdays (M-F)
  - 9 = Weekends (Sat & Sun)

_Additional Notes_:<br>
• This command must be the only command issued as part of the API call and cannot be combined with other commands.<br>
• Hours must be specfiied in 24-hour (military) format.<br>
• Verify alarm was properly set via the web app or touch display the first few times you try the API.

See the [Alarms](/alarmmain.md) topics for more information on setting and using alarms.

***Setting the Time***<br>
Similar to an alarm, setting the time requires a very specific set of parameters:
`api/settime=1&mon=4&day=15&yr=2026&hr=14&min=30&sec=0`

- ***settime***: 1 (or any non-zero value)
- ***mon***: Current month (1-12)
- ***day***: Current day (1-31)
- ***yr***: Current year (must be full 4-digit year)
- ***hr***: Current hour in 24-hour format (0-23)
- ***min***: current minutes (0-59)
- ***sec***: current seconds (0-59)

_Additional Notes_<br>
• To set the time, the time source should be manual.  Otherwise, the time sent will be overwritten by the next time sync cycle.<br>
• **ALL** parameters are required (even seconds).  If any parameters are missing or invalid, the command will be discarded.

See [Clock and Time Settings](/time.md) for more information on setting the time and selecting a source.

### GET Commands
These commands return data to the browser as a response and do not change any aspect of the lamp or its settings.  These commands cannot be combined and each must be sent individually.  If the external system requires it, these should be sent as GET commands.

|Command | Parameter(s) | Returns | Example/Notes
|--------|:------------:|:---------:|-----
|`ping`| 1 | OK | /api?ping=1<br>This command _must_ be sent to the specific controller's IP address
|`primaryipaddr`|1|IP address<br>(192.168.1.205)| /api?primaryipaddr=1<br>Returns IP address of primary controller.
|`displayipaddr`| 1 |IP address<br>(192.168.1.51)| /api?displayipaddr=1<br>Returns IP address of display controller. 
|`bulbipaddr`| 1 |IP address<br>(192.168.1.52)| /api?bulbipaddr=1<br>Returns IP address of RGBW light bulb. 
|`primarymac`| 1 | MAC Address<br>(00:00:00:00:00)|/api?primarymac=1<br>Returns MAC address of primary controller.
|`displaymac`| 1 | MAC Address<br>(ff:ff:ff:ff:ff)|/api?displaymac=1<br>Returns MAC address of display controller.

_Additional Notes_:<br>
• The ping command must be sent to the controller (primary or display) IP address that you wish to ping<br>
• The bulb's factory firmware does not provide an API call for ping or MAC address.  Use the bulb's web page and/or a standard newtork 'ping' command for the bulb.

### DIRECT Commands
These are special commands, or shortcut versions of some of the above.  Unlike the common commands, most of these need to be sent to a particular controller's IP address:<br>
- Start OTA for Primary Controller: `[primary-controller-ip]/otaupdate`
- Start OTA for Display Contoller: `[display-controller-ip]/otaupdate`

Note the omission of the /api in the URL.  In most cases, unless otherwise indicated, these direct commands also do not use parameters.  In addition, since most of these commands replicate the controller commands found in the web application, the result or entry page of the particular controller is returned to the browser.  When the return value below lists "web page", then directly calling the command will return a web page from the web application.

#### Primary Controller Direct Commands
These should all be sent to the IP address of the **primary** controller

|Command | Parameter(s) | Returns | Example/Notes
|--------|:------------:|:---------:|-----
|`/restart`|_none_ |web page| http://primary_ip/restart<br>Reboots the primary controller.
|`/restartall`|_none_|web page| http://primary_ip/restartall<br>Reboots all three controllers (bulb, primary and display)
|`/otaupdate`|_none_|web page| http://primary_ip/otaupdate<br>Places controller in OTA update mode. See [Using the Arduino IDE](/arduino.md) for more information on OTA updates.

#### Display Controller Direct Commands
These should all be sent to the IP address of the **display** controller

|Command | Parameter(s) | Returns | Example/Notes
|--------|:------------:|:---------:|-----
|`/restart`|_none_ |web page| http://display_ip/restart<br>Reboots the display controller.
|`/otaupdate`|_none_|web page| http://display_ip/otaupdate<br>Places controller in OTA update mode. See [Using the Arduino IDE](/arduino.md) for more information on OTA updates.

#### RGBW Light Bulb Direct Commands
The light bulb default firmware provides a few direct commands use the ESPHome API (ESPHome not required).  However, this API requires some additional information as part of the full command.

First, you will need the bulb name in addition to the IP address.  See [Onboarding](/onboarding.md) for more information on how to obtain the bulb's name.  All direct commands start with the following base command:

`http://[bulb-ip-address]/light/[bulbname]` followed by the command and parameters (if any)

|Command | Parameter(s) | Returns | Example/Notes
|--------|:------------:|:---------:|-----
| _empty_| _none_ | JSON | This is the only command that is a GET and doesn't set any properties.  See notes that follow.
|`/turn_on`| _optional_|OK|Turns on the bulb.  You may include optional parameters to also set properties like brightness and color. See notes that follow.  If parameters are omitted, light will be turned on with the last used properties.
|`/turn_off`| _none_|OK|Simply turns the light bulb off.

_Additional Notes:_<br>
• When using just the base URL and an empty command, the bulb will return a JSON payload of the current state.  This payload may vary based on whether the bulb is using rgb [color_mode] vs. color_temp [color_mode]
```
{
  "id": "light-bedside_lamp_bulb",
  "state": "OFF",
  "color_mode": "rgb",
  "brightness": 200,
  "color": {
    "r": 0,
    "g": 255,
    "b": 0
  }
}

{
  "id": "light-bedside_lamp_bulb",
  "state": "ON",
  "color_mode": "color_temp",
  "brightness": 198,
  "color": {

  },
  "color_temp": 287
}
```
• The `/turn_on`command can be used with or without parameters.  With no parameters, the command simply turns the bulb on with its last settings.  But you can also include additional parameter to set the bulb properties along with the turn on command.  These will vary slightly depending upon whether you are using RGB or white color temp mode

_RGB Color Mode_
`turn_on?color_mode=rgb&r=0&g=255&b=0?brightness=196`
- color_mode: rgb
- r, g, b: Individual color components, ranging from 0-255
- brightness: Bulb brightness, ranging from 0-255 (_optional_)

_White Color Temp Color Mode_
`turn_on?color_mode=color_temp&color_temp=275&brightness=96`
- color_mode: color_temp
- color_temp: white color temperature, ranging from 150 (cool white) to 350 (warm white)
- brightness: Bulb brightness, ranging from 0-255 (_optional_)

The bulb's web application is not used by the firmware, but you can also bring up the bulb's web interface by just going to the IP address of the light bulb with no addtional parameters.

![api_02](images/api_02a.jpg)

This could be handy to get the bulb's name, MAC address or other information not available via the API.

**IMPORTANT NOTE**: This respository does not include the Kauf bulb's default factory-installed ESPHome firmware.  It simply makes use of the provided ESPHome API.  Please do not ask for assistance with the bulb's firmware as I did not develop nor do I maintain it.  For information on the RBGW Light bulb, refer to the following sources:

- [Kauf Common Github Repository](https://github.com/KaufHA/common)
- [Kauf RGBWW Bulb Github Repository](https://github.com/KaufHA/kauf-rgbww-bulbs)

If you experience issues with the Kauf bulb, please post your issue in one of the above repositories.