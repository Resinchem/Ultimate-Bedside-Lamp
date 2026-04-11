---
layout: default
title: Web App Overview
parent: Setting Up the System
nav_order: 1
---

# Web Application Overview
{: .no_toc }

---

<p align="center">
  <img src="images/07Web_01_Header.jpg" alt="Web App Header">
</p>

The web application is the primary interface for the lamp and all its various settings. While the system can be controlled via MQTT, API, or the touch interface, the web application is the **only** interface that provides access to every available setting and option.

As covered in [Concepts]({{ '/concepts' | relative_url }}), this project uses three ESP-based controllers. While the app automatically handles most communication behind the scenes, there are specific system commands where you must know which controller you are currently accessing.

### Identifying the "Active" Controller
Because commands can vary by controller, there are several visual indicators to help you identify which interface is currently active in your browser:

* **Information Block:** At the top of every page, the info block lists the specific **Device Name** and **IP Address** you assigned during onboarding.
* **Version Numbers:** Each controller may have different firmware version numbers displayed in the header.
* **Browser Tabs:** The tab in your browser will automatically update to show the device name and IP of the active controller.
* **Interface Color:** For quick visual reference, the **Primary** controller uses a light gray background, while the **Display** controller uses a pale yellow background.

![Browser Tabs](images/07Web_02_Tabs2.jpg)

> **⚠️ Performance Note**<br>The web app is served directly from the ESP32 controllers while they are simultaneously managing lighting, audio, and displays. Expect a slight delay (a few seconds) for page loads. Avoid rapid repeat clicking, as this can queue up multiple requests and lead to unexpected behavior.
{: .warning }

---

### Accessing the Interface
The application is fully responsive and accessible via any modern web browser on your network. 

> **💡 Hint:** For the best experience on a smartphone, rotate your device to **landscape mode**.
{: .note }

To log in, enter the IP address of your **Primary Controller** (e.g., `192.168.1.205`) into your browser's address bar. 

<p align="center">
  <img src="images/07Web_03_Main.jpg" alt="Main Web Interface">
</p>

---

### Understanding Active vs. Saved Settings
The system manages configuration using a "Dual-State" logic. Understanding this is vital for ensuring your preferences persist after a power outage or reboot.

* **Default Settings:** These are values saved in the configuration file. They are loaded automatically whenever the system boots.
* **Active Settings:** These are values currently in use. If you change a color or brightness level, it becomes "Active" immediately.

> **💡 Persistence Logic**<br>Active settings persist until they are changed again or the system restarts. If the system reboots, it will discard "Active" changes and revert to the "Default" settings. To make an active change permanent, you must click a **Save** or **Save and Reboot** button.
{: .note }

![Save and Reboot Controls](images/07Web_04_Save.jpg)

* **Save and Reboot:** The controller writes your current settings to the flash memory and immediately restarts to apply them as the new **Default** values.
* **Other Save Buttons:** Other Save buttons (without '_& reboot_') will save your changes to the appropriate configuration file but do not require a reboot.

---

### General Navigation
The web application manages the "handoff" between controllers automatically. For the best experience, use the **Back**, **Return**, or **Cancel** buttons provided within the web app interface itself. 

Avoid using your browser’s forward/back buttons, as this may load a cached version of the page that doesn't reflect the true current state of the lamp.

![Navigation Buttons](images/07Web_05_Nav.jpg)

---

### Kauf RGBW Bulb Interface
The RGBW bulb runs its own ESPHome-based web interface. Under normal operation, you will **not** need to access this directly, as the Primary Controller handles all bulb commands.

<p align="center">
  <img src="images/07Web_06_Bulb.jpg" alt="Kauf Bulb Interface">
</p>

If you need to access it for advanced troubleshooting or ESPHome API details, you can navigate directly to the bulb's IP address. For more information on the bulb's native capabilities, refer to the Kauf documentation:

* [Kauf Web Site](https://kaufha.com/blf10/)
* [Kauf GitHub Common Info](https://github.com/KaufHA/common)
* [Kauf GitHub RGBW Bulb Repository](https://github.com/KaufHA/kauf-rgbww-bulbs)

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/setupmain' | relative_url }}" class="btn btn-outline"><- Previous: Setting Up the System</a>
  <a href="{{ '/display' | relative_url }}" class="btn btn-purple">Next: Main Display -></a>
</div>