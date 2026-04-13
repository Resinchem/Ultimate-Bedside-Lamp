---
layout: default
title: Manually Creating MQTT Entities
parent: Home Assistant Discovery
nav_order: 3
---

# Manually Creating MQTT Entities
{: .no_toc }

---

<p align="center">
  <img src="images/discoverymanual_01.jpg" alt="[PLACEHOLDER: Manual YAML Configuration]">
</p>

While the "One-Click" Discovery is recommended for most users, some Home Assistant enthusiasts prefer the precise control afforded by manual YAML configuration. This method allows you to define unique IDs, specific icons, and custom availability logic directly in your configuration files.

> **❗ Advanced Users Only**<br>Manually creating MQTT entities requires a solid understanding of Home Assistant’s YAML structure and MQTT integration. If you are not comfortable editing `configuration.yaml`, it is highly recommended to use the [Automatic Discovery]({{ '/discoverymanage' | relative_url }}) instead.
{: .important }

---

### YAML Example: Auto-Dimming Switch
If you choose the manual path, you must ensure your topics match the **Publish** and **Subscribe** topics defined in your [MQTT Settings]({{ '/mqtt' | relative_url }}).

Below is an example of how the **Auto-Dimming** entity might be defined in your Home Assistant configuration. 

*Note: This example assumes your chosen topic is `bedlamp`.*

```yaml
mqtt:
  - switch:
      unique_id: bedlamp_autodim
      name: "Auto Dimming"
      state_topic: "stat/bedlamp/autodim"
      command_topic: "cmnd/bedlamp/autodim"
      availability:
        - topic: "stat/bedlamp/display/status"
      payload_on: "ON"
      payload_off: "OFF"
      state_on: "ON"
      state_off: "OFF"
      optimistic: false
      qos: 0
      retain: true    
```

### Further Reference
Because the Home Assistant environment evolves frequently, the firmware is designed to follow standard MQTT conventions. For a comprehensive guide on creating lights, binary sensors, and buttons via YAML, please refer to the official documentation:

* **[Home Assistant MQTT Integration Documentation](https://www.home-assistant.io/integrations/mqtt/)**

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/discoveryentities' | relative_url }}" class="btn btn-outline"><- Previous: Editing or Hiding Entities</a>
  <a href="{{ '/firmwaremain' | relative_url }}" class="btn btn-purple">Next: Firmware Updates -></a>
</div>