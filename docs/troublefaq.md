---
layout: default
title: FAQ & Getting Help
parent: Troubleshooting
nav_order: 4
---

# FAQ & Getting Help
{: .no_toc }

---

<p align="center">
  <img src="images/troublefaq_01.jpg" alt="[PLACEHOLDER: FAQ and Help]">
</p>

Before you open a GitHub issue or head to the discussions board, check these Frequently Asked Questions. Most "mysteries" have a simple explanation rooted in hardware limitations or configuration settings.

---

## Frequently Asked Questions

### Can I use a different ESP32 board, different display or different "X" component?
{: style="color: #edbd32; margin-top: 2rem;" }


**Short answer:** Probably not.  
**Long answer:** The firmware is written with specific pin-mappings and hardware register commands for the components listed in the [Build Guide](https://resinchemtech.blogspot.com/2026/05/ultimate-bedside-lamp.html). This guide includes a parts list that indicates which components can be substitued and those likely to require a firmware modficiation.  While you *can* modify the code to work with other hardware, I cannot provide support for these versions. If you swap a component, you are officially the lead developer for your specific fork!

### Why is my display screen just white/blank after flashing?
{: style="color: #edbd32; margin-top: 2rem;" }


This usually indicates a mismatch in the `TFT_eSPI` library configuration or an incomplete flash. Ensure you have selected the `Minimal SPIFFS` partition scheme in the Arduino IDE. If the partition sizes are wrong, the controller can't find the instructions it needs to "wake up" the screen.

### My clock shows the wrong time. How do I fix it?
{: style="color: #edbd32; margin-top: 2rem;" }


The lamp can get time from an **NTP Server** (Internet required), HTTP API, or via **MQTT** (Local broker required). 
1. Check your `Time Source` setting in the Display Controller’s web app. 
2. If using NTP, verify your `Time Zone` string (e.g., `EST5EDT`). 
3. If using the API, assure the external source system is publishing the time to the proper URL and that the time stamp is in the correct format.
4. If using MQTT, ensure your Home Assistant or Node-RED instance is actually publishing a timestamp to the `mqtt_time_topic`.
5. If using the 'Manual' time source, the clock will drift a few seconds a day.  If the clock loses power or if the display controller reboots, the time will reset to midnight.  In both cases, you need to manually set the time again.

### Why does the audio pop or fail to play?
{: style="color: #edbd32; margin-top: 2rem;" }


The `DFPlayer Mini` is very sensitive to power and SD card initialization. 
* A brief "pop" sound may be heard during boot up of the display controller as communication is established.  This is normal.
* Ensure your microSD card is **FAT32** formatted.
* Ensure files are in a folder named `/mp3` and named `0001.mp3`, `0002.mp3`, etc.
* If you hear a "buzz" during playback, you likely need a 1K ohm resistor on the RX line to the player, as detailed in the hardware wiring diagram.

### Can I control this lamp with Alexa or Google Home?
{: style="color: #edbd32; margin-top: 2rem;" }


Not directly from the lamp itself. The lamp is designed for **local control** to protect your privacy. However, if you integrate the lamp into **Home Assistant** via Discovery, you can then expose those entities to Alexa or Google Home through the standard Home Assistant cloud or Nabu Casa integrations.

📢 _"Hey Google. Turn on the bedside lamp bulb and set to 50% brightness"_

## Getting Additional Help

If your question isn't answered above, don't worry—the project has several avenues for support.

### [GitHub Discussions](https://github.com/Resinchem/Ultimate-Bedside-Lamp/discussions) (Recommended)
This is the best place for "How do I...?" questions or "Is this normal?" concerns. Discussions allow for code snippets and images, making it much easier for me (and the community) to see what’s going on. 
* **Etiquette:** Include your firmware version and a description of what you've already tried. "It won't work" is a riddle; "My MQTT status is 'Offline' despite the correct IP" is a solvable problem!

### [GitHub Issues](https://github.com/Resinchem/Ultimate-Bedside-Lamp/issues)
Please reserve Issues for **confirmed or suspected bugs** in the official firmware. 
* If you find a bug, please provide steps to reproduce it. 
* If the issue is related to custom hardware or modified code, it will be moved to the Discussions area or closed.

_Please don't email me directly unless specifically requested.  The volume of email received means your message will likely be lost in the "noise" and may not receive a response.  The best way to initially reach out is via the [Discussions](https://github.com/Resinchem/Ultimate-Bedside-Lamp/discussions) area._

> **💡 A Final Note on Support**<br>I’m an electronics hobbyist, just like you. I maintain these repos in my spare time between coffee refills and actual work. Please be patient—I’ll do my best to help you get your lamp shining bright!
{: .note }

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/troublediscovery' | relative_url }}" class="btn btn-outline"><- Previous: Home Assistant & MQTT</a>
</div>