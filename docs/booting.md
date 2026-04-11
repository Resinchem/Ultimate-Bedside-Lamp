# The Boot Process
<div align="center">

![05Boot_01_top](images/06Boot_01_Top.jpg)
</div>
As covered under the concepts section, this project consists of three independent ESP-based controllers.  These controllers communicate wireless using a special internal API.  But this means that communication and some configuration parameters have to be shared at boot time.  By default, the system uses the display and lights to indicate the boot process (and any potential issues).  Many of these default settings can be changed via the web app, but you can use them during bench testing to assure the system is communicating properly.

#### *Note : The boot process cannot complete until you've configured the initial system settings in the web app.  The controllers located each other by IP address, so you must supply this information before the system can complete a normal boot process.*  See and complete the steps under the [Setting up the System Interfaces](/interfaces.md)

Here's what you will see, by default, when powering on the system.  This assumes that all three controllers are powered on at approximately the same time and you've completed the system interface setup.  For more technical details, or if you are just curious, the tech process follow.

## Visual Boot Indicators

- The RGBW Bulb boots first.  The other controller wait on this step to be completed.
- The primary controller will briefly flash the LED strip, testing red, green and blue.
- The primary controller will briefly flash the RGBW bulb, testing red, green and blue.
- Both the LED strip and bulb are then set to the the default start state (off initially).
- The display controller actually shows its boot process right on the display
![05Boot_02_Display](images/06Boot_02_Display.jpg)

    - Initially, the first three items are the most critical.  
    - The first items shows the IP address of the display controller, meaning it is WIFI connected
    - The second line shows the connection to the primary controller
        - When all three controllers are booting, the display controller halts at this point and waits on the primary controller to finish booting.  It will show a counter until a connection is established.
        - After 10 attempts (once per second) with no communication to the primary controller, the boot process will show as a failed attempt.
    - The third items show that over-the-air (OTA) updates are enabled for installing firmware updates.
    - The remaining items may show \*disabled\* or \*failed* until they are setup and configured in the app.
        - Touch functionality shows that dipslay's touch functionality is configured and working
        - The clock's time has been successfully synced to an external source (NTP, MQTT or API).
        - MQTT has been enabled (or is disabled)
        - The Alarms file can be found and has been processed
        - The SD card (alarm sounds) is enabled, had been found and initialized
    - The LED will show yellow during the boot, green if the boot completes and red if there is an issue. The LED use can be toggled off/on in the app.

*_REMEMBER THAT IT IS NORMAL THAT A NUMBER OF THIS LAST FIVE ITEMS MAY SHOW AS DISABLED OR FAILED UPON INITIAL BOOT AND UNTIL CONFIGURED IN THE APP!_*

You can watch the video to see an example of the default boot process.  Also note that your speaker may omit a brief pop or click sound when the SD card test is executed.  This is normal as it is the DFPlayer initializing.

## Technical Details

For general use, you really don't need to know or fully understand how the system boots and communicates.  But if you are looking at modifying the firmware or have an issue with your system booting, then this information may be helpful.  Or maybe you're just a nerd like me and are interested in the inner workings!

First, it is important to understand that the ESP32 based controllers (primary and display) have their own local saved configuration file that is loaded at the start of the boot.  This contains, among other items, the WIFI credentials to connect to your network.  There are also other saved settings.  Once communication is established, the two controllers exchange required settings with each other so, for example, the display controller also knows how to communicate with MQTT because that information (which is stored in the primary's config) is shared from the primary controller as part of the boot process.

### RGBW Light Bulb

The RGBW has the most lightweight firmware, so in a normal situation, the bulb will complete its boot process first.  For this reason, it isn't dependent on the other controllers for its boot process.  Note that the default bulb power-on state is set to return to the prior state.  So if the bulb was on before the reboot, it will turn back on after the reboot.  But the final bulb state will be set by the primary controller after it completes its boot.  Just note that if the buib was on before the reboot, it may briefly turn back on temporarily before the desired default boot up state will be applied by the primary controller.

### Primary Controller

The primary controller boot process waits on the RGBW bulb and until it receives a ping response.  It then tests the output to the LED strip by briefly flashing it in red, green and blue.  This can be used not only to validate that the LED strip works, but that the number and type of LED strip is correct.  The number of LEDs can be changed in the web app, but changing the _type_ of LED strip requires a firmware modification.  The LED strip is then set to the default boot state as defined in the app (on/off, color, brightness, etc.). Showing the boot process with the LED strip can be toggled off in the app.

After testing the LED strip, the primary controller then tests the connection to the bulb.  By default, it will flash the bulb red, green and blue and then set the bulb to the default startup state as defined in the app.  Like the LED strip, both the start up state and whether to show the boot process using the bulb can be set in the web app.

### Display Controller

The display controller will always complete the boot process last.  This is because it waits on the primary controller to finish booting first (which in turn is waiting on the bulb).  After reading the local configuration file and establishing a WIFI connection, the boot process then goes into a loop where it pings the primary controller.  It will attempt this ping, once per every few second, for up to 10 attempts.  Once a successful ping occurs, the two controllers exchange needed information from their own local configurations.

The display controller then continues and checks various components and API calls (like to an NTP server for time).  These additional components may return \*DISABLED* or \*FAILED* depending on wiether those components have been enabled and configured in the app.

When the boot completes, the current time is shown on the display (this will always be midnight until your time settings are configured).  When the time is shown, you know that all three controllers have finished booting and the system is ready for use.

**SPECIAL NOTE**:<br>
Many of the configuration values, particularly the system integrations, are shared by the primary and display controller.  During the boot process, configuration values from the primary controller are shared with the display controller (which then stores these values locally).  What this means is that if the configuration values are changed and saved on the primary controller, a reboot of the primary controller is needed to load these new settings.  However, since the display controller may require updating for the new values, the system will generally reboot the display controller as part of the primary controller update process.  Just note that when changing configurations on the primary controller that is followed by a reboot, the display controller will also reboot automatically.  This is by design.

See the section on [Setting up the System Interfaces](/interfaces.md) for more information.