# Setting Up a Weather Source
<div align="center">

![weather_01](images/weather_01.jpg)
</div>
Currently, the system only uses temperature, but future updated may add other features from external weather sources.  While designed to display outdoor temperature, you can just as easily use it for indoor temperature if that is the source you use.  The system does not have its own temperature sensor, so it simply displays whatever value it receives from an external source. Use of any weather service is entirely optional and the system will work if this option is disabled.<br><br>

**Until a weather source is configured (or disabled), the clock will always shown an initial temperature of 0°.**

To acccess the weather integration settings, return to system integrations from the primary controller's main web page.

![05_System_03](images/05System_03_Main.jpg)

The weather integration settings can be found in a section at the bottom of the system integrations page.

![weather_02](images/weather_02.jpg)

***Temperature Units***<br>
Temperature can be shown in °F or °C.

***Temperature Source***<br>
You can disable the temperature or select one of three sources for the current temperature:
- _None_: Temperature will be disabled and nothing will be shown on the display. No other functionality is impacted.

- _OWM (Open Weather Map)_: This free* service uses your latitude and longitude to create a 'micro-forecast' for your precise location.  The API can run a plethora of current and forecast data.  Setup information for using OWM is covered below.  Use of OWM requires an Internet connection for your system.  If you don't want your system to use the Internet, select a different weather source or simply disable the temperature.

- _MQTT_: The system can receive the current temperature (indoor or outdoor) via MQTT.  This does require that you have an MQTT broker, an external system that publishes the temperature and you must also consider MQTT under the system integrations.  See [MQTT Setup and Topics](/mqtt.md) for more information on enabling MQTT and the topics used, including the temperature.  When selecting MQTT, no other settings are needed for the weather integration.

- _API_: The HTTP API can also be used to set/update the temperature.  When this method is selected, the temperature is only updated when a new API command is received. See [API HTTP Command List](/api.md) for more information on using the API.  No other settings are needed for API weather integration.

<br>

***Open Weather Map Configuration***<br>
These settings are only used if OWM is selected as the source.  They are ignored otherwise.  To use Open Weather Map, you must create an account and generate an API key.
<div align="center">

![weather_03](images/weather_03.jpg)
</div>

The system uses the One Call 3.0 API.  This API is free for up to 1,000 API calls a day.  That means you could update the current temperature once every 2 minutes and still not exceed your "free" API calls.  But since the temperature normally doesn't change that rapidly, a refresh interval will be used to limit the API calls to a minimum of 10 minutes.  This assures that that system will not make more than 144 calls in a given day.  See the [Open Weather Map](https://openweathermap.org/) web site to create an account and generate an API key.  

***OWM API Key***<br>
Simply copy/paste your OWN API key here.  As you can see from this example, you can create multiple keys for different services, like Home Assistant, if you use OWM for more than one service.

![weather_04](images/weather_04.jpg)

***OWM Latitude and Longitude***<br>
Enter the latitude and longitude for your location.  If you don't know your current location coodinates, simply open up Google Maps, find your location and click a spot nearby.  An info card will pop up that lists the latitude and longitude for that location.  Note that depending on your locale, one or both values may be shown as negative numbers.  If a negative number is shown, copy it as a negative number as well.  Do not eliminate the negative sign or you will get the wrong weather.

***OWM Refresh Interval***<br>
This specifies how often the system sends an API call to update the temperature.  As mentioned above, the free OWM is limited to 1,000 API calls a day.  But since the temperature doesn't normally change that rapidly, an API call is only needed every so often.  The minimum refresh value is 10 minutes (resulting in a total of 144 calls a day).  The maximum refresh interval is 1,440 minutes, or a refresh just once a day.

***Page Buttons***<br>

**IMPORTANT**: _Note that unlike some other pages in the web app, the system integration page only has one set of buttons at the bottom that **apply to ALL settings on the page**! This means that if you clikc the reset or save buttons, ALL settings on the page are impacted._  

***Reset Button***<br>
The reset button will reload all the saved configuration values.  This not only applies to the OWM settings but all other system integrations on the page.  If you changed any other values, they will also be reset or reloaded when clicking the RESET button.

***Save and Reboot Button***<br>
This will not only save your weather settings, but ALL settings on the integrations page.  If you changed any other settings, please verify all values are correct before saving.  When you click SAVE & REBOOT, all current integration settings are saved to the configuration file and the controller will reboot, reading these new settings and applying them as the system defaults.

***Cancel Button***<br>
This simply discards any changes made and returns to the controller's main page.