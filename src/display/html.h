// v0.52
// Literal strings
const char *onboard = R"literal(
  <!-- ========================
        ONBOARDING/MOBILE PAGE 
       ======================== -->   
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*VAR_APP_NAME* Onboarding</title>
      <style>
        body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }
      </style>
      </head>
      <body>
        <h1>*VAR_APP_NAME* Onboarding</h1>
        Please enter your WiFi information below. These are CASE-SENSITIVE and limited to 64 characters each.<br><br>
        <form method="post" enctype="application/x-www-form-urlencoded" action="/onboard">
          <table>
            <tr>
            <td><label for="ssid">SSID:</label></td>
            <td><input type="text" name="ssid" maxlength="64" value="*SSID*"></td>
            </tr>
            <tr>
            <td><label for="wifipw">Password:</label></td>
            <td><input type="password" name="wifipw" maxlength="64" value="*WIFIPW*"></td>
            </tr>
          </table><br>
          <b>Device Name: </b>Please give this device a unique name from all other devices on your network, including other installs of *VAR_APP_NAME*. 
          This will be used to set the WiFi and OTA hostnames.<br><br>
          16 alphanumeric (a-z, A-Z, 0-9) characters max, no spaces:
          <table>
            <tr>
            <td><label for="devicename">Device Name:</label></td>
            <td><input type="text" name="devicename" maxlength="16" value="*DEVICENAME*"></td>
            </tr>
          </table>
          <br><br>
          <input type="submit" value="Submit">
        </form>
      </body></html>
)literal";

const char *html_footer = R"literal(
  <footer>
    <br>&copy;2026 by Resinchem Tech. All rights reserved. Documentation, source and license available on <a href="https://github.com/Resinchem/Ultimate-Bedside-Lamp" target="_blank">Github</a>.
  </footer>
  </body>
  </html>
)literal";

const char *mainpage = R"literal(
  <!-- MAIN SETTINGS AND DEFAULTS PAGE -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Main</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* Main Page</H1>
    Firmware Version: *VAR_CURRENT_VER*<br><br>
    <table border="1">
      <tr><td style="padding-left: 5px; padding-right: 5px;">Device Name:</td>
          <td style="padding-left: 5px; padding-right: 5px; color:#016200;">*DEVICENAME*</td</tr>
      <tr><td style="padding-left: 5px; padding-right: 5px;">WiFi Network:</td>
          <td style="padding-left: 5px; padding-right: 5px; color:#016200;">*SSID*</td</tr>
      <tr><td style="padding-left: 5px; padding-right: 5px;">MAC Address:</td>
          <td style="padding-left: 5px; padding-right: 5px; color:#016200;">*MACADDR*</td</tr>
      <tr><td style="padding-left: 5px; padding-right: 5px;">IP Address:</td>
          <td style="padding-left: 5px; padding-right: 5px; color:#016200;">*IPADDR*</td</tr>
    </table><br>
    <span id="spanprimnone" style="color: #ff0000; font-weight: bold;">&nbsp</span> 
    <span id="spanprimbutton" style="display: none;">
      <button id="btnprimary" style="text-align: center; background-color:#cccccc; font-size: 14px; border-radius: 8px; width: 160px; height: 28px;"
              onclick="redirectToPrimary();";"">&lt;&lt; Primary Controller</button>
      <input type="text" id="primaddress" style="display: none" value="0">
    </span>
    <br>
    <table>
      <tr>
        <td><h2>Master Display Brightness</h2></td>
        <td>&emsp;<a href="/">Refresh & Sync</a></td>
      </tr>
    </table>
    <form id="formlive" action="/default-action" method="post">
      <table style="border: 0; border-spacing: 10px;">
        <tr>
          <td style="text-align: right;"><label for="brightness">&#9728;:</label></td>
          <td><input type="range" id="brightness" name="brightness" min="0" max="100" step="1" value="0" 
               title="Change brightness of the display" onchange="updateBrightness()"></td>
          <td>&emsp;<span id="brightpct">50%</span></td> 
        </tr><tr id="autodimtablerow">
          <td>&nbsp;</td>
          <td><input type="checkbox" id="autodim" name="autodim"  value="0" title="Toggle auto-dimming" onchange="updateAutoDim()">&ensp;Auto-Dimming</td>
          <td>&emsp;<span id="autodimmsg" name="autodimmsg" style="color: red; visibility: hidden;">"Temporarily changed. See 'Display' to make permanent change."</span></td>
        </tr>  
      </table>
      <input type="hidden" id="autodimval" name="autodimval" value="0">
      <input type="hidden" id="autodimhold" name="autodimhold" value="0">
      <table border=1 style="display: none;">
        <tr>
          <td>Update Field:</td>
          <td><input type="text" id="updatefield", name="updatefield" value = ""></td>  
       </tr>
      </table>      
    </form>
    <h2>Settings and Defaults</h2>
    <table border=1 cellpadding="5px">
      <tr>
        <td><button type="button" id="btnsystem" name="btnsystem" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="location.href = './system';">System</button></td>
        <td>System settings and integrations</td>
      </tr><tr>
        <td><button type="button" id="btndisplay" name="btndisplay" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="location.href = './display';">Display</button></td>
        <td>Config, touch, brightness, etc.</td>
      </tr><tr>
        <td><button type="button" id="btnclock" name="btnclock" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="location.href = './clock';">Clock</button></td>
        <td>Clock style, time and time zone settings</td>
      </tr><tr>
        <td><button type="button" id="btnalarms" name="btnalarms" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="location.href = './alarms';">Alarms</button></td>
        <td>Alarm setup, sounds and options</td>
    </table>
    <br>
    <h2>Display Controller Commands</h2>
    <b>Note</b>: The commands <b><i>only</i></b> apply to the <u>display</u> controller.<br>
    <table border="1" cellpadding="5px">
      <tr>
        <td><button type="button" id="btnrestart" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 140px; height: 28px;" 
              onclick="confirmRestart()">Restart</button></td>
        <td>This will reboot the <i><b>display controller</b></i> and reload default boot values.</td>
      </tr><tr>
        <td><button type="button" id="btnupdate" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 140px; height: 28px;"
             onclick="location.href = './firmwareupdate';">Firmware Upgrade</button></td>
        <td>Upload and apply new firmware from a compiled .bin file.&nbsp;<i>(beta feature)</i></td>
      </tr><tr>
        <td><button type="button" id="btnotamode" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 140px; height: 28px;"
             onclick="location.href = './otaupdate';">Arudino OTA</button></td>
        <td>Put system in Arduino OTA mode for approx. 20 seconds to flash modified firmware from IDE.</td>
      </tr><tr>
        <td><button type="button" id="btnconfigdump" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 140px; height: 28px;"
             onclick="location.href = './configdump';">Config Dump</button></td>
        <td>See a dump of the current config file contents.</td>
      </tr><tr>
        <td><button type="button" id="btnreset" style="text-align: center; background-color: #fa0f2e; color: #ffffff; font-size: 14px; border-radius: 8px; width: 140px; height: 28px;" 
             onclick="confirmReset()">RESET ALL</button></td>
        <td><b><font color=red>WARNING</font></b>: This will clear all settings, including WiFi, on the <i><b>display controller</b></i>.</td>
      </tr>
    </table>
    <script>
      function fetchData() {
        const brightness = document.getElementById("brightness");
        const brightpct = document.getElementById("brightpct");
        const autoDim = document.getElementById("autodim");
        const autoDimVal = document.getElementById("autodimval");
        const autoDimHold = document.getElementById("autodimhold");
        const autoDimMsg = document.getElementById("autodimmsg");
        const tableRow = document.getElementById("autodimtablerow");
        const primCtrl = document.getElementById("spanprimnone");
        const primBtn = document.getElementById("spanprimbutton");
        const primAddr = document.getElementById("primaddress");
        fetch("/jsonmain")
          .then(response => response.json())
          .then(data => {
            brightness.value = data.cur_brightness;
            brightpct.textContent = (data.cur_brightness).toString() + "%";
            autoDimHold.value = data.auto_dim_hold;
            autoDim.value = data.auto_dim;
            if (Number(data.auto_dim_hold) == 1) {
              autoDim.checked = true;
              autoDimVal.value = 1;
              brightness.disabled = true;
            } else {
              autoDim.checked = false;
              autoDimVal.value = 0;
              brightness.disabled = false;
            }
            if (Number(data.auto_dim) == 1) {
              tableRow.style.display = 'table-row';
              if (data.auto_dim == data.auto_dim_hold) {
                autoDimMsg.style.visibility = "hidden";
              } else {
                autoDimMsg.style.visibility = "visible";
              }
            } else {
              tableRow.style.display = 'none';
            }
            primAddr.value = data.prim_address;
            if ((data.prim_ctrl_enabled) == 0) {
              primCtrl.textContent = "Primary controller not connected!  See 'System' to setup.";
            } else {
              primCtrl.textContent = "";
              primCtrl.style.display = "none";
              primBtn.style.display = "inline";
            }
          })
          .catch(error => console.error("Error fetching data:", error));
      }
      function redirectToPrimary() {
        let primURL = "http://";
        let addr = (document.getElementById("primaddress").value).toString();
        window.location.href = primURL + addr; 
      }
      function updateBrightness() {
        const brightness = document.getElementById("brightness");
        const brightpct = document.getElementById("brightpct");
        const updateField = document.getElementById("updatefield");
        const frmLive = document.getElementById("formlive");
        brightpct.textContent = brightness.value + "%";
        updateField.value = "brightness";
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function updateAutoDim() {
        const autoDim = document.getElementById("autodim");
        const autoDimMsg = document.getElementById("autodimmsg");
        const autoDimVal = document.getElementById("autodimval");
        const autoDimHold = document.getElementById("autodimhold");
        const brightness = document.getElementById("brightness");
        const updateField = document.getElementById("updatefield");
        const frmLive = document.getElementById("formlive");
        if (autoDim.checked) {
          brightness.disabled = true;
          autoDimVal.value = 1;
        } else {
          brightness.disabled = false;
          autoDimVal.value = 0;
        }
        if (autoDim.value == autoDimHold.value) {
          autoDimMsg.style.visibility = "hidden";
        } else {
          autoDimMsg.style.visibility = "visible";
        }
        //Submit
        updateField.value = "autodim";
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function confirmRestart() {
        var userConfirmed = confirm("Are you sure you want to restart the display controller?");
        if (userConfirmed) {
          window.location.href = "./restart";
        }
      }
      function confirmReset() {
        var userConfirmed = confirm("Are you SURE you wish to reset the Display Controller?  You will need to onboard again!");
        if (userConfirmed) {
          window.location.href = "./reset";
        }
      }
      document.addEventListener("DOMContentLoaded", fetchData);
    </script>  
)literal";

const char *systempage = R"literal(
<!-- ================================= 
      SYSTEMS INTEGRATIONS PAGE
      ================================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Integrations</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* System Integrations</H1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT: If you make any changes on this page, you <i>must</i> click 'Save and Reboot' at bottom of page.<br>
    Changes will be applied <b>only</b> after saving and rebooting.</font>
    <form id="frmsystem" action="/applyintegrate" method="post">
      <H2 style="text-decoration: underline;">Primary Controller Integration</H2>
      The primary controller must first be onboarded and connnected to the same WiFi network.  See the documentation for instructions.<br>
      <span id="limitedmsg"><i>This touch panel display will have limited functionality until this integration is established.</i><br></span>
      <table border=0 cellpadding="5px">
        <tr>
          <td>Primary Controller IP Address:</td>
          <td><input type="number" id="primip1" name="primip1" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
              <input type="number" id="primip2" name="primip2" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
              <input type="number" id="primip3" name="primip3" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
              <input type="number" id="primip4" name="primip4" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>
              &nbsp;<input type="text" id="primdisabled" name="primdisabled" 
               style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="Loading. Please wait..." readonly></td>
        </tr>
      </table><br>
      <hr>
      <p id="integratemsg">The following integrations will be imported, if enabled, from the <b>primary</b> controller once it is integrated.</p>
      <H2 style="text-decoration: underline;">Weather Integration</H2>
      <i>To change these values, please see <span id="systemweather">"System"</span> under the primary controller settings.</i><br><br>
      <table border=0>
        <tr>
          <td>Temperature Units:</td>
          <td><input type="text" id="tempunits" name="tempunits" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value = "&deg;F" readonly></td>
        </tr>
        <tr>
          <td>Temperature Source:</td>
          <td><input type="text" id="weathersource" name="weathersource" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value = "None/Disabled" readonly>
          <input type="text" id="tempdisabled" name="tempdisabled" 
                       style="width: 20em; color: #660d70; font-size: 15px; font-weight: bold; font-style: italic; border:none; background-color: #d0d1a9; 
                              pointer-events: none; user-select: none; tabindex: -1;" 
                       value="Loading. Please wait..." readonly></td>
        </tr>
      </table><br>
      <u><b>Open Weather Map</b></u><br>
      <i>These values only applicable if OWM selected above - ignored otherwise</i><br><br>
      <table border="0">
        <tr>
          <td>OWM API Key:</td>
          <td><input type="text" id="owmkey" name="owmkey" 
               style="width: 20em; border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               title="Open Weather Map API key" value="" readonly></td>
          <td><input type="text" id="owmconnected" name="owmconnected" 
               style="color: #660d70; font-size: 15px; font-weight: bold; font-style: italic; border:none; background-color: #d0d1a9; 
                      pointer-events: none; user-select: none; tabindex: -1;" 
                      value="Loading. Please wait..." readonly></td>
        </tr><tr>
          <td>OWM Latitude:</td>
          <td><input type="text" id="owmlat" name="owmlat" style="width: 6em; border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="" readonly></td>
        </tr><tr>
          <td>OWM Longitude:</td>
          <td><input type="text" id="owmlong" name="owmlong" style="width: 6em; border:none; background-color: #d0d1a9;" 
               value="" readonly></td>
        </tr><tr>
          <td>OWM Refresh Interval:</td>
          <td colspan="2"><input type="text" id="tempupdperiod" name="tempupdperiod" 
                           style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr>
      </table><br>
      <H2 style="text-decoration: underline;">MQTT Integration</H2>
      ONLY enter this information if you already have an MQTT broker configured.<br>
      <i>To change MQTT information, please see <span id="systemmqtt">"System"</span> under the primary controller.</i><br>
      <br>
      <table>
        <tr>
          <td><label for="mqttaddr1">Broker IP Address:</label></td>
          <td><input type="text" id="mqttaddr" name="mqttaddr" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="0.0.0.0" readonly></td>
          <td><input type="text" id="mqttdisabled" name="mqttdisabled" style="width: 20em; color: #660d70; font-size: 15px; font-weight: bold; font-style: italic; 
               border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="Loading. Please wait..." readonly></td>
        </tr><tr>
          <td><label for="mqttport">MQTT Broker Port:</label></td>
          <td><input type="text" id="mqttport" name="mqttport" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="" readonly></td>
        </tr><tr>
          <td><label for="mqttuser">MQTT User Name:</label></td>
          <td><input type="text" id="mqttuser" name="mqttuser" autocomplete="off" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="" readonly></td>
        </tr><tr>
          <td><label for="mqttpw">MQTT Password:</label></td>
          <td><input type="password" id="mqttpw" name="mqttpw" autocomplete="off" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="" readonly></td>
        </tr><tr>
          <td><label for="mqtttopicsub">MQTT Subscribe Topic:</label></td>
          <td><input type="text" id="mqtttopicsub" name="mqtttopicsub" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="" readonly></td>
        </tr><tr>
          <td><label for="mqtttopicpub">MQTT Publish Topic:</label></td>
          <td><input type="text" id="mqtttopicpub" name="mqtttopicpub" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
               value="" readonly></td>
        </tr><tr>
          <td><label for="mqttperiod">Telemetry Period:</label></td>
          <td colspan="2"><input type="text" id="mqttperiod" name="mqttperiod" style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" 
                           value="" readonly></td>
        </tr>
      </table><br>
      <i>If you 'Save & Reboot', all changes made will be saved and written to the configuration file.<br>
      The <b>display controller will reboot</b> and any integration changes will be loaded.</i><br><br>
      <table>
        <tr>
          <td><button type="button" id="btnreset" name="btnreset" title="Reconnect and set all values to current boot defaults" 
               style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#e0df80;"
               onclick="document.location='http://*IPADDR*/system'">Reset</button>&nbsp&nbsp;</td>
          <td><button type="submit" id="btnsubmit" title="Save current changes to config and reboot controller" 
               style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#FAADB7;">
               Save & Reboot</button>&nbsp&nbsp;</td>
          <td><button type="button" id="btncancel" title="Cancel changes and return to main page" 
               style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Cancel</button></td>
        </tr>
      </table>
    </form>
    <script>
      function fetchData() {
        fetch("/jsonsystems")
          .then(response => response.json())
          .then(data => {
            const primIP1 = document.getElementById("primip1");
            const primIP2 = document.getElementById("primip2");
            const primIP3 = document.getElementById("primip3");
            const primIP4 = document.getElementById("primip4");
            const tempUnits = document.getElementById("tempunits");
            const weatherSource = document.getElementById("weathersource");
            const owmKey = document.getElementById("owmkey");
            const owmLat = document.getElementById("owmlat");
            const owmLong = document.getElementById("owmlong");
            const owmUpdPeriod = document.getElementById("tempupdperiod");
            const btnImport = document.getElementById("btnimport");
            const mqttAddr = document.getElementById("mqttaddr");
            const mqttPort = document.getElementById("mqttport");
            const mqttUser = document.getElementById("mqttuser");
            const mqttPW = document.getElementById("mqttpw");
            const mqttPub = document.getElementById("mqtttopicpub");
            const mqttSub = document.getElementById("mqtttopicsub");
            const mqttPeriod = document.getElementById("mqttperiod");
            //Messages
            const primMsg = document.getElementById("primdisabled");
            const tempMsg = document.getElementById("tempdisabled");
            const owmMsg = document.getElementById("owmconnected");
            const mqttMsg = document.getElementById("mqttdisabled");
            const importMsg = document.getElementById("importdisabled");
            let primConnected = (data.prim_connected == 1);
            primIP1.value = data.prim_ip_1;
            primIP2.value = data.prim_ip_2;
            primIP3.value = data.prim_ip_3;
            primIP4.value = data.prim_ip_4;
            if (primConnected) {
              const weatherLink = document.getElementById("systemweather");
              const mqttLink = document.getElementById("systemmqtt");
              const primAddr = "http://" + (data.prim_ip_1).toString() + "." + (data.prim_ip_2).toString() + "." + (data.prim_ip_3).toString() + "." + (data.prim_ip_4).toString() + "/system";
              primMsg.value = "Connected";
              primMsg.style.color = "#1e5c14";
              //Enabled links
              weatherLink.innerHTML = "<a href='" + primAddr + "'>&quot;System&quot;</a>";
              mqttLink.innerHTML = "<a href='" + primAddr + "'>&quot;System&quot;</a>";
              //Load other values and integration status
              //Temp/Weather
              if (data.temp_units == 12) {
                tempUnits.value = "Celcius";
              } else {
                tempUnits.value = "Fahrenheit";  
              }
              weatherSource.value = data.weather_source;
              if (data.weather_source != "None/Disabled") {
                tempMsg.value = "Enabled";
                tempMsg.style.color = "#1e5c14";
                if (data.weather_source == "Open Weather Map (OWM)") {
                  owmKey.value = data.owm_key;
                  owmLat.value = data.owm_lat;
                  owmLong.value = data.owm_long;
                  owmUpdPeriod.value = (data.temp_upd_period).toString() + " minutes";
                  //owm connectivity
                  if (data.owm_connected) {
                    owmMsg.value = "Connected";
                    owmMsg.style.color = "#1e5c14";
                  } else {
                    owmMsg.value = "Not Connected!";
                    owmMsg.style.color = "#ff0000";
                  }
                } else {
                  owmKey.value = "";
                  owmLat.value = "";
                  owmLong.value = "";
                  owmUpdPeriod.value = "";
                  owmMsg.value = "";
                }
              } else {
                owmKey.value = "";
                owmLat.value = "";
                owmLong.value = "";
                owmUpdPeriod.value = "";
                owmMsg.value = ""
                tempMsg.value = "Disabled";
                tempMsg.style.color = "#ff0000";
              }
              //MQTT
              mqttAddr.value = data.mqtt_addr; 
              if (data.mqtt_enabled == 1) {
                mqttPort.value = data.mqtt_port;
                mqttUser.value = data.mqtt_user;
                mqttPW.value = data.mqtt_pw;
                mqttPub.value = data.mqtt_pub;
                mqttSub.value = data.mqtt_sub;
                mqttPeriod.value = (data.mqtt_period).toString() + " seconds";
                if (data.mqtt_connected == 1) {
                  mqttMsg.value = "Enabled & Connected";
                  mqttMsg.style.color = "#1e5c14";
                } else {
                  mqttMsg.value = "Enabled - Not Connected!";
                  mqttMsg.style.color = "#a67307";
                }
              } else {
                mqttMsg.value = "Disabled";
                mqttMsg.style.color = "#ff0000";
                mqttPort.value = "";
                mqttUser.value = "";
                mqttPW.value = "";
                mqttPub.value = "";
                mqttSub.value = "";
                mqttPeriod.value = "";
              }
            } else {
              primMsg.value = "Not Connected!";
              primMsg.style.color = "#ff0000";
              tempMsg.value = "Primary Controller Required!";
              tempMsg.style.color = "#ff0000";
              owmMsg.value = "";
              owmMsg.style.color = "#ff0000";
              mqttMsg.value = "Primary Controller Required!";
              mqttMsg.style.color = "#ff0000";
            }
            primIP1.disabled = false;
            primIP2.disabled = false;
            primIP3.disabled = false;
            primIP4.disabled = false;
          })
          .catch(error => console.error("Error fetching data:", error));
      }
      document.addEventListener("DOMContentLoaded", fetchData);
    </script>
)literal";

