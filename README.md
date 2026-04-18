# Ultimate Bedside Lamp
<div align="center">

![lamp_image](docs/images/01Home_1A_topThumb.jpg)
</div>

## Key Features
- **Integrated Hardware:** A single device providing lighting, clock and alarm functions.
   * Three independent ESP-based controllers act together as a single device
- **Versatile Lighting:** Indivdually controllable RGBW bulb and LED Strip for "normal" and "night" modes.
   * Each light source can have independent state, color and brightness settings
- **High-End Display:** Auto-dimming touch screen with configurable fonts, colors and sizes
   * Show time in 12 or 24-hour format.  Outdoor or indoor temperature in °F or °C.
   * Configurable dimming levels based on measured ambient light levels
   * Control lighting, set alearms and more right from the touch panel
- **Dual Touch Sensors:** Up to two configurable touch sensors
   * _Primary functions_: Toggle the lights, or change brightness of lights or the display with a simply tap.
   * _Special Alarm functions_: Buttons can automatically become alarm snooze/stop buttons when alarm sounds.
- **Advanced Alarms:** Define up to five active alarms
   * Allows long-term advanced scheduling and mutliple active alarms can co-exist
   * **Gentle Wake Option:** Alarm sounds quietly and slowly increases in volume.
   * Selectable Alarm Sounds: Use one of the provided sounds or provide your own MP3 sounds
   * Customizable Snooze Range: Set your snooze time from 0-60 minutes.
- **Flexible Control Options**: Multiple methods provided for interacting with the system.
   * Single integrated web app 
   * Local touch controls via display and optional touch sensors
   * _Optionally_ integrate and control from external systems (e.g. Home Assistant, NodeRED, etc.)
- **Smart Integration**: Mutliple methods to _optionally_ interface with external systems
   * MQTT or HTTP API for exchanging data and controlling many features of the system.
   * **Home Assistant**: One-click integration with Home Assistant.  No configuration or YAML needed!
- **Local Control:** Only WIFI needed for full basic functionality.  Internet-based data use is **optional**
   * _Optionally_ sync time to an external NTP server (but also supports local server)
   * _Optionally_ get current weather/temperature from OpenWeatherSource (OWS)
- **Device Charging**: Dual USB ports for charging two devices simultaneously


### Guides, Instructions and More Information

- [Video Overview]() - video overview of the project (YouTube)
- [Build Information](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html) - written guide with parts list, wiring diagrams and general build instructions
- [Firmware Guide](https://resinchem.github.io/Ultimate-Bedside-Lamp/) - Instructions for installing, configuring and using the firmware and integrated web application.

Please review the above carefully.  For reasons described, this project expects a very specific set of hardware.  Substitutions will likely require firmware modifications.

**_I do not have the bandwidth to maintain multiple versions of the firmware for different hardware combinations. <br>Please do not submit issues requesting support for alternate hardware._** 

### Questions?
Can't find an answer above?  Please post your question in the [Discussion](https://github.com/Resinchem/Ultimate-Bedside-Lamp/discussions) are of the repository.  I will do my best to provide guidance as time allows.
