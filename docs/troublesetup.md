---
layout: default
title: Initial Setup & Connectivity
parent: Troubleshooting
nav_order: 1
---

# Initial Setup & Connectivity
{: .no_toc }

---

<p align="center">
  <img src="images/troublesetup_01.jpg" alt="[Troubleshooting image]">
</p>

This section covers the initial flashing of the binary firmware and the onboarding steps. If you are attempting an upgrade of existing firmware or are trying to flash source code from an IDE, please refer to the [Firmware Updates]({{ '/firmwaremain' | relative_url }}) or [Advanced Technical Information]({{ '/advanced' | relative_url }}) sections instead.

## Installation Issues
Getting the firmware onto the ESP32 is usually the hardest part. If the computer and the controller aren't talking, check these common culprits.

### No COM Port Found
When you connect your standalone ESP32 (Primary) or the Cheap Yellow Display (Display) to your PC, you must select a COM port. If no new port appears, you are likely missing the USB-to-Serial drivers.

> **💡 Note**<br>These drivers are provided by third parties. Use caution when downloading and installing files. If the standard drivers fail, check your PC manufacturer's site (especially for Lenovo laptops).
{: .note }

* **[CP2012 Drivers](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=overview)** – Windows and Mac.
* **CH342, CH343, CH9102 Drivers:** **[Windows](https://www.wch.cn/downloads/CH343SER_ZIP.html)** \| **[Mac](https://www.wch.cn/downloads/CH34XSER_MAC_ZIP.html)**
* **CH340, CH341 Drivers:** **[Windows](https://www.wch.cn/downloads/CH341SER_ZIP.html)** \| **[Mac](https://www.wch.cn/downloads/CH341SER_MAC_ZIP.html)**

*If the driver pages appear in Chinese, right-click and select "Translate to English" in your browser, or look for the "down arrow" (↓) download button. I did not develop nor am I reponsible for these drivers.*

### ESP32 Will Not Connect
If the drivers are installed but the board won't connect, you may need to perform the "Boot Button Dance" to force the board into flashing mode:

1. Hold down the **BOOT** button on the ESP32.
2. Apply power (plug in USB) or press the **EN/RESET** button while still holding BOOT.
3. Release both buttons simultaneously.

### Flash Succeeds, but no Hotspot Appears
If the flash finishes but you don't see an onboarding access point, the flash was likely incomplete or corrupt. Erase the flash and try again. If the problem persists, try a different utility:

1. **[ESPConnect](https://thelastoutpostworkshop.github.io/ESPConnect/) (Recommended)** – Requires a Chromium-based browser and internet connection.
2. **[ESPHome Web Flasher](https://github.com/esphome/esphome-flasher/releases/latest)** – A reliable desktop alternative.
3. **[NodeMCU PyFlasher](https://github.com/nodemcu/nodemcu-flasher)** – Use as a last resort (Windows only).

If all else fails, you may need to try compiling and flashing the source code from the Arduino IDE. It is possible that there is a compatiblity issue with your particular ESP32. See [Modifying the Firmware](/modifications.md) for more information on using the Arduino IDE to flash the firmware.


## Onboarding Issues
Once the flash is successful, the controller broadcasts a Wi-Fi hotspot. You should see a network named: `deviceName_AP` (where `deviceName` is what you entered during onboarding).

* **Check the Network:** Ensure your phone or laptop is searching on the same Wi-Fi frequency (2.4GHz).
* **Proximity:** The ESP32 doesn't have the broadcasting power of a high-end router. Stay in the vicinity of the controller during this process.
* **Wait for it:** It can take 2–3 minutes for the hotspot to initialize. If it never appears after a power cycle, refer back to the flashing troubleshooting above.

## Obtaining the Controller IP Address
Usually, your router’s DHCP list will show the device by the name you gave it. If you're stuck:

* **Display Controller:** The IP address is listed on the screen during the boot process.
* **Primary Controller:** Since there is no screen, you will need a network scanning tool (like Fing) or your router's admin page to find the IP address via the device's MAC address.

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/troubleshooting' | relative_url }}" class="btn btn-outline"><- Previous: Troubleshooting Overview</a>
  <a href="{{ '/troubleoperation' | relative_url }}" class="btn btn-purple">Next: Operation & Configuration  Issues-></a>
</div>