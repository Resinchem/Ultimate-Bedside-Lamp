# Using MQTT and the API
<div align="center">

![integrationmain_01](images/integrationmain_01.jpg)

</div>

You can integrated and control the bedside lamp into other systems, such as Home Assistant, via two primary methods, MQTT and the HTTP API.

### MQTT

Message Queuing Telemetry Transport (MQTT) is a lightweight, open-source messaging protocol designed for the Internet of Things (IoT) and machine-to-machine (M2M) communication.  It is supported by most major programming languages, including C++, Python, Java, PHP and more.  Many home automation systems also support MQTT.  If you have an MQTT broker and an external system that can publish and subscribe to MQTT topics, then you can integrate this project.  If you want know more about MQTT and how it works, you can visit [MQTT.org](https://mqtt.org/).

For use with this project, see the section [MQTT Setup & Topics](/mqtt.md)

### HTTP API

The Appication Programming Interface (API) is designed to accept commands directly from special URLs sent over HTTP.  This methods does not require any additional components or "middlement".  You can simply enter these commands directly into your browser's address bar.  This also means that if you have an external system that can POST an HTTP string, then you can also integrate this project.  Note that while the HTTP API has nearly the same command list as MQTT, it doesn't have the same ability to 'receive' data or show states of entities like MQTT.

For use with this project, see the section [API HTTP Command List](/api.md)

***Neither MQTT or use of the API are required.  These are both optional ways to integrate and control the lamp from extrenal systems*** but are not required for full functionality of the system.