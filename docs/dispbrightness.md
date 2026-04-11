# Managing the Display Brightness
<div align="center">

![dispbrightness_01](images/dispbrightness_01.jpg)
</div>

This section covers the various methods of controlling the display's brightness level.  Again, a reminder that you should understand the differences between the system's **DEFAULT** or boot brightness setting and the **ACTIVE** brightness setting.  In most case, and unless otherwise notes, the processes described here only update the **ACTIVE** setting.  This means that changes will be lost and the display will revert back to the **DEFAULT** brigntess settings if power is lost or the controller is rebooted.  For more infomration on the differences between DEFAULT and ACTIVE settings, please review the section on [Web Application Overview](/webapp.md).

**NOTES ON AUTO-DIMMING AND AUTO-BRIGHTNESS**<br>
- If auto-dimming is enabled, then any manual brightness adjustments will be overridden in a matter of seconds by the auto-dim settings.  As a general rule, if you want to manually control the display's brightness, you first need to disable the auto-dim setting if it is enabled.  The instructions below include information on how to do this for the **ACTIVE** setting.  If you want to change the **DEFAULT** setting or to calibrate auto-dimming see the [Using Auto-Dimming](/autodim.md) topic.

- Regardless of the auto-dim settings, if the current **ACTIVE** brightness is less than the **DEFAULT** active brightness, you can simply tap anywhere on the display to _temporarily_ increase the brightness to the default level.  After approximately 10 seconds of inactivity, the screen will automatically return to the prior brightness.  This acts much like your phone or tablet, where you can just tap the screen to temporarily "waken" the display.

- Other certain actions may also temporarily brighten the screen.  For example, when an alarm sounds, the screen automatically brightens temporarily.  When the alarm is snoozed or stopped, the brightness returns to the prior setting.  In a similar manner, the screen will automatically brighten (if dimmed below a certain level) when entering any of the settings pages on the display.  Again, the brightness will return to the previous level when the settings pages are exited and the clock returns to normal operating mode.

### Control via the Web Application

The active controls for the display are managed from the main Display page of the web app.  From the main controller's page, simply select 'Display'.

![dispbrightness_03](images/dispbrightness_03.jpg)

This will navigate to the primary display controller's page.  Controls for the display brightness are shown right at the top.

![dispbrightness_02](images/dispbrightness_02.jpg)

As mentioned above, if auto-dimming is enabled, any changes to the display brightness will immediately be overwritten by the auto-dim settings.  So if enabled, uncheck this box to manually set the brightness.  Note that anytime you change the **ACTIVE** state of the auto-dim, a message will be shown to remind you that the change is only temporary and the state will be restored upon a restart or reboot of the controller.  See the link above if you wish to change the **DEFAULT**, or boot state, of auto-dimming.

With auto-dimming disabled, simply move the slider to immediately change the display's brightness.  Move all the way to the left (0%) in effect shuts off the display.  Even when completely turned off, you can tap the screen to brighten it for approximately 10 seconds.

***Refresh and Sync***<br>
Similar to controlling the lights, the web page has no way of knowing if the brightness has been changed via one of the other methods listed below.  This means that if the display page is already shown and you change the brightness via the touch panel or external source, the web app will be unaware and will now be out-of-sync with the actual brightness.  Simply reload the web page or click the REFRESH & SYNC link to update the web page to match the current active brightness settings of the display.

### Control via the Touch Display
Assuming touch is enable for the display, you can also control the display brightness right from the display itself.

From the normal "clock" display of the touch display, simply tap the gear icon located in the upper right corner.  This displays the main settings page with the display brightness controls shown right underneath the light controls.

![dispbrightness_05](images/dispbrightness_05.jpg)

Remember you need to disable auto-dimming to manually change the brightness.  You can change the **ACTIVE** state of auto-dimming by just tapping the X or ✔ icon.  Once disabled, you can manually adjust the display's brightness with the slider.  Changes will be shown immediately and when you are satisfied with the brightness, tap the red X in the upper right corner of the display or just wait a few seconds and the system will automatically revert back to the clock/normal mode.

### Control from External Sources

The display brightness and auto-dim setting can also be controlled from an external system, such as Home Assistant.  This can be done through either the optional MQTT integration or via the native HTTP API.

***MQTT***<br>
If you have an MQTT broker and have also enabled and configured MQTT for the system, you can send commands for the auto-dim state and brightness of the display.

Examples:
- Set Auto-Dim Active State: 
  - _Topic_: cmnd/[your-mqtt-topic]/autodim
  - _Payload_: **on** or **off** (or 1/0)

- Set the Display Brightness:
  - _Topic_: cmnd/[your-mqtt-topic]/dispbrightness
  - _Payload_: 0 - 255  (numeric value representing 0-100%)

See the [MQTT Setup and Topics](/mqtt.md) for more information on configuring and using MQTT with your project.

***HTTP API***<br>
The API can be used by any system that can post a URL to the IP address of the controller.  No additional components or configuration is required.  The HTTP API commands for controlling auto-dimming and the display brightness are similar to MQTT.

Examples:
- _Set the auto-dim state_:&nbsp;&nbsp; http://controller_ip_address/api?autodim=0 &nbsp;&nbsp;  (or "1" for on)
- _Set the display brightness_:&nbsp;&nbsp; http://controller_ip_address/api?dispbrightness=96&nbsp;&nbsp;  (from 0 - 255)
  - The API also supports "up" and "down" for the brightness value which will increase/decrease the brightness by approximately 10%.

See the section [API HTTP Command List](/api.md) for details on using the API and the available commands.
