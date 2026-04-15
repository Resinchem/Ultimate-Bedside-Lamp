---
layout: default
title: Getting Started
nav_order: 3
has_children: true
has_toc: false
---

# Getting Started
{: .no_toc }

---

<p align="center">
  <img src="images/startingmain_01.jpg" alt="Project Overview">
</p>

This guide focuses strictly on the installation, configuration, and operation of the custom firmware. It assumes you have already assembled your hardware, whether as a completed build or a breadboard prototype.

**🛠️ Hardware & Build Reference**<br>If you are looking for wiring diagrams, assembly instructions, or a complete parts list, please refer to the following external resources:
* **[{{site.substitutions.youtube_title}}]({{ site.links.youtube_video }})**
* **[Written Build Guide](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html)**
{: .note }

---

## Setup Roadmap
To achieve a fully functional system, you must complete the following steps in the specific order listed below. Each section builds upon the previous one.

### 1. [Firmware Installation]({{ '/installation' | relative_url }})
The first step is flashing the custom C++ firmware to your Primary and Display (CYD) controllers using a compatible flashing utility or the Arduino IDE.

### 2. [Network Onboarding]({{ '/onboarding' | relative_url }})
Once flashed, you will connect each controller to your local Wi-Fi network. Remember: this system is designed for local-first operation and does not require an active internet connection for core functionality.

### 3. [System Interfaces]({{ '/interfaces' | relative_url }})
Establish the internal communication paths between the controllers and configure the API settings to ensure the "Triumvirate" works as a single unified device.

### 4. [Boot Process]({{ '/booting' | relative_url }})
Verify your setup by observing the default boot sequence. This section explains how to confirm that all services (Wi-Fi, MQTT, and Internal API) have initialized correctly.

---

## What's Next?
The system will not be fully functional until all four steps above are completed. Once finished, you can proceed to the **General Use** and the remaining topics to customize your alarms, lighting modes, and _optional_ Home Assistant integrations.

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/concepts' | relative_url }}" class="btn btn-outline"><- Previous: Concepts & Terminology</a>
  <a href="{{ '/installation' | relative_url }}" class="btn btn-purple">Next: Firmware Installation -></a>
</div>