const char *postIntegrations = R"literal(
  <!-- ----------------------------
        INTEGRATIONS UPDATED PAGE 
       ---------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Integrations Submitted</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* Integrations Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">Attempting to Update Setings:</h2><br>
    <table border=0>
      <tr>
        <td>Connecting to Primary Controller:</td>
        <td><span id="primresults" style="font-weight: bold;">*PRIMRESULT*</span></td>
      </tr><tr>
        <td>Importing MQTT Settings:</td>
        <td><span id="mqttresults" style="font-weight: bold;">*MQTTRESULT*</span></td>
      </tr><tr>
        <td>Importing Weather Settings:</td>
        <td><span id="weatherresults" style="font-weight: bold;">*WEATHERRESULT*</span></td>
      </tr><tr>
        <td>Updating Configuration File:</td>
        <td><span id="configresults" style="color: #1f6614; font-weight: bold;">OK</span></td>
      </tr>
    </table><br><br>
    The display controller will now reboot.  After boot completes, you may:<br><br>
    <button type="button" id="btnintegrations" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/system'">Return to Systems Integrations</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
    <script>
      function updateResults() {
        const prim = document.getElementById("primresults");
        const mqtt = document.getElementById("mqttresults");
        const weather = document.getElementById("weatherresults");
        if (prim.textContent == "1") {
          prim.style.color = "#1f6614";
          prim.textContent = "OK";
        } else {
          prim.style.color = "#ff0000";
          prim.textContent = "*FAILED*";
        }
        if (mqtt.textContent == "1") {
          mqtt.style.color = "#1f6614";
          mqtt.textContent = "OK";
        } else {
          mqtt.style.color = "#ff0000";
          mqtt.textContent = "*FAILED*";
        }
        if (weather.textContent == "1") {
          weather.style.color = "#1f6614";
          weather.textContent = "OK";
        } else {
          weather.style.color = "#ff0000";
          weather.textContent = "*FAILED*";
        }
      }
      document.addEventListener("DOMContentLoaded", updateResults);
    </script>
)literal";

