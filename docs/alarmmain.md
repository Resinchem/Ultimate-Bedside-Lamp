---
layout: default
title: Alarms
nav_order: 6
has_children: true
has_toc: false
---

# Alarms
{: .no_toc }

---

<p align="center">
  <img src="images/alarmmain_01.jpg" alt="Alarms Overview Header">
</p>

## The Most Loved (and Hated) Feature!

This section covers everything related to the alarm system. Whether you prefer a gentle, sunrise-style fade-in or a more "urgent" wake-up call, these settings allow you to customize exactly how you transition from sleep to reality. I’ve designed the system to be as reliable as possible, because we know that a failed alarm isn't just a technical glitch—it's a ruined Tuesday.

### Key Features
* **Multi-Alarm Support:** Up to five independent alarms can be active at any time.
* **Custom Audio:** Use the provided sounds or upload your own `.mp3` files to the microSD card.
* **Gentle Wake:** Optional setting that starts the alarm at a low volume and fades it in over 60 seconds.
* **Flexible Scheduling:** Set alarms for specific dates or recurring days (Weekdays, Weekends, or specific days of the week).
* **Custom Snooze:** Adjustable snooze duration from 0 to 60 minutes.

---

### In This Section

* **[Setting Up Alarm Sounds]({{ '/sounds' | relative_url }})** – Installing audio files and mapping track titles. **Start here.**
* **[Alarm Options and Settings]({{ '/alarmoptions' | relative_url }})** – Global configuration for snooze and volume behavior.
* **[Setting and Editing Alarms]({{ '/alarms' | relative_url }})** – Using the scheduling interface and setting recurrences.
* **[Snoozing and Stopping Alarms]({{ '/alarmactions' | relative_url }})** – Managing the hardware response when an alarm is sounding.

---

> **⚠️ Performance Note**<br>Alarms will only sound when the system is in **Normal** operating mode (displaying the clock). If the system is in a Test Mode, powered off, or rebooting when an alarm is triggered, the event will be skipped. Missed alarms are not "queued" for later.
{: .note }

> **❗ Important: Fallback Recommendation**<br>While the system has been extensively tested, it is **highly recommended** that you use a secondary backup alarm until you have full confidence in your configuration. Sorry, but I can't be responsible for missed appointments or events due to misbehaving alarms (but you can still blame me when explaining to the boss why you missed the staff meeting!).
{: .warning }

### Critical Operational Notes
* **Test Mode Overrides:** Alarms will not sound if you are currently editing settings on the touch display or running hardware tests.
* **Snooze Persistence:** If the clock loses power or is restarted during an active **SNOOZE** period, the alarm is canceled and will not resume after the reboot.
* **Conflict Resolution:** If an alarm is already sounding and a second scheduled alarm event occurs, the second event is ignored.

---

<div style="display: flex; justify-content: space-between; align-items: center; margin-top: 40px; border-top: 1px solid #333; padding-top: 20px;">
  <a href="{{ '/commands' | relative_url }}" class="btn btn-outline"><- Previous: Controller Commands</a>
  <a href="{{ '/sounds' | relative_url }}" class="btn btn-purple">Next: Setting Up Alarm Sounds -></a>
</div>