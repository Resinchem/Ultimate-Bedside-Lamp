# MQTT Setup and Topics
<div align="center">

![mqtt_01](images/mqtt_01.jpg)
</div>

If you wish to use MQTT with the system, you must meet a few prerequisites and also enable and configure MQTT in the web application.

### Prerequistes
To use MQTT you must have a local MQTT broker available on your network.  While you can technically use a cloud-based MQTT provider, this is not recommended just due to lag introduced between sending a command and having it received by the system.  Many types of brokers are available, some free and some paid, but one of the most popular free versions is [Eclipse Mosquitto](https://mosquitto.org/).  If you need a local broker, I recommend this one.  

If you are a Home Assistant user, you can turn Home Assistant into your MQTT broker by simply installing the MQTT broker add-on.  Also note that if using Home Assistant and MQTT, the system offers a 'single-click' way to integrate the lamp into Home Assistant.  This is covered under the [Home Assitant Discovery](/discoverymain.md) section of this document. 

### MQTT Topics
MQTT works by using a subcribe/publish method.  A system "subscribes" or listens for messages on certain topics.  A system "publishes" or sends messages to certain topics.  The system allows you to define these topics.  Note that your external system must uses these same topics for the system to properly communicate.  The lamp system uses one topic for publishing and a different topic for subscribing. 

- All messages sent by the lamp are published to a topic prepended with **stat/**
- All topics subscribed to by the lamp are prepended with **cmnd/**

### Enabling and Configuring MQTT
By default, MQTT is disabled in the system.  If you meet the prerequisites and wish to enable it, MQTT must be configured.  To do so, we'll revisit the primary controller's System Settings and Integrations.

![mqtt_02](images/mqtt_02.jpg)

The MQTT integration shares a page (and a single set of update buttons) with other integrations.  MQTT setup can be found about 3/4 ways down the page.

![mqtt_03](images/mqtt_03.jpg)

***Broker IP Address***<br>
Enter the IP address of your MQTT broker.  Note that if using the Home Assistant add-on/app, then the IP address will be the same as your Home Assistant IP.

***MQTT Broker Port***<br>
Enter the port for your broker.  By default, this is 1883 but can be changed at the broker.

***MQTT User Name***<br>
Enter in a valid user ID to connect to the broker.  You can use an existing account or create a dedicated account just for the lamp.  See your broker documentation for information on creating user accounts.

***MQTT Password***<br>
Enter in the account password for connecting to your MQTT broker.  See your broker's documentation.

***Subscribe Topic***<br>
Specify up to 16 alphanumeric characters to use as the _subscribe_ topic.  The system will listen for messages to be posted to this topic and then take action based on the message content.  So the subscribe topic is generally used by an external system to send commands **TO** the lamp.  As mentioned above, the subscribe topic is automatically prepended with _cmnd/_

&nbsp;&nbsp;&nbsp;&nbsp;```cmnd/[**your-subscribe-topic**]/*```

Using the above screen shot as an example, my lamp system will subscribe to and receive commands at the following topic:

&nbsp;&nbsp;&nbsp;&nbsp;```cmnd/bedlamp/*```

The asterisk (*) says to listen for all topics sent with a topic that begins with the above.  Any external system will need to send commands to this same beginning topic.  For example, to send a command to change the state of the LED strip, the following topic would be used:

&nbsp;&nbsp;&nbsp;&nbsp;```cmnd/bedlamp/ledstate```

A full list of CMND topics and valid payloads are listed below.

***Publish Topic***<br>
Specify up to 16 alphanumeric characters to use as the _publish_ topic. This system will send data to the broker using the this topic.  Normally this is to report the current **state** for an entity (e.g. light is ON, brightness is 50%, etc.).  All publish topics will automatically prepended with _stat/_.

&nbsp;&nbsp;&nbsp;&nbsp;```stat/[**your-publish-topic**]/state_topic```

Again, using the above example, the current **state** of the LED strip would be _published_ to:

&nbsp;&nbsp;&nbsp;&nbsp;```stat/bedlamp/ledstate```

Note: Send subscribe and publish topics are automatically prepended with either _stat/_ or _cmnd/_, you can use the same topic for both the subscribe and publish topics.  In fact, this is recommended unless your external system has different requirements.

***Telemetry Period*** (currently unused) <br>
By default, both controllers (primary and display) will publish initial states of all entities following a boot process.  In addition, a message is published anytime a tracked entity changes (e.g. your turn a light from off to on).  However, if the system is idle for a period of time, no new messages are sent.  The telemetry period is the time after which the controller will republish all it's states whether there have been any changes or not.  This can occasionally be needed if there is an interruption in the communication between the controller and broker (WIFI down, broker down, etc.) or for some reasons the system loses syncrhonization with an external system.  Valid values are 60 - 600 seconds.  Don't set this value any lower than necessary as it just adds extra processing overhead and sends a lot of unnecessary data to the broker.  In fact, it is recommended to start at the maximum value (600 seconds - 10 minutes) and only shorten if you have issues with the broker sending/receiving messages consistently.

***MQTT Discovery***<br>
Discovery can be used along with MQTT to have your device automatically discovered and added as a new device in Home Assistant via a single button click (no YAML editing or other configuration required).  Information on configuring this feature is covered seperately under the [Home Assistant Discovery](/discoverymain) topics.  The integration page just provides a shortcut link directly to the Advanced page where Discovery is configured and enabled.  You can also reach the same page via the Advanced button on the main web page.

***Buttons***<br>
As mentioned above, and unlike some other settings pages, there is only ONE set of buttons at the bottom of the page.  These buttons apply to **ALL** fields and integrations on the page and not just to MQTT.  So when you save or cancel changes as described below, the operation will apply to any and all values changed, not just MQTT.

- **Reset Button**: Click to restore any changed values back to the current saved configuration values.  Note that while values are restored, they are not tested for a valid connaction again.  Reload the page if you wish to repeat the integration checks.  This applies to **ALL** fields and settings on the page.

- **Save and Reboot**: This will save **ALL** current and changed values shown on the page.  After writing to the configuration file, the _primary_ **AND** _display_ controllers will reboot.  A restart of the display controller is also necessary because it uses some of these integrations locally and the values are received from the primary controller as part of the boot process.  The light bulb controller is not impacted, so it remains up and is not restarted.

- **Cancel Button**: This discards changes made to any and all values on the page and returns to the primary controller's main web page.

**REMINDER**: Because the primary and display controller share many configuration values, changing anything that requires a change to the saved configuration file and a reboot of the primary controller will also require a reboot of the display controller.  The system generally handles this, but you can use the [Controller Commands](/commands.md) for an individual controller and force a reload of current configuration files if needed.

## MQTT Topics and Payloads
This document is not meant as an MQTT tutorial.  If you want to know more about MQTT, I have a series of videos on using MQTT.  While many are Home Assistant-related, you can start with my video on [MQTT 101: Integrating your DIY Devices](https://youtu.be/Q0S0xOW35k8) that provides an overview of MQTT and how it works.  This section will simply list the valid topics and payloads for interfacing with the system remotely or from another system.

### State / Publish Topics

The System will output (or publish) current value and state changes to a common starting topic string.  This string always begins with _/stat_ (for state), followed by your specified publish topic (see above), and finally the specific topic of the entity.  All state topics are published with a retained flag of TRUE.

***Boot Topics***<br>
When MQTT is enabled and configured, the system will output a few 'diagnostic' MQTT values.  This topics have an additional "subtopic" to differentiate between the three controllers.  These values are _only_ published at boot time and are not otherwise updated, with the except of LWT which will be updated by the broker if connectivity to the system is lost.

|Topic Suffix|Payload Type|Example/Range|Notes
|------------|:------------:|:-------------------:|-----
|`/bulb/ipaddr`|String|192.168.1.52|RGBW Light Bulb IP address
|`/bulb/status`|String|online or offline|Bulb Last will and testament. (LWT)
|`/display/ipaddr`|String|192.168.1.51|Display controller's IP address
|`/display/lastboot`|date-time string| 2026-04-02 14:38|Date/Time display last booted
|`/display/macaddress`|String|08:B6:1F:3C:1F:B0|WIFI MAC address of display ESP32
|`/primary/ipaddr`|String|192.168.1.50|Primary controller's IP address
|`/primary/macaddress`|String|04:83:08:42:14:B8|WIFI MAC address of primary ESP32

***All Other State Topics***<br>

All other state topics use **/stat/[your-publish-topic]/[topic-suffix]** format.  For example, assume your publish topic is "bedlamp" as shown above.  Then the topic for the current auto-dim settings will be published to: `stat/bedlamp/autodim`.  All following state topics use this format unless otherwise noted.

|Topic Suffix|Payload Type|Example/Range|Notes
|------------|:------------:|:-------------------:|-----
|`/alarms`|JSON |*See note below|A JSON array of current alarm sesttings
|`/alarmtrack`|number| 0 - 20 |Indicates the current index/track for alarm sound
|`/alarmvolume`|number|0 - 30|Current max volume level for alarms
|`/gentlewake`|ON / OFF|ON|Current state of the alarm Gentle Wake feature
|`/snoozetime`|number|0 - 60|Snooze length duration (in minutes) 
|`/bulbbrightness`|number|0 - 255|Current brightness level of the light bulb
|`/bulbcolor`|String| 255,0,0|Red, green, blue values for current bulb color. Will be updated to white (255,255,255) when bulb is in color_temp mode.
|`/bulbmode`|String| 'color_temp' or 'rgb'|Last mode of the light bulb.  
|`/bulbstate`|String| ON or OFF|Current state of the light bulb.  Always published in upper case.
|`/bulbtemp`|number| 150 - 350 | White temperature, in mireds, when the bulb is in color_temp mode.
|`/clockcolor`|Hex String|#ffffff|Current color of the clock display. Published as a hex color _with_ the leading pound sign (#)
|`/dispbrightness`|number|0 - 255|Current brightness value of the display
|`/autodim`
|`/ledbrightness`|number|0 - 255|Current brightness of the LED strip.  May indicate 0 when LED strip is off.
|`/ledcolor`|String| 0,255,0|Current RGB color of the LED strip:  rr,gg,bb where each value can range from 0-255.
|`/ledstate`|String| ON or OFF|Current state of the LED strip.  Always published in upper case.
|`/temperature`|number| 71|If temperture is synced, then the last temperature value received.  Value will be output in the same units as configured under the temperature integration (°C or °F).  If not integrated, will show 0.
|`/timesync`|String| 'OK' or 'FAIL'|If time is synced to an external source, indicates success of last sync.
|`/touch1func`|String| see notes |Current primary function for Touch Sensor 1. Possible values are:<br>None, Toggle Bulb, Toggle LEDs, Bulb Brighter, Bulb Dimmer, LEDs Brighter, LEDs Dimmer, Display Brighter, Display Dimmer
|`/touch1funca`|String| see notes |Current alarm function for Touch Sensor 1. Possible values are:<br>None, Snooze, or Stop
|`/touch2func`|String| see notes |Current primary function for Touch Sensor 2. Possible values are the same as listed above for touch1func.
|`/touch2funca`|String| see notes |Current alarm function for Touch Sensor 2. Possible values are the same as listed above for touch1funca.


**\*JSON Payloads - Alarms**<br>
Most state MQTT topics post a single value as the payload.  The one excecption is the /alarms topic.  Instead of a single value or set of values for the current alarm settings, a payload containing the entire saved alarm array is published.  Here's an example of the JSON payload:

```
[
    {
        "index": 1,
        "active": 0,
        "date": "2026-04-06",
        "time": "09:19",
        "repeat": 0
    },
    {
        "index": 2,
        "active": 0,
        "date": "2026-02-18",
        "time": "11:58",
        "repeat": 4
    },
    {
        "index": 3,
        "active": 1,
        "date": "2026-02-24",
        "time": "14:13",
        "repeat": 9
    },
    {
        "index": 4,
        "active": 1,
        "date": "2026-04-06",
        "time": "07:30",
        "repeat": 5
    },
    {
        "index": 5,
        "active": 0,
        "date": "2025-10-28",
        "time": "07:58",
        "repeat": 7
    }
  ]
```
Numeric repeat values correspond to 0=None, 1=Sunday, 2=Monday. 3=Tuesday, 4=Wednesday, 5=Thursday, 6=Friday, 7=Saturday, 8=Weekends, 9=Weekdays.

It will be contingent upon any external system to deserialize the JSON payload to utlitize any of the values.  The scope of doing this is dependent upon the external system in use and is beyond the scope of this document.

***Special Time State Topic***<br>
There is one additional state topic that is only used if the time source for your clock is set to MQTT.  This is a special command, used internally, for force the system issuing the time topic to perform an immediate refresh of the current time.

`/gettime`

The topic is always published with a payload of `now`.  But unlike other state topics, it is published with a retained flag of FALSE.  This means that topic will be dismissed after reception.  You only need to be concerned with this topic if your external system is supplying the time to the clock via MQTT.  In this case, you should configure the remote system to listen for this topic and immediately publish an updated time to MQTT.  See the command topics for special use of MQTT time.

### Command / Subscribe Topics

Subscribes to all the following topics automatically.  You will use these commands from an external system to change or update entities of the lamp system.  All command topics should be published with a retain flag of **FALSE**. If you see "ghost" actions, such a the light or LED strip suddenly turning off/on by itself, check for retained CMND topics and clear them out.  Most command topics have a corrsponding state topic.  So if, for example, you send a command to turn on the light bulb, the changed state of the light bulb is immediately published to the corresponding /stat topic.

All command topics use `cmnd/[your-subscribe-topic]/[topic-suffix]` format. Recall that the MQTT setup has different topics for subscribing and publishing.  These may be the same or different.  Just assure that the external system is publishing its command messages to the proper topic.

Example - Publish a command to turn on the light bulb:
- Topic:  `cmnd/bedlamp/bulbstate`
- Payload: `ON`
- Retained: FALSE

|Topic Suffix|Payload(s)|Example|Notes
|------------|:------------:|:-------------------:|-----
|`/alarmactive`| alarm_num:active | 2:0 | Sets active state of an alarm.  First value represents the alarm index (1-5) and the second parameter (0 or 1) says to disable or enable the alarm's active flag.  The two numeric values must be separated with a colon (:) and no extra spaces.  The example sets alarm #2 to inactive.
|`/alarmtrack`| number| 4|Sets alarm track to use for all alarms.  Should correspond to the index number of the alarm sound.  See alarms for more information on alarm numbers/slots.
|`/alarmupdate`| **snooze** or **stop** | snooze | Immediately snoozes or stop an actively sounding alarm.  Ignored if an alarm is not sounding.
|`/alarmvolume`| number | 23 |Alarm volume or maximum volume if gentle wake is enabled.  Valid range is 0-30. Note that 0 or a very low value may result in no sound when an alarm triggers.
|`/gentlewake`| **OFF** or **ON**<br>**0** or **1**|ON|Enable or disable the alarm gentle wake feature.  You can use either OFF/ON or 0/1 for the payload.
|`/snoozetime`| number | 10 |Sets the length of an alarm snooze, in minutes.  Valid range is 0 - 60 minutes.  Using 0 will disable the ability to snooze alarms.
|`/playalarm`| number | 5 |Plays the current alarm track for the specified "number" of seconds.  The alarm is played at the current alarm volume level.  If you wish to play a different track (without changing the default alarm track), see the HTTP API method which also includes the ability to list the track number to play.
|`/setalarm`| JSON* | See notes below.|Used to set or update an alarm.  The payload must be sent as a JSON payload by the external app.  *See more below for the structure of this payload.
|`/bulbbrightness`|number| 128 |Sets the bulb brightness.  **This command will also toggle the bulb on if it is off**.
|`/bulbcolor`| RGB or Hex String | 0,255,0<br>#00ff00 | Sets the color of the RGBW light bulb. **This command will also toggle the bulb on if it is off.** It also sets the bulb mode to [`rgb`].
|`/bulbtemp`| number | 250 | Sets the white color temp of the bulb, in mireds.  Valid range is 150-350.  **This command will also toggle the bulb on if it is off.** It also sets the bulb mode to [`color_temp`]. 
|`/bulbstate`|**OFF** or **ON**<br>**0** or **1**|ON| Sets the state of the light bulb. You can use either OFF/ON or 0/1 for the payload.
|`/bulbrestart` | 1 | 1 | Issues a command to reboot the RGBW bulb.  The actual payload is ignored by must be a non-zero value.
|`/clockcolor`| RGB or Hex String | 255,255,255<br>#ffffff | Set the color of the clock display.  Can be sent as an RGB string in rr,gg,bb format (each value can be from 0-255) or as a hexidecimal color string, (e.g. \#ffffff)
|`/dispbrightness`|number| 128 | Sets the brightness level of the display.  Valid range is 0-255.  **NOTE**: Auto-dim should be disabled when manually setting the color or the auto-dim process will immediately overwrite any new brightness value.
|`/autodim`| **OFF** or **ON**<br>**0** or **1**|OFF|Enable or disable the display's auto-dimming feature.  You can use either OFF/ON or 0/1 for the payload.  You should set auto-dim to OFF before attempting to manually set the display brightness.
|`/settime`| Datetime String | 2026-04-01 14:30:00 | Sets the current date and time.  Payload must be in yyyy=mm-dd hh:mm:ss format.  Hours must be specified using 24-hour (military time) format. Normally used with the manual (non-syncing) time source.  Otherwise, the next normal time sync will reset the clock based on the sync source.  Note if using MQTT as the time source, there is a special topic for serving the time.  See below.
|`/settemperature`| number | 72 |Only applicable if weather source is MQTT or API.  Ignored if Open Weather Map is the source.  Payload should the the temperature in the desired units (°F or °C) as the temperature will be set to the raw payload value with no unit conversions performed.
|`/ledbrightness`| number | 96 | Sets the brightness of the LED strip.  Valid range is 0 - 255.  Note that some LED strips may not illuminate at very low brightness values. **This command will also turn the LEDs on if they are off.**  Setting a brightness = 0 also sets the LED state to OFF.
|`/ledcolor`|RGB or Hex String | 96,255,128<br>#60ff80 | Set the color of the clock display.  Can be sent as an RGB string in rr,gg,bb format (each value can be from 0-255) or as a hexidecimal color string, (e.g. \#ffffff) **This command will also turn the LEDs on if they are off.** Setting the color to "black" (0,0,0 or \#000000) will also make it appear that the LEDs are OFF even though the actual state will still be ON.
|`/ledstate`| **OFF** or **ON**<br>**0** or **1**|ON| Sets the state of the LED strip. You can use either OFF/ON or 0/1 for the payload.
|`/touch1func`| number | 2 | Sets the primary function for touch sensor 1.  Valid values are 0 - 8:<br>0 = None<br>1=Toggle Light Bulb<br>2 = Toggle LED Strip<br>3 = Increase Bulb Brightness<br>4 = Decrease Bulb Brightness<br>5 = Increase LED Strip Brightness<br>6 = Decrease LED Strip Brightness<br>7 = Increase Display Brightness<br>8 = Decrease Display Brightness
|`/touch1funca`| number | 1 | Sets the alarm function for touch sensor 1.  Valid values or 0 - 2:<br>0 = None<br>1 = Snooze<br>2 = Stop
|`/touch2func`| number | 7 | Sets the primary function for touch sensor 2.  Valid values are 0 - 8 and correspond to the same functions as Touch Sensor 1.
|`/touch2funca`| number | 2 | Sets the alarm function for touch sensor 2.  Valid values or 0 - 2 and correspond to the same alarm functions as Touch Sensor 1.
|`/displayrefresh`| 1 | 1 |Forces an immediate republish of all current MQTT topics and values from the display controller. Payload ignored, but must be included as any non-zero value. Use of '1' recommended.
|`/primaryrefresh`| 1 | 1 |Forces an immediate republish of all current MQTT topics and values from the primary controller. Payload ignored, but must be included as any non-zero value. Use of '1' recommended.
|`/refreshall`| 1 | 1 |Forces an immediate republish of all current MQTT topics and values from **ALL** controllers. Payload ignored, but must be included as any non-zero value. Use of '1' recommended.
|`/displayrestart`| 1 | 1 | Forces an immediate reboot of the display controller.  Any unsaved configuration changes will be lost.  This is equivalent to using the Display controller's RESTART command from the web app. Payload ignored, but it must be included as any non-zero value.
|`/primaryrestart`| 1 | 1 |Forces an immediate reboot of the primary controller.  Any unsaved configuration changes will be lost.  This is equivalent to using the Primary controller's RESTART command from the web app. Payload ignored, but it must be included as any non-zero value.
|`/restartall`| 1 | 1 | This forces an immediate restart of all THREE controllers (bulb, primary and display).  and unsaved configurations will be lost.  This is equivalent to power cycling the system or using the RESTART ALL button on under the primary controller's commands in the web app. Payload ignored, but it must be included as any non-zero value.
|`/dispsaveconfig`| 1 | 1 | **USE WITH EXTREME CAUTION!!!** This command will save ALL applicable _active_ values as the new _default_ boot values.  If the display has auto-dim disable, display color is green at 50% brightness, then auto-dim: OFF, and a display with a green color at 50% brightness will become the new boot defaults and will be the state the display is set to whenever the system restarts.  This may lead to unforeseen or undesirable results.  Be absolutely sure all states are what you want for the new startup/boot values before issuing this command.  The controller will also reboot as part of this process.
|`/primsaveconfig`| 1 | 1 | **USE WITH EXTREME CAUTION!!!**  This command saves all primary-related active settings as the new boot defaults.  See the notes for the /dispsaveconfi above as the same warnings apply here.  If executed, note that _both_ the primary and display controllers will reboot.

****Alarm JSON Payload***<br>
To set an alarm via MQTT, you must create the alarm settings as a JSON payload.  The structure of the JSON payload is as follows:
```
{
  "alarmnum": 3,
  "active": 1,
  "date": "2026-04-06",
  "time": "16:30",
  "repeat": 0
}
```
|Key|Value|Description
|----|----|----
| alarmnum | 1 - 5 | Alarm "slot" number you are editing or updating
| active | 0 or 1 | Sets alarm as inactive (0) or active (1)
| date | date string | Alarm date.  Must be in yyyy-mm-dd format
| time | time string | Alarm time.  Must be in hh:mm in 24-hour (military) format
| repeat | 0 - 9 | Repeat setting.  Valid values are:<br>0 = None<br>1 - 7 = Sunday, Monday, Tuesday... Saturday<br>8 = Weekdays<br>9 = Weekends

After attempting to set an alarm via MQTT the first few times, it is recommended that you verify any alarm changes via the web app or touch display until you are sure that the payload is being formed (and published) appropriate so that it is read correctly by the system.

***Special Time Topic***<br>
In addition to the normal 'settime' command for manually setting the time, if you opt to use MQTT time for syncing and/or updating the time, a special independent time topic is used. As covered under the [Clock and Time Settings](/time.md), MQTT can be used to initially set the clock's time during the boot process but can also be used to actually keep the clock's time current.  

![mqtt_04](images/mqtt_04a.jpg)

When MQTT is selected as the time source for the clock, you can specify a special independent topic that will receive time updates.  Why is this separate and different from the `/settime` command above?  You may already have a system that is publishing the time to MQTT.  For example, I have a Home Assistant automation that publishes the current time once per minute.  This is used by numerous devices, so instead of creating an additional automation and separate time topic just for the bedside lamp, I can leverage the existing topic (as long as the payload also matches).  In the above example, the time is updated every minute using the topic `cmnd/hatime/mqtttime` which is a completely different topic from other standard topic for the lamp project.

The source system should be configured to publish the time once per minute to this special time topic if you wish to use MQTT as the time source.

**IMPORTANT NOTE**: If using MQTT as the time source, along with the "live" update option, and the MQTT broker goes down, time will not be updated on the clock until the broker is restored.  This could potentially lead to missed alarms.  See the [Clock and Time Settings](/time.md) for full details on selecting and configuring a time source for the clock.