const char *displaypage = R"literal(
<!-- ================================= 
      DISPLAY SETTINGS PAGE
      ================================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Display Settings</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* Display Settings and Options</H1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT:
      <ul>
        <li>TEST/CALIBRATE Button: You may test changes without saving them.  Changes will be lost upon next reboot/restart.</li>
        <li>SAVE and REBOOT: This will save the current values as the new boot defaults.  Controller will reboot.</li>
        <li><i>Each section must be saved independently!</i></li>
      </ul>
    </font>
    See documentation for option details and additional information.<br><br>
    <button type="button" id="btnback" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 140px; height: 35px;"
             onclick="location.href = '/';"><< Return to Main</button><br><br>       
    <h2 style="text-decoration: underline;">Display Configuration</h2>
    <i>Note: Dimensions and touch address cannot be changed here.  If you are using a different display, please see the documentation.</i><br><br>
    <table border="0">
      <tr>
        <td>Display Dimensions:</td>
        <td><input type="text" id="dispdimensions" name="dispdimensions" 
             style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
      </tr><tr>
        <td>Touch Address:</td>
        <td><input type="text" id="touchaddr" name="touchaddr" 
             style="border:none; background-color: #d0d1a9; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
      </tr>
    <table>
    <form id="dispsettings" action="/applydisplay" method="post">
    <table border="0">
      <tr>
        <td>Screen Rotation:</td>
        <td><select id="disprotate" name="disprotate" title="Set rotation to match installed orientation">
              <option value="0">0&deg;</option>
              <option value="1">90&deg;</option>
              <option value="2">180&deg;</option>
              <option value="3">270&deg;</option>
            </select></td>
      </tr><tr>
        <td>Default Brightness:</td>
        <td><input type="range" id="brightness" name="brightness" min="0" max="100" step="1" title="Set display brightness at boot time" value="0"</td>
      </tr><tr>
        <td>Enable Touch:</td>
        <td><input type="checkbox" id="chktouch" name="chktouch" value="0" title="Enable capacitance touch for display" onchange="updateTouchFlag()"></td>
      </tr><tr>
        <td>Enable Onboard LED:</td>
        <td><input type="checkbox" id="chkled" name="chkled" value="0" title="Use onboard LED during boot and to show active alarms" onchange="updateLEDFlag()"></td>
      </tr>
    </table><br>
    <button type="submit" id="disptestbtn" name="disptestbtn" value="test_display" title="Test current display settings"
            style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#a4f086;"
            >Test</button>&ensp;
    <button type="submit" id="dispresetbtn" name="dispresetbtn" value="reset_display" title="Reset all values to current boot defaults"
            style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#e0df80;"
            >Reset</button>&ensp;
    <button type="submit" id="dispsavebtn" name="dispsavebtn" value="save_display" title="Save Display Config changes and reboot controller to activate"
            style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#FAADB7;"
            >Save & Reboot</button><br><br>
    <input type="hidden" id="touchactive" name="touchactive" value="0">
    <input type="hidden" id="ledactive" name="ledactive" value="0">
    <input type="hidden" id="disptestactive" name="disptestactive" value="0">&emsp;<input type="hidden" id="whichdata" name="whichdata" value = "">
    <span id="disptestmsg" style="color: #015201; visibility: hidden;">Check display. Click 'Stop Test' or 'Reset' to return display to normal operation.</span>        
    </form><br>
    <h2 style="text-decoration: underline;">Auto-Dimming Options</h2>
    Configure display to auto-dim based on ambient light levels.  See documenation for details.<br>
    <i>Note: Ambient light reported as a percentage from 0% (dark) to 100% (bright)</i><br><br>
    <form id="dimsettings" action="/dummy" method="post">
      <table border="0">
        <tr>
          <td>Enable Auto-Dimming:</td>
          <td><input type="checkbox" id="chkdim" name="chkdim" value="1" onchange="toggleDimControls()"></td>
        </tr><tr>
          <td>Auto-Dim Debounce:</td>
          <td><input type="number" id="dimdebounce" name="dimdebounce" min="0" max="10" step="1" style="width: 40px;" 
              title="Time light level must be in range before brightness change" disabled>&ensp;seconds (0 min - 10 max)</td>
        </tr>
      </table><br>
      <table border="0">
        <tr>
          <th>&nbsp;</th>
          <th>Ambient Light %</th>
          <th>&emsp;&emsp;&emsp;</th>
          <th>Display Brightness</th>
          <th>&emsp;</th>
        </tr><tr>
          <td><font color="ffff00"><b>Bright</b></font></td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td><input type="range" id="dimbright5" name="dimbright5" min="0" max="100" step="1" value="50" 
               title="Display brightness when light level above top ambient percentage" onchange="updateDispPcts()" disabled></td>
          <td>&emsp;<span id="brightpct5">&nbsp</span>
        </tr><tr>  
          <td>&nbsp;</td>
          <td style="text-align: center;"><input type="number" id="amblevel4" name="amblevel4" min="0" max="100" step="1" 
            title="Brightest light level for auto-dimming" value="0" style="width: 40px;" disabled>%</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td>&nbsp</td>
        </tr><tr>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td><input type="range" id="dimbright4" name="dimbright4" min="0" max="100" step="1" value="50" 
               title="Display brightness when light level between top two ambient values" onchange="updateDispPcts()" disabled></td>
          <td>&emsp;<span id="brightpct4">&nbsp</span>
        </tr><tr>  
          <td>&nbsp;</td>
          <td style="text-align: center;"><input type="number" id="amblevel3" name="amblevel3" min="0" max="100" step="1" 
              title="Second highest light level for auto-dimming" value="0" style="width: 40px;" disabled>%</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
        </tr><tr>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td><input type="range" id="dimbright3" name="dimbright3" min="0" max="100" step="1" value="50" 
            title="Display brightness when light level between middle two ambient values" onchange="updateDispPcts()" disabled></td>
          <td>&emsp;<span id="brightpct3">&nbsp</span>
        </tr><tr>  
          <td>&nbsp;</td>
          <td style="text-align: center;"><input type="number" id="amblevel2" name="amblevel2" min="0" max="100" step="1" 
            title="Third highest light level for auto-dimming" value="0" style="width: 40px;" disabled>%</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td>&nbsp</td>
        </tr><tr>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td><input type="range" id="dimbright2" name="dimbright2" min="0" max="100" step="1" value="50" 
               title="Display brightness when light level between bottom two ambient values" onchange="updateDispPcts()" disabled></td>
          <td>&emsp;<span id="brightpct2">&nbsp</span>
        </tr><tr>  
          <td>&nbsp;</td>
          <td style="text-align: center;"><input type="number" id="amblevel1" name="amblevel1" min="0" max="100" step="1" 
            title="Lowest light level for auto-dimming" value="0" style="width: 40px;" disabled>%</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
        </tr><tr>
          <td><font color="#696969">Dim</font></td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
          <td><input type="range" id="dimbright1" name="dimbright1" min="0" max="100" step="1" value="50" 
               title="Minimum display brightness when light level below bottom ambient level" onchange="updateDispPcts()" disabled></td>
          <td>&emsp;<span id="brightpct1">&nbsp</span>
        </tr>
      </table><br>
      <button type="button" id="dimtestbtn" name="dimtestbtn" title="Show calibration page"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#a4f086;"
              onclick="testDim()">Calibrate</button>&ensp;
      <button type="button" id="dimresetbtn" name="dimresetbtn" title="Reset all values to current boot defaults"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#e0df80;"
              onclick="fetchData('dim')">Reset</button>&ensp;
      <button type="submit" id="dimsavebtn" name="dimsavebtn" title="Save Auto-Dim changes and reboot controller to activate"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#FAADB7;"
              >Save & Reboot</button>
      <input type="hidden" id="dimactive" name="dimactive" value="0">
      </form>
    <script>
      function fetchData(whichData = "all") {
        fetch("/jsondisplay")
          .then(response => response.json())
          .then(data => {
            const testRunning = (data.disp_test_active == 1);
            document.getElementById("whichdata").value = whichData;   
            if ((whichData == "disp") || (whichData == "all")) {
              document.getElementById("dispdimensions").value = data.disp_resolution;
              document.getElementById("touchaddr").value = data.touch_addr;
              document.getElementById("disprotate").selectedIndex = data.disp_rotate;
              document.getElementById("brightness").value = data.brightness;
              if (data.touch_enabled) {
                document.getElementById("chktouch").checked = true;
                document.getElementById("touchactive").value = "1";
              } else {
                document.getElementById("chktouch").checked = false;
                document.getElementById("touchactive").value = "0";
              }
              if (data.use_led) {
                document.getElementById("chkled").checked = true;
                document.getElementById("ledactive").value = "1";
              } else {
                document.getElementById("chkled").checked = false;
                document.getElementById("ledactive").value = "0";
              }
            }
            if ((whichData == "dim") || (whichData == "all")) {
              const enableDim = data.auto_dim;
              const debounce = document.getElementById("dimdebounce");
              const al_1 = document.getElementById("amblevel1");
              const al_2 = document.getElementById("amblevel2");
              const al_3 = document.getElementById("amblevel3");
              const al_4 = document.getElementById("amblevel4");
              const db_1 = document.getElementById("dimbright1");
              const db_2 = document.getElementById("dimbright2");
              const db_3 = document.getElementById("dimbright3");
              const db_4 = document.getElementById("dimbright4");
              const db_5 = document.getElementById("dimbright5");
              const db_1_pct = document.getElementById("brightpct1");
              const db_2_pct = document.getElementById("brightpct2");
              const db_3_pct = document.getElementById("brightpct3");
              const db_4_pct = document.getElementById("brightpct4");
              const db_5_pct = document.getElementById("brightpct5");
              debounce.value = data.dim_debounce;
              al_1.value = data.amb_level_1;
              al_2.value = data.amb_level_2;
              al_3.value = data.amb_level_3;
              al_4.value = data.amb_level_4;
              db_1.value = data.dim_bright_1;
              db_2.value = data.dim_bright_2;
              db_3.value = data.dim_bright_3;
              db_4.value = data.dim_bright_4;
              db_5.value = data.dim_bright_5;
              db_1_pct.textContent = db_1.value + "%";
              db_2_pct.textContent = db_2.value + "%";
              db_3_pct.textContent = db_3.value + "%";
              db_4_pct.textContent = db_4.value + "%";
              db_5_pct.textContent = db_5.value + "%";
              if (enableDim) {
                document.getElementById("chkdim").checked = true;
                document.getElementById("dimactive").value = "1";
                debounce.disabled = false;
                al_1.disabled = false;
                al_2.disabled = false;
                al_3.disabled = false;
                al_4.disabled = false;
                db_1.disabled = false;
                db_2.disabled = false;
                db_3.disabled = false;
                db_4.disabled = false;
                db_5.disabled = false;
              } else {
                document.getElementById("chkdim").checked = false;
                document.getElementById("dimactive").value = "0";
                debounce.disabled = true;
                al_1.disabled = true;
                al_2.disabled = true;
                al_3.disabled = true;
                al_4.disabled = true;
                db_1.disabled = true;
                db_2.disabled = true;
                db_3.disabled = true;
                db_4.disabled = true;
                db_5.disabled = true;
              }
            }
            if (testRunning) {
              testDisplay();
            }
        })
        .catch(error => console.error("Error fetching data:", error));
      }
      function updateTouchFlag() {
        const chkTouch = document.getElementById("chktouch");
        const txtTouch = document.getElementById("touchactive");
        if (chkTouch.checked) {
          txtTouch.value = "1";
        } else {
          txtTouch.value = "0";
        }
      }
      function updateLEDFlag() {
        const chkLED = document.getElementById("chkled");
        const txtLED = document.getElementById("ledactive");
        if (chkLED.checked) {
          txtLED.value = "1";
        } else {
          txtLED.value = "0";
        }
      }
      function updateDimFlag() {
        const chkDim = document.getElementById("chkdim");
        const txtDim = document.getElementById("dimactive");
        if (chkDim.checked) {
          txtDim.value = "1";
        } else {
          txtDim.value = "0";
        }
      }
      function updateDispPcts() {
        const db_1 = document.getElementById("dimbright1");
        const db_2 = document.getElementById("dimbright2");
        const db_3 = document.getElementById("dimbright3");
        const db_4 = document.getElementById("dimbright4");
        const db_5 = document.getElementById("dimbright5");
        const db_1_pct = document.getElementById("brightpct1");
        const db_2_pct = document.getElementById("brightpct2");
        const db_3_pct = document.getElementById("brightpct3");
        const db_4_pct = document.getElementById("brightpct4");
        const db_5_pct = document.getElementById("brightpct5");
        db_1_pct.textContent = db_1.value + "%";
        db_2_pct.textContent = db_2.value + "%";
        db_3_pct.textContent = db_3.value + "%";
        db_4_pct.textContent = db_4.value + "%";
        db_5_pct.textContent = db_5.value + "%";
      }
      function enableControls(enable) {
        const rotate = document.getElementById("disprotate");
        const bright = document.getElementById("brightness");
        const touch = document.getElementById("chktouch");
        const led = document.getElementById("chkled");
        const btnTestDisp = document.getElementById("disptestbtn");
        const btnResetDisp = document.getElementById("dispresetbtn");
        const btnSaveDisp = document.getElementById("dispsavebtn");
        const enableDim = document.getElementById("chkdim");
        const debounce = document.getElementById("dimdebounce");
        const al_1 = document.getElementById("amblevel1");
        const al_2 = document.getElementById("amblevel2");
        const al_3 = document.getElementById("amblevel3");
        const al_4 = document.getElementById("amblevel4");
        const db_1 = document.getElementById("dimbright1");
        const db_2 = document.getElementById("dimbright2");
        const db_3 = document.getElementById("dimbright3");
        const db_4 = document.getElementById("dimbright4");
        const db_5 = document.getElementById("dimbright5");
        const btnTestDim = document.getElementById("dimtestbtn");
        const btnResetDim = document.getElementById("dimresetbtn");
        const btnSaveDim = document.getElementById("dimsavebtn");
        rotate.disabled = !enable;
        bright.disabled = !enable;
        touch.disabled = !enable;
        led.disabled = !enable;
        btnTestDisp.disabled = !enable;
        btnResetDisp.disabled = !enable;
        btnSaveDisp.disabled = !enable;
        enableDim.disabled = !enable;
        if (enableDim.checked) {
          debounce.disabled = !enable;
          al_1.disabled = !enable;
          al_2.disabled = !enable;
          al_3.disabled = !enable;
          al_4.disabled = !enable;
          db_1.disabled = !enable;
          db_2.disabled = !enable;
          db_3.disabled = !enable;
          db_4.disabled = !enable;
          db_5.disabled = !enable;
        } else {
          debounce.disabled = true;
          al_1.disabled = true;
          al_2.disabled = true;
          al_3.disabled = true;
          al_4.disabled = true;
          db_1.disabled = true;
          db_2.disabled = true;
          db_3.disabled = true;
          db_4.disabled = true;
          db_5.disabled = true;
        }
        btnTestDim.disabled = !enable;
        btnResetDim.disabled = !enable;
        btnSaveDim.disabled = !enable;
      }
      function enableDimControls() {
        //Controls need to be enable for submit
        document.getElementById("dimdebounce").disabled = false;
        document.getElementById("amblevel1").disabled = false;
        document.getElementById("amblevel2").disabled = false;
        document.getElementById("amblevel3").disabled = false;
        document.getElementById("amblevel4").disabled = false;
        document.getElementById("dimbright1").disabled = false;
        document.getElementById("dimbright2").disabled = false;
        document.getElementById("dimbright3").disabled = false;
        document.getElementById("dimbright4").disabled = false;
        document.getElementById("dimbright5").disabled = false;
      }
      function toggleDimControls() {
        const chkdim = document.getElementById("chkdim");
        const txtDim = document.getElementById("dimactive");
        const debounce = document.getElementById("dimdebounce");
        const al_1 = document.getElementById("amblevel1");
        const al_2 = document.getElementById("amblevel2");
        const al_3 = document.getElementById("amblevel3");
        const al_4 = document.getElementById("amblevel4");
        const db_1 = document.getElementById("dimbright1");
        const db_2 = document.getElementById("dimbright2");
        const db_3 = document.getElementById("dimbright3");
        const db_4 = document.getElementById("dimbright4");
        const db_5 = document.getElementById("dimbright5");
        if (chkdim.checked) {
          debounce.disabled = false;
          al_1.disabled = false;
          al_2.disabled = false;
          al_3.disabled = false;
          al_4.disabled = false;
          db_1.disabled = false;
          db_2.disabled = false;
          db_3.disabled = false;
          db_4.disabled = false;
          db_5.disabled = false;
          txtDim.value = "1";
        } else {
          debounce.disabled = true;
          al_1.disabled = true;
          al_2.disabled = true;
          al_3.disabled = true;
          al_4.disabled = true;
          db_1.disabled = true;
          db_2.disabled = true;
          db_3.disabled = true;
          db_4.disabled = true;
          db_5.disabled = true;
          txtDim.value = "0";
        }
      }
      function testDisplay() {
        const frm = document.getElementById("frmdispsettings");
        const testFlag = document.getElementById("disptestactive");
        const chkTouch = document.getElementById("chktouch");
        const chkLED = document.getElementById("chkled");
        const touchFlag = document.getElementById("touchactive");
        const ledFlag = document.getElementById("ledactive");
        const btnTest = document.getElementById("disptestbtn");
        const btnReset = document.getElementById("dispresetbtn");
        const msg = document.getElementById("disptestmsg");
        if (btnTest.innerHTML == "Test") {
          //start test
          enableControls(0);
          testFlag.value = "1";
          if (chkTouch.checked) {
            touchFlag.value = "1";
          } else {
            touchFlag.value = "0";
          }
          if (chkLED.checked) {
            ledFlag.value = "1";
          } else {
            ledFlag.value = "0";
          }
          btnTest.innerHTML = "Stop Test";
          btnTest.disabled = false;
          btnReset.disabled = false;
          msg.style.visibility = "visible";
        } else {
          //stop test
          enableControls(1);
          testFlag.value = "0";
          btnTest.innerHTML = "Test";
          msg.style.visibility = "hidden";
        } 
      }
      function resetDisplay() {
        const msg = document.getElementById("disptestmsg");
        const testFlag = document.getElementById("disptestactive");
        const btnTest = document.getElementById("disptestbtn");
        enableControls(1);
        testFlag.value = "0";
        btnTest.innerHTML = "Test";
        msg.style.visibility = "hidden";
        fetchData("disp");
      }
      function testDim() {
        var userConfirmed = confirm("Any unsaved changes will be lost.  Are you sure you wish to enter calibration?");
        if (userConfirmed) {
          window.location.href = "/calibratedim";
        }
      }
      document.addEventListener("DOMContentLoaded", fetchData());
      document.getElementById("dispsettings").addEventListener("submit", async function (event) {
        event.preventDefault();
        const frm = document.getElementById("dispsettings");
        const testMsg = document.getElementById("disptestmsg");
        const btnVal = event.submitter.value;
        if (btnVal === "save_display") {
          this.action = "/applydisplay";
          this.submit();
          return;
        }
        if (btnVal == "test_display") {
          await testDisplay();
          const data = {
            brightness: Number(frm.elements.brightness.value),
            disp_rotate: Number(frm.elements.disprotate.value),
            touch_enabled: Number(frm.elements.touchactive.value),
            use_led: Number(frm.elements.ledactive.value),
            disp_test_active: Number(frm.elements.disptestactive.value)
          };
          const jsonData = JSON.stringify(data);
          try {
            const response = await fetch("/testdisplay", 
            { method: "POST", 
              headers: {
                "Content-Type": "application/json",
              },
              body: jsonData,
            });
            if (response.ok) {
              //this.reset();
            } else {
              throw new Error("Could not extract data");
            }
          } catch (error) {
            testMsg.textContent = "Error: ${error.message}";
            testMsg.style.color = "red";
          }
        } else if (btnVal === "reset_display") {
          const testFlag = Number(document.getElementById("disptestactive").value);
          await resetDisplay();
          const data = {
            disp_test_active: testFlag
          };
          const jsonData = JSON.stringify(data);
          try {
            const response = await fetch("/resetdisplay", 
            { method: "POST", 
              headers: {
                "Content-Type": "application/json",
              },
              body: jsonData,
            });
            if (response.ok) {
              //this.reset();
              fetchData("disp");
            } else {
              throw new Error("Could not extract data");
            }
          } catch (error) {
            testMsg.textContent = "Error: ${error.message}";
            testMsg.style.color = "red";
          }
        }
      });
      document.getElementById("dimsettings").addEventListener("submit", async function (event) {
        event.preventDefault();
        const frm = document.getElementById("dimsettings");
        //Enable all controls for post
        await enableDimControls();
        this.action = "/applydim";
        this.submit();
        return;
      });
    </script>
)literal";

