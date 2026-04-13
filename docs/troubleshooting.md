---
layout: default
title: Troubleshooting
nav_order: 11
has_children: true
has_toc: false
---

# Troubleshooting
{: .no_toc }

---

<p align="center">
  <img src="images/troubleshooting_01.jpg" alt="Troubleshooting Overview">
</p>

If you find there is a "ghost in the machine" or your lamp isn't behaving as expected, don't panic. Electronics can be finicky, but most issues can be resolved with a bit of systematic checking. 

Before diving into the specific fixes below, first verify that you have reviewed the applicable sections throughout this document and the [Build Guide](https://resinchemtech.blogspot.com/2026/04/ultimate-bedside-lamp.html) to ensure a step wasn't missed during the assembly or configuration process.

---

### 🛑 The Golden Rule of Hardware
As emphasized throughout this project, **the firmware is designed to work with very specific hardware components.** 

> **❗ Support Disclaimer**
> I am unable to provide technical support for builds that use substituted hardware components. If you decide to go "off-book" with different screens, sensors, or controllers, you are officially the lead engineer for that version! Please don't ask for custom firmware modifications for unsupported hardware.
{: .important }

**A Note on the Kauf Bulb:** The RGBW light bulb uses its own factory-installed firmware. If your issues are specific to the bulb's internal software (and not how the lamp talks to it), please consult the [Kauf Bulb GitHub](https://github.com/KaufHA/kauf-rgbww-bulbs) for assistance.

---

### Triage: Where is the problem?

To help you find a solution without scrolling through pages of unrelated text, the troubleshooting guide is broken down into the following categories:

* **[Initial Setup & Connectivity]({{ '/troublesetup' | relative_url }})** – Issues with COM ports, USB drivers, flashing errors, and the initial onboarding hotspot.
* **[Daily Operation & Configuration]({{ '/troubleoperation' | relative_url }})** – Problems with "Save & Reboot" persistence, firmware update failures, and reviewing Config Dumps.
* **[Home Assistant & MQTT]({{ '/toublediscovery' | relative_url }})** – Discovery issues, "stuck" MQTT topics, and integration prerequisites.
* **[FAQ & Getting Help]({{ '/troublefaq' | relative_url }})** – Frequently Asked Questions and the proper etiquette for opening a GitHub Issue or starting a Discussion.

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/advancedpartitions' | relative_url }}" class="btn btn-outline"><- Previous: Partitions and Flashing</a>
  <a href="{{ '/troublesetup' | relative_url }}" class="btn btn-purple">Next: Initial Setup Troubleshooting -></a>
</div>