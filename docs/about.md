# About this Project
<div align="center">

![about_01](images/about_01.jpg)
</div>
After trying dozens of different alarm clocks and various bedside lamps over the years, I was never able to find the perfect combination.  Some alarm clocks were way too bright for a dark bedroom... or the time was too small to read at night without glasses. Others had jarring buzzers or beeping for the alarm with no volume control.  And until recently, these couldn't be controlled via automation.
<br><br>
For the lamp, I needed a "night light" mode that provided very dim light for those middle-of-the-night trips to the bathroom or kitchen, but could provide brighter light for normal tasks or bedtime reading.  

In addition, I need to charge two devices every night, which required even more power cables and outlets.

After trying different combinations of retail devices, I decided it was finally time to build an all-in-one system that resolved the previous issues and created what I would come to call my "Ultimate Bedside Lamp" system.

### Goals
My overall goals for this project would address my previous issues and maybe add a few cool features along the way:
- Single device with both lighting, clock and alarms
- "Normal" and night lighting modes (would be accomplished by using an RGBW bulb and an LED strip)
- Configurable font and size for the display
- Auto-Dimming Display
- Dual charging ports
- Alarms
  - Allow multiple alarms to exist simultaneously
  - Schedule an alarm for the future (days, weeks or even months in advance)
  - Gentle wake option to sound alarm quietly at first, then slowly increase the volume
  - Selectable alarm sounds or tracks
  - Customizable snooze time, from 0 - 60 minutes
- Multiple control options
  - Web app
  - Touch panel
  - External Systems (e.g. Home Assistant)
- Only require WIFI for full operation, with _optional_ external data use.

These were my particular wants and needs.  Yours are likely to be different.  I tried to make the system as modular as possible, but changes are likely to require firmware modifications.

### Caveats
As mentioned, this was designed and built for my specific needs.  The firmware expects very specific hardware and any substitutions may also require that you modify the firmware.  For example, the touch panel uses a display library called [TFT_eSPI](https://github.com/Bodmer/TFT_espi).  For this library to work, you must define your display in the library's configuration.  This also means that you'd have to compile and flash your own version of the firmware.

The [written guide](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html) includes a parts list that also indicates those components that can be replaced without issue and those that would likely require firmware modifications.
<div style="background-color: yellow; color: black; padding: 2px; font-weight: bold">
I simply do not have the bandwidth to create and maintain multiple versions of the firmware to support different hardware combinations. Please do not submit issues requesting use of different hardware!
</div><br>

If you have a question about whether a different component might work and whether firmware modifications would be needed, please post your questions in the [Discussions](https://github.com/Resinchem/Ultimate-Bedside-Lamp/discussions) area of the repo and I'll do my best to address it.