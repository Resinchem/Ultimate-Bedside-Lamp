---
layout: default
title: Using MQTT and the API
nav_order: 7
has_children: true
has_toc: false
---

# Using MQTT and the API
{: .no_toc }

---

<p align="center">
  <img src="images/integrationmain_01.jpg" alt="Integrations Overview">
</p>

This section is for those who prefer to let their lamp take orders from a script rather than a finger. Whether you're a Home Assistant power user or a terminal enthusiast, these tools let you bypass the UI and talk directly to the hardware. 🛠️

> **💡 Note: Optional Features**<br>Neither MQTT nor the API are required for the lamp to function. These are strictly optional tools provided for those who wish to control their lamp through external hardware or software.
{: .note }

---

### 1. MQTT Integration
Message Queuing Telemetry Transport (MQTT) is a lightweight messaging protocol designed for IoT communication. If you have an MQTT broker and a system that can publish/subscribe to topics, you can fully automate the lamp’s states and settings. 

* **Learn more at:** [MQTT.org](https://mqtt.org/)
* **Implementation:** [MQTT Setup & Topics]({{ '/mqtt' | relative_url }})

### 2. HTTP API
The Application Programming Interface (API) allows the lamp to accept commands directly via standard URLs. This method requires no middleman (like a broker); you can even test commands by entering them directly into your browser's address bar. 

* **Implementation:** [API HTTP Command List]({{ '/api' | relative_url }})


<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/alarmactions' | relative_url }}" class="btn btn-outline"><- Previous: Responding to Alarms</a>
  <a href="{{ '/mqtt' | relative_url }}" class="btn btn-purple">Next: MQTT Setup & Topics -></a>
</div>