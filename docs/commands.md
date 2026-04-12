---
layout: default
title: Controller Commands
parent: General System Use
nav_order: 3
---

# Controller Commands
{: .no_toc }

---

<p align="center">
  <img src="images/commands_01.jpg" alt="Controller Commands Header">
</p>

Under normal operation, the system firmware abstracts the hardware so you don't have to worry about which of the three ESP-based controllers you are interacting with. However, **Controller Commands** are the exception—these functions generally apply *only* to the specific controller currently active in your browser.

> **💡 Identifying the Active Controller**<br>As detailed in the [Web Application Overview]({{ '/webapp' | relative_url }}), look for these indicators to know which unit you are controlling:
> * **Primary Controller:** Light Gray background; assigned Device Name in the header.
> * **Display Controller:** Pale Yellow background; "Display" in the header/tab.
{: .note }

---

## Primary Controller Commands
These are found at the bottom of the primary controller’s web page. All **yellow** buttons in this section impact only the primary unit.

![Primary Commands Interface](images/commands_02.jpg)

* **Restart:** Reboots the primary controller only. Other units remain operational, though some cross-device features will pause until the primary unit finishes its boot sequence.
* **Firmware Upgrade:** Accesses the update page for the primary controller. 
    > **⚠️ Warning**<br>Installing the wrong firmware file on the wrong controller will break your system. See [Installing Updates]({{ '/updates' | relative_url }}) before proceeding.
    {: .warning }
* **Arduino OTA:** Enables "Over-the-Air" programming for 20 seconds, allowing you to begin flashing code directly from the Arduino IDE. See [Modifying the Firmware]({{ '/firmwaremods' | relative_url }}).
* **Config Dump:** Displays the contents of the internal SPIFFS saved configuration files. 
    * **Config File:** The primary boot values.
    * **Discover File:** Only present if [Home Assistant Discovery]({{ '/discoverymain' | relative_url }}) has been utilized.

#### System-Wide Action
* **Restart ALL:** This is the only "global" command. It sends a reboot signal to all three controllers simultaneously to refresh the entire system.

> **❗ HIGH RISK: RESET PRIMARY**<br>This command wipes **ALL** configuration data from the primary controller, including saved Wi-Fi credentials. Use this only if you intend to return the unit to its factory state. You will have to repeat the [Onboarding]({{ '/onboarding' | relative_url }}) process.
{: .important }

---

## Display Controller Commands
To access these, click the **Display** button from the primary controller’s main menu to switch to the Display Controller interface.

![Accessing Display Menu](images/dispbrightness_03.jpg)

The commands are located at the bottom of the display settings page. While the interface is similar, these actions only impact the display hardware.

![Display Commands Interface](images/commands_03.jpg)

* **Restart / Firmware Upgrade / Arduino OTA:** These operate identically to the primary commands but target only the display controller.
* **Config Dump:** The display unit manages three distinct files:
    * **Main Config File:** Primary display and boot settings.
    * **Alarm File:** Saved schedules and alarm preferences.
    * **Sound Library:** Maps track names to physical indices on the microSD card.

> **❗ HIGH RISK: RESET ALL (Display)**<br>This command wipes all display-specific data and Wi-Fi credentials. If the IP address changes after a reset, you must update the [System Interface]({{ '/interfaces' | relative_url }}) settings on the primary controller to restore communication.
{: .important }
---
## External Control
Some of these commands can also be send via MQTT or via the HTTP API.  See * [Using MQTT and the API]({{ '/integrationmain' | relative_url }}) for a list of applicable commands.


<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/dispbrightness' | relative_url }}" class="btn btn-outline"><- Previous: Managing Display Brightness</a>
  <a href="{{ '/alarmmain' | relative_url }}" class="btn btn-purple">Next: Alarms -></a>
</div>