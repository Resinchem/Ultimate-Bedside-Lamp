---
layout: default
title: Home Assistant Discovery
nav_order: 8
has_children: true
has_toc: false
---

# Home Assistant Discovery
{: .no_toc }

---

<p align="center">
  <img src="images/discoverymain_01.jpg" alt="Home Assistant Discovery Overview">
</p>

If you are a Home Assistant user, you can integrate your bedside lamp to control its features directly from your dashboards or utilize them in complex automations and scripts. The system offers over 25 distinct entities—including lights, sensors, and buttons—without requiring any manual YAML configuration on the Home Assistant side.

<p align="center">
  <img src="images/discoverymain_02.jpg" alt="Home Assistant Integration Example">
</p>

### Prerequisites
Before attempting to enable Discovery, ensure your environment meets the following requirements:

1. **Home Assistant:** Version 2026.3 or later is recommended.
2. **MQTT Broker:** A properly configured broker must be available on your network (e.g., the official Mosquitto add-on).
3. **MQTT Enabled:** You must have successfully [Enabled and Configured MQTT]({{ '/mqtt' | relative_url }}) in the lamp’s system settings and verified the connection.

---

### Understanding Discovery Naming
The Discovery process creates a new "Device" in Home Assistant using the **Discovery Device Name** you specify. This name is critical because it determines how your entities appear in the Home Assistant database.

#### Entity ID Generation
All created entity names are prepended with the device name. The system automatically converts the name to lowercase and replaces spaces with underscores.

**Example: Device Name "Bedside Lamp"**
* `light.bedside_lamp_bulb`
* `light.bedside_lamp_leds`
* `binary_sensor.bedside_lamp_touch1state`

> **💡 Note**<br>Once Discovery is enabled, the device name cannot be changed without first removing the Discovery and re-adding it. Consider your naming choice carefully to avoid breaking future dashboards or automations.
{: .note }

---

### In This Section

* **[Enabling and Disabling Discovery]({{ '/discoverymanage' | relative_url }})** – Step-by-step instructions for the initial setup and removal of the HA integration.
* **[Editing or Hiding Discovered Entities]({{ '/discoveryentities' | relative_url }})** – Managing entity groups and a complete reference table of all discovered entities.
* **[Manually Creating MQTT Entities]({{ '/discoverymanual' | relative_url }})** – Advanced instructions for users who prefer manual YAML configuration.

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/api' | relative_url }}" class="btn btn-outline"><- Previous: API HTTP Command List</a>
  <a href="{{ '/discoverymanage' | relative_url }}" class="btn btn-purple">Next: Enabling and Disabling Discovery -></a>
</div>