---
layout: default
title: Editing or Hiding Discovered Entities
parent: Home Assistant Discovery
nav_order: 2
---

# Editing or Hiding Discovered Entities
{: .no_toc }

---

<p align="center">
  <img src="images/discoveryentities_01.jpg" alt="[PLACEHOLDER: Editing and Entity Reference]">
</p>

One of the most powerful features of the Discovery system is the ability to choose exactly which components of the lamp are exposed to Home Assistant. You can add or remove groups of entities at any time to keep your dashboard clean and relevant.

---

## Updating an Existing Discovery
The primary controller stores your previous settings in a `discovery.json` file. This allows you to modify your entity list without creating duplicates in Home Assistant.

### Modifying Entity Groups
1. Navigate to the **Discovery** page in the **Advanced** menu.
2. Your previously selected groups will be pre-populated.
3. Check any additional groups you wish to add, or uncheck groups you wish to remove.
4. Click the **UPDATE DISCOVERY** button.

> **💡 Device Name Limitation**<br>The **Device Name** is used to generate unique MQTT discovery topics. Because of this, you cannot change the Device Name once Discovery is enabled. To change the name, you must first [Remove Discovery]({{ '/discoverymanage' | relative_url }}) and then recreate it with the new name.
{: .note }

### The Update Button
When you click **UPDATE DISCOVERY**, the system immediately sends "clearance" messages for any removed groups and "discovery" messages for any new ones. 

> **⚠️ Dashboard Warning**<br>If you remove an entity group that is currently in use on a Home Assistant dashboard or in an automation, those items will show as "Unavailable" or may break entirely.
{: .warning }

---

## Handling System Resets or Hardware Changes
Because Discovery relies on a unique string (Device Name + MAC Address), certain hardware events require attention:

* **System Reset (Same ESP32):** If you perform a factory reset but use the same hardware and the same Device Name, running Discovery again will simply "take over" the existing entities in Home Assistant.
* **New ESP32 / Different Name:** If you replace the controller or change the name, Home Assistant will treat it as a brand-new device. In this case, it is best to delete the old device from the Home Assistant **Devices** page to avoid clutter.

## Entities Created Reference Table
The following table lists the entities created in Home Assistant based on your selections. 

*Note: `device_name` refers to your specified Discovery Device Name (converted to lowercase with underscores).*

| Entity Group | Entities Created | Friendly Name |
| :--- | :--- | :--- |
| **RGBW Light Bulb** | `light.device_name_bulbstate`<br>*(Includes brightness, color & temperature attibutes)* | Light Bulb |
| **LED Strip** | `light.device_name_ledstate`<br>*(Includes brightness and color attributes)* | LED Strip |
| **TFT Display** | `number.device_name_dispbrightness`<br>`switch.device_name_autodim`<br>`text.device_name_clockcolor` | Display Brightness<br>Auto-Dimming<br>Clock Color |
| **Touch Sensors** | `binary_sensor.device_name_touch1state`<br>`binary_sensor.device_name_touch2state`<br>`select.device_name_touch1func`<br>`select.device_name_touch2func` | Touch 1 State<br>Touch 2 State<br>Touch 1 Main Function<br>Touch 2 Main Function |
| **Alarms** | `number.device_name_alarmvolume`<br>`number.device_name_alarmtrack`<br>`number.device_name_snoozetime`<br>`switch.device_name_gentlewake`<br>`button.device_name_alarm_snooze`<br>`button.device_name_alarm_stop` | Alarm Volume<br>Alarm Track<br>Snooze Duration<br>Gentle Wake<br>Snooze Alarm<br>Stop Alarm |
| **Diagnostics** | `sensor.device_name_ip_display`<br>`sensor.device_name_ip_bulb`<br>`sensor.device_name_ip_primary`<br>`button.device_name_rb_display`<br>`button.device_name_rb_primary` | IP Addr Display<br>IP Addr Light Bulb<br>IP Addr Primary<br>Restart Display<br>Restart Primary |

> **💡 Customization**<br>You can safely rename these entities or change their icons directly within the Home Assistant UI without affecting the lamp's firmware.
{: .note }

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/discoverymanage' | relative_url }}" class="btn btn-outline"><- Previous: Enabling and Disabling Discovery</a>
  <a href="{{ '/discoverymanual' | relative_url }}" class="btn btn-purple">Next: Manually Creating MQTT Entities -></a>
</div>