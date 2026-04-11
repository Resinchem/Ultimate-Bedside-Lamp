---
layout: default
title: Welcome
nav_order: 1
has_toc: false
---

# Ultimate Bedside Lamp and Clock
{: .no_toc }

---
<p align="center">
  <img src="images/01Home_1A_topThumb.jpg" width="250">
</p>
This site contains information regarding the installation, configuration and use of the firmware for this project. It <u>does not</u> contain build instructions, parts lists or anything related to the wiring or physical build.  For information on how to build the system, please see the following:<br><br>

> ### ⚠️ Build Instruction Notice
> This documentation **does not** contain build instructions, parts lists, or wiring diagrams. For the physical build details, please refer to the following resources:
{: .note }

* **YouTube Overview: [{{site.substitutions.youtube_title}}]({{site.links.youtube_video}})**
* **Written guide, with parts list, wiring diagrams and more: [Building the Ultimate Bedside Lamp](https://resinchemtech.blogspot.com/2026/03/ultimate-bedside-lamp.html)**

### How this document is organized
The documentation follows a logical setup flow, grouped into the sections seen in the sidebar:
1.  **Welcome:** About the project, concepts and terminology.  
2.  **Getting Started:** Initial firmware flashing, onboarding and interface setup.
3.  **Setting Up the System:** Web app overview and setting various system options.
4.  **Alarms:** Various methods for setting and responding to alarms, including selecting an alarm sound.
5.  **General System Use:** General operation of the system

The remaining topics cover optional and more advanced options, along with a troubleshooting section.

**If you are setting up your system for the first time, begin with the [Getting Started](/startingmain) section.**

### Hardware Substitutions

The firmware is written for a very specific set of hardware. Any substitutions (different displays, ESP boards, touch sensors) will likely require corresponding changes to the C++ firmware code.

*If you use different hardware, you will need to fork this repository and modify the code yourself. See the [**Modifying the Firmware**](/modifications.md) section for more info.*

Unfortunately, I simply do not have the bandwidth to create, maintain and upgrade multiple versions of the firmware for different hardware configurations.

### Opening Issues
* **Issues:** Reserved strictly for firmware errors or bugs.
* **Discussions:** For enhancement requests, "how-to" questions or problems that aren't firmware releated.

**Issues opened for feature requests or alternate hardware will be closed without response.**
{: .label .label-yellow }

| | [Next: About the Project ->]({{ '/about' | relative_url }}){: .btn .btn-purple } |