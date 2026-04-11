# Defining and Using the Touch Sensors
<div align="center">

![touchsensor_01](images/touchsensor_01.jpg)
</div>

If built as shown in the [Build Article](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html), then there are two capacitive touch sensors mounted under the lid in the approximate positions shown in the above photo.  The small sensors are very sensitive and work fine when placed behind a thin non-conductive substrate, such as PLA.  In fact, I've tested these using 5 mm thick PLA and they worked just fine.  Other surfaces, like wood, are also fine.  You just cannot use a metallic or other conductive surface.

The location of the sensors is somewhat arbitrary, so if you are using a conductive material for the lid, you could locate the touch sensors elsewhere, like the front or side of the enclosure.  The only caveat is that you need to assure there is at least a few inches between the sensors.  If placed side-by-side, they are sensitive enough that it trigging one sensor would also likely trigger the other.

Use of the touch sensors is optional (although really handy!) and you can just disable the features in the settings below if you don't want to use touch sensors in your project.

The touch control configuration can be found under the 'Advanced' settings from the controller's main web page.

![touchsensors_02](images/touchsensor_02.jpg)

The configuration for the touch sensors is at the top of the Advanced configuration page.

![touchsensors_03](images/touchsensor_03.jpg)

As you can see, each touch sensor can be enabled/configured separately.  The options and settings for each are indentical, so I'll only describe them once but they apply to both.  Note that each sensor can have two functions, as follows:

- _Primary Function_: This is the function that will occur during a touch when an active alarm is not sounding (alarms are idle or in a snooze state).

- _Alarm Function_: This is the function that will occur when an active alarm is occurring (alarm is sounding).

**Enable Checkbox**<br>
Check or clear this box to enable/disable the touch sensor.  If you opted not to install touch sensors, then be sure to clear this checkbox to prevent the firmware from attempting to configure and read the sensor values.

**Primary Function**<br>
Currently, you can select from any of the following primary functions in the drop down list:<br><br>
![touchsensor_04](images/touchsensor_04.jpg)

- _None_: Do nothing when the sensor is touched.  This might seem similar to enable/disable, but there is a distinct difference.  If the sensor is enabled but has the primary sensor set to _none_, then an MQTT message will still be sent (if MQTT is also enabled).  This means that the touch sensors could be used for automations in an external system, like Home Assistant.  If the sensor is disabled, then no messages will be sent to MQTT as the system assumes the sensors are not installed.

- _Toggle Light Bulb_: Tapping the sensor will toggle the state of the primary RGBW light bulb, turning it off or on.

- _Toggle LED Strip_: Tapping the sensor will toggle the LED strip off or on.

- _Increase Bulb Brightness_: Each tap will brighten the bulb by approximately 10%.  When the maximum brightness is reached, the next tap will "wrap" the brightness back to the dimmest value.  The bulb must be in an 'on' state for the touch to be registered (e.g. increasing brightness will not turn on the bulb if off).

- _Decrease Bulb Brightness_: This is the converse of increase.  Each tap will decrease the bulb brightness by approximately 10%, wrapping to full brightness if tapped after minimum brightness is reached.  The bulb must be in an "on" state for the touch to apply.  

- _Increase LED Strip Brightness_: Each tap will increase the the LED strip brightness by approximately 10%.  After reaching maximum brightness, the next tap will "wrap" to the dimmest setting.  Note that the LED strip must already be 'on' for the touch to apply.  Tapping to increase brightness of a strip that is 'off' will not turn on the strip.

- _Decrease LED Strip Brightness_: The converse of increase.  Each tap decreases brightness by approximately 10%, wrapping from the dimmest to brightest level.  The LED strip must already be 'on' for the touch to register.

- _Increase Display Brightness_: Each tap will increase the brightness by approximately 10%.  Unlike the bulb and LED strip, attempting to increase brightness beyond the maximum value will _not_ wrap to the dimmest value.  Instead, attempts to increase the brightness beyond the maximum are simply ignored.

- _Decrease Display Brightness_: The inverse.  Each tap decreases brightness by approximately 10%.  The brightness will _not_ wrap from dimmest to brightest.

Additional Notes on Display Brightness: If auto-dim is enabled, then any attempts to change the display's brightness will be overwritten by the auto-dim process.  If you find that when you change the display's brightness, it automatically returns to a different setting, check the auto-dim state.  More information on auto-dim can be found in the [Using Auto-Dimming](/autodim.md) topic.

**Alarm Function**<br>
The alarm function is used whenever an active alarm is sounding.  If an alarm has been snoozed, then the touch sensors will revert to the primary function until the alarm sounds again.  Note there are multiple ways to silence an active alarm.  The touch sensors are just one way.  Other methods are covered under the [Alarms](/alarmmain.md) topic.

Current alarm functions are:

![touchsensor_05](images/touchsensor_05.jpg)
- _None_: Nothing occurs when the sensor is touched during an alarm event.  However, if MQTT is configured, an MQTT message will still be sent.  See the notes on "none" under the primary function.

- _Snooze Alarm_: Tapping will snooze the currently sounding alarm.  The sensor will return to its primary function until the snooze period expires and alarm sounds again.  When the alarm sounds again, the function of the sensor returns to 'Snooze'.

- _Stop Alarm_: Tapping will stop the alarm.  Unlike snooze, the alarm will not sound again so the sensor immediately returns to the primary function.

**Touch Duration**<br>
The touch duration is entered in milliseconds and determines how long the sensor must be touched before the function is executed.  This is more or less a 'debounce' setting.  Since the sensors are so sensitive and the values are read every few milliseconds, a single tap could register multiple times, doing something like toggling a light on and then toggling it right back off again.  If you find that a tap is registering more than one, slowly increase this value until only a single tap is detected.  A delay of at least 250 ms is required, but can be increased to the maximum of 2,000 ms (2 seconds).  Note that using long values also result in a delay in executing the command.  For example, with a setting of 2,000 ms, the sensor must be touched _continuously_ for at least 2 seconds before the touch will register.  For this reason, keep this value as short as possible while still eliminating doulble-touches.  A value around 500 ms is a good place to start.  

**Reset Button**<br>
The reset button will reload the previously saved touch control settings from the configuration file, resetting any changed values.  This only applies to the touch sensor settings and not other sections of the Advanced page.

**Save and Reboot Button**<br>
This will write the current touch settings to the configuration file.  The system will then reboot and load the new touch values.  Note that if you wish to test different touch options, you must currently save and reboot between changes.  A test button may be added in a future release.

### Combining Sensor Uses

By having two sensors, with two functions each (alarm and non-alarm), you can create some interesting control combinations to suit your particular needs.

![touchsensor_06](images/touchsensor_06.jpg)

In this example, I've set the primary function of the sensors to toggle the LED strip and light bulb off/on.  For the alarm function, I've set one to snooze and the other to stop.

When an alarm is not sounding, the touch sensors can be used to toggle either the LED strip or the main light bulb off/on.  However, when an alarms sounds, the sensors immediately change to 'Snooze' and 'Stop' buttons.  So say both lights are off and an alarm sounds.  The first time, I just hit the 'Snooze' sensor and the alarm stops. Both lights remain off.  Then after the snooze period, the alarm then sounds again.  This time, I hit the Stop sensor and the alarm stops.  Since the sensor immediately revert to their primary state, I can tap the same sensor a second time to toggle on the main light bulb.

Additional features may be added in future releases, but for now, you can configure the sensors to best fit your needs and wants.
