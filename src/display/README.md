## Display Controller Source Files
### <span style="background-color:yellow;color:black;">These files are for the **DISPLAY CONTROLLER ONLY!**</span>

**Do not attempt to install or flash these files to any of the other system controllers (primary, bulb).  This will not work and result in a system that won't boot and will likely require a new upload via USB cable to repair.**

You do not need these source files if you just wish to flash the firmware to your controller.  Instead, locate and download the `Display_Ctrl_vX.XX.bin` file from the assets of the [latest release](https://github.com/Resinchem/Ultimate-Bedside-Lamp/releases/latest).

You only need these source files if you wish to modify the firmware (or are just curious to see how it works).

|File Name | Purpose/Use
|---|---
|`bl_display.ino`|The main sketch
|`html.h`|Header file that contains HTML for the internal web application
|`icons20pt7b.h`|Header font file with symbols and icons for the clock's display

**NOTE**: Due to the size of the main sketch, non-standard partitions are needed for the ESP32.  See [Modifying the Firmware](https://resinchem.github.io/Ultimate-Bedside-Lamp/modifications.html) for more information.