const char *postdisplaysettings = R"literal(
  <!-- -------------------------------
         Handle display settings save
       ------------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Settings Updating</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* Display Defaults Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">*WHICHONE* defaults saved to configuration file.</h2><br>
    The display controller will now reboot.  After boot completes, you may:<br><br>
    <button type="button" id="btnlights" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/display'">Return to display Settings</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
)literal";

const char *calibrate = R"literal(
  <!-- -------------------------------
         Light Level Calibration Page
       ------------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Dimming Calibration</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1>Display Auto-Dimming Calibration</h1>
    This page is used to retrieve live ambient light data from the light sensor.
    <font color=red>
    <ul>
      <li>Values are returned from dark (0%) to full bright (100%).</li>
      <li>Value will update once per second.</li>
      <li><b>No settings are changed or saved on this page!</b></li>
    </ul>
    </font>
     <i>See the documentation for additional details.</i><br><br>
    <button type="button" id="btnback" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 150px; height: 35px;"
         onclick="location.href = './display#dimsettings';"><< Auto-Dim Settings</button><br><br>
    <form id="testdata">      
      <table style="border: 0; color: #095402; font-size: 30px;">
        <tr>
          <td>Current Light Level:</td>
          <td>&emsp;<span id="lightlevel">Loading...</span>%</td>
          <td>&nbsp;</td>
        </tr><tr>
          <td>Display Brightness:</td>
          <td>&emsp;<input type="range" id="dispbrighttest" name="dispbrighttest" min="0" max="100" step="1" value="50" 
              title="Set a TEST brightness level for current light level. Value will not be saved." onchange="updateBrightness()">
          <td>&emsp;<span id="brightpct">50%</span></td>
        </tr>
      <table><br>
      <input type="hidden" id="initbrightset" name="initbrightset" value="0">
    </form>
    <span id="errmsg" style="color: red; display: none;">&nbsp;<br></span>
    Change ambient light level in the room to expected common values and note the light level percentage above.<br>
    Once you have up to four different levels, you can use these to specify the display brightness for a given ambient light range.<br><br>
    <h3>This is <u>just an example</u> of how you might set up display auto-brightness based on different ambient light levels:</h3><br>
    <table border=0>
      <tr>
        <th>&nbsp;</th>
        <th>Ambient Light %</th>
        <th>&emsp;&emsp;&emsp;</th>
        <th>Display Brightness</th>
        <th>&emsp;</th>
      </tr><tr>
        <td><font color="ffff00"><b>Bright</b></font></td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td><input type="range" id="dimbright5" name="dimbright5" min="0" max="100" step="1" value="90" 
              style="pointer-events: none; user-select: none; tabindex: -1; opacity: 0.9;" readonly></td>
        <td>&emsp;(Ambient Light > 85%)</td>
      </tr><tr>  
        <td>&nbsp;</td>
        <td style="text-align: center;">85%</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr><tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td><input type="range" id="dimbright4" name="dimbright4" min="0" max="100" step="1" value="70" 
              style="pointer-events: none; user-select: none; tabindex: -1; opacity: 0.7;" readonly></td>
        <td>&emsp;(Ambient Light >60% & <85%)
      </tr><tr>  
        <td>&nbsp;</td>
        <td style="text-align: center;">60%</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr><tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td><input type="range" id="dimbright3" name="dimbright3" min="0" max="100" step="1" value="50" 
              style="pointer-events: none; user-select: none; tabindex: -1; opacity: 0.5;" readonly></td>
        <td>&emsp;(Ambient Light >40% & <60%)</td>
      </tr><tr>  
        <td>&nbsp;</td>
        <td style="text-align: center;">40%</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr><tr>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td><input type="range" id="dimbright2" name="dimbright2" min="0" max="100" step="1" value="30"
              style="pointer-events: none; user-select: none; tabindex: -1; opacity: 0.3;" readonly></td>
        <td>&emsp;(Ambient Light >20% & <40%)</td>
      </tr><tr>  
        <td>&nbsp;</td>
        <td style="text-align: center;">20%</td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
      </tr><tr>
        <td><font color="#696969">Dim</font></td>
        <td>&nbsp;</td>
        <td>&nbsp;</td>
        <td><input type="range" id="dimbright1" name="dimbright1" min="0" max="100" step="1" value="10" disabled></td>
        <td>&emsp;(Ambient Light <20%)</td>
      </tr>
    <table><br>
    <script>
      function fetchData() {
        const initLoaded = document.getElementById("initbrightset");
        let ambientLevel = 0;
        let dispLevel = 50;
        fetch("/calibrateupdate") 
          .then(response => response.text())
          .then(data => {
            const delimPos = data.indexOf("|");
            if (delimPos > 0) {
              ambientLevel = data.substring(0, delimPos);
              dispLevel = data.substring(delimPos+1);
            }
            if (delimPos > 0) {
              document.getElementById("lightlevel").textContent = data.substring(0, delimPos);
              dispLevel = (data.substring(delimPos + 1));
            } else {
              document.getElementById("lightlevel").textContent = data;
              dispLevel = 0;
            }
            if (initLoaded.value == "0") {
              initLoaded.value = "1";
              document.getElementById("dispbrighttest").value = dispLevel;
              document.getElementById("brightpct").textContent = dispLevel + "%";

            }
          });
      }
      function updateBrightness() {
        const frm = document.getElementById("testdata");
        const errMsg = document.getElementById("errmsg");
        frm.action = "/setcalbright";
        frm.submit();
      }
      setInterval(fetchData, 1000);
      document.addEventListener("DOMContentLoaded", fetchData);
    </script>
)literal";

