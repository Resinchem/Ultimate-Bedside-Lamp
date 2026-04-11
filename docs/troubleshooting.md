# Troubleshooting
<div align="center">

![troubleshooting_01](images/troubleshooting_01.jpg)
</div>

If you experience issues with operation of your system, first verify you've read the applicable information thoughout this document, along with the build instructions.  As stated numerous times throughout both documents:

**The firmware is desgined to work with very specific hardware.  Substition of any hardware components will likely require firmware modifications.**

The [Build Guide](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html) details all the parts used and indicate which parts can be substituted, those that _may_ require firmware modifications and those that **will** require hardware modifications.
<div style="background-color: yellow; color: black; font-weight: bold"; align="center">

&nbsp;I am unable to provide or support versions that use different hardware components! <i>Please don't ask!</i>

</div>

⚠ The information in this section only applies to the ESP32-based primary and display controller.  The RGBW Light bulb uses the factory-installed firmware.  If you have issues with the bulb's firmware, please post a Github issue or contact the developer for [Kauf bulb firmware](https://github.com/KaufHA/kauf-rgbww-bulbs) for assistance.  _I cannot provides support for this firmware as it is not mine, I did not develop it nor do I maintain it._
<br><br>
However, this section will attempt to provide some tips or pointers for resolving some of the more common issues you may encounter with the primary or display controllers.  It may be updated from time-to-time as additional issues are discovered.

