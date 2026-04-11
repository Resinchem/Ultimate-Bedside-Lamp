# Controlling the Lights
<div align="center">

![lightcontrol_01](images/lightcontrol_01.jpg)
</div>

This section covers the various methods of controlling the main RGBW light bulb along with the LED strip.  Again, a reminder that you should understand the differences between the system's **DEFAULT** or boot settings and the **ACTIVE** settings.  In most case, and unless otherwise notes, the processes described here only update the **ACTIVE** settings.  This means that changes will be lost and the lights will revert back to the **DEFAULT** settings if power is lost or the controller is rebooted.  For more infomration on the differences between DEFAULT and ACTIVE settings, please review the section on [Web Application Overview](/webapp.md).

### Control via the Web Application
Using the web application gives you the most control over the system's lights.  The light controls are listed right at the top of the main page of the primary controller.

![lightcontrol_02](images/lightcontrol_02.jpg)

The control and options for the RGBW light bulb are on the left and LED strip are on the right.  Both sets of controls work in a similar manner.

***State Buttons***<br>
Simply click the on/off button to toggle the state of the desired light.  Note that due to the fact that the page must be submitted to the ESP32, then the ESP32 has to send the command and wait back for the results and then finally post the updated state to the web page, there may be a small delay from the time you click the button and when the button state on the web page updates.  Remember that the web server (and all other functions) are operating simultaneously in a tiny ESP32.  For this reason, the web application isn't always as "snappy" as some other sites or applications.  See the Refresh & Sync section below for more information.

***Color Pickers***
Clicking the color box for either light will open up a color picker.  

![lights_05](images/lights_05.jpg)

You can select a color for the light (bulb or strip) by using any of the three methods:
- Use the slider to select the general color or color range and then click in the large box for the exact color.
- Use the eye-dropper to pick a color from anywhere on your desktop (even outside of the app).
- Manually enter in a color value.  You can switch between RGB, HSL and Hex formats by clicking the small arrow next to the current color input mode.

As soon as you click away from the color picker, the color is sent to the light.

_RGBW Bulb Only_: When you select a color from the color picker, the bulb is automatically switched to RGB color mode.

***Brightness***<br>
The slider with the small "sun" symbol represents the light brightness.  Move the slider to the right to increase the brightness and to the left to decrease brightness.  As soon as you 'release' the slider, the new brightness value is sent to the light.  Again note that there may be a slight delay as the ESP32 handles multiple processes before it can process your web request.

***White Temperature*** - APPLIES TO RGBW BULB ONLY<br>
The RGBW bulb also has a white "temperature" setting, indicated by a small thermometer icon, which controls how 'cool' or 'warm' the white light is.  Simply slide the control to the left for a 'cooler' white (more bluish-white) or to the right for a 'warmner' what (more yellow-white).  Note that when you change the white temperature value, the bulb will automatically switch to 'white color' or temperature mode.  In addition, the color picker box will change to white, not only to truly refect the current color, but to also let you know at a glance that the bulb is using temperature mode for it's color.  As general rule, you don't have to worry about the bulb "mode" as it is switched automatically based on whether you change the color or temperature.

***Refresh & Sync***<br>
The web server runs in the ESP32.  However, it knows nothing about the "web client" or the browser that is running on your local device.  It can only send data to a browser in response to a request from that same browser.  What this means that if you have the web page displayed and then change one of the lights using a different method as covered below, the browser has no way of knowing that these values were changed elsewhere.

What this means is that if you change the lights outside of the web app, the web app may become "out of sync" with the actual states of the lights.  If this happens, simply click the **REFRESH & SYNC** link to request that the server send the current state of the light, including the color, brightness and temperature (if applicable).  This in effect, "syncs" the current web page to the actual active values of the lights.

### Control via the Touch Display
Assuming touch is enable for the display, you can also control the _states_ of the lights right from the device itself.

From the normal "clock" display of the touch display, simply tap the gear icon located in the upper right corner.  This displays the main settings page with the light controls at the top.

![lightcontrol_03](images/lightcontrol_03.jpg)

Tapping either button will toggle the state of the corresponding of the light off/on.  Similar to the web app, there may a small delay between when the light turns on and when the button on the display updates.  This is normal and a result of the ESP32 load to manage all the various processes occuring.

**Note**: Only the states (off/on) can be controlled via the touch panel.  Light will turn on with whatever the current **ACTIVE** color, brightness and temperature (bulb only) are in effect at the time.  To change color, brightness or temperature, you must use the web app or one of the other methods covered in this section.

To return to the clock or normal operating mode, you can click the red X in the upper right corner or just wait a few seconds.  If no touches are registered after approximately 10 seconds, the settings will automatically exit and the system returns to normal clock mode.

### Control from External Sources

The lights can also be completely controlled (state, brightness, color, etc.) from an external system, such as Home Assistant.  This can be done through either the optional MQTT integration or via the native HTTP API.

***MQTT***<br>
If you have an MQTT broker and have also enabled and configured MQTT for the system, you can send commands for the state, brightness and/or color (and temperature for the light bulb).

Examples:

- Set state of LED strip:
  - Topic: cmnd/[your-mqtt-topic]/ledstate
  - Payload: on or off (or 0 / 1)

- Set brightness of LED strip:
  - Topic: cmnd/[your-mqtt-topic]/ledbrightness
  - Payload: numeric value from 0 - 255 (setting to 0 is equivalent to setting state to "OFF")

- Set color of LED strip:
  - Topic: cmnd/[your-mqtt-topic]/ledcolor
  - Payload: color value expressed as RGB (255,0, 0) or as a Hex color string (#ff0000)

See the [MQTT Setup and Topics](/mqtt.md) for more information on configuring and using MQTT with your project.

***HTTP API***
The API can be used by any system that can post a URL to the IP address of the controller.  No additional components or configuration is required.  The HTTP API commands for controlling the lights are similar to MQTT.

Examples:

- _Set the bulb state_:&nbsp;&nbsp; http://controller_ip_address/api?bulbstate=on &nbsp;&nbsp;  (or off)
- _Set the bulb brightness_:&nbsp;&nbsp; http://controller_ip_address/api?bulbbrightness=96
- _Set the bulb RGB Color_:&nbsp;&nbsp; http://controller_ip_address/api?bulbcolor=ff0000
- _Set the bulb white temp_:&nbsp;&nbsp; http://controller_ip_address/api?bulbtemp= 225

See the section [API HTTP Command List](/api.md) for details on using the API and the available commands.