const char *clockpage = R"literal(
  <!-- =============================
        TIME AND CLOCK SETTINGS
       ============================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Clock and Time</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* Clock and Time Settings</H1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT: If you make any changes on this page, you <i>must</i> click 'Save and Reboot' for each modified section.<br>
    Changes will be applied <b>only</b> after saving and rebooting.</font><br><br>
    <i>See documentation for option details and additional information.</i><br><br>
    <button type="button" id="btnback" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 140px; height: 35px;"
             onclick="location.href = '/';"><< Return to Main</button><br><br>       
    <h2 style="text-decoration: underline;">Clock Size and Appearance</h2>
    This section applies to the main clock/time display.<br><br>
    <input type="hidden" id="whichdata" name="whichdata" value = "">
    <form id="formstyle" action="/applyclock" method="post">
    <table border="0">
      <tr>
        <td style="padding-top: 5px; padding-bottom: 5px;">Clock Style:</td>
        <td style="padding-top: 5px; padding-bottom: 5px;"><select id="clockstyle" name="clockstyle" title="Select font style for clock/display">
            <!-- styles added dynamically via JS -->
            </select></td>
        <td style="padding-top: 5px; padding-bottom: 5px;"><span id="stylemsg" name="stylemsg" style="color: red;">&nbsp;</span></td>
      </tr><tr>
        <td style="padding-top: 5px; padding-bottom: 5px;">Color:</td>
        <td style="padding-top: 5px; padding-bottom: 5px;"><input type="color" id="clockcolor" name="clockcolor" title="Set default clock color"
            style="width: 100%;" value="#ffffff"/></td>
        <td style="padding-top: 5px; padding-bottom: 5px;"><span id="colormsg" name="colormsg" style="color: red;">&nbsp;</span></td>
      </tr><tr>
        <td style="padding-top: 5px; padding-bottom: 5px;">Hour Display:</td>
        <td style="padding-top: 5px; padding-bottom: 5px;">
            <input type="radio" id="12hr" name="hourformat" value="12">12-hour&ensp;
            <input type="radio" id="24hr" name="hourformat" value="24">24-hour</td>
        <td style="padding-top: 5px; padding-bottom: 5px;"><span id="hourfmtmsg" name="hourfmtmsg" style="color: red;">&nbsp;</span></td>
      </tr>
    </table><br>
    <button type="submit" id="clocktestbtn" name="clocktestbtn" value="test_clock" title="Test clock settings"
            style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#a4f086;"
            >Test</button>&ensp;
    <button type="submit" id="clockresetbtn" name="clockresetbtn" value="reset_clock" title="Reset back to default settings"
            style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#e0df80;"
            >Reset</button>&ensp;
    <button type="submit" id="clocksavebtn" name="clocksavetbtn" value="save_clock" title="Save current settings as new defaults"
            style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#faadb7;"
            >Save & Reboot</button>
    </form><br>
    <h2 style="text-decoration: underline;">Time Options and Syncing</h2>
    If manual is selected, time will need to be reset upon each reboot or power loss.<br><br>
    <form id="formtime" action="/applytime" method="post">
    <table border="0">
      <tr>
        <td>Time Source:</td>
        <td><input type="radio" id="sourcenone" name="timesource" value="0" title="Manually set time" onchange="timeSourceChanged()">Manual&ensp;
            <input type="radio" id="sourcentp" name="timesource" value="1" title="Get time from NTP server" onchange="timeSourceChanged()">NTP&ensp;
            <input type="radio" id="sourcemqtt" name="timesource" value="2" title="Get time from MQTT (must be sent by external source)" onchange="timeSourceChanged()">MQTT&ensp;
            <input type="radio" id="sourceapi" name="timesource" value="3" title="Get time from API (must be sent by external source)" onchange="timeSourceChanged()">API</td>
        <td>&emsp;&emsp;<span id="timesrcmsg" name="timesrcmsg" style="color: red"><i>&nbsp;</i></span></td>
      </tr>
    </table><br>
    <span style="font-size: 16px; text-decoration: underline;">Manual Date/Time</span>&ensp;<i>Only applicable if source is Manual.</i><br>
    <table border="0">
      <tr>
        <td>Set Date and Time:</td>
        <td><input type="datetime-local" id="localtime" name="localtime" min="2025-01-01T00:00:00" max="2099-12-31T23:59:59" 
             title="MUST be in mm/dd/yyyy hh:mm am/pm format" value="" disabled><td>
        <td>&ensp;<button type="submit" id="settimebtn" name="setimebtn" value="set_time" title="Apply time to clock now"
            style="text-align: center; font-size: 16px; border-radius: 10px; width: 110px; height: 30px; background-color:#FAADB7;"
            disabled>Apply Now</button></td>
        <td>&ensp;<span id="timemsg" name="timemsg" style="color: red;">&nbsp;</span></td>
      </tr>
    </table><br>
    <!-- Hidden fields for manual time processing -->
    <input type="hidden" id="manualyear" name="manualyear" value="2025">
    <input type="hidden" id="manualmonth" name="manualmonth" value="1">
    <input type="hidden" id="manualday" name="manualday" value="1">
    <input type="hidden" id="manualhour" name="manualhour" value="0">
    <input type="hidden" id="manualminute" name="manualminute" value="0">
    <!-- end hidden -->
    <span style="font-size: 16px; text-decoration: underline;">NTP Server Settings</span>&ensp;<i>Only applicable if source is NTP.</i><br>
    <table border="0">
      <tr>
        <td>NTP Server URL:</td>
        <td><input type="text" id="ntpserver" name="ntpserver" maxlength="50" style="width: 15em;" 
             title="NTP Server URL. Use 'Lookup' if unsure"  value="" disabled></td>
        <td><a href="https://www.ntppool.org" target="_blank">Lookup...</a></td>
      </tr><tr>
        <td>Time Zone (posix):</td>
        <td><input type="text" id="timezone" name="timezone" maxlength="50" style="width: 15em;" 
             title="Time zone in POSIX format. Use 'Lookup' if unsure" value="" disabled></td>
        <td><a href="https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv" target="_blank">Lookup...</a></td>
      </tr><tr>
        <td>Sync Interval (minutes):</td>
        <td><input type="number" id="syncint" name="syncint" min="15" max="1440" step="15" style="width: 4em;" 
             title="How often to sync time from NTP server, in minutes" value="60" disabled>&nbsp;(15 min - 1440 max)</td>
        <td><span id="syncintcmsg" name="syncintcmsg" style="color: red">&nbsp;</span></td>
      </tr>
    <table><br>
    <span style="font-size: 16px; text-decoration: underline;">MQTT Time Settings</span>&ensp;<i>Only applicable if source is MQTT.</i><br>
    <table border="0">
      <tr>
        <td>MQTT Time Full Topic:</td>
        <td><input type="text" id="mqtttopic" name="mqtttopic" maxlength="50" style="width: 10em;" 
             title="Topic must match the topic used by external source" value="" disabled></td>
        <td>&ensp;<i>Clock will listen on this topic for time updates</i></td>
      </tr><tr>
        <td>MQTT Time Use:</td>
        <td><input type="radio" id="mqttsync" name="mqttuse" value="0" disabled>Sync Only&ensp;
            <input type="radio" id="mqttlive" name="mqttuse" value="1" disabled>Live Updates</td>
        <td><span id="mqttusemsg" name="mqttusemsg" style="color: red;">&nbsp;</span></td>
      </tr>
    <table><br>
    <span style="font-size: 16px; text-decoration: underline;">API Time Settings</span>&ensp;<i>Only applicable if source is API.</i><br>
    <table border="0">
      <tr>
        <td>API Time Use:</td>
        <td><input type="radio" id="apisync" name="apiuse" value="0" disabled>Sync Only&ensp;
            <input type="radio" id="apilive" name="apiuse" value="1" disabled>Live Updates</td>
        <td><span id="apiusemsg" name="apiusemsg" style="color: red;">&nbsp;</span></td>
      </tr>
    <table><br>
    <button type="button" id="timeresetbtn" name="timeresetbtn" value="reset_time" title="Reset back to default settings"
            style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#e0df80;" 
            onclick="fetchData('time')">Reset</button>&ensp;
    <button type="submit" id="timesavebtn" name="timesavetbtn" value="save_time" title="Save current settings as new defaults"
            style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#faadb7;"
            >Save & Reboot</button>
    </form>
    <br>
    <script>
      function fetchData(whichData = "all") {
        let source = 0;
        fetch("/jsonclock")
          .then(response => response.json())
          .then(data => {
            document.getElementById("whichdata").value = whichData;   
            if ((whichData == "clock") || (whichData == "all")) {
              const selectStyle = document.getElementById("clockstyle");
              selectStyle.innerHTML = "" 
              //Populate select with array values
              data.styles.forEach(style => {
                const option = document.createElement("option");
                option.value = style.index_val;
                option.textContent = style.name;
                selectStyle.appendChild(option);
              });
              selectStyle.value = String(data.clock_style);
              document.getElementById("clockcolor").value = data.clock_color;
              if (data.hour_format == 12) {
                document.getElementById("12hr").checked = true;
              } else {
                document.getElementById("24hr").checked = true;
              }
            }
            if ((whichData == "time") || (whichData == "all")) {
              if (data.mqtt_connected != 1) {
                document.getElementById("sourcemqtt").disabled = true;
                document.getElementById("timesrcmsg").textContent = "MQTT not setup/avilable";
              } else {
                document.getElementById("sourcemqtt").disabled = false;
                document.getElementById("timesrcmsg").textContent = "";
              }
              switch (data.time_source) {
                case 0:
                  document.getElementById("sourcenone").checked = true;
                  source = 0;
                  break;
                case 1:
                  document.getElementById("sourcentp").checked = true;
                  source = 1;
                  break;
                case 2:
                  document.getElementById("sourcemqtt").checked = true;
                  source = 2;
                  break;
                case 3:
                  document.getElementById("sourceapi").checked = true;
                  source = 3;
                  break;
                default:
                  document.getElementById("sourcenone").checked = true;
                  source = 0;
              }
              document.getElementById("ntpserver").value = data.ntp_server;
              document.getElementById("timezone").value = data.time_zone;
              document.getElementById("syncint").value = data.ntp_sync_int;
              document.getElementById("mqtttopic").value = data.mqtt_time_topic;
              if (data.mqtt_use == 0) {
                document.getElementById("mqttsync").checked = true;
              } else {
                document.getElementById("mqttlive").checked = true;
              }
              if (data.api_use == 0) {
                document.getElementById("apisync").checked = true;
              } else {
                document.getElementById("apilive").checked = true;
              }
              enableTimeFields(source);
            }
          });   
        //.catch(error => console.error("Error fetching data:", error));
      }
      function enableTimeFields(source) {
        const localTime = document.getElementById("localtime");
        const setTimeBtn = document.getElementById("settimebtn");
        const resetTimeBtn = document.getElementById("timeresetbtn");
        const saveTimeBtn = document.getElementById("timesavebtn");
        const ntpServer = document.getElementById("ntpserver");
        const timeZone = document.getElementById("timezone");
        const syncInt = document.getElementById("syncint");
        const mqttTopic = document.getElementById("mqtttopic");
        const mqttSync = document.getElementById("mqttsync");
        const mqttLive = document.getElementById("mqttlive");
        const apiSync = document.getElementById("apisync");
        const apiLive = document.getElementById("apilive");
        if (source == 9) {
          //enable all for form submit
          localTime.disabled = false;
          setTimeBtn.disabled = false;
          ntpServer.disabled = false;
          timeZone.disabled = false;
          syncInt.disabled = false;
          mqttTopic.disabled = false;
          mqttSync.disabled = false;
          mqttLive.disabled = false;
          apiSync.disabled = false;
          apiLive.disabled = false;
        } else if (source == -1) {
          //Disable all for clock test
          localTime.disabled = true;
          setTimeBtn.disabled = true;
          ntpServer.disabled = true;
          timeZone.disabled = true;
          syncInt.disabled = true;
          mqttTopic.disabled = true;
          mqttSync.disabled = true;
          mqttLive.disabled = true;
          apiSync.disabled = true;
          apiLive.disabled = true;
          resetTimeBtn.disabled = true;
          saveTimeBtn.disabled = true;
        } else {
          localTime.disabled = (source != 0);
          setTimeBtn.disabled = (source != 0);
          ntpServer.disabled = (source != 1);
          timeZone.disabled = (source != 1);
          syncInt.disabled = (source != 1);
          mqttTopic.disabled = (source != 2);
          mqttSync.disabled = (source != 2);
          mqttLive.disabled = (source != 2);
          apiSync.disabled = (source != 3);
          apiLive.disabled = (source != 3);
          resetTimeBtn.disabled = false;
          saveTimeBtn.disabled = false;
        }
      }
      function enableClockFields(enable) {
        const timeStyle = document.getElementById("clockstyle");
        const timeColor = document.getElementById("clockcolor");
        const hourDisp12 = document.getElementById("12hr");
        const hourDisp24 = document.getElementById("24hr");
        const timeSaveBtn = document.getElementById("clocksavebtn");
        timeStyle.disabled = (enable == 0);
        timeColor.disabled = (enable == 0);
        hourDisp12.disabled = (enable == 0);
        hourDisp24.disabled = (enable == 0);
        timeSaveBtn.disabled = (enable == 0);
      }
      function timeSourceChanged() {
        let selectedSource = 9;
        const timeSource = document.getElementsByName("timesource");
        const localTime = document.getElementById("localtime");
        for (let i = 0; i < timeSource.length; i++) {
          if (timeSource[i].checked) {
            selectedSource = Number(timeSource[i].value);
            break;
          }
        }
        enableTimeFields(selectedSource);
      }
      function getTimeSource() {
        let retVal = 0;
        const timeSource = document.getElementsByName("timesource");
        for (let i = 0; i < timeSource.length; i++) {
          if (timeSource[i].checked) {
            retVal = Number(timeSource[i].value);
            break;
          }
        }
        return retVal;
      }
      function setCurTime() {
        const frmTime = document.getElementById("formtime");
        const timeField = document.getElementById("localtime");
        const newTime = timeField.value;
        if (!newTime) {
          document.getElementById("timemsg").textContent = "Please provide a valid date/time!";
          return false;
        } else {
          document.getElementById("timemsg").textContent = "";
        }
        const selectedTime = new Date(newTime);
        document.getElementById("manualyear").value = selectedTime.getFullYear();
        document.getElementById("manualmonth").value = Number(selectedTime.getMonth());
        document.getElementById("manualday").value = selectedTime.getDate();
        document.getElementById("manualhour").value = selectedTime.getHours();
        document.getElementById("manualminute").value = selectedTime.getMinutes();
        return true;
      }
      document.addEventListener("DOMContentLoaded", fetchData());
      document.getElementById("formstyle").addEventListener("submit", async function (event) {
        event.preventDefault();
        const frm = document.getElementById("formstyle");
        const btnVal = event.submitter.value;
        if (btnVal == "test_clock") {
          const btnTest = document.getElementById("clocktestbtn");
          const clockStyle = Number(document.getElementById("clockstyle").value);
          var colorHex = document.getElementById("clockcolor").value; 
          const selectedHour = document.getElementsByName("hourformat"); 
          const styleMsg = document.getElementById("stylemsg");
          let hourFormat = 12;
          if (btnTest.textContent == "Test") {
            //Start test
            btnTest.textContent = "Stop Test";
            enableClockFields(0);
            enableTimeFields(-1);
            for (let i=0; i < selectedHour.length; i++) {
              if (selectedHour[i].checked) {
                hourFormat = Number(selectedHour[i].value);
                break;
              }
            }
            const clockData = {
              new_style: clockStyle,
              new_color: colorHex,
              hour_format: hourFormat
            };
            fetch("/testclock", {
              method: "POST",
              headers: {
                "Content-Type": "application/json",
              },
              body: JSON.stringify(clockData),
            })
            .then(response => {
              if (response.ok) {
                styleMsg.textContent = "";
              } else {
                styleMsg.textContent = "Clock testing could not be enabled.";
              }
            });
          } else {
            btnTest.textContent = "Test";
            enableClockFields(1);
            enableTimeFields(getTimeSource());
            styleMsg.textContent = "";
          }
        } else if (btnVal == "reset_clock") {
          const styleMsg = document.getElementById("stylemsg");
          fetch("/resetclock", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: "",
          })
          .then(response => {
            if (response.ok) {
              styleMsg.textContent = "";
              enableClockFields(1);
              fetchData("clock");
             }
          });
        } else if (btnVal == "save_clock") {
          await enableClockFields(1);
          this.action = "/applyclock";
          this.submit();
          return;
        }
      });
      document.getElementById("formtime").addEventListener("submit", async function (event) {
        event.preventDefault();
        const frm = document.getElementById("formtime");
        const btnVal = event.submitter.value;
        if (btnVal == "save_time") {
          //Enable all controls for post
          await enableTimeFields(9);
          this.action = "/applytime";
          this.submit();
          return;
        } else if (btnVal == "set_time") {
          if (setCurTime()) {
            const yearValue = Number(document.getElementById("manualyear").value);
            const monthValue = Number(document.getElementById("manualmonth").value);
            const dayValue = Number(document.getElementById("manualday").value);
            const hourValue = Number(document.getElementById("manualhour").value);
            const minuteValue = Number(document.getElementById("manualminute").value);
            const errMsg = document.getElementById("timemsg");
            const timeData = {
              newyear: yearValue,
              newmonth: monthValue,
              newday: dayValue,
              newhour: hourValue,
              newminute: minuteValue
            };
            fetch("/settime", {
              method: "POST",
              headers: {
                "Content-Type": "application/json",
              },
              body: JSON.stringify(timeData),
            })
            .then(response => {
              if (response.ok) {
                errMsg.style = "color: green;"
                errMsg.textContent = "Time successfully set.  To restore syncing, reboot the display controller.";
              } else {
                errMsg.style = "color: red;"
                errMsg.textContent = "Time could not be set.  Check date/time values.";
              }
            });
          }
        }
      });
    </script>
)literal";

