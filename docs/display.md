# Display Settings and Options
<div align="center">

![08Display_01_top](images/08Display_01_top.jpg)
</div>

The main display is a model of a Cheap Yellow Display (CYD) with capacitive touch, 3.5" display and a resolution of 480x320 pixels.  As covered in the [build article](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html), use of a different display will likely require firmware modifications (including defining the display in the TFT_eSPI library).  However, there are a few display configuration options and settings that you can use to tweak the display.

### Configuring the Display

The display configuration can be found by clicking the 'Display' button on the primary page of the web application (which switches over to the display controller interface - shown with a pale yellow background), then click 'Display' on the display's page to enter the configuration.

![08Display_02_Mains](images/08Display_02_Mains.jpg)<br><br>
There are two sections to the configuration page.  The top section deals with configuration of the actual display itself.
<div align="center">

![08Display_03_DispConfig](images/08Display_03_DispConfig.jpg)
</div>

_Display Dimensions and Touch Address_

These values are actually defined in the TFT_eSPI library that is included in the firmware.  This means that these values cannot be changed via the web app, but requires modification of the library and a recompilation of the firmware.  See the section on [Modifying the Firmware](/firmwaremods.md) if this applies to you.

_Screen Rotation_

You can set the screen rotation to 0°, 90°, 180° or 270° (default).  Note that 0° and 180° are 'portrait' orientation.  The firmware is designed for landscape orientation (90° or 270°) and some text may be cut off if you attempt to run in one of the portrait orientations. If you find that the display is upside down, then select a value 180° from the current rotation value to fix.  Note that you can use the test button to help with orientation (see below).

_Default Brightness_

Use this slider to set the intial display brightness after boot.  Note that this is the **DEFAULT** value and won't reflect any **ACTIVE** changes made elsewhere.  If [Auto-Dimming](/autodim.md) is enabled, then the default brightness is overridden based on the current ambient light level.  You can use the Test button to try out different brightness levels before saving your changes.  Also note that the default brightness level is used when the clock is "brightened" when the screen is tapped or the settings are entered, irregardless of the current active brightness settings.

_Enable Touch_

This enables the capacitive touch feature of the display.  If disabled, you won't be able to access the settings or other menus on the display.  It is recommended that you only uncheck this box if your display doesn't support capacitive touch.

_Enable Onboard LED_

If your display contains a front-facing RGB LED and this option is enabled, then the LED is used to show certain processes or results.

![08Display_04_OnboardLED](images/08Display_04_OnboardLED.jpg)

Currently, the onboard LED is used to show the boot process (yellow when in process, green when successful, red upon failure) and when the display is in Test mode (red).  Future enhancements may make additional use of this LED.  However, if your display does not have the LED or you don't wish to use it, clear the checkbox.

_TEST Button_

The test button can be used to test the display rotation, LED and touch functionality.  When clicked, the display will show the output seen in the top image on this page.  It indicates where it believes the top, right, bottom and left hand edges are, along with an abbrviated scale in the X and Y directions.  If enabled, the onboard LED will light up in red. If you also have checked the Enable Touch checkbox (and your display supports capacitive touch), you can test out the touch functionality.

![08Display_05_Touch](images/08Display_05_Touch.jpg)

Simply drag your finger around the screen and a line should follow. You can even try multi-touch with two fingers, although multi-touch isn't currently used by the application.  

To exit testing and return to the main display, just click the "Stop Test" button.

_RESET Button_

If you make and test various settings, but want to return all values to the previously saved defaults, just click the 'Reset' button and all values will be restored.

_SAVE AND REBOOT Button_

This will commit your current display settings to the saved configuration file and they will then become the new boot or startup default values.  To do this, the board writes the new values and then reboots to load these values as the new defaults.  If you leave the display configuration page without saving, then any changes are lost and the previous default values remain in effect.

### The Standard Clock Display

By default, when no other features are active, the system displays the clock and time.  But there are a few icons that are shown across the top of the display.
<div align="center">

![display_06](images/display_06.jpg)
</div>

- _Outside Temperature_: The current local outdoor temperature, in °F or °C.  You can specify a number of different sources to provide this value, including Open Weather Map (Internet connectivity required), MQTT or the HTTP API.  More info on setting a weather source can be under the [Setting a Weather Source](/weather.md).  Until a temperature source is configured, the temperature will always display 0°.

- _WiFi_: Indicates whether the display controller is connected to WiFi.  If not connected, the icon will be shown with a slash ![display_07](images/display_07.jpg)

- _Primary Link_: This indicates that the display controller is "linked" or connected to the primary controller via API.  If not connected, the icon will be shown with a slash ![display_08](images/display_08.jpg)

- _Active Alarms_: This icon is only shown if one or more scheduled and active alarms exist.  If no upcoming alarms exist, this icon will not be shown.

- _Settings_: If touch is enabled, tapping this icon will enter the settings page.  Under the settings, you can control the state of the RGBW bulb, LED Strip, alter the display brightness (and auto-dim setting) and set, enable or disable any of the five available alarm slots.  See the sections under [Using the System](/usingmain.md) for more information on using the display to control the lights or modify an alarm.

- _Time_: This is the default display, which shows the current time in the center of the display.  The time can be set (and synced) from a variety of sources.  You can opt to show the time in 12-hour or 24-hour format and can also change the font, color and size of the time display.  These settings are covered in the section on [Setting the Time and Clock Options](/time.md).  If a time source hasn't yet been configured, the time will always start at 12:00 am (midnight) when the system initially boots.

- _Date_: This simply shows the current date.  If the date/time has not been synced or set, then the date will always show January 1, 2026 after booting.

Use of the display during normal operation to snooze/stop a sounding alarm or to use the settings to change or control certain features are covered under other sections of this guide.