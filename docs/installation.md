<div style="font-size: 24px;">Initial Firmware Installation</div>
<hr>
<div align="center">

![03_01_Flashing](images/03Flashing_1_TopImage.jpg)<br>
</div>
The first preparation step you need to complete is the installation of the firmware onto the controllers.  This process is called flashing and it previously was one of the steps that caused folks a lot of problems.  This was due to the need to download one of a number of different utilities, figure out the necessary settings and then trying the flash.  Some utilities worked better with some firmware.

For the installation of the firmware onto the primary and display controllers, we are now going to use a relative new web-based tool called ESPConnect.

![03_02_ESPConnLogo](images/03Flashing_2_ESPConnectLogo.jpg)

ESPConnect is a free open-source, browser-based web tool for interacting with ESP32-based microcontrollers.  One of its features is to flash firmware.  It is based on the same technology as other web flashers like those for WLED, Tasmota and ESPHome.

## Prerequisites

- Computer or laptop with an available USB port
- A microUSB _data_ cable.  Note that a cable only designed for power will not work
- Chrome, Edge, Brave, Arc or other Chromium browser based on version 89 or newer*
- A copy of the firmware .bin file (avaliable in the 'Releases' area of the repo)

*_If you don't have a Chromium-based browser installed and/or don't want to use one, then you will need to use one of the older desktop utilities methods.  If you are unfamiliar with this method, I cover the process in this video: [Beginner's Guide to Flashing Custom Firmware](https://youtu.be/74NGHj-cOls) (jump to the 8:08 mark to see use of desktop utilities)._

### Kauf RGBW WIFI Bulb
No installation necessary!  The bulb comes with ESPHome pre-installed.  If an updated version of the firmware is available, the instructions included with the bulb will provide the steps needed.  **Do not install any firmware from this or any other source than the official Kauf ESPHome firmware.**  The device will not work in this project if you replace the firmware.  If you want to modify or make minor changes to the bulb (e.g. device name), this should be done using the supplied official ESPHome code.

### Primary and Display Controllers
The process for both controllers is identical.  The only difference is the firmware .bin file used.<br>  
For the primary controller, use the firmware ```Primary_Ctrl_vx.xx.bin```<br>
For the display controller, use the firmware ```Display_Ctrl_vx.xx.bin```<br><br>
_(vx.xx represents the firmware version number, such as v0.87, but note that the current version number will likely be different for the two different controllers. The most current versions can always be found in the assets section of the latest release)._

Open a Chromium based browser and navigate to https://thelastoutpostworkshop.github.io/ESPConnect/

<div align="center">

![03_03_ESPConnLogo](images/03Flashing_3_ESPConnect.jpg)
</div>
If you haven't done so yet, connect the controller (the ESP32 for primary, the Cheap Yellow Display for display) and click the 'Connect' button in the upper left corner.<br><br>

![03_04_ESPConnect](images/03Flashing_4_ESPConnect.jpg)

Select the COM port for your controller.  If you are unsure of which COM port it is, simply cancel the dialog, disconnect the ESP32 and try to connect again.  One of the COM ports disappeared.  Close the dialog, reconnect the ESP32 and now connect again, choosing the new COM port that appears.  If you do not see a new COM port when connecting the ESP32, or the tool refuses to connect, you may be missing a USB-to-Serial driver.  This is normally a CP2102 but could also be a CH340.  Locate the necessary drivers and install them on your local machine.

Once connected, (1) use the left sidebar menu to select the Flash Tools option.

![03_05_ESPConnect](images/03Flashing_5_ESPConnect.jpg)

2. Click in the Firmware Binary box and use the file browser to select the firmware file.
    - Be sure to select the proper firmware here!
      - Primary_Ctrl_vx.xx is the firmware for the standalone ESP32 board
      - Display_Ctrl_vx.xx is the firmware for the CYD Display

3. Leave the flashware offset at the default 0x0
4. Be sure to check the 'Erase entire flash' check box.  If your board had previous firmware, you can optionally use the section right above the Flash Firmware to create a backup of any existing firmware.

ESPConnect will show a progress bar during the flashing process:

![!03_06_ESPConnect](images/03Flashing_6_ESPConnect.jpg)

Once the flashing is complete, power cycle the ESP32 (unplug then reconnect to USB port).  Wait a few moments, then use your phone, tablet or laptop to display all available WIFI networks:

![03_07_ESPConnect](images/03Flashing_7_ESPConnect.jpg)

If you see a WIFI hotspot called BedsideLamp01_AP (or ```BL_Display01_AP``` if flashing the display firmware), then the flash was successful.

_Note: If you do a full reset of a controller, which requires onboarding again, the broadcasting hotspot may use the initial device name you assigned instead of the defaults specified above.  The onboarding process is the same regardless of the hotspot name._

Repeat the above steps for the other controller.  The process is the same for both the primary and display controllers.  The only difference is the name of the AP hotspot that gets created.  Once both controllers have been flashed and are broadcasting their respecting hotspot, you are ready to join the controllers to your WIFI.  This is covered in the next section, [Onboarding and First Time Setup](/onboarding.md).