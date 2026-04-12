---
layout: default
title: Advanced Technical Information
nav_order: 10
has_children: true
has_toc: false
---

# Advanced Technical Information
{: .no_toc }

---

<p align="center">
  <img src="images/advanced_01.jpg" alt="Advanced Technical Header">
</p>

These sections contain the "under the hood" technical details of the Bedside Lamp system. This information is generally not required for standard setup, configuration, or daily use. Instead, it is provided for the curious, the tinkerers, and those brave souls looking to modify the firmware for their own bespoke projects.

> **🐉 Here Be Dragons**<br>By entering these pages, you are leaving the "safe zone" of standard consumer setup. I am happy to answer questions about the official firmware, but if you start rewriting the logic or swapping out hardware components, you’re officially the Captain of your own ship. I simply don't have the bandwidth to troubleshoot custom forks!
{: .important }

---

### System Architecture Overview
The system relies on a distributed processing model across three separate ESP controllers. While they work together as a cohesive unit, each maintains its own firmware, filesystem, and logic.

* **HTML & UI:** To keep modifications straightforward, the entire web application (HTML, CSS, and JavaScript) is stored as embedded strings within a dedicated `html.h` header file. This means you don't need to worry about managing SPIFFS assets for the UI—just compile and flash.

---

### In This Section

* **[Firmware and File Structure]({{ '/advancedstructure' | relative_url }})** – A breakdown of the Kauf bulb firmware and the specific source files for the Primary and Display units.
* **[Configuration Files]({{ '/advancedconfig' | relative_url }})** – Detailed JSON mappings of the `config.json`, `discovery.json`, and alarm files.
* **[Display Library and Fonts]({{ '/advanceddisplay' | relative_url }})** – Technical requirements for the TFT_eSPI library and custom icon fonts.
* **[Partitions and Flashing]({{ '/advancedpartitions' | relative_url }})** – Memory offsets and partition tables for manual flashing via the Arduino IDE or third-party utilities.

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/modifications' | relative_url }}" class="btn btn-outline"><- Previous: Modifying the Firmware</a>
  <a href="{{ '/advancedstructure' | relative_url }}" class="btn btn-purple">Next: Firmware and File Structure -></a>
</div>