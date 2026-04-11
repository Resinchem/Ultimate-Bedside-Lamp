# Responding to Alarms
<div align="center">

![alarmactions_01](images/alarmactions_01.jpg)
</div>
The previous sections covered the various ways to edit and/or activate alarms.  This section shows the various methods of responding to an alarm when it sounds.<br><br>

***Snoozing vs. Stopping***<br>
For all methods, a sounding alarm may be 'snoozed' or 'stopped'.  If snoozed, the alarm will be silenced for the amount of time configured in the [Alarm Options and Settings](/alarmoptions.md) and the system will return to normal operations. After the snooze time expires, the alarm will sound again.  Once again, it can be snoozed again or stopped.  The snooze period must be at least one minute or longer for these processes to work as described.  If snoozing is disable (period=0), then none of the snooze features will apply.

When an alarm is stopped, it is permanently cancelled.  It will not sound again unless it has a repeat setting, at which point it will repeat and start over based on the repeat schedule (e.g. every Wednesday).

### Using the Touch Display

When an alarm sounds, the screen will automatically brighten to the DEFAULT brightness value if the current ACTIVE value is dimmer.  In addition, a SNOOZE and STOP button will appear on the display.

![alarmaction_02](images/alarmactions_02.jpg)

Simply tap either button to snooze or stop the alarm.  If you snooze the alarm, then the display will update as follows:

![alarmaction_03](images/alarmactions_03.jpg)

The STOP button will remain on the display during a snooze period in case you opt to cancel the alarm before the current snooze expires.  If the screen was brightened when the alarm sounds, it will return to the lower brightness level (or current auto-dim level if auto-dim is enabled) after a few seconds and while the snooze is occurring.

When a snooze period expires, the alarm will begin sounding again, the screen will brighten (if appropriate) and the SNOOZE button will once again be shown next to the STOP button.  Hitting snooze starts the snooze process over again.  Hitting STOP at any point will cancel the current alarm and the system will return to normal operating mode until the next alarm is encountered.

### Using the Touch Sensors

![touchsensor_06](images/touchsensor_06.jpg)

If you configured your optional touch sensors to have Snooze and Stop as their alarm functions (see [Defining the touch sensors](/touchsensors.md)), then you can simply tap the top of the enclosure to snooze or stop an alarm.  The functionality works identically to the touch display as described above.  In fact, the display will show the stop and snooze buttons in the same manner, even if the touch sensors are used to snooze/stop the alarm.  However, if you snooze the alarm, both touch sensors will return to their primary function (shown as toggling the bulb and LED strip above) while the snooze is active.

In other words, if an alarm sounds and you tap the snooze button (say your snooze time is 5 minutes).  During the 5 minute snooze cycle, both touch sensors revert back to their primary function until the alarm sounds again.  This means that if you wish to stop/cancel an alarm during a snooze period, you must use the touch display or alternate method, as attempting to tap Sensor 2 in this case will toggle on the light bulb and not stop the alarm.  Snooze and Stop Alarm functions are only actie for the touch sensors _when an active alarm is sounding_ and not during a snooze or any other normal operation.

### Using External Systems

Much like settings alarms, you can also snooze or stop an active alarm using either MQTT or the HTTP API. In fact external systems, like Home Assistant, can be used to set, snooze, cancel or activate/deactive alarms.  This provides a variety of automation possibliities.

***MQTT***<br>
If you have a properly configured MQTT broker and have setup MQTT under the System Integrations, you can send a simply MQTT command to snooze/stop an alarm.

Example:
- Topic: ```cmnd/[your-mqtt-topic]/alarmupdate```
- Payload: ```snooze or stop```

This will exceute the same process as using the touch display or touch sensor so the results are identical, including what is shown on the display.  See the [MQTT Setup & Topics](/mqtt.md) section for more details on using MQTT with the system.

***HTTP API***<br>
The HTTP API can also be used to send a SNOOZE or STOP commmand for an actively sounding alarm.

Example:
```
http://your-controller-ip/api?alarmupdate=snooze
http://your-controller-ip/api?alarmupdate=stop
```
Again, the same process and results occur as using the touch options described above. See the [HTTP API Command List](/api.md) for more information on using the HTTP API.