const char *postclocksettings = R"literal(
  <!-- -------------------------------
         Handle time settings save
       ------------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - *WHICHONE* Settings Updating</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* *WHICHONE* Settings Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">*WHICHONE* defaults saved to configuration file.</h2><br>
    The display controller will now reboot.  After boot completes, you may:<br><br>
    <button type="button" id="btnlights" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/clock'">Return to Clock Settings</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
)literal";

const char *alarmpage = R"literal(
  <!-- =============================
        ALARM SETTINGS PAGE
       ============================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Alarms Settings</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
      .center-table-content {border: none;}
      .center-table-content td,
      .center-table-content th {
        text-align: center;      /* Centers content horizontally */
        vertical-align: middle;  /* Centers content vertically */
        padding-right: 20px;
      }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* Alarm Settings</H1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT: Be sure to click 'Update' after making any changes to alarms.</font><br><br>
    <i>See documentation for option details and additional information.</i><br><br>
    <button type="button" id="btnback" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 140px; height: 35px;"
             onclick="location.href = '/';"><< Return to Main</button><br><br>       

    <h2 style="text-decoration: underline;">General Alarm Settings</h2>
    <!-- hidden field outside of forms -->
    <input type="hidden" id="whichdata" name="whichdata">
    <form id="formgen" action="/alarmsettings" method="post">
      <table border="0">
        <tr>
          <td>Use SD Card:</td>
          <td colspan="2"><input type="checkbox" id="usesd" name="usesd" value="1" title="Must be enable to use alarm sounds" onchange="toggleControls()">
          &emsp;&emsp;<span id="usesdmsg" name="usesdmsg" style="color: red;">&nbsp;</span></td>
        </tr>
        <tr>
          <td>Alarm Sound:</td>
          <td><select id="alarmsound" name="alarmsound" style="width: 100%;">
            <!-- values added dynamically via JS -->
            </select></td>
          <td>&emsp;<button type="button" id="soundsetup" name="soundsetup" value="soundsetup" title="Setup sounds on SD Card"
                      style="text-align: center; font-size: 16px; border-radius: 10px; width: 105px; height: 30px; background-color:#a4f086;"
                      onclick="window.location='/sounds'"
                      >Library...</button></td>
        </tr>  
        <tr>
          <td>Alarm Volume:</td>
          <td><input type="range" id="volume" name="volume" min="0" max="30" step="1" title="Set alarm volume (or max for Gentle Wake)" value="15"></td>
          <td>&emsp;<span id="volmsg" name="volmsb" style="color: red">&nbsp;</span></td>
        </tr><tr>
          <td>Use Gentle Wake:</td>
          <td><input type="checkbox" id="gentlewake" name="gentlewake" title="Gently increase alarm volume over a 60 second period" value="1"></td>
          <td>&nbsp</td>
        </tr><tr>
          <td>Snooze Time:</td>
          <td colspan="2"><input type="number" id="snoozetime" name="snoozetime" min="0" max="60" step="1" title="Set snooze time in minutes" value="0"
               style="width: 35px;">
               &emsp;(minutes - 0 to disable, 60 max)
          </td>
      </table><br>
      <!-- Hidden fields -->
      <input type="hidden" id="usesdval" name="usesdval" value="0">      
      <input type="hidden" id="gentlewakeval" name="gentlewakeval" value="0">      
      <button type="submit" id="testsound" name="testsound" value="test_sound" title="Test selected sound and volume"
              style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#a4f086;" 
              >Test Sound</button>&ensp;
      <button type="button" id="generalresetbtn" name="generalresetbtn" value="reset_general" title="Reset back to default settings"
              style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#e0df80;" 
              onclick="fetchData('general')">Reset</button>&ensp;
      <button type="submit" id="gensavebtn" name="gensavetbtn" value="save_gen" title="Save current settings as new defaults"
              style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#faadb7;"
              >Save & Reboot</button>
    </form><br>
    <h2 style="text-decoration: underline;">Current Saved Alarms</h2>
    Be sure to click 'Update' to save any changes made. See documentation for more info.<br>
    <b>Note:</b> <i>Date is required, but ignored for any repeating alarms.</i><br><br>
    <form id="formalarms">
      <table class="center-table-content">
        <tr>
          <th>Active</th>
          <th>Date</th>
          <th>Time</th>
          <th>Repeat</th>
          <th>&nbsp;</th>
        </tr><tr>
          <td><input type="checkbox" id="active_1" name="active_1" title="Enable/disable this alarm" value="1"></td>
          <td><input type="date" id="date_1" name="date_1" title="Date ignored for repeating alarms" required></td>
          <td><input type="time" id="time_1" name="time_1" title="Alarm time - must be in hh:mm am/pm format" required></td>
          <td><select id="repeat_1" name="repeat_1" title="Select if alarm should repeat.  Set to none for non-repeating"></select></td>
          <td><span id="alarm_msg">&nbsp;</span></td>
        </tr><tr>
          <td><input type="checkbox" id="active_2" name="active_2" title="Enable/disable this alarm" value="1"></td>
          <td><input type="date" id="date_2" name="date_2" title="Date ignored for repeating alarms" required></td>
          <td><input type="time" id="time_2" name="time_2" title="Alarm time - must be in hh:mm am/pm format" required></td>
          <td><select id="repeat_2" name="repeat_2" title="Select if alarm should repeat.  Set to none for non-repeating"></select></td>
          <td>&nbsp;</td>
        </tr><tr>
          <td><input type="checkbox" id="active_3" name="active_3" title="Enable/disable this alarm" value="1"></td>
          <td><input type="date" id="date_3" name="date_3" title="Date ignored for repeating alarms" required></td>
          <td><input type="time" id="time_3" name="time_3" title="Alarm time - must be in hh:mm am/pm format" required></td>
          <td><select id="repeat_3" name="repeat_3" title="Select if alarm should repeat.  Set to none for non-repeating"></select></td>
          <td>&nbsp;</td>
        </tr><tr>
          <td><input type="checkbox" id="active_4" name="active_4" title="Enable/disable this alarm" value="1"></td>
          <td><input type="date" id="date_4" name="date_4" title="Date ignored for repeating alarms" required></td>
          <td><input type="time" id="time_4" name="time_4" title="Alarm time - must be in hh:mm am/pm format" required></td>
          <td><select id="repeat_4" name="repeat_4" title="Select if alarm should repeat.  Set to none for non-repeating"></select></td>
          <td>&nbsp;</td>
        </tr><tr>
          <td><input type="checkbox" id="active_5" name="active_5" value="1"></td>
          <td><input type="date" id="date_5" name="date_5" required></td>
          <td><input type="time" id="time_5" name="time_5" required></td>
          <td><select id="repeat_5" name="repeat_5"></select></td>
          <td>&nbsp;</td>
        </tr>
      </table><br>
      <!-- Hidden fields -->
      <input type="hidden" id="active_val_1" name="active_val_1" value="0">
      <input type="hidden" id="active_val_2" name="active_val_4" value="0">
      <input type="hidden" id="active_val_3" name="active_val_3" value="0">
      <input type="hidden" id="active_val_4" name="active_val_4" value="0">
      <input type="hidden" id="active_val_5" name="active_val_5" value="0">

      <button type="button" id="alarmresetbtn" name="alarmresetbtn" value="reset_alarm" title="Reload current saved alarms"
              style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#e0df80;" 
              onclick="fetchData('alarms')">Reload</button>&ensp;
      <button type="submit" id="alarmsavebtn" name="alarmsavetbtn" value="save_alarm" title="Save current alarms"
              style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#faadb7;"
              >Save Alarms</button>
      </form><br>
    <script>
      const MAX_ALARMS = 5;  //Must match Arduino global
      const repeatOptions = [
        { val: 0, text: "None" },
        { val: 1, text: "Sunday" },
        { val: 2, text: "Monday" },
        { val: 3, text: "Tuesday" },
        { val: 4, text: "Wednesday" },
        { val: 5, text: "Thursday" },
        { val: 6, text: "Friday" },
        { val: 7, text: "Saturday" },
        { val: 8, text: "Weekdays (M-F)" },
        { val: 9, text: "Weekends (S-S)" }
      ];
      const formalarms = document.getElementById('formalarms');
      const statusSpan = document.getElementById('alarm_msg');
      function initRepeatDropdowns() {
        // We have 5 alarm rows
        for (let i = 1; i <= 5; i++) {
          const selectEl = document.getElementById(`repeat_${i}`);
          if (selectEl) {
            selectEl.innerHTML = "";
            repeatOptions.forEach(opt => {
              selectEl.add(new Option(opt.text, opt.val));
            });
          }
        }
      }
      function handleAlarmChange(event) {
        setUnsavedStatus();
        const target = event.target;
        const row = target.closest('tr'); 
        if (row) {
          // Find the 'Active' checkbox within THIS specific row
          const activeCheckbox = row.querySelector('input[type="checkbox"]');
          if (activeCheckbox && target !== activeCheckbox) {
            activeCheckbox.checked = true;
          }
        }        
      }
      function setUnsavedStatus() {
        if (statusSpan) {
          statusSpan.textContent = 'Changes not saved.';
          statusSpan.style.color = 'red';
        }
      }
      function fetchData(whichData = "all") {
        let source = 0;
        fetch("/jsonalarms")
          .then(response => response.json())
          .then(data => {
            document.getElementById("whichdata").value = whichData;   
            if ((whichData == "general") || (whichData == "all")) {
              const selectSound = document.getElementById("alarmsound");
              if (data.use_sd == 1) {
                document.getElementById("usesd").checked = true;
                document.getElementById("usesdval").value = 1;
                selectSound.disabled = false;
                document.getElementById("volume").disabled = false;
                document.getElementById("gentlewake").disabled = false;
                document.getElementById("snoozetime").disabled = false;
                document.getElementById("usesdmsg").innerHTML = "";
              } else {
                document.getElementById("usesd").checked = false;
                document.getElementById("usesdval").value = 0;
                selectSound.disabled = true;
                document.getElementById("volume").disabled = true;
                document.getElementById("gentlewake").disabled = true;
                document.getElementById("snoozetime").disabled = true;
                document.getElementById("usesdmsg").innerHTML = "If disabled, alarms will produce no sound!";
              }
              selectSound.innerHTML = ""  
              //Populate select with array values
              data.tracks.forEach(track => {
                const option = document.createElement("option");
                option.value = track.index;
                option.textContent = track.title;
                selectSound.appendChild(option);
              });
              selectSound.value = data.alarm_track;
              
              document.getElementById("volume").value = data.alarm_vol;
              if (data.gentle_wake == 1) {
                document.getElementById("gentlewake").checked = true;
                document.getElementById("gentlewakeval").value = 1;
              } else {
                document.getElementById("gentlewake").checked = false;
                document.getElementById("gentlewakeval").value = 0;
              }
              document.getElementById("snoozetime").value = data.snooze_time;
            }
            if ((whichData == "alarms") || (whichData == "all")) {
              if (data.alarms) {
                data.alarms.forEach(alarm => {
                  const idSuffix = alarm.index; 
                  const activeEl = document.getElementById(`active_${idSuffix}`);
                  const timeEl   = document.getElementById(`time_${idSuffix}`);
                  const dateEl   = document.getElementById(`date_${idSuffix}`);
                  const repeatEl = document.getElementById(`repeat_${idSuffix}`);
                  if (activeEl) activeEl.checked = (alarm.active === 1);
                  if (dateEl) dateEl.value = alarm.date; 
                  if (timeEl) timeEl.value = alarm.time;
                  // Repeat (Select Dropdown)
                  if (repeatEl) repeatEl.value = alarm.repeat;
                });
              }
              statusSpan.textContent = "";
            }
          });   
      }
      function toggleControls() {
        let enabled = document.getElementById("usesd").checked
        document.getElementById("alarmsound").disabled = !enabled;
        document.getElementById("volume").disabled = !enabled;
        document.getElementById("gentlewake").disabled = !enabled;
        document.getElementById("snoozetime").disabled = !enabled;
        document.getElementById("testsound").disabled = !enabled;
        if (enabled) {
          document.getElementById("usesdmsg").innerHTML = "";
        } else {
          document.getElementById("usesdmsg").innerHTML = "If disabled, alarms will produce no sound!";
        }
      }
      function enableFields(whichForm = "all") {
        //Needed for submit so controls are included in POST
        if ((whichForm == "all") || (whichForm == "general")) {
          document.getElementById("alarmsound").disabled = false;
          document.getElementById("volume").disabled = false;
          document.getElementById("gentlewake").disabled = false;
          document.getElementById("snoozetime").disabled = false;
          document.getElementById("testsound").disabled = false;
          //Set hidden fields for checkboxes
          document.getElementById("usesdval").value = ((document.getElementById("usesd").checked) ? 1 : 0);
          document.getElementById("gentlewakeval").value = ((document.getElementById("gentlewake").checked) ? 1 : 0); 
        }
        if ((whichForm == "all") || (whichForm == "alarms")) {
          document.getElementById("active_val_1").value = ((document.getElementById("active_1").checked) ? 1 : 0);
          document.getElementById("active_val_2").value = ((document.getElementById("active_2").checked) ? 1 : 0);
          document.getElementById("active_val_3").value = ((document.getElementById("active_3").checked) ? 1 : 0);
          document.getElementById("active_val_4").value = ((document.getElementById("active_4").checked) ? 1 : 0);
          document.getElementById("active_val_5").value = ((document.getElementById("active_5").checked) ? 1 : 0);
        }
      }
      function setTestFields(enable) {
        document.getElementById("usesd").disabled = !enable;
        document.getElementById("alarmsound").disabled = !enable;
        document.getElementById("volume").disabled = !enable;
        document.getElementById("gentlewake").disabled = !enable;
        document.getElementById("snoozetime").disabled = !enable;
        document.getElementById("soundsetup").disabled = !enable;
        document.getElementById("generalresetbtn").disabled = !enable;
        document.getElementById("gensavebtn").disabled = !enable;
        if (enable) {
          document.getElementById("testsound").innerHTML = "Test Sound";
        } else {
          document.getElementById("testsound").innerHTML = "Stop Test";
        }
      }
      document.addEventListener("DOMContentLoaded", () => {
        initRepeatDropdowns();
        if (formalarms) {
          formalarms.addEventListener('change', handleAlarmChange);
        }
        if (formalarms) {
          formalarms.addEventListener('submit', handleAlarmsFormSubmit);
        }
        fetchData();
      });
      document.getElementById("formgen").addEventListener("submit", async function (event) {
        event.preventDefault();
        const frm = document.getElementById("formgen");
        const btnVal = event.submitter.value;
        var okToSave = false;
        if (document.getElementById("usesd").checked) {
          if (document.getElementById("volume") < 1) {
            document.getElementById("volmsg").innerHTML = "Please set volume above 0 or alarm will not play!";
          } else {
            okToSave = true;
          }
        } else {
          var userConfirmed = confirm("Alarms will not produce any sound if you continue.  Continue anyway?");
          if (userConfirmed) {
            okToSave = true;
          }
        }
        if (okToSave) {
          if (btnVal == "test_sound") {
            const btnText = document.getElementById("testsound");
            const data = {
              mode: ((btnText.innerHTML == "Test Sound") ? 1 : 0),
              tracknum : Number(document.getElementById("alarmsound").value),
              volume: Number(document.getElementById("volume").value),
              gentlewake: ((document.getElementById("gentlewake").checked) ? 1 : 0)
            };
            if (btnText.innerHTML == "Test Sound") {
              //Start test
              await setTestFields(false);
             
            } else {

              //Stop test
              await setTestFields(true);
            }
            const jsonData = JSON.stringify(data);
            try {
              const response = await fetch("/soundtest", 
              { method: "POST", 
                headers: {
                  "Content-Type": "application/json",
                },
                body: jsonData,
              });
              if (response.ok) {
                //this.reset();
              } else {
                throw new Error("Could not extract data");
              }
            } catch (error) {
              //nothing for now
            }
          } else {
            await enableFields("general");
            this.action = "/alarmsettings";
            this.method = "POST";
            this.submit();
            return;
          }
        }
      });
      function handleAlarmsFormSubmit(event) {
        event.preventDefault();
        statusSpan.textContent = "Saving...";
        statusSpan.style.color = "#f0a905";
        enableFields("alarms");
        const alarmsToSend = [];
        for (let i = 1; i <= MAX_ALARMS; i++) {
          const activeEl = document.getElementById(`active_${i}`);
          const timeEl   = document.getElementById(`time_${i}`);
          const dateEl   = document.getElementById(`date_${i}`);
          const repeatEl = document.getElementById(`repeat_${i}`);
          // Collect data and package into JSON object
          const alarmData = {
            index: i, 
            active: activeEl.checked ? 1 : 0, 
            date: dateEl.value,
            time: timeEl.value,
            repeat: parseInt(repeatEl.value) 
          };
           alarmsToSend.push(alarmData);
        }
        // Send JSON data to the server
        fetch("/alarmupdate", {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(alarmsToSend)
        })
        .then(response => {
          if (response.ok) {
            statusSpan.textContent = 'Alarm settings saved successfully.';
            statusSpan.style.color = '#025203';
          } else {
            return response.text().then(text => {
              statusSpan.textContent = `Error saving: ${text || response.statusText}`;
              statusSpan.style.color = 'red';
            });
          }
        })
        .catch(error => {
          statusSpan.textContent = `Network Error: ${error.message}`;
          statusSpan.style.color = 'red';
        }); 
      }
    </script>
)literal";

