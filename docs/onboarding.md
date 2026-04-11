<div style="font-size: 24px;">Onboarding and First Time Setup</div>
<hr>
<div align="center">

![04Onboard_1_Header](images/04Onboard_01a_Header.jpg)
</div>
Once you have flashed the firmware to the primary and display controllers, you now need to add these and the RGBW bulb to your WIFI network.

### Kauf RGBW WIFI Bulb
Follow the instructions provided with the bulb to complete the onboarding/WIFI process.  This is very similar to how the other controllers will be onboarded below.  I won't detail those steps here, but there is one important piece of information you will need after joining the bulb to your WIFI.

#### ESPHome Device Name
When you get to the point of configuruing your system so that the controllers can "speak" to one another, you will need to know the bulb's ESPHome API name.  If you are a Home Assistant/ESPHome user, you can import the ESPHome node and change the name (see the bulb documentation and [Kauf web site](https://kaufha.com/blf10/) for information on changing the name).

If you are not a Home Assistant/ESPHome user, then after onboarding to your WIFI, locate the IP address assigned by your router and enter it in a browser.  This will display the bulb's native interface.

![04Onboard_2_bulb](images/04Onboard_02_Header.jpg)

The above screenshot shows the entity name _after_ I renamed the light entity in ESPHome/Home Assistant.  The default name will likely be something like "Kauf Bulb 8a37db" (the last six character are the last six of the MAC address).  If you want to change this name, you will need to import the node into ESPHome, adopt in Home Assistant and change the name of the created ```light``` entity.

_If you plan on renaming the device using Home Assistant, complete that step now and before configuring the lamp/clock interface settings (covered in a later section).  After renaming, check the bulb's interface again to assure the name change occurred._

Regardless of whether using a custom or default name, you must convert this name to the Home Assistant standard entity naming format.  In essence, change all letter to lower case and substitute any spaces with the underscore.  For example, using both my renamed example and the ficticious default name, the device name that will be used later for configuration would look like:

<u>_Examples_</u>: (original name --> HA entity name)<br>
Bedside Lamp Bulb --> bedside_lamp_bulb<br>
Kauf Bulb 8a37db --> kauf_bulb_8a37db

_While you can interact directly with the bulb through its web interface, this won't be used for the overall project.  And the device name is only used for seting up the interfaces between controllers.  The name won't be used anyplace else and if you eventually import the lamp/clock project into Home Assistant, you will be able to specify the device name._

Just make a note of both the bulb's IP address (see note below) and device name.

### Primary and Display Controllers
The process for both controllers is identical... but you need to complete the process twice... once for each controller.

![04Onboard_3_APs](images/04Onboard_03_AP.jpg)

If you successfully flashed both controllers, they will both be broadcasting a WIFI hotspot.  Note that they are using different names, so you know with which controller you are working.  If you do not see one of the hotspots, trying power cycling that controller.  If the hotspot doesn't appear in a few moments, then you may not have successfully flashed the project.  Try the flashing again, as covered under [Initial Firmware Installation](/installation.md).

_Note: If you have reset a controller that was previously used, then hotspot may reflect the device name you initially assigned instead of the original defaults shown above._

Using a phone, tablet or laptop, join the controller's WIFI hotspot.  Depending upon your phone and OS, you may be given a warning that the device has no Internet connectivity.  This is fine and expected.  Just tell your device to remain connected anyway.

Once connected, you may be prompted to "log in" which will open a browser.  If you aren't prompted, then open up a web browser on your device and enter in the IP Address:  ```192.168.4.1```

![04Onboard_04_Forms](images/04Onboard_04_Forms.jpg)

As you can see, both controller's onboarding forms are the same.  Only the name at the top will remind you of which controller you are onboarding.  The display controller will always include "DISPLAY" in the top title.  The entries for both are the same (except you'll use _different_ device names as described below).

**_SSID_**: Enter in the name of your WIFI network.  Use the same name for both controllers as they must be on the same wireless network to communicate.

**_Password_**: Enter in your WIFI password.  Again, this will be the same for both controllers.

**_Device Name_**: This is the one place where the entered value will be DIFFERENT for the primary and display controller.  In fact, each device (including the bulb) **must** have a different device name. The device name is used both internally (wifi client name, mDNS, MQTT client, etc.) but is also used and displayed within the web application for the few situations where you need to know which controller is active.  If you end up with more than one lamp build, the device names are also used to differentiate between multiple lamps, both on your network and in the web applications.

Use a short, but descriptive name of up to 16 characters.  You can only use letters and numbers, no spaces.  If you wish to simulate a space in your device name, use the underscore (_) character.  I recommend using 'display' as part of the display controller's device name, but this isn't required.

**_Submit and Save_**<br>
After completing all three fields, click the 'Submit' button.  The controller will reboot and attempt to connect to your WIFI.  If successful, the hotspot should disappear and not return.  You should also be able to now find the device in your router.  If your router supports it, the entry will be listed with the device names you just assigned.

![04Onboard_05_router](images/04Onboard_05_Router.jpg)

As you can see from my example, both the bulb and display controller are listed with the assigned IP address.  For each of the devices, you will need to know its IP address when setting up the interfaces in the web application.  But before doing so, see the following section.

### Using Reserved or Static IP Addresses

It is <u><b>strongly</b></u> recommended that you give all three devices (bulb, primary and display controllers) a static or reserved IP address immediately after onboarding.

Internally, the devices communicate with one another by IP address.  Once configured, if a device's IP address changes, which could be the result of a power failure or a simple lease expiration, then the overall lamp/clock will also quit functioning because the controllers will no longer know how to find each other.

Use your router to assign a reserved or static IP address for the bulb, primary controller and display controller.  Be sure to power cycle each device after changing to assure it gets the new IP address.

Note the IP addresses of all three devices.  You'll also need the bulb's "Home Assistant-compatible" name as described above.  

### Connect Controllers to peripherals or install in the final project build

At this point, you can power down the controllers and move them to the build (breadboard or final version).  It is recommended that you install or connect the controllers to the various peripherals (touch sensors, LED strip, DFPlayer, etc.) before proceeding any further as the configuration and initial boot processes will look for certain components during the start up routine.  Unexpected results can occur if trying to complete a normal boot and configuration if any of the expected components are missing.

If you haven't yet built the project or a breadboard version, you can find an overview and full build details (including the specific required parts) at the following:

[YouTube Overview]()<br>
[Complete Written Blog How-To Build Article](https://resinchemtech.blogspot.com/2026/03/ultimate-bedside-lamp.html)