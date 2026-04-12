---
layout: default
title: Display Library and Fonts
parent: Advanced Technical Information
nav_order: 3
---

# Display Library and Fonts
{: .no_toc }

---

<p align="center">
  <img src="images/advanceddisplay_01.jpg" alt="[PLACEHOLDER: Display and Library Configuration]">
</p>

This information is specifically applicable to the **Display Controller**. Driving a high-resolution touch screen with an ESP32 requires a bit of heavy lifting, specifically regarding how we handle custom icons and the library configuration used to talk to the screen hardware.

---

### Custom Icon Fonts

> **🔍 Display Controller ONLY**<br>
> Only the display controller utlizes a custom font file.  There is no font file included, or required, with the primary controller's firmware
{: .important }

To display the various icons on the touch panel (like the alarm gear, the snooze button, etc.) without the overhead of individual image files, the system uses a specialized font file.

* **File:** `icons20pt7b.h`
* **Implementation:** This file contains the glyph data for the icons. If you are flashing the provided `.bin` files, this is already baked into the binary. However, if you are compiling from source, this header must be present in your sketch folder and included at the top of your code:

```cpp
#include "icons20pt7b.h"
```

---

### The TFT_eSPI Library
The display controller utilizes the [TFT_eSPI Library](https://github.com/Bodmer/TFT_espi) by Bodmer. This is a powerful library, but it can be a bit of a "diva" when it comes to configuration.

Unlike most Arduino libraries, TFT_eSPI requires the hardware definitions (pins, display driver type, clock speed) to be defined within the library folder itself—specifically in a file called `User_Setup.h` (or via a custom setup file called in `User_Setup_Select.h`).

#### Swapping Hardware
If you decide to venture off the beaten path and use a different display—even a different "flavor" of the Cheap Yellow Display—you will likely run into issues where the screen is mirrored, the colors are inverted, or it simply remains a very dark brick. 

> **❗ The Support "Shield"**
> I cannot provide support or troubleshooting for hardware components other than those specifically listed in the Build Guide. If you want to use a different display, the task of hunting through the TFT_eSPI configuration files and modifying the firmware logic is a journey you must take alone!
{: .important }

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/advancedconfig' | relative_url }}" class="btn btn-outline"><- Previous: Configuration Files</a>
  <a href="{{ '/advancedpartitions' | relative_url }}" class="btn btn-purple">Next: Partitions and Flashing -></a>
</div>