const char *postalarmsettings = R"literal(
  <!-- ------------------------------------
         Handle general alarm settings save
       ------------------------------------ -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Alarm General Settings Updating</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* General Alarm Settings Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">General Alarm defaults saved to configuration file.</h2><br>
    The display controller will now reboot.  After boot completes, you may:<br><br>
    <button type="button" id="btnlights" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/alarms'">Return to Alarm Settings</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
)literal";

const char *soundpage = R"literal(
  <!-- =============================
        SOUND LIBRARY SETUP PAGE
       ============================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Sound Library Setup</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
      table th,
      table td {
        text-align: center;
        vertical-align: middle;
      }
    </style>
  </head>
  <body> 
    <H1>*VAR_APP_NAME* Sound Library</H1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT: Be sure to click 'Update' after making any changes to sounds.</font><br>
    Index must match order of files on the SD card.<br>
    <i>See documentation for option details and additional information.</i><br><br>
    <button type="button" id="btnback" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 150px; height: 35px;"
             onclick="location.href = '/';"><< Return to Main</button><br><br>       
    <button type="button" id="btnalarm" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 150px; height: 35px;"
             onclick="location.href = '/alarms';"><< Return to Alarms</button><br>       
    <table border="0">
      <tr>
        <td><h2 style="text-decoration: underline;">Sound Files</h2></td>
        <td>&emsp;<span id="message" name="message" style="color: red;">&nbsp;</span></td>
      </tr>
    </table>
    <form id="soundform" action="/soundsettings" method="post">
      <table id="tracklist" border=0>
        <thead>
          <tr>
          <th>Index</th>
          <th>File Name</th>
          <th>Track Title</th>
          <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
          <!-- rows created via javascript -->
        </tbody>
      </table><br>
      <button type="button" id="btnreset" name="btnreset" value="reset_sound" title="Reload current saved sounds"
              style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#e0df80;" 
              onclick="location.reload()">Reset</button>&ensp;
      <button type="submit" id="btnupdate" name="btnupdate" value="save_sound" title="Save current tracks"
              style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 35px; background-color:#faadb7;"
              >Update</button>
    </form>
    <script>
      const MAX_SOUNDS = 20;  //Must match Arduino global
      const formSounds = document.getElementById('soundform');
      const statusSpan = document.getElementById('message');
      function fetchData() {
        fetch("/jsonsounds") 
          .then(response => response.json())
          .then(data => {
              const tableBody = document.querySelector("#soundform tbody");
              //Add table row
              data.tracks.forEach(track => {
                const row = document.createElement("tr");
                Object.keys(track).forEach(key => {
                  const cell = document.createElement("td");
                  const input = document.createElement("input");
                  input.type = "text";
                  input.value = track[key];
                  input.name = `${key}_${track.index}`;
                  input.id = `${key}_${track.index}`;
                  if (key == "index") {
                    input.style.width = "30px";
                    input.style.pointerEvents = "none";
                    input.style.userSelect = "none";
                    input.tabIndex = -1;
                    input.style.opacity = "0.9";
                    input.style.border = "none";
                    input.style.backgroundColor = "transparent";
                    input.style.outline = "none";
                    input.readOnly = true;
                  }
                  cell.appendChild(input);
                  row.appendChild(cell);
                });
                tableBody.appendChild(row); 
              });
          })
          .catch(error => console.error("Error fetching data:", error));
      }
      function setUnsavedStatus() {
        statusSpan.innerText = "You have unsaved changes!"
      }
      function handleSoundFormSubmit(event) {
        event.preventDefault();
        statusSpan.textContent = "Saving...";
        statusSpan.style.color = "#f0a905";
        const soundsToSend = [];
        for (let i = 1; i <= MAX_SOUNDS; i++) {
          const indexEl = document.getElementById(`index_${i}`);
          const filenameEl = document.getElementById(`filename_${i}`);
          const titleEl = document.getElementById(`title_${i}`);
          // Collect data and package into JSON
          const soundData = {
            index: indexEl.value, 
            filename: filenameEl.value,
            title: titleEl.value,
          };
           soundsToSend.push(soundData);
        }
        fetch("/soundupdate", {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(soundsToSend)
        })
        .then(response => {
          if (response.ok) {
            statusSpan.textContent = 'Sound tracks saved successfully.';
            statusSpan.style.color = '#025203';
          } else {
            return response.text().then(text => {
              statusSpan.textContent = `Error saving: ${text || response.statusText}`;
              statusSpan.style.color = 'red';
            });
          }
        })
        .catch(error => {
          statusSpan.textContent = `Network Error: ${error.message}`;
          statusSpan.style.color = 'red';
        }); 
      }
      document.addEventListener("DOMContentLoaded", () => {
        if (formSounds) {
          formSounds.addEventListener('change', setUnsavedStatus);
          formSounds.addEventListener('submit', handleSoundFormSubmit);
        }
        fetchData();
      });
    </script>
)literal";

