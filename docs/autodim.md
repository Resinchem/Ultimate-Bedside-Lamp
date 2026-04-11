# Auto-Dimming
<div align="center">

![autodim_01](images/autodim_01.jpg)
</div>
The display can be configured to auto-dim based on measured ambient light level.  The brighter the room atmosphere, the brigher the display.  Unlike many other clocks with auto-dimming, in this case, you can specify various ambient light levels and the corresponding display brightness level.<br><br>

![autodim_01b](images/autodim_01b.jpg)

### Setup Auto-Dimming
To setup auto-dimming, return to the Display Settings page:

![autodim_02](images/autodim_02.jpg)

The auto-dim setup can be found on the lower half of the page (under the general display settings).
<div align="center">

![autodim_03](images/autodim_03.jpg)
</div>

These options are used to set the **DEFAULT** auto-dimming values, which means that these settings will be used and applied each time the system starts up or reboots.

#### _Enable Auto-Dimming_ 

Check this box to use auto-dimming by default.  If unchecked, then the brightness level will remain constant unless manually changed.

#### _Auto-Dim Debounce_ 

The ambient light sensor takes a new reading every few milliseconds.  To prevent the display from rapidly changing brightness due to a short change in ambient lighting (say like a reflection), you can specify a "debounce" value, in seconds.  This means that a light level must be crossed _and maintain the new level_ for at least the specified amount of time before the display will change brightness.  Note that the longer the delay, the longer it will take for the display to change.  For example, if you have a 5 second debounce value and turn on the room lights, the display won't respond and change its brightness for at least 5 seconds.  You can experiment with this value to find the optimum debounce value for your situation.  Use 0 to disable debouncing or a maximum of 10 seconds.  A value of 2-5 seconds is recommended as a starting value.

#### _Ambient Light % and Display Brightness_

You can specify up to four different ambient light levels (by percentage) and five difference corresponding display brightness values.  To understand how these work, I'll use the above screen shot as an example.

- Ambient Light Percentage - Specify an ambient light level from **HIGHEST** value at the top to the **LOWEST** value at the bottom.  In my example, the highest or brightest value, 50%, is set at the top and the lowest or dimmest level, 5%, is set at the bottom.  Don't know what percentages to use?  No problem, you can use the 'Calibrate' button.  Calibration is described below.

- Display Brightness - Specify up to five display brightness values, again, specifying the brightest level at the top and decreasing as you move down.  Again, using the example above:
    - If the current light level is above the top Ambient light percentatge (50%), then the highest display brightness value will be applied (94%) will be applied.
    - If the current light level is between the first and second percentages, the display brightness will be set to the second brightness.
    - This repeats for the next couple of ambient and display percentages until the final display brightness, which will be used if the ambient light level is less than the minimum ambient light level percentage.
<div align="center">

![autodim_04](images/autodim_04.jpg)
</div>

#### _Cablibration_
You probably don't know what "50%" room or ambient lighting level actually looks like!  But no problem.. you can click the provided "Calibrate" button where you can see the measured light levels and experiment with different display brightness levels.  Note that you will be prompted that any unsaved auto-dim values will be lost when entering calibration mode.

![autodim_05](images/autodim_05.jpg)

The calibration page shows the "live" ambient room level measurement, updated once per second.  Try different light settings in the room... main lights on/off, curtains opened/closed, etc... to replicate normal different stages of lighting that you might experience.  

You can also experiment with different display brightness levels by using the slider control.  So you can set the room to "maximum" brightess you might expect and then determine the display brightness level you want for that room light level.

**IMPORTANT NOTE**: None of the values displayed or set on this page are saved.  Changing the display brightness via the slider will change the active display brightness, but otherwise all values are temporary and only used to determine the values you want to use.  The bottom portion is not editable and is simply a reference that shows an example as to how you may want to configure your values.

For this reason, you should jot down or otherwise note the ambient and display brightness levels you want to use.  Once known, click the 'Auto-Dim' settings button at the top of the page to return to the auto-dim settings where you can now enter the desired values.

#### _Reset Button_
You can click the Reset Button at any time to restore the auto-dim values to their last saved defaults.  Note that this will overwrite any changes you may have made, but not yet saved.

#### _Save and Reboot Button_
Once you have your desired auto-dim settings entered, click the "Save and Reboot" button to commit these values to the saved configuration file and to make them the new boot **DEFAULT** values.  The system will then reboot and load your new auto-dim values as the defaults.

Note that there are methods to temporarily override the default auto-dim setting (e.g. changing the **ACTIVE** value).  These methods are covered under the [Managing Display Brightness](dispbrightness.md) topic.