- [Installation Issues](#troubleinstall)
- [Onboarding Issues](#troubleonboard)
- [IP Address Issues](*troubleip)
- [Configuration Issues](#troubleconfig)
- [Other Issues](#troubleother)
- [Getting Additional Help](#troublhelp)

### Installation Issues <a id='troubleinstall'></a>

This covers initial flash of the binary firmware and the onboarding steps.  If you are attempting an upgrade of existing firmware or are trying to flash source code from an IDE, please see the [Firmware  Updates and Modification](/firmwaremain.md) and [Advanced Technical Information](/advanced.md) sections.

***No COM Port Found***<br><br>
Per the [Installation](/installation.md) section, after connecting your standalone ESP32 (primary controller) or the Cheap Yellow Display (display controller) to your PC, you have to 'connect' and select the new COM port that appeared.  But what if you don't see a new COM port?  You may be missing the USB-to-Serial drivers on your local machine.  Most modern Windows and Mac machines have these system drivers, but if yours are missing, see the following external links for obtaining and installing the necessary drivers. Different ESP32 boards may use different drivers/USB chips:

**NOTE**: These drivers are provided by third-parties.  I have no way of updating or supporting drivers or other utlities that I did not develop.  Use caution when downloading and installing any unknown files to your computer.

- [CP2012 Drivers](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=overview) - Windows and Mac
- CH342, CH343, CH9102 Drivers: [Windows](https://www.wch.cn/downloads/CH343SER_ZIP.html), [Mac](https://www.wch.cn/downloads/CH34XSER_MAC_ZIP.html)

- CH340, CH341 Drivers: [Windows](https://www.wch.cn/downloads/CH341SER_ZIP.html), [Mac](https://www.wch.cn/downloads/CH341SER_MAC_ZIP.html)
  - Note that pages for the various "CH" dirvers listed above may appear in Chinese.  If you have a supported browser (like Chrome), simply right-click anywhere on the page and select "Translate to English"
  - If your browser does not support translation, then look for the button that has a "down arrow" (↓) on the button.  
  - In some rare cases, the standard Windows driver may not work on your machine (I had this issue specifically with Lenovo laptops).  In that case, check your machine's manufacturer's site for the driver. 

***ESP32 Will Not Connect***<br><br>
Even if you have the proper driver(s) installed, you may find that certain ESP32s still will not connect to a flashing utility.  Some ESP32 boards have to be placed in "flashing" mode before they will accept a connection via USB.

Normally, you can force a board to enter flashing mode by holding down the **BOOT** button on the ESP32 dev board when applying power or connecting to the USB port.  In even rarer cases, you need to press and hold **BOOT**, followed by pressing the **EN or RESET** button, then releasing both at the same time.  Check the information for your board's manufactuer if you are still having issues getting the board to connect to the flashing utlity.

***Flash appears to succeed, but onboarding access point never appears***<br><br>
If the flash appears to complete, but you do not see and onboarding access point hot spot being broadcast, it is almost always a sign of an incomplete or corrupt flash.  If this occurs (and is repeatable - the first step is to simply erase the flash and try again), it may be necessary to try a different flashing utility.
- [ESPConnect](https://thelastoutpostworkshop.github.io/ESPConnect/) - **Recommended Method** (most thorougly tested)
  - Use of this utility does require a Chromium-based web browser (Chrome, Edge, Brave, Arc, etc.) 
  - An active Internet connection is also required  
  - If you can't, or don't want to, meet these requirements, then you can use a desktop utility to flash the initial .bin files to your controllers.

- [ESPHome Web Flasher](https://github.com/esphome/esphome-flasher/releases/tag/1.4.0) - alternate utility (Windows, Mac, Linux)
  - Download the version of your operating system and execute (no install required)
  - Repository has been 'archived' as the desktop utility has been superceded by the new "web" version, but this has the same requirements as ESPConnect.  So if you are going to use a Chromium-based web browser anyway, ESPConnect would be the more tested choice.

- [NodeMCU PyFlasher](https://github.com/nodemcu/nodemcu-flasher) - another option (currently Windows only) 
  - Only try if neither of the above succeed.

I've found for some reason, particular boards/utlities/firmware work better with one utility over another.  If you are having issues getting a successful flash, I recommend starting with ESPConnect, then working your way down the list until the flash succeeds.

If none of these utitlies work, then it may be necessary to manually compile and flash the firmware using the Arduino IDE.  More information on using the Arduino IDE can be found in the section on [Modifying the Firmware](/modifications.md).

### Onboarding Issues <a id='troubleonboard'></a>
If the initial flash succeeded, the controller will begin broadcasting a WiFi access point or hotspot.  Using a mobile device (e.g. phone, tablet, laptop), you should see a new WiFi network called:<br><br>
`deviceName_AP` (where deviceName = the device name you entered during onboarding)<br><br>
If you do not see this hotspot, assure your remote device is on the same WIFI network as entered on the onboarding page.  Also assure you are in the vicintity of the controller.  The broadcasting power of the ESP32 is not that great, so you may not see the hotspot if located in a different room or 

If after 2-3 minutes, and after trying power cycling the ESP32, the hotspot is still not showing, there is a good possiblity there was an incomplete or corrupt flash.  See the flashing troubleshooting information above.

Also be sure you are _exactly_ following all the steps as specified in the [Installation](/installation.md) and [Onboarding](/onboarding.md) sections.

### Obtaining the Controller IP address <a id='troubleip'></a>
The normal method of getting the newly assigned IP address for your controller is to simply use your router or other network utlity to find the device and IP address.  In most cases (and if supported by your router), the controller will be listed in your router's DHCP listing by the device name you provided during onboarding.

However, if for some reason, you are unable to use your router to get the IP address for the controller, as long as the display controller boots, it lists the IP address as part of the boot process.  However, there is no corresponding display or output for the primary controller, so you will need to find an alternative network tool that can display MAC and IP addresses for connected devices.

### Configuration Issues <a id='troubleconfig'></a>
If you make and save changes to the setup or configuration and these changes do not appear to persist between power cycles or reboots, check the following:

***RTFM!***<br> Be sure you've reviewed the appropriate sections in this guide and also in the [Build Guide](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html). Often an error or issue is the result of not following the listed instructions or making assumptions about the firmware that are incorrect.

***Check Release Notes***<br> If you recently updated the firmware to a new version, be sure to read the release notes, particularly any listed _breaking changes_.  In some cases, the new version may have made changes to the underlying configuration file and additional post-update steps are necessary.

***Assure You are Using the Proper 'Save and Reboot' buttons***<br> Many pages of the web application have separates "sections" with their own 'Save & Reboot' buttons.  When multiple 'Save' buttons are present on a single web page of the web application, a given save button only _saves the configuration for that data_.  Any other changes you may have made to other sections will be lost if a save is executed on a different section.

![troubleshooting_02](images/troubleshooting_02.jpg) 

As shown above, if changes are made to both sections (LED and Light Bulb), but only the top Save & Reboot button is clicked, only the changes to the LED section will be saved to the configuration file.  Changes to the light bulb settings will be lost.  In this case, if both sections need to be updated, you must complete one section and use the Save & Reboot button for that section.  After a reboot, you can return to the same page and then update the other section.

Why use this convoulted multi-update process?  For one, it allows you to update _just_ those sections of the configuration that are needed.  Plus trying to place _all_ the various configuration options on a single page with a single Update button would make for a very large and long page the the ESP32'slowly web server might have issue in creating and sending to the browser.  And if the system integrations are inadvertenly changed, the system may cease functioning.

***Review the Contents of the Config Files***<br>

Finally, if you can access the web app, you can use a controller's [Config Dump](/commands.md) command to display the current contents of the configuration files.  If the web application is not available or will not load, as a last ditch effort, you can reconnect to USB and use [ESPConnect](https://thelastoutpostworkshop.github.io/ESPConnect/) to view the configuration files.

![troubleshooting_03](images/troubleshooting_03.jpg)

The above screen shot is from a test display controller.  You can see all three configuration files (alarm, the main config and sounds).  For normal JSON files (config and sounds), you can use the 'eye' icon to view the contents of the file from right inside the ESPConnect tool.  For binary files (e.g. Alarms), or really for any of the configuration files, you can also download a copy and then use any standard text editor to view the contents.

**⚠ Warning** - Do not delete any configuration files using the 'trash' icon provided.  Deleting a configuration file will, at a minimum, delete your system's configuration and worse case may require that the controller be flashed via USB and onboarded again.  The configuration files are not designed for manual editing, but you can always review the contents to assist with any troubleshooting issues.  In some cases, if you open an issue, you may be requested to upload one or more of your configuration files.  This can be done using the web app's config dump command or by copying/pasting the configuration from ESPConnect.

### Other Issues <a id='troubleother'></a>

If you experience other problems not covered anywhere in the provided documentation, then consider either posting your question or problem under the Github Discussion are or opening a Github Issue.

**[Github Discussions](https://github.com/Resinchem/Ultimate-Bedside_Lamp/discussions)**<br>
If you have a general question, are experiencing issues that may or may not be firmware related, or any other topics related to the project, the first stop should be the discussion area of the repository.  You may find someone else that had a similar question or problem and a resolution may already be posted.

If not, then feel free to start a new post.  The author of the repository (aka "me') monitors these discussions and may be able to assist.  For the best support, include as much information as possible, including your current firmware versions, what you are seeing vs. what you expect, and any troubleshooting steps you've already tried.  Just saying "something doesn't work" without any details is likely to be met with silence!

**[Github Issues](https://github.com/Resinchem/Ultimate-Bedside-Lamp/issues)**<br>
If you believe your problem or issue is directly related to a bug in the firmware, then please open an issue in the repository.  But before doing so, please verify the following:
- You are using the specified hardware as listed in the [Build Guide](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html).  Issues relating to use of other hardware will be marked as 'Unsupported' and the issue will be closed without comment.

- You have read the documentation, including the above troubleshooting steps.

- You provide the versions of the firmware you are using

Depending upon the issue, I may request additional information like your configuration files.  If you are unsure whether the problem is firmware-related or another issue, then please start with [Discussons](https://github.com/Resinchem/Ultimate-Bedside_Lamp/discussions).  If determined to be a true firmware-related blog, I can move your discussion topic to a new Issue.

Issues not directly related to a potential firmware problem will either be moved to the Discussions area or simply closed without comment.  Sorry... but the volume of various requests across my different Github repos means I need to first focus on true firmware issues, and on to things like general questions and potential enhancements.  Keeping the Issues section to only firmware-related bugs assists with these priorities.

### Getting Additional Help <a id='troublehelp'></a>
The best (and recommended) method of requesting help with the project is via use of the [Github Discussion](https://github.com/Resinchem/Ultimate-Bedside_Lamp/discussions) area.  Github discussions allow use of markdown, posting of images, code blocks and more that can't be done in a simple comment sections of services like YouTube or Blogger.

If necessary, I may request that the conversation be moved to email or Discord for more advanced help.  But please don't email me directly unless requested.  With the volume of email I get daily, it is likely your request will go unnoticed or will get lost amid all the spam, so I need to have requested an email and be watching for it to assure your question or answer does not go unnoticed.