const char *restart = R"literal(
  <!-- =============================
        CONTROLLER REBOOT PAGE
       ============================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Reboot</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>Display Controller restarting...</H1><br>
      <H3>Please wait</H3><br>
      After the controller completes the boot process, you may click the following link to return to the main page:<br><br>
      <a href="http://*IPADDR*">Return to settings</a><br><br>
)literal";

const char *updateFirmware = R"literal(
  <!-- ======================
        FIRMWARE UPDATE PAGE 
       ====================== -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Firmware</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1>*VAR_APP_NAME* Firmware Update</h1><br>
    <b>IMPORTANT:</b> Please read the release notes for the firmware before upgrading!<br>
    Some upgrades may require onboarding and configuring your device again.<br><br>
    <button type="button" id="btnback" style="font-size: 16px; border-radius: 8px; width: 100px; height: 30px;" onclick="location.href = './'"><< Back</button>
    <br><br>
    <b>Current Firmware Version:</b> *VAR_CURRENT_VER* <br><br>
    <span style="color: red; font-weight: bold;">Verify that file selected is firmware for the <u>DISPLAY controller</u></span><br><br>
    <form method='POST' enctype='multipart/form-data' id='upload-form'>
      <table style="border: 1px solid black;"><tr>
      <td><input type='file' id='file' name='update'></td>
      <td><input type='submit' value='Update'></td>
      </tr></table>
    </form>
    <br>
    <table style="width:25%">
    <tr><td>
    <div id='prg' style='width:0;color:#d0d1a9;text-align:center'>0%</div>
    </td></tr></table>
    <br>
    If the upload is successful, the controller will automatically reboot. Once complete, you can use the link below to return to the main page.<br>
    Check the version on the main page to assure a successful update.<br<br>
    <a href="./">Return to settings</a><br><br>
    <h3>Basic Troubleshooting</h3>
    Here are a few items of note:<br>
    <ul>
    <li>Depending on the issue, an error message may be displayed that will point to the issue</li>
    <li>If version number does not change, the update failed and firmware rolled back to prior version.</li>
      <ul>
      <li>Try flashing the firmware again</li>
      <li>Verify you are using a valid firmware file for your ESP board</li>
      <li>Check the firmware version's release notes.
      </ul>
    <li>The controller appeared to reboot, but you can no longer access the web app</li>
      <ul>
      <li>Check to see if the DISPLAY_AP hotspot is broadcasting (you may need to onboard again)</li>
      <li>If possible, check the clock display for additional information.</li>
      <li>Assure a new IP address wasn't assigned by your router.  If possible, assign a static/reserved IP for the controller</li>
      </ul>
    <li>If the update continues to fail over-the-air, try flashing via USB if possible (may require onboarding again)</li>
    </ul>
    <script>
      var prg = document.getElementById('prg');
      var form = document.getElementById('upload-form');
      form.addEventListener('submit', el=>{
        prg.style.backgroundColor = 'blue';
        el.preventDefault();
        var data = new FormData(form);
        var req = new XMLHttpRequest();
        var fsize = document.getElementById('file').files[0].size;
        req.open('POST', '/update?size=' + fsize);
        req.upload.addEventListener('progress', p=>{
          let w = Math.round(p.loaded/p.total*100) + '%';
            if(p.lengthComputable){
              prg.innerHTML = w;
              prg.style.width = w;
            }
            if(w == '100%') prg.style.backgroundColor = 'black';
        });
        req.send(data);
      });
    </script>
)literal";


const char *otaHtml = R"literal(
  <!-- =============================
        ARDUINO IDE OTA UPDATE PAGE
       ============================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - OTA Update</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0f4004; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* Arduino OTA Upload...</H1>
    <table border=1 cellpadding="5">
    <tr><td><b>Device Name:</b></td><td>*DEVICENAME*</td></tr>
    <tr><td><b>Firmware Ver:</b></td><td>*VAR_CURRENT_VER*</td></tr>
    </table><br>
    <H2>Start upload from Arduino IDE now</H2>
    <ul>
      <li>Display (if connected) should show 'UPLOAD'.</li>
      <li>If code not is received after approx. 20 seconds, OTA mode will exit and system will return to normal operation.</li>
      <li>If upload is successful, the controller will automatically restart.</li>
      <li>This page will not refresh on its own.</li>
    </ul>
    <br>
    After upload/reboot is complete, you may <a href='./'>Return to Main Settings</a><br>
)literal";

const char *configpage = R"literal(
  <!-- ========================
        CONFIG FILE DUMP PAGE
       ======================== -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Config File Dump</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* Configuration Files</H1>
    <button type="button" id="btncancel" style="text-align: center; font-size: 16px; border-radius: 8px; width: 100px; height: 30px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">&lt;&lt; Back</button><br>
    <h3 style="text-decoration: underline;">Main Config File</h3>
    The following is the main JSON configuration file loaded <b><u>at last boot</u></b>.<br>
    It does <i>not</i> list current settings or any default settings changed but not yet saved.<br>
    <textarea id="filedump" rows="60" cols="50">*CONFIGJSON*</textarea><br>
    <h3 style="text-decoration: underline;">Alarm File</h3>
    These are the current raw alarm settings, saved in the /alarm.bin file.<br>
    System time captured at load is not part of the file, but is included for reference.<br>
    <textarea id="alarmdump" rows="40" cols="50">*CONFIGALARM*</textarea><br>
    <h3 style="text-decoration: underline;">Sound Library</h3>
    Current Sound Library Config<br>
    <textarea id="sounddump" rows="40" cols="50">*CONFIGSOUND*</textarea><br>
)literal";

const char *reset = R"literal(
  <!-- =============================
        CONTROLLER FULL RESET PAGE
       ============================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Full Reset</title>
    <style>
      body { background-color: #d0d1a9; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
      <H1>Display Controller Resetting...</H1><br>
      <H3>After this process is complete, you <b>must</b> setup your display controller again:</H3>
      <ul>
        <li>Connect a device to the controller's local access point: *VAR_APP_NAME*_AP</li>
        <li>Open a browser and go to: 192.168.4.1</li>
        <li>Enter your WiFi information and set other default settings values</li>
        <li>Click Save. The controller will reboot and join your WiFi</li>
      </ul><br>
      Once the above process is complete, you can return to the main settings page by rejoining your WiFi and entering the IP address assigned by your router in a browser.<br>
      You will need to reenter all of your settings for the system as all values will be reset to original defaults<br><br>
      <b>This page will NOT automatically reload or refresh</b><br>
)literal";
