// v0.37
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
  <!-- ================================= 
        MAIN SETTINGS AND DEFAULTS PAGE
       ================================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Main</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
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
    <table>
      <tr>
        <td><h2>Master Lighting Controls</h2></td>
        <td>&emsp;<a href="/">Refresh & Sync</a></td>
      </tr>
    </table>
    <form id="formlive" action="/default-action" method="post">
      <table style="border: 0; border-spacing: 10px;">
        <tr>
          <th colspan="2">Main Light</th>
          <th colspan="2">Base LEDs</th>
        </tr>
        <tr>
          <td>&nbsp;</td>
          <td><button type="button" id="lightbulbbtn" name="lightbulbbtn" title="Toggle current state of the light bulb" 
            style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px;" onclick="toggleBulbState()">Off</button>
          </td> 
          <td>&nbsp;</td>
          <td><button type="button" id="ledstripbtn" name="ledstripbtn" title="Toggle current state of the LED strip"
            style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px;" onclick="toggleLEDState()">Off</button>
          </td> 
        </tr>
        <tr>
          <td><label for="lightbulbcolor">&#127912;:</label></td>
          <td style="padding-right: 10px;">
              <input type="color" id="lightbulbcolor" name="lightbulbcolor" title="Change color of light bulb" 
                style="width: 100%;" value="#000000" onchange="updateBulbColor('lightbulbcolor', 'bulbcolorval')"/>
          </td>
          <td><label for="ledstripcolor">&#127912;:</label></td>
          <td style="padding-right: 10px;">
              <input type="color" id="ledstripcolor" name="ledstripcolor" title="Change color of the LED strip"
                style="width: 100%;" value="#000000" onchange="updateLEDColor('ledstripcolor', 'ledcolorval')" />
          </td>
        </tr>
        <tr>
          <td style="text-align: right;"><label for="bulbbrightness">&#9728;:</label></td>
          <td><input type="range" id="bulbbrightness" name="bulbbrightness" min="0" max="100" step="1" value="50" 
               title="Change brightness of light bulb" onchange="updateBulbBrightness()"></td>
          <td style="text-align: right;"><label for="ledbrightness">&#9728;:</label></td>
          <td><input type="range" id="ledbrightness" name="ledbrightness" min="0" max="100" step="1" value="25" 
               title="Change brightness of LED strip" onchange="updateLEDBrightness()"></td>
        </tr>
        <tr>
          <td style="text-align: right;"><label for="bulbtemperature">&#127777;:</label></td>
          <td><input type="range" id="bulbtemperature" name="bulbtemperature" min="150" max="350" step="1" value="250" 
               title="Set white color temperature (will change bulb to 'white')" onchange="updateBulbTemperature()"></td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
        </tr>
        <tr>
          <td>&nbsp;</td>
          <td><textarea id="bulberrormsg" name="bulberrormsg" rows="1" columns="15" 
                style="width: 150px; resize: none; color: #1e5c14; font-size: 16px; font-weight: bold; overflow: hidden; 
                  border:none; background-color: #cccccc;" readonly>&nbsp;</textarea>
          </td>
          <td>&nbsp;</td>
          <td><textarea id="lederrormsg" name="lederrormsg" rows="1" columns="15" 
                style="width: 150px; resize: none; color: #1e5c14; font-size: 16px; font-weight: bold; overflow: hidden; 
                  border:none; background-color: #cccccc;" readonly>&nbsp;</textarea>
          </td>
        </tr>
      </table>
      <!--
      <br><input type="text" id="errormsg" name="errormsg" style="color: #FF0000; font-size: 16px; font-weight: bold; border:none; background-color: #cccccc;" 
           value="" readonly><br> -->
      <!-- Hidden fields for storing states/testing -->
      <table border=1 style="display: none;">
      <tr>
        <td>Bulb State:</td><td><input type="text" id="bulbstate" name="bulbstate" value="Off"></td>
        <td>LED State:</td><td><input type="text" id="ledstate" name="ledstate" value="Off"></td>
      </tr><tr>
        <td>Bulb Color:</td><td><input type="text" id="bulbcolorval" name="bulbcolorval" value="#000000"></td>
        <td>LED Color:</td><td><input type="text" id="ledcolorval" name="ledcolorval" value="#000000"></td>
      </tr><tr>
        <td>Bulb Brightness:</td><td><input type="text" id="bulbbrightval" name="bulbbrightval" value = ""></td>
        <td>LED Brightness:</td><td><input type="text" id="ledbrightval" name="ledbrightval" value = ""></td>
      </tr></tr>  
        <td>Bulb Temperature:</td><td><input type="text" id="bulbtempval" name="bulbtempval" value = ""></td>
        <td>Bulb Color Mode:</td><td><input type="text" id="bulbcolormode", name="bulbcolormode", value = "0"></td>
      </tr></tr>
        <td>Update Field:</td><td><input type="text" id="updatefield", name="updatefield" value = ""></td>
        <td>&nbsp;</td><td>&nbsp;</td> 
      </tr></table>
    </form>  
    <h2>Settings and Defaults</h2>
    <input type="text" id="dispip" value="na" style="display: none;">
    <table border=1 cellpadding="5px">
      <tr>
        <td><button type="button" id="btnsystem" name="btnsystem" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="location.href = './system';">System</button></td>
        <td>System settings and integrations</td>
      </tr><tr>
        <td><button type="button" id="btnlights" name="btnlights" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="location.href = './lights';">Lights</button></td>
        <td>Main Bulb & LED strip settings</td>
      </tr><tr>
        <td><button type="button" id="btndisplay" name="btndisplay" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="showDisplayController();">Display</button></td>
        <td>Display Controller and Settings</td>
      </tr><tr>
        <td><button type="button" id="btnclock" name="btnclock" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="showDisplayClockSettings();">Clock</button></td>
        <td>Time zone, clock and alarm settings</td>
      </tr><tr>
        <td><button type="button" id="btnadvanced" name="btnadvanced" 
            style="text-align: center; font-size: 14px; border-radius: 8px; width: 100px; height: 30px; background-color:#ADC6C7;" 
            onclick="location.href = './advanced';">Advanced</button></td>
        <td>Touch, Home Assistant Discovery, Voice Setup...</td>
    </table>
    <br>
    <h2>Controller Commands</h2>
    <b>Note</b>: Unless stated otherwise, all commands apply to the <b><i>primary controller</i></b> only.<br>
    <table border="1" cellpadding="5px">
      <tr>
        <td><button type="button" id="btnrestart" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 150px; height: 28px;" 
             onclick="confirmRestart()">Restart</button></td>
        <td>This will reboot the <i>primary controller</i> and reload default boot values.</td>
      </tr><tr>
        <td><button type="button" id="btnupdate" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 150px; height: 28px;"
             onclick="location.href = './firmwareupdate';">Firmware Upgrade</button></td>
        <td>Upload and apply new firmware from a compiled .bin file.&nbsp;<i>(beta feature)</i></td>
      </tr><tr>
        <td><button type="button" id="btnotamode" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 150px; height: 28px;"
             onclick="location.href = './otaupdate';">Arudino OTA</button></td>
        <td>Put system in Arduino OTA mode for approx. 20 seconds to flash modified firmware from IDE.</td>
      </tr><tr>
        <td><button type="button" id="btnconfigdump" style="text-align: center; background-color:#e0df80; font-size: 14px; border-radius: 8px; width: 150px; height: 28px;"
             onclick="location.href = './configdump';">Config Dump</button></td>
        <td>See a dump of the current config file contents.</td>
      </tr><tr>
        <td><button type="button" id="btnrestartall" style="text-align: center; background-color: #FAADB7; font-size: 14px; border-radius: 8px; width: 150px; height: 28px;" 
             onclick="confirmRestartAll()">Restart ALL</button></td>
        <td>Reboot ALL connected controllers (including this primary one).</td>
      </tr><tr>
        <td><button type="button" id="btnreset" style="text-align: center; background-color: #ff0000; color: #ffffff; font-size: 14px; border-radius: 8px; width: 150px; height: 28px;" 
             onclick="confirmReset()">RESET PRIMARY</button></td>
        <td><b><font color=red>WARNING</font></b>: This will clear all settings, including WiFi, on the <b>primary</b> controller!</td>
      </tr>
    </table>
    <script>
      function fetchData() {
        const bulbButton = document.getElementById("lightbulbbtn");
        const ledButton = document.getElementById("ledstripbtn");
        fetch("/jsonmain")
          .then(response => response.json())
          .then(data => {
            //RGB Bulb
            const errMsgBulb = document.getElementById("bulberrormsg");
            const errMsgLED = document.getElementById("lederrormsg");
            document.getElementById("lightbulbcolor").value = "#" + ((data.top_bulb_color).toString(16)).padStart(6, "0");
            document.getElementById("bulbcolorval").value = data.top_bulb_color;
            document.getElementById("bulbbrightness").value = data.top_bulb_brightness;
            document.getElementById("bulbtemperature").value = data.top_bulb_temperature;
            document.getElementById("bulbcolormode").value = data.top_bulb_colormode;
            //Other Hidden Return Values
            document.getElementById("bulbbrightval").value = data.top_bulb_brightness;
            document.getElementById("bulbtempval").value = data.top_bulb_temperature;
            document.getElementById("dispip").value = data.disp_ip;
            if ((data.top_bulb_enabled) && (data.top_bulb_connected)) {
              bulbButton.innerHTML = data.top_bulb_state;
              if (data.top_bulb_state === "On") {
                bulbButton.style.backgroundColor = "#9bde81";
                document.getElementById("bulbstate").value = "On";
              } else {
                bulbButton.style.backgroundColor = "#bbbbbb";
                document.getElementById("bulbstate").value = "Off";
              }
              errMsgBulb.value ="Connected.";
            } else {
              bulbButton.innerHTML = "Disabled";
              bulbButton.style.backgroundColor = "#bbbbbb";
              document.getElementById("bulbstate").value = "Off";
              //Lock controls
              bulbButton.disabled = true;
              document.getElementById("lightbulbcolor").disabled = true;
              document.getElementById("bulbbrightness").disabled = true;
              document.getElementById("bulbtemperature").disabled = true;
              //Show error msg
              if (data.top_bulb_enabled) {
                //enabled but not connected 
                errMsgBulb.style.color = "#a67307";
                errMsgBulb.rows = "2";
                errMsgBulb.value ="Not connected. Check IP Addr.";  
              } else {
                //not enabled
                errMsgBulb.style.color = "#ff0000";
                errMsgBulb.rows = "2";
                errMsgBulb.value ="Not enabled. See 'System'.";  
              }
            }
            //LEDs 
            document.getElementById("ledbrightval").value = data.led_strip_brightness;
            document.getElementById("ledstripcolor").value = "#" + ((data.led_strip_color).toString(16)).padStart(6, "0");
            document.getElementById("ledcolorval").value = ((data.led_strip_color).toString(16)).padStart(6, "0");
            document.getElementById("ledbrightness").value = data.led_strip_brightness; 
            if (data.led_strip_enabled) {
              ledButton.innerHTML = data.led_strip_state;
              if (data.led_strip_state === "On") {
                ledButton.style.backgroundColor = "#9bde81";
                document.getElementById("ledstate").value = "On";
              } else {
                ledButton.style.backgroundColor = "#bbbbbb";
                document.getElementById("ledstate").value = "Off";
              }
              errMsgLED.value = "Connected."
            } else {
              //LEDs disabled (numLEDs = 0)
              ledButton.innerHTML = "Disabled";
              ledButton.style.backgroundColor = "#bbbbbb";
              document.getElementById("bulbstate").value = "Off";
              //Show error message
                errMsgLED.style.color = "#ff0000";
                errMsgLED.rows = "2";
                errMsgLED.value ="Not enabled. See 'System'.";  
              //Lock controls
              ledButton.disabled = true;
              document.getElementById("ledstripcolor").disabled = true;
              document.getElementById("ledbrightness").disabled = true;
            }
          })
          .catch(error => console.error("Error fetching data:", error));
      }
      function toggleBulbState() {
        const frmLive = document.getElementById("formlive");
        const bulbState = document.getElementById("bulbstate");
        const updateField = document.getElementById("updatefield"); 
        updateField.value = "bulb_state";
        if (bulbState.value == "Off") {
          bulbState.value = "On";
        } else {
          bulbState.value = "Off";
        }
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function updateBulbBrightness() {
        const brightness = document.getElementById("bulbbrightness");
        const brightval = document.getElementById("bulbbrightval");
        const updateField = document.getElementById("updatefield");
        const frmLive = document.getElementById("formlive");
        bulbbrightval.value = brightness.value;
        updateField.value = "bulb_brightness";
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function updateBulbColor(colorPickerId, textInputId) {
        const colorPicker = document.getElementById(colorPickerId);
        const textInput = document.getElementById(textInputId);
        const colorMode = document.getElementById("bulbcolormode");
        const updateField = document.getElementById("updatefield");
        const frmLive = document.getElementById("formlive");
        colorMode.value = 0;  //rgb
        textInput.value = colorPicker.value;
        updateField.value = "bulb_color";
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function updateBulbTemperature() {
        const tempSlider = document.getElementById("bulbtemperature");
        const tempVal = document.getElementById("bulbtempval");
        const updateField = document.getElementById("updatefield");
        const frmLive = document.getElementById("formlive");
        const colorMode = document.getElementById("bulbcolormode");
        colorMode.value = 1;  //color_temp
        tempVal.value = tempSlider.value;
        updateField.value = "bulb_temperature";
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function toggleLEDState() {
        const frmLive = document.getElementById("formlive");
        const ledState = document.getElementById("ledstate");
        const updateField = document.getElementById("updatefield"); 
        updateField.value = "led_strip_state";
        if (ledState.value == "Off") {
          ledState.value = "On";
        } else {
          ledState.value = "Off";
        }
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function updateLEDBrightness() {
        const brightSlider = document.getElementById("ledbrightness");
        const brightVal = document.getElementById("ledbrightval");
        const updateField = document.getElementById("updatefield");
        const frmLive = document.getElementById("formlive");
        brightVal.value = brightSlider.value;
        updateField.value = "led_strip_brightness";
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function updateLEDColor(colorPickerId, textInputId) {
        const colorPicker = document.getElementById(colorPickerId);
        const textInput = document.getElementById(textInputId);
        const updateField = document.getElementById("updatefield");
        const frmLive = document.getElementById("formlive");
        updateField.value = "led_strip_color";
        textInput.value = colorPicker.value;
        frmLive.action = "/webliveupdate";
        frmLive.submit();
      }
      function showDisplayController() {
        const dispIP = document.getElementById("dispip");
        if (dispIP.value == "na") {
          alert("Display not integrated! Setup in 'System'.");
        } else {
          let urlPath = "http://" + dispIP.value;
          window.location.href = urlPath;
        }
      }
      function showDisplayClockSettings() {
        const dispIP = document.getElementById("dispip");
        if (dispIP.value == "na") {
          alert("Display not integrated! Setup in 'System'.");
        } else {
          let urlPath = "http://" + dispIP.value + "/clock";
          window.location.href = urlPath;
        }
      }
      function confirmRestart() {
        var userConfirmed = confirm("Are you sure you want to restart the PRIMARY controller?");
        if (userConfirmed) {
          window.location.href = "./restart";
        }
      }
      function confirmRestartAll() {
        var userConfirmed = confirm("Are you sure you want to restart ALL controllers (a boot delay may be needed)?");
        if (userConfirmed) {
          window.location.href = "./restartall";
        }
      }
      function confirmReset() {
        var userConfirmed = confirm("Are you sure you want to RESET the primary controller? You will need to onboard again!");
        if (userConfirmed) {
          window.location.href = "./reset";
        }
      }
      document.addEventListener("DOMContentLoaded", fetchData);
    </script>
)literal";

const char *systempage = R"literal(
  <!-- ==============================
        INTEGRATIONS/INTERFACES PAGE 
       ============================== -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - System Integrations</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* System Integrations</H1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT: If you make any changes on this page, you <i>must</i> click 'Save and Reboot' at bottom of page.<br>
    Changes will be applied <b>only</b> after saving and rebooting.</font>
    <form id="frmsystem" action="/applyintegrate" method="post">
      <H2 style="text-decoration: underline;">Integration Boot Delay</H2>
      If using components with their own controller (light bulb, display, etc.), it is possible 
      that the primary controller will boot and attempt to connect before the external components have finished booting. 
      You can specify a <i>short</i> delay <u>if needed</u> to allow these external components to boot before the primary controller.
      <table border=0 cellpadding="5px">
        <tr>
          <td>Boot Delay (secs):</td>
          <td><input type="number" id="bootdelay" name="bootdelay" min="0" max="60" step="1" style="width: 40px;" title="Boot delay in seconds" value="0" disabled></td>
          <td>(0 - 60 max)&emsp;<input type="text" id="delaydisabled" name="delaydisabled" 
              style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #cccccc;" value="Loading. Please wait..." readonly></td>
        <tr>
      <table>
      <H2 style="text-decoration: underline;">LED Light Strip Integration</H2>
      To enable LED strip integration, set the number of LEDs in your strip (max *MAXLEDS*).<br>
      Enter zero to disable the LED strip or if not using an LED strip.
      <table border=0 cellpadding="5px">
        <tr>
          <td>Number of LEDs:</td>
          <td><input type="number" id="numleds" name="numleds" min="0" max="99" step="1" style="width: 40px;" title="Total number of LEDs" value="0" disabled>
              &nbsp;<input type="text" id="ledsdisabled" name="ledsdisabled" style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #cccccc;" 
                    value="Loading. Please wait..." readonly></td>
        </tr>
      </table>
      <H2 style="text-decoration: underline;">Main Light Bulb Integration</H2>
      The primary light bulb must first be connected to the same WiFi network using the bulb's onboarding instructions.<br>
      Leave or set the IP address to 0.0.0.0 if not using (or to disable) the main light bulb.<br>
      <table border=0 cellpadding="5px">
        <tr>
          <td>Bulb IP Address:</td>
          <td><input type="number" id="bulbip1" name="bulbip1" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
              <input type="number" id="bulbip2" name="bulbip2" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
              <input type="number" id="bulbip3" name="bulbip3" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
              <input type="number" id="bulbip4" name="bulbip4" min="0" max="255" step="1" style="width: 40px;" value="0" disabled></td>
          <td><input type="text" id="bulbdisabled" name="bulbdisabled" style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #cccccc;" 
                     value="Loading. Please wait..." readonly></td>
        </tr>
        <tr>
          <td><label for="bulbname">Bulb Name*:</label></td>
          <td><input type="text" id="bulbname" name="bulbname" maxlength="31" size="25" title="ESPHome Device Name" disabled></td>
          <td><span id="errbulbname" style="color: red;">&nbsp;</span></td>
        </tr>
        <tr>
          <td>&nbsp;</td>
          <td colspan="2">*Must match ESPHome device name - max 31 chars.</td>
        </tr>
      </table>
      <H2 style="text-decoration: underline;">Touch Panel Display Integration</H2>
      The touch panel should first be onboarded and connnected to the same WiFi network.  See the documentation for instructions.<br>
      Leave or set the IP address to 0.0.0.0 if not using (or to disable) the touch panel display.
        <table border=0 cellpadding="5px">
          <tr>
            <td>Display IP Address:</td>
            <td><input type="number" id="dispip1" name="dispip1" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
                <input type="number" id="dispip2" name="dispip2" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
                <input type="number" id="dispip3" name="dispip3" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>.
                <input type="number" id="dispip4" name="dispip4" min="0" max="255" step="1" style="width: 40px;" value="0" disabled>
                &nbsp;<input type="text" id="dispdisabled" name="dispdisabled" style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #cccccc;" 
                     value="Loading. Please wait..." readonly></td>
          </tr>
        </table><br>
      <H2 style="text-decoration: underline;">MQTT Integration</H2>
      ONLY enter this information if you already have an MQTT broker configured.<br>
      <i>To disable or remove MQTT functionality, set the IP address to 0.0.0.0</i><br>
      If a successful connection is made, a retained message of "connected" will be published to the topic "stat/your_pub_topic/mqtt" when the controller reboots.<br><br>
      <table>
        <tr>
          <td><label for="mqttaddr1">Broker IP Address:</label></td>
          <td><input type="number" min="0" max="255" step="1" id="mqttaddr1" name="mqttaddr1" 
               style="width: 40px;" value="0" disabled>.
              <input type="number" min="0" max="255" step="1" id="mqttaddr2" name="mqttaddr2" style="width: 40px;" value="0" disabled>. 
              <input type="number" min="0" max="255" step="1" id="mqttaddr3" name="mqttaddr3" style="width: 40px;" value="0" disabled>.
              <input type="number" min="0" max="255" step="1" id="mqttaddr4" name="mqttaddr4" style="width: 40px;" value="0" disabled></td>
          <td><input type="text" id="mqttdisabled" name="mqttdisabled" style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #cccccc;" 
                     value="Loading. Please wait..." readonly></td>
        </tr><tr>
          <td><label for="mqttport">MQTT Broker Port:</label></td>
          <td><input type="number" min="0" max="65535" step="1" id="mqttport" name="mqttport" style="width: 65px;" title="MQTT Port" value="1883" disabled></td>
          <td><span id="errmqttport" style="color: red;">&nbsp;</span></td>
        </tr><tr>
          <td><label for="mqttuser">MQTT User Name:</label></td>
          <td><input type="text" id="mqttuser" name="mqttuser" maxlength="64" autocomplete="username" title="User Name for MQTT" value="" disabled></td>
          <td><span id="errmqttuser" style="color: red;">&nbsp;</span></td>
        </tr><tr>
          <td><label for="mqttpw">MQTT Password:</label></td>
          <td><input type="password" id="mqttpw" name="mqttpw" maxlength="64" autocomplete="current-password" title="Password for MQTT" value="" disabled></td>
          <td>&nbsp;</td>
        </tr><tr>
          <td><label for="mqtttopicsub">MQTT Subscribe Topic:&nbsp;cmnd/</label></td>
          <td><input type="text" id="mqtttopicsub" name="mqtttopicsub" maxlength="16" value="" title="The system will subscribe to this topic to receive commands" disabled></td>
          <td><span id="errmqttsub">(16 alphanumeric chars max - no spaces, no symbols)</span></td>
        </tr><tr>
          <td><label for="mqtttopicpub">MQTT Publish Topic:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;stat/</label></td>
          <td><input type="text" id="mqtttopicpub" name="mqtttopicpub" maxlength="16" value="" title="The system will publish state changes to this topic" disabled></td>
          <td><span id="errmqttpub">(16 alphanumeric chars max - no spaces, no symbols)</span></td>
        </tr><tr>
          <td><label for="mqttperiod">Telemetry Period:</label></td>
          <td colspan="2"><input type="number" min="60" max="600" step="1" id="mqttperiod" name="mqttperiod" 
               style="width: 40px;" value="" title="How often to refresh states when idle. Actual state changes will always be published immediately" disabled>
              &nbsp;seconds (60 min, 600 max)</td>
        </tr><tr>
          <td><label for="discovery">MQTT Discovery:</label></td>
          <td colspan="2"><a href="http://*IPADDR*/advanced?section=discovery">Configure Home Assistant MQTT Discovery</a> (beta)</td>
        </tr>
      </table><br>
      <H2 style="text-decoration: underline;">Weather Integration</H2>
      <i>Optional.</i> Only used if a display is integrated.<br><br>
      <table border=0>
        <tr>
          <td>Temperature Units:</td>
          <td style="padding-left: 5px;"><input type="radio" id="degf" name="tempunits" title="Use °F for temperatures" value="13" disabled>&nbsp;Fahrenheit</td>
          <td style="padding-left: 5px;"><input type="radio" id="degc" name="tempunits" title="Use °C for temperatures" value="12" disabled>&nbsp;Celcius</td>
          <td>&nbsp;</td>
          <td>&nbsp;</td>
        </tr>
        <tr>
          <td>Temperature Source:</td>
          <td style="padding-left: 5px;"><input type="radio" id="tempnone" name="weathersource" value="0" title="No temperature/disable" disabled>&nbsp;None/Disable</td>
          <td style="padding-left: 5px;"><input type="radio" id="tempowm" name="weathersource" value="1" title="Use Open Weather Map for temperature" disabled>&nbsp;OWM</td>
          <td style="padding-left: 5px;"><input type="radio" id="tempmqtt" name="weathersource" value="2" title="Get temperature from MQTT source" disabled>&nbsp;MQTT</td>
          <td style="padding-left: 5px;"><input type="radio" id="tempapi" name="weathersource" value="3" title="Get temperature from API source" disabled>&nbsp;API
          &nbsp;<input type="text" id="tempdisabled" name="tempdisabled" style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #cccccc;" 
                     value="Loading. Please wait..." readonly></td>
        </tr>
      </table><br>
      <u><b>Open Weather Map</b></u><br>
      <i>These values only applicable if OWM selected above - ignored otherwise</i>
      <table border="0">
        <tr>
          <td>OWM API Key:</td>
          <td><input type="text" id="owmkey" name="owmkey" maxlength="34" style="width: 20em;" 
               title="Enter or paste your Open Weather Map API key" value="" disabled></td>
          <td><input type="text" id="owmconnected" name="owmconnected" style="color: #660d70; font-size: 16px; font-weight: bold; font-style: italic; border:none; background-color: #cccccc;" 
                     value="Loading. Please wait..." readonly></td>
        </tr><tr>
          <td>OWM Latitude:</td>
          <td><input type="number" id="owmlat" name="owmlat" min="-90" max="90" step="0.0001" style="width: 6em;" value=""
               title="Enter the latitude at your location" disabled></td>
          <td><span id="errowmlat" style="color: red;">&nbsp;</span></td>

        </tr><tr>
          <td>OWM Longitude:</td>
          <td><input type="number" id="owmlong" name="owmlong" min="-180" max="180" step="0.0001" style="width: 6em;" value="" 
               title="Enter the longitude at your location" disabled></td>
          <td><span id="errowmlong" style="color: red;">&nbsp;</span></td>
        </tr><tr>
          <td>OWM Refresh Interval:</td>
          <td colspan="2"><input type="number" id="tempupdperiod" name="tempupdperiod" min="10" max="1440" step="1" style="width: 5em;" 
                           title="How often temperature is refreshed from OWM" value="" disabled>
          &nbsp;minutes (min 10 - max 1440)</td>
        </tr>
      </table><br>
      <i>If you 'Save & Reboot', all changes made will be saved and written to the configuration file.<br>
      The <b>controller will reboot</b> and any integration changes will be loaded.</i><br><br>
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
            const bootDelay = document.getElementById("bootdelay");
            const numLEDs = document.getElementById("numleds");
            const bulbIP1 = document.getElementById("bulbip1");
            const bulbIP2 = document.getElementById("bulbip2");
            const bulbIP3 = document.getElementById("bulbip3");
            const bulbIP4 = document.getElementById("bulbip4");
            const bulbName = document.getElementById("bulbname");
            const dispIP1 = document.getElementById("dispip1");
            const dispIP2 = document.getElementById("dispip2");
            const dispIP3 = document.getElementById("dispip3");
            const dispIP4 = document.getElementById("dispip4");
            const mqttIP1 = document.getElementById("mqttaddr1");
            const mqttIP2 = document.getElementById("mqttaddr2");
            const mqttIP3 = document.getElementById("mqttaddr3");
            const mqttIP4 = document.getElementById("mqttaddr4");
            const mqttPort = document.getElementById("mqttport");
            const mqttUser = document.getElementById("mqttuser");
            const mqttPW = document.getElementById("mqttpw");
            const mqttTopicSub = document.getElementById("mqtttopicsub");
            const mqttTopicPub = document.getElementById("mqtttopicpub");
            const mqttPeriod = document.getElementById("mqttperiod");
            const degF = document.getElementById("degf");
            const degC = document.getElementById("degc");
            const tempNone = document.getElementById("tempnone");
            const tempOWM = document.getElementById("tempowm");
            const tempMQTT = document.getElementById("tempmqtt");
            const tempAPI = document.getElementById("tempapi");
            const owmKey = document.getElementById("owmkey");
            const owmLat = document.getElementById("owmlat");
            const owmLong = document.getElementById("owmlong");
            const owmUpdPeriod = document.getElementById("tempupdperiod");
            //Messages
            const delayMsg = document.getElementById("delaydisabled");
            const ledMsg = document.getElementById("ledsdisabled");
            const bulbMsg = document.getElementById("bulbdisabled");
            const dispMsg = document.getElementById("dispdisabled");
            const mqttMsg = document.getElementById("mqttdisabled");
            const tempMsg = document.getElementById("tempdisabled");
            const owmMsg = document.getElementById("owmconnected");
            //Boot Delay
            bootDelay.value = data.boot_delay;
            delayMsg.value = "";
            bootDelay.required = true;
            //LED Strip
            numLEDs.value = data.num_leds;
            numLEDs.required = true;
            if (data.leds_enabled) {
              ledMsg.value = "Enabled & Connected";
              ledMsg.style.color = "#1e5c14";
            } else {
              ledMsg.value = "Disabled";
              ledMsg.style.color = "#FF0000";
            }
            //Bulb
            bulbIP1.value = data.bulb_addr_1;
            bulbIP2.value = data.bulb_addr_2;
            bulbIP3.value = data.bulb_addr_3;
            bulbIP4.value = data.bulb_addr_4;
            bulbName.value = data.bulb_name;
            if (data.bulb_enabled) {
              if (data.bulb_connected) {
                bulbMsg.value = "Enabled & Connected";
                bulbMsg.style.color = "#1e5c14";
              } else {
                bulbMsg.value = "Enabled - Not Connected!";
                bulbMsg.style.color = "#a67307";
              }

            } else {
             bulbMsg.value = "Disabled";
             bulbMsg.style.color = "#FF0000";
            }
            //Touch Display
            dispIP1.value = data.disp_addr_1;
            dispIP2.value = data.disp_addr_2;
            dispIP3.value = data.disp_addr_3;
            dispIP4.value = data.disp_addr_4;
            if (data.disp_enabled) {
              if (data.disp_connected) {
                dispMsg.value = "Enabled & Connected";
                dispMsg.style.color = "#1e5c14";
              } else {
                dispMsg.value = "Enabled - Not Connected!";
                dispMsg.style.color = "#a67307";
              }
            } else {
             dispMsg.value = "Disabled";
             dispMsg.style.color = "#ff0000";
            }
            //MQTT
            mqttIP1.value = data.mqtt_addr_1;
            mqttIP2.value = data.mqtt_addr_2;
            mqttIP3.value = data.mqtt_addr_3;
            mqttIP4.value = data.mqtt_addr_4;
            mqttPort.value = data.mqtt_port;
            mqttUser.value = data.mqtt_user;
            mqttPW.value = data.mqtt_pw;
            mqttTopicSub.value = data.mqtt_topic_sub;
            mqttTopicPub.value = data.mqtt_topic_pub;
            mqttPeriod.value = data.mqtt_tele_period;
            if (data.mqtt_enabled) {
              if (data.mqtt_connected) {
                mqttMsg.value = "Enabled & Connected";
                mqttMsg.style.color = "#1e5c14";
              } else {
                mqttMsg.value = "Enabled - Not Connected!";
                mqttMsg.style.color = "#a67307";
              }
            } else {
             mqttMsg.value = "Disabled";
             mqttMsg.style.color = "#ff0000";
            }
            const tempSymbol = Number(data.temp_units);
            //Weather
            if (tempSymbol == 12) {
              degC.checked = true
            } else {
              degF.checked = true
            }
            const tempSource = Number(data.weather_source);
            switch (tempSource) {
              case 0:
                tempNone.checked = true;
                break;
              case 1:
                tempOWM.checked = true;
                break;
              case 2:
                tempMQTT.checked = true;
                break;
              case 3:
                tempAPI.checked = true;
                break;
              default:
                //Nothing
            }
            if (tempSource > 0) {
              tempMsg.value = "Enabled";
              tempMsg.style.color = "#1e5c14";
              if (tempSource == 1) {
                //owm connectivity
                if (data.owm_connected) {
                  owmMsg.value = "Connected";
                  owmMsg.style.color = "#1e5c14";
                } else {
                  owmMsg.value = "Not Connected!";
                  owmMsg.style.color = "#ff0000";
                }
              } else {
                owmMsg.value = "";
              }
            } else {
              tempMsg.value = "Disabled";
              tempMsg.style.color = "#ff0000";
              owmMsg.value = "";
              owmMsg.style.color = "#ff0000";
            }
            owmKey.value = data.owm_key;
            owmLat.value = data.owm_lat;
            owmLong.value = data.owm_long;
            owmUpdPeriod.value = data.temp_update_period;
            //Enable controls now that data is loaded
            bootDelay.disabled = false;
            numLEDs.disabled = false;
            bulbIP1.disabled = false;
            bulbIP2.disabled = false;
            bulbIP3.disabled = false;
            bulbIP4.disabled = false
            bulbName.disabled = false;
            dispIP1.disabled = false;
            dispIP2.disabled = false;
            dispIP3.disabled = false;
            dispIP4.disabled = false;
            mqttIP1.disabled = false;
            mqttIP2.disabled = false;
            mqttIP3.disabled = false;
            mqttIP4.disabled = false;
            mqttPort.disabled = false;
            mqttUser.disabled = false;
            mqttPW.disabled = false;
            mqttTopicSub.disabled = false;
            mqttTopicPub.disabled = false;
            mqttPeriod.disabled = false;
            degF.disabled = false;
            degC.disabled = false;
            tempNone.disabled = false;
            tempOWM.disabled = false;
            tempMQTT.disabled = false;
            tempAPI.disabled = false;
            owmKey.disabled = false;
            owmLat.disabled = false;
            owmLong.disabled = false;
            owmUpdPeriod.disabled = false;
          })
          .catch(error => console.error("Error fetching data:", error));
      }
      function validData() {
        let dataOK = true;
        let ctrlName = "";
        //Bulb
        let bulbIP1 = Number((document.getElementById("bulbip1").value));
        let bulbIP2 = Number((document.getElementById("bulbip1").value));
        let bulbIP3 = Number((document.getElementById("bulbip1").value));
        let bulbIP4 = Number((document.getElementById("bulbip1").value));
        if ((bulbIP1 > 0) || (bulbIP2 > 0) || (bulbIP3 > 0) || (bulbIP4 > 0)) {
          const bulbName = document.getElementById("bulbname");
          if (bulbName.value.trim() == "") {
            dataOK = false;
            if (ctrlName == "") ctrlName = "bulbname";
            bulbName.style = "background-color: #f5f39d;";
            document.getElementById("errbulbname").textContent = "Name is required for bulb integration!";
          }
        }
        //MQTT
        let mqttIP1 = Number(document.getElementById("mqttaddr1").value);
        let mqttIP2 = Number(document.getElementById("mqttaddr2").value);
        let mqttIP3 = Number(document.getElementById("mqttaddr3").value);
        let mqttIP4 = Number(document.getElementById("mqttaddr4").value);
        if ((mqttIP1 > 0) || (mqttIP2 > 0) || (mqttIP3 > 0) || (mqttIP4 > 0)) {
          const mqttPort = document.getElementById("mqttport");
          const mqttSub = document.getElementById("mqtttopicsub"); 
          const mqttPub = document.getElementById("mqtttopicpub");
          if (mqttPort.value.trim() === "") {
            dataOK = false;
            if (ctrlName == "") ctrlName = "mqttport";
            mqttPort.style = "background-color: #f5f39d;";
            document.getElementById("errmqttport").textContent = "Port is required for MQTT integration!";
          }
          if (mqttSub.value.trim() === "") {
            dataOK = false;
            if (ctrlName == "") ctrlName = "mqtttopicsub";
            mqttSub.style = "background-color: #f5f39d;";
            document.getElementById("errmqttsub").textContent = "Subscribe topic is required for MQTT!";
          } else if ((mqttSub.value.trim().indexOf(" ")) > 0) {
            dataOK = false;
            if (ctrlName == "") ctrlName = "mqtttopicsub";
            mqttSub.style = "background-color: #f5f39d;";
            document.getElementById("errmqttsub").textContent = "Topic cannot contain spaces!";
          } else if ((mqttSub.value.trim().indexOf("/")) == 0) {
            const errSub = document.getElementById("errmqttsub");
            dataOK = false;
            if (ctrlName == "") ctrlName = "mqtttopicsub";
            mqttSub.style = "background-color: #f5f39d;";
            errSub.textContent = "Topic cannot begin with '/'";
            errSub.style ="color: red;"
          }
          if (mqttPub.value.trim() == "") {
            dataOK = false;
            if (ctrlName == "") ctrlName = "mqtttopicpub";
            mqttPub.style = "background-color: #f5f39d;";
            document.getElementById("errmqttpub").textContent = "Publish topic is required for MQTT!";
          } else if ((mqttPub.value.trim().indexOf(" ")) > 0) {
            dataOK = false;
            if (ctrlName == "") ctrlName = "mqtttopicpub";
            mqttPub.style = "background-color: #f5f39d;";
            document.getElementById("errmqttpub").textContent = "Topic cannot contain spaces!";
          } else if ((mqttPub.value.trim().indexOf("/")) == 0) {
            errPub = document.getElementById("errmqttpub");
            dataOK = false;
            if (ctrlName == "") ctrlName = "mqtttopicpub";
            mqttPub.style = "background-color: #f5f39d;";
            errPub.textContent = "Topic cannot begin with '/'";
            errPub.style = "color: red;";
          }
        }
        //OWM
        if (document.getElementById("tempowm").checked) {
          const keyVal = document.getElementById("owmkey")
          const latVal = document.getElementById("owmlat");
          const longVal = document.getElementById("owmlong");
          const keylatMsg = document.getElementById("errowmlat");   //Shared message for key and latitude
          if ((keyVal.value.trim() == "") || (keyVal.value == "N/A")) {
            dataOK = false;
            if (ctrlName == "") ctrlName = "owmkey";
            keyVal.style = "background-color: #f5f39d;";
            document.getElementById("errowmlat").textContent = "API key is required for OWM!";
          }
          if (latVal.value.trim() == "") {
            dataOK = false;
            if (ctrlName == "") ctrlName = "owmlat";
            latVal.style = "background-color: #f5f39d;";
            if (keylatMsg.textContent.trim() == "") {
              keylatMsg.textContent = "Latitude is required for OWM!";
            }
          }
          if (longVal.value.trim() == "") {
            dataOK = false;
            if (ctrlName = "") ctrlName = "owmlong";
            longVal.style = "background-color: #f5f39d;";
            document.getElementById("errowmlong").textContent = "Longitude is required for OWM!";
          }
        }
        if (!dataOK) {
          event.preventDefault(); // Prevent form submission
          if (ctrlName != "") {
            document.getElementById(ctrlName).focus();
          }
        }
      }
      document.addEventListener("DOMContentLoaded", fetchData);
      document.getElementById("frmsystem").addEventListener("submit", validData);
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
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* Integrations Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">Integration changes saved to configuration file.</h2><br>
    Any integrated lights will be set to off and the device will reboot. <i>If integrated, the display will also reboot</i>.<br><br> 
    After the boot(s) completes, you may:<br><br>
    <button type="button" id="btnintegrations" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/system'">Return to Systems Integrations</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
)literal";


const char *lightspage = R"literal(
  <!-- ======================
        LIGHT SETTINGS PAGE 
       ====================== -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Light Settings</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
      tr { height: 30px; }
    </style>
  </head>
  <body>
    <h1>*VAR_APP_NAME* Light Options and Settings</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT:
      <ul>
        <li>Light(s) must be enabled via Integrations page before changes can be made here.</li>
        <li>TEST Button: You may test changes without saving them.  Changes will be lost upon next reboot/restart.</li>
        <li>SAVE and REBOOT: This will save the current values as the new boot defaults.  Controller will reboot.</li>
      </ul>
    </font>
    See documentation for option details and additional information.<br><br>
    <button type="button" id="btnupdate" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 140px; height: 35px;"
             onclick="location.href = '/';"><< Return to Main</button><br>       
    <h2 style="text-decoration: underline;">LED Light Strip Settings</h2>
    <p id="lednotintegrated" hidden><font color="red"><b>LEDs are not enabled!</b></font> Enable via <a href="./system">Integrations</a> page.<br></p>
    <form id="formled" action="/default-action" method="post">
      <table id="ledtable" border="0">
        <tr>
          <td>Total Number of LEDs:</td>
          <td><input type="number" id="numleds" name="numleds" min="0" max="99" step="1" title="Total number of individual LED pixels in LED strip." 
               style="width: 40px;" value="0" onchange="verifyNumLEDs()"></td>
          <td><input type="text" id="msgnumleds" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Start-up State:</td>
          <td><input type="radio" id="ledstartupoff" name="ledstartstate" value="Off" title="Set LED strip to 'Off' after booting"><label for="ledstartupoff">Off</label>
              <input type="radio" id="ledstartupon" name="ledstartstate" value="On" title="Set LED strip to 'On' after booting"><label for="ledstartupon">On</label>
          </td>
          <td><input type="text" id="msgledstartup" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Default Color:</td>
          <td><input type="color" id="ledcolor" name="ledcolor" title="Set default color of LEDs after boot" 
               style="width: 100%;" value="#000000"/>
          </td>
          <td><input type="text" id="msgledcolor" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Default Brightness:</td>
          <td><input type="range" id="ledbright" name="ledbright" min="0" max="100" step="1" value="50" title="Set default LED Strip brightness after boot"></td>
          <td><input type="text" id="msgledbright" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Show Test During Boot:</td>
          <td><input type="radio" id="bootlightsledoff" name="bootlightsled" value="0" title="Disable LED test during boot"><label for="bootlightsledoff">Off</label>
              <input type="radio" id="bootlightsledon" name="bootlightsled" value="1" title="Enable LED test during boot"><label for="bootllightsledon">On</label>
          </td>
          <td><input type="text" id="msgledboot" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr>
      </table><br>
      <input type="hidden" id="ledcolorval" value = "">
      <input type="hidden" id="ledtestflag" value="0">
      <button type="submit" id="ledtestbtn" name="ledtestbtn" value="test_leds" title="Test current LED settings"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#a4f086;"
              >Test</button>&ensp;
      <button type="button" id="ledresetbtn" name="ledresetbtn" title="Reset all values to current boot defaults"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#e0df80;"
              onclick="fetchData('led')">Reset</button>&ensp;
      <button type="submit" id="ledsavebtn" name="ledsavebtn" value="save_leds" title="Save changes and reboot controller to activate"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#FAADB7;"
              >Save & Reboot</button>
    </form>
    <br>
    <h2 style="text-decoration: underline;">Main Light Bulb Settings</h2>
    <p id="bulbnotintegrated" hidden><font color="red"><b>Light bulb is not enabled!</b></font> Enable via <a href="./system">Integrations</a> page.<br></p>
    <form id="formbulb" action="/default-action" method="post">
      <table id="bulbtable" border="0">
        <tr>
          <td>Bulb IP Address:</td>
          <td><input type="text" id="bulbip"
               style="color: #0000ff; width: 120px; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
          <td>&nbsp;Change via <a href="./system">Integrations</a></td>
        </tr><tr>
          <td>Bulb Name:</td> 
          <td colspan="2"><input type="text" id="bulbname"
               style="color: #0000ff; width: 250px; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
          <td>&nbsp;</td>
        </tr><tr>
          <td>Start-up State:</td>
          <td><input type="radio" id="bulbstartupoff" name="bulbstartstate" value="0"  title="Set bulb to 'Off' after booting"><label for="bulbstartupoff">Off</label>
              <input type="radio" id="bulbstartupon" name="bulbstartstate" value="1" title="Set bulb to 'On' after booting"><label for="bulbstartupon">On</label></td>
          <td><input type="text" id="msgbulbstartup" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Default Color Mode:</td>
          <td><input type="radio" id="bulbmodergb" name="bulbcolormode" value="0" title="Set bulb to RGB mode (using RGB color) after boot"><label for="bulbmodergb">RGB</label>
              <input type="radio" id="bulbmodewhite" name="bulbcolormode" value="1" title="Set bulb to WHITE mode (using color temp) after boot"><label for="bulbmodewhite">White</label></td>
          <td><input type="text" id="msgbulbcolormode" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Default RGB Color:</td>
          <td><input type="color" id="bulbcolor" name="bulbcolor" title="Default RGB color after boot" 
               style="width: 100%;" value="#000000"/>
          </td>
          <td><input type="text" id="msgbulbcolor" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Default White Temp.:</td>          
          <td><input type="range" id="bulbtemperature" name="bulbtemperature" min="155" max="370" step="1" value="250" title="Default white temperature after reboot"></td>
          <td><input type="text" id="msgbulbtemp" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Default Brightness:</td>
          <td><input type="range" id="bulbbright" name="bulbbright" min="0" max="100" step="1" value="50" title="Default bulb brightness after boot"></td>
          <td><input type="text" id="msgbulbbright" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr><tr>
          <td>Show Test During Boot:</td>
          <td><input type="radio" id="bootbulboff" name="bootlightsbulb" value="0" title="Disable bulb test during boot"><label for="bootbulboff">Off</label>
              <input type="radio" id="bootbulbon" name="bootlightsbulb" value="1" title="Enable LED test during boot"><label for="bootbulbon">On</label>
          </td>
          <td><input type="text" id="msgbulbboot" maxlength="50" size="50"
               style="color: #ff0000; font-weight: bold; background-color: #cccccc; border: none; pointer-events: none; user-select: none; tabindex: -1;" value="" readonly></td>
        </tr>
      <table><br>
      <input type="hidden" id="bulbcolorval" value = "">
      <button type="submit" id="bulbtestbtn" name="bulbtestbtn" value="test_bulb" title="Test current bulb settings"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#a4f086;"
              >Test</button>&ensp;
      <button type="button" id="bulbresetbtn" name="bulbresetbtn" title="Reset all bulb values to current boot defaults"
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#e0df80;"
              onclick="fetchData('bulb')">Reset</button>&ensp;
      <button type="submit" id="bulbsavebtn" name="bulbsavebtn" value="save_bulb" title="Save changes and reboot controller to activate" 
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#FAADB7;"
              >Save & Reboot</button>
    </form><br>
    <script>
      function fetchData(whichData) {
        const numLEDs = document.getElementById("numleds");
        const numLEDsMsg = document.getElementById("msgnumleds");
        const ledStartUpMsg = document.getElementById("msgledstartup");
        const ledColorMsg = document.getElementById("msgledcolor");
        const ledBrightMsg = document.getElementById("msgledbright");
        const ledLedBootMsg = document.getElementById("msgledboot");
        fetch("/jsonlights")
          .then(response => response.json())
          .then(data => {
            if ((whichData == "led") || (whichData == "all")) {
              //------ LEDS ------
              if (data.use_leds) {
                numLEDs.value = data.num_leds;
                numLEDs.max = data.max_leds;
                if (data.led_state) {
                  document.getElementById("ledstartupon").checked = true;
                } else {
                  document.getElementById("ledstartupoff").checked = true;
                }
                document.getElementById("ledcolor").value = "#" + ((data.led_color).toString(16)).padStart(6, "0");
                document.getElementById("ledcolorval").value = ((data.led_color).toString(16)).padStart(6, "0");
                document.getElementById("ledbright").value = data.led_brightness;
                if (data.use_boot_lights_led) {
                  document.getElementById("bootlightsledon").checked = true; 
                } else {
                  document.getElementById("bootlightsledoff").checked = true;
                }
                document.getElementById("ledtestflag").value = data.led_test_flag;
              } else {
                //hide table and show not-integrated <p>
                document.getElementById("formled").style.display = "none";
                document.getElementById("lednotintegrated").hidden = false;
              }
              //Clear any previous error msgs (for reset button)

              document.getElementById("msgnumleds").value = "";
              document.getElementById("numleds").style = "background-color: #ffffff;";
            }
            if ((whichData == "bulb") || (whichData == "all")) {
              //----- LIGHT BULB -----
              if (data.bulb_enabled) {
                document.getElementById("bulbip").value = data.bulb_ip;
                document.getElementById("bulbname").value = data.bulb_name;
                if (data.bulb_state) {
                  document.getElementById("bulbstartupon").checked = true;
                } else {
                  document.getElementById("bulbstartupoff").checked = true;
                }
                document.getElementById("bulbcolor").value = "#" + ((data.bulb_color).toString(16)).padStart(6, "0");
                document.getElementById("bulbcolorval").value = ((data.bulb_color).toString(16)).padStart(6, "0");
                if (data.bulb_color_mode) {
                  document.getElementById("bulbmodewhite").checked = true;
                } else {
                  document.getElementById("bulbmodergb").checked = true;
                }
                document.getElementById("bulbcolorval").value = ((data.bulb_color).toString(16)).padStart(6, "0");
                document.getElementById("bulbbright").value = data.bulb_brightness;
                document.getElementById("bulbtemperature").value = data.bulb_temperature;
                if (data.use_boot_lights_bulb) {
                  document.getElementById("bootbulbon").checked = true; 
                } else {
                  document.getElementById("bootbulboff").checked = true;
                }
              } else {
                //hide table and show not-integrated <p>
                document.getElementById("formbulb").style.display = "none";
                document.getElementById("bulbnotintegrated").hidden = false;
              }
            }
          })
          .catch(error => console.error("Error fetching data:", error));
      }
      async function setFieldsDisabled(prefix, state) {
        const fields = (prefix === 'led') 
          ? ["numleds", "ledstartupon", "ledstartupoff", "ledcolor", "ledbright", "bootlightson", "bootlightsoff", "ledresetbtn", "ledsavebtn"]
          : ["bulbstartupon", "bulbstartupoff", "bulbmodergb", "bulbmodewhite", "bulbcolor", "bulbtemperature", "bulbbright", "bootbulbon", "bootbulboff", "bulbresetbtn", "bulbsavebtn"];
        fields.forEach(id => {
          const el = document.getElementById(id);
          if (el) el.disabled = !state;
        }); 
      }
      function verifyNumLEDs() {
        const frmLED = document.getElementById("formled");
        const numLEDs = document.getElementById("numleds");
        const numLEDsMsg = document.getElementById("msgnumleds");
        if (numLEDs.value.trim() === "") {
          numLEDsMsg.value = "Cannot be blank. Leave as '0' to disable LEDs";
          numLEDs.style = "background-color: #f5f39d;";
          numLEDs.value = "0";
        } else {
          numLEDsMsg.value = "";
          numLEDs.style = "background-color: #ffffff;";
        }
      }
      document.addEventListener("DOMContentLoaded", fetchData("all"));
      document.getElementById("formled").addEventListener("submit", async function (event) {
        event.preventDefault();
        const frmLED = document.getElementById("formled");
        const btnVal = event.submitter.value;
        const numLEDs = document.getElementById("numleds");
        const numLEDsMsg = document.getElementById("msgnumleds");
        const colorPicker = document.getElementById("ledcolor");
        const ledColorMsg = document.getElementById("msgledcolor");
        const ledBright = document.getElementById("ledbright");
        const ledBrightMsg = document.getElementById("msgledbright");
        const btnTest = document.getElementById("ledtestbtn");
        var okToSave = true;
        //check fields for validity
        if (numLEDs.value.trim() === "") {
          numLEDsMsg.value = "Cannot be blank. Leave as '0' to disable LEDs";
          numLEDs.style = "background-color: #f5f39d;";
          numLEDs.value = "0";
          okToSave = false;
        } else {
          numLEDsMsg.value = "";
          numLEDs.style = "background-color: #ffffff;";
        }
        if ((okToSave) && ((ledBright.value < 1) || (document.getElementById("ledcolor").value == "#000000")) ) {
          if ((btnTest.innerHTML == "Test") || (btnVal == "save_leds")){
            var userConfirmed = confirm("If color is black or brightness is 0, then LEDs will always appear 'off' even when 'on'. Continue anyway?");
            if (!userConfirmed) {
              okToSave = false;
            }
          }
        }
        if (okToSave) {
          if (btnVal == "test_leds") {
            const isStarting = (btnTest.innerHTML == "Test");
            const data = {
              mode: ((btnTest.innerHTML == "Test") ? 1:0),
              numleds: Number(numLEDs.value),
              color: colorPicker.value,
              brightness: Number(ledBright.value)
            };
            await setFieldsDisabled('led', !isStarting);
            try {
              await fetch("/testleds", {
               method: "POST", 
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(data)
              });
              btnTest.innerHTML = isStarting ? "Stop" : "Test";
            } catch (e) { console.error(e); }
            return;
          } else { 
            await setFieldsDisabled('led', true);
            this.action = "/saveleds";
            this.submit();
            return;
          }
        }
      });
      document.getElementById("formbulb").addEventListener("submit", async function (event) {
        event.preventDefault();
        const btnVal = event.submitter.value;
        const btnTest = document.getElementById("bulbtestbtn");
        const bulbColorMsg = document.getElementById("msgbulbcolor");
        const bulbTempMsg = document.getElementById("msgbulbtemp");
        const bulbBrightMsg = document.getElementById("msgbulbbright");
        var okToSave = true;
        // Bulb validation (if needed)
        if ((Number(document.getElementById("bulbbright").value) < 1) || ((document.getElementById("bulbcolor").value == "#00000") && (Number(document.getElementById("bulbcolormode").value) == 0))) {
          if ((btnTest.innerHTML == "Test") || (btnVal == "save_bulb")) {
            var userConfirmed = confirm("If color mode is RGB and color is black or brightness is 0, then bulb will always appear 'off' even when 'on'. Continue anyway?");
            if (!userConfirmed) {
              okToSave = false;
            }
          }
        }
        if (okToSave) {
          if (btnVal == "test_bulb") {
            const isStarting = (btnTest.innerHTML == "Test");
            const data = {
              mode: isStarting ? 1 : 0,
              color: document.getElementById("bulbcolor").value,
              brightness: Number(document.getElementById("bulbbright").value),
              temp: Number(document.getElementById("bulbtemperature").value),
              colormode: (document.getElementById("bulbmodergb").checked ? 0 : 1)
            };
            await setFieldsDisabled('bulb', !isStarting);
            try {
              await fetch("/testbulb", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(data)
              });
              btnTest.innerHTML = isStarting ? "Stop Test" : "Test";
            } catch (e) { console.error(e); }
            return;
          } else {
            await setFieldsDisabled('bulb', true);
            this.action = "/savebulb";
            this.submit();
          }
        }
      });
    </script>
)literal";

const char *postLEDSettings = R"literal(
  <!-- ----------------------------
        LED DEFAULTS UPDATED PAGE 
       ---------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - LED Defaults Submitted</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* LED Defaults Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">LED defaults saved to configuration file.</h2><br>
    Any integrated lights will be set to off and the device will reboot.  After boot completes, you may:<br><br>
    <button type="button" id="btnlights" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/lights'">Return to Light Settings</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
)literal";

const char *postBulbSettings = R"literal(
  <!-- ----------------------------
        BULB DEFAULTS UPDATED PAGE 
       ---------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Bulb Defaults Submitted</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* Bulb Defaults Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">Light bulb defaults saved to configuration file.</h2><br>
    Any integrated lights will be set to off and the device will reboot.  After boot completes, you may:<br><br>
    <button type="button" id="btnlights" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/lights'">Return to Light Settings</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
)literal";

const char *advancedpage = R"literal(
  <!-- =======================
        ADVANCED OPTIONS PAGE 
       ======================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Advanced Options</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1>*VAR_APP_NAME* Advanced Settings & Options</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <font color=red>**IMPORTANT: For each section, you must click "Save & Reboot" before any changes will go into effect.</font><br><br>
    See documentation for option details and additional information.<br><br>
    <button type="button" id="btnreturn" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 140px; height: 35px;"
             onclick="location.href = '/';"><< Return to Main</button><br>       
    <h2 style="text-decoration: underline;">Touch Control Settings</h2>
    <form id="frmtouch" action="/default-action" method="post">
      <table border=0>
        <tr>
          <td><b>Touch Control 1</b>:</td>
          <td><input type="checkbox" id="touch1enable" name="touch1enable" value="1" onchange="toggleTouch1Controls()">
              <label for="touch1enable">Enable</label></td>
          <td>&nbsp;</td>
        </tr><tr>
          <td>Primary Function:</td>
          <td><select id="touch1control1" name="touch1control1" title="Select non-alarm primary function" value="0">
                <option value="0">None</option>
                <option value="1">Toggle Light Bulb</option>
                <option value="2">Toggle LED Strip</option>
                <option value="3">Increase Bulb Brightness</option>
                <option value="4">Decrease Bulb Brightness</option>
                <option value="5">Increase LED Strip Brightness</option>
                <option value="6">Decrease LED Strip Brightness</option>
                <option value="7">Increase Display Brightness</option>
                <option value="8">Decrease Display Brightness</option>
              </select></td>  
          <td>&nbsp;</td>
        </tr><tr>
          <td>Alarm Function:</td>
          <td><select id="touch1control2" name="touch1control2" title="Select function when alarm sounding" value="0">
                <option value="0">None</option>
                <option value="1">Snooze Alarm</option>
                <option value="2">Stop Active Alarm</option>
              </select></td>  
          <td>&nbsp;</td>
        </tr><tr>
          <td><span title="Time (in ms) sensor must be pressed before activation">Touch Duration (ms):</span></td>
          <td><input type="number" id="touch1duration" name="touch1duration" title="Time (in ms) sensor must be pressed before activation" 
              min="250" max="2000" step="10" style="width: 45px;" value="250">&nbsp;250 min - 2000 max</td>
          <td>&nbsp;</td>
        </tr>
      </table>
      <br>
      <table border=0>
        <tr>
          <td><b>Touch Control 2</b>:</td>
          <td><input type="checkbox" id="touch2enable" name="touch2enable" value="1" onchange="toggleTouch2Controls()">
              <label for="touch1enable">Enable</label></td>
          <td>&nbsp;</td>
        </tr><tr>
          <td>Primary Function:</td>
          <td><select id="touch2control1" name="touch2control1" title="Select non-alarm primary function">
                <option value="0">None</option>
                <option value="1">Toggle Light Bulb</option>
                <option value="2">Toggle LED Strip</option>
                <option value="3">Increase Bulb Brightness</option>
                <option value="4">Decrease Bulb Brightness</option>
                <option value="5">Increase LED Strip Brightness</option>
                <option value="6">Decrease LED Strip Brightness</option>
                <option value="7">Increase Display Brightness</option>
                <option value="8">Decrease Display Brightness</option>
              </select></td>  
          <td>&nbsp;</td>
        </tr><tr>
          <td>Alarm Function:</td>
          <td><select id="touch2control2" name="touch2control2" title="Select function when alarm sounding">
                <option value="0">None</option>
                <option value="1">Snooze Alarm</option>
                <option value="2">Stop Active Alarm</option>
              </select></td>  
          <td>&nbsp;</td>
        </tr><tr>
          <td>Touch Duration (ms):</td>
          <td><input type="number" id="touch2duration" name="touch2duration" title="Time (in ms) sensor must be pressed before activation"
               min="250" max="2000" step="10" style="width: 45px;" value="250">&nbsp;250 min - 2000 max</td>
          <td>&nbsp;</td>
        </tr>
      </table><br>
      <table border=1 style="display: none;"> 
        <tr>
          <td><input type="text" id="touch1enableval" name="touch1enableval" value="0"></td>
          <td><input type="text" id="touch2enableval" name="touch2enableval" value="0"></td>
        </tr>
      </table>
      <button type="button" id="touchresetbtn" name="touchresetbtn title="Reset values to boot defaults" 
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#e0df80;"
              onclick="fetchData('touch')">Reset</button>&ensp;
      <button type="button" id="touchsavebtn" name="touchsavebtn title="Save current settings as new boot defaults"   
              style="text-align: center; font-size: 16px; border-radius: 8px; width: 140px; height: 40px; background-color:#FAADB7;"
              onclick="saveTouch()">Save & Reboot</button>
    </form>
    <br>
    <h2 style="text-decoration: underline;">Home Assistant MQTT Discovery</h2>
    This will automatically create a device and desired entities automatically in Home Assistant.<br>
    <i><font style="color:red">Assure MQTT is properly configured in Home Assistant.  See the documentation for other prerequisites and more info.</font></i><br><br>
    <form id="frmdiscover" action="/savediscovery" method="post">
      <table border="0">
        <tr>
          <td>Device Name:</td>
          <td><input type="text" id="devicename" name="devicename" maxlength="32" pattern="[a-zA-Z0-9\s]+" title="Up to 32 Alphanumeric characters and spaces"></td>
        </tr>
      </table>
      This device name can be the same or different than the device name used when setting up the system. The value entered here will be the device name in Home Assistant.<br>
      It will also be prepended to all entity names after converting to lowercase and replacing spaces with underscores (e.g. Bedside Lamp = 'light.bedside_lamp_ledstrip').<br><br>
      <font style="color: #126e22">Select the entities you wish to include in Home Assistant.  Omitted entities can still be used via YAML/MQTT.</font><br><br>
      <table border="0">
        <tr>
          <td style="vertical-align: top;"><input type="checkbox" id="bulb" name="bulb" value="bulb" style="vertical-align: top;"></td>
          <td style="vertical-align: top;"><label for="bulb">RGBW Light Bulb</label><br>
              &ensp;- Bulb State<br>
              &ensp;- Bulb Brightness<br>
              &ensp;- Bulb Color<br>
              &ensp;- Bulb Temperature (white)
          </td>  
          <td style="vertical-align: top;"><input type="checkbox" id="led" name="led" value="led"></td>
          <td style="vertical-align: top;"><label for="led">LED Strip</label><br>
              &ensp;- LED State<br>
              &ensp;- LED Brightness<br>
              &ensp;- LED Color
          </td>
          <td style="vertical-align: top;"><input type="checkbox" id="display" name="display" value="display"></td>
          <td style="vertical-align: top;"><label for="display">TFT Display</label><br>
              &ensp;- Brightness<br>
              &ensp;- Auto-Dimming<br>
              &ensp;- Clock Color
          </td>
        </tr>
        <tr>
          <td style="vertical-align: top;"><input type="checkbox" id="touch" name="touch" value="touch"></td>
          <td style="vertical-align: top;"><label for="touch">Touch Sensors</label><br>
              &ensp;- Touch State<br>
              &ensp;- Main function<br>
              &ensp;- Alarm (alt) function
          </td>  
          <td style="vertical-align: top;"><input type="checkbox" id="alarms" name="alarms" value="alarms"></td>
          <td style="vertical-align: top;"><label for="alarms">Alarms</label><br>
              &ensp;- Volume<br>
              &ensp;- Gentle Wake<br>
              &ensp;- Alarm Track (sound)<br>
              &ensp;- Snooze Time<br>
              &ensp;- Start/Stop<br>
              &ensp;- Play Sound
          </td>
          <td style="vertical-align: top;"><input type="checkbox" id="diagnostic" name="diagnostic" value="diagnostic"></td>
          <td style="vertical-align: top;"><label for="Diagnostic">Diagnostics</label><br>
              &ensp;- Controller IP addresses (3)<br>
              &ensp;- Controller Reboot Buttons (2)<br>
              &ensp;- MAC addresses & Firmware versions (disable by default)
          </td>
        </tr>
      <table><br>
      <input type="hidden" id="discaction" name="discaction" value="">
      <button type="submit" id="btneanble" name="btnenable" value="save" title="Enable or update Discovery"
        style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 45px; background-color:#99f2a9;"
        >Enable Discovery
      </button>&ensp;
      <button type="submit" id="btndisable" name="btndisable" value="delete" title="Disable Discovery and remove device"
        style="text-align: center; font-size: 16px; border-radius: 10px; width: 128px; height: 45px; background-color:#f2a2a5;"
        >Disable Discovery
      </button>&ensp;
    </form><br><br>
    <form id="frmvoice" action="/default-action" method="post">
    </form>
    <br>
    <h2 style="text-decoration: underline;">Voice Setup and Options</h2>
    <i>Coming soon!</i><br>
    <form id="frmvoice" action="/default-action" method="post">
    </form><br>
    <script>
      function fetchData(whichData) {
        fetch("/jsonadvanced")
          .then(response => response.json())
          .then(data => {
            if ((whichData == "all") || (whichData == "touch")) {
              const chkTouch1Enabled = document.getElementById("touch1enable");
              const chkTouch2Enabled = document.getElementById("touch2enable");
              const touch1Duration = document.getElementById("touch1duration");
              const touch1Function1 = document.getElementById("touch1control1");
              const touch1Function2 = document.getElementById("touch1control2");
              const touch2Duration = document.getElementById("touch2duration");
              const touch2Function1 = document.getElementById("touch2control1");
              const touch2Function2 = document.getElementById("touch2control2");
              if (data.touch1_enabled == 0) {
                chkTouch1Enabled.checked = false;
                document.getElementById("touch1enableval").value = 0;
                //'Disable' other fields
                touch1Duration.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
                touch1control1.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
                touch1control2.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
              } else {
                chkTouch1Enabled.checked = true;
                document.getElementById("touch1enableval").value = 1;
                //'Enable' fields
                touch1Duration.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
                touch1control1.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
                touch1control2.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
              }
              if (data.touch2_enabled == 0) {
                chkTouch2Enabled.checked = false;
                document.getElementById("touch2enableval").value = 0;
                touch2Duration.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
                touch2control1.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
                touch2control2.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
              } else {
                  chkTouch2Enabled.checked = true;
                  document.getElementById("touch2enableval").value = 1;
                  touch2Duration.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
                  touch2control1.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
                  touch2control2.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
              }
              //Set values
              touch1Duration.value = (data.touch1_duration);
              touch1control1.value = (data.touch1_control_1).toString();
              touch1control2.value = (data.touch1_control_2).toString();
              touch2Duration.value = (data.touch2_duration);
              touch2control1.value = (data.touch2_control_1).toString();
              touch2control2.value = (data.touch2_control_2).toString();
            }
            if ((whichData == "all") || (whichData == "discovery")) {

              document.getElementById("devicename").value = data.disc_devname;            
              document.getElementById("bulb").checked = data.disc_bulb;
              document.getElementById("led").checked = data.disc_led;
              document.getElementById("display").checked = data.disc_disp;
              document.getElementById("touch").checked = data.disc_touch;
              document.getElementById("alarms").checked = data.disc_alarms;
              document.getElementById("diagnostic").checked = data.disc_diag;
              if (data.disc_exists) {
                document.getElementById("devicename").disabled = true;
                document.getElementById("btneanble").innerHTML = "Update Discovery";
                document.getElementById("btndisable").innerHTML = "Remove Discovery";
              }
            }  
          })
          .catch(error => console.error("Error fetching data:", error));
      }
      function toggleTouch1Controls() {
        const touch1 = document.getElementById("touch1enable");
        const touch1val = document.getElementById("touch1enableval");
        const touch1Duration = document.getElementById("touch1duration");
        const touch1Function1 = document.getElementById("touch1control1");
        const touch1Function2 = document.getElementById("touch1control2");
        if (touch1.checked) {
          touch1Function1.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
          touch1Function2.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
          touch1Duration.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
          touch1val.value = 1;
        } else {
          touch1Function1.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
          touch1Function2.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
          touch1Duration.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
          touch1val.value = 0;
        }
      }
      function toggleTouch2Controls() {
        const touch2=document.getElementById("touch2enable");
        const touch2val = document.getElementById("touch2enableval");
        const touch2Duration = document.getElementById("touch2duration");
        const touch2Function1 = document.getElementById("touch2control1");
        const touch2Function2 = document.getElementById("touch2control2");
        if (touch2.checked) {
          touch2Function1.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
          touch2Function2.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
          touch2Duration.style = "pointer-events: auto; background-color: #ffffff; color: #000000;";
          touch2val.value = 1;
        } else {
          touch2Function1.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
          touch2Function2.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
          touch2Duration.style = "pointer-events: none; background-color: #e9ecef; color: #6c757d;";
          touch2val.value = 0;
        }
      }
      function saveTouch() {
        const frmTouch = document.getElementById("frmtouch");
        frmTouch.action = "/savetouch";
        frmTouch.submit();
      }
      document.addEventListener("DOMContentLoaded", fetchData("all"));
      document.getElementById("frmdiscover").addEventListener("submit", function(e) {
        const act = e.submitter.value;
        document.getElementById("discaction").value = act;
        const msg = (act === "save") ? "This will IMMEDIATELY add or update the device in Home Assistant. Continue?" : "This will IMMEDIATELY remove this device from Home Assitant. Continue?";
        if (act === "save") {
          document.getElementById("devicename").disabled = false;
        }
        if (!confirm(msg)) e.preventDefault();
      });
    </script>  
)literal";

const char *postTouchSettings = R"literal(
  <!-- -----------------------------
        TOUCH DEFAULTS UPDATED PAGE 
       ----------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Touch Settings Submitted</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* Touch Settings Updated</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <h2 id="subheading" style="color: #0d4d08;">Touch settings saved to configuration file.</h2><br>
    Any integrated lights will be set to off and the device will reboot.  After boot completes, you may:<br><br>
    <button type="button" id="btnlights" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*/advanced'">Return to Touch Settings</button><br><br>
    <button type="button" id="btnmain" style="text-align: center; font-size: 16px; border-radius: 8px; width: 160px; height: 40px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">Return to Main Page</button><br><br>
    <i>Do not use the browser back button as the page may not reflect most recent data.</i><br>
)literal";

const char *postDiscoveryAdd = R"literal(
  <!-- -------------------------
        DISCOVERY ENABLED PAGE 
       ------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Discovery Settings Submitted</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* Discovery Enabled</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <button type="button" id="btnmain" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 160px; height: 35px;"
      onclick="location.href = '/';"><< Return to Main</button><br><br>       
    <button type="button" id="btnadv" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 160px; height: 35px;"
      onclick="location.href = '/advanced';"><< Return to Advanced</button><br>
    <h2>Discovery Messages sent to Home Assistant!</h2><br>
    If this is your initial discovery, the device should be found in Home Assistant at:<br>
    Settings&xrarr;Devices & Services&xrarr;Integrations&xrarr;MQTT<br>
)literal";  

const char *postDiscoveryRemove = R"literal(
  <!-- -------------------------
        DISCOVERY DISABLED PAGE 
       ------------------------- -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Discovery Removal Submitted</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1 id="topheading" style="color: #1e5c14;">*VAR_APP_NAME* Discovery Disabled</h1>
    Firmware Version: *VAR_CURRENT_VER*<br>
    Device Name: *DEVICENAME*<br><br>
    <button type="button" id="btnmain" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 160px; height: 35px;"
      onclick="location.href = '/';"><< Return to Main</button><br><br>       
    <button type="button" id="btnadv" style="text-align: center; background-color:#ADC6C7; font-size: 14px; border-radius: 8px; width: 160px; height: 35px;"
      onclick="location.href = '/advanced';"><< Return to Advanced</button><br>
    <h2>Discovery REMOVAL Messages sent to Home Assistant!</h2><br>
    The device and all entites should have been removed from your Home Assistant<br><br>
    <i><font color="red">Note: If you renamed any entities in Home Assistant after the last Discovery, these may not have been automatically removed.&nbsp;
    You may need to locate and manually remove these using the Home Assistant entities page.</font></i><br>
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
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <h1>*VAR_APP_NAME* Firmware Update</h1><br>
    <b>IMPORTANT:</b> Please read the release notes for the firmware before upgrading!<br>
    Some upgrades may require onboarding and configuring your device again.<br><br>
    <button type="button" id="btnback" style="font-size: 16px; border-radius: 8px; width: 100px; height: 30px;" onclick="location.href = './'"><< Back</button>
    <br><br>
    <b>Current Firmware Version:</b> *VAR_CURRENT_VER* <br><br>
    <span style="color: red; font-weight: bold;">Verify that file selected is firmware for the <u>PRIMARY controller</u></span><br><br>
    <form method='POST' enctype='multipart/form-data' id='upload-form'>
      <table style="border: 1px solid black;"><tr>
      <td><input type='file' id='file' name='update'></td>
      <td><input type='submit' value='Update'></td>
      </tr></table>
    </form>
    <br>
    <table style="width:25%">
    <tr><td>
    <div id='prg' style='width:0;color:#cccccc;text-align:center'>0%</div>
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
      <li>Check to see if the *VAR_APP_NAME* hotspot is broadcasting (you may need to onboard again)</li>
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
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
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
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>Primary Controller restarting...</H1><br>
    <table border=1 cellpadding="5">
      <tr><td><b>Device Name:</b></td><td>*DEVICENAME*</td></tr>
      <tr><td><b>Firmware Ver:</b></td><td>*VAR_CURRENT_VER*</td></tr>
    </table><br>
    <H3>Please wait</H3><br>
    After the controller completes the boot process, you may click the following link to return to the main page:<br><br>
    <a href="http://*IPADDR*">Return to settings</a><br>
)literal";

const char *restartall = R"literal(
  <!-- =============================
        REBOOT ALL CONTROLLERS PAGE
       ============================= -->
  <!DOCTYPE html>
  <html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>*DEVICENAME* - Reboot</title>
    <style>
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>Restarting All Controllers</H1><br>
    <table border=1 cellpadding="5">
      <tr><td><b>Device Name:</b></td><td>*DEVICENAME*</td></tr>
      <tr><td><b>Firmware Ver:</b></td><td>*VAR_CURRENT_VER*</td></tr>
    </table><br><br>
    <b>Issuing restart commands:</b><br>
    <table border=0>
      <tr>
        <td>Light Bulb Restart...&emsp;&emsp;</td>
        <td>*BULBRESULT*</td>
      </tr><tr>
        <td>Display Controller Restart...&emsp;&emsp;</td>
        <td>*DISPRESULT*</td>
      </tr><tr>
        <td>Primary Controller Restart...&emsp;&emsp;</td>
        <td><font color=#13400f><b>OK</b></font></td>
      </tr>
    </table><br>
    <H3>Please wait...</H3><br>
    After the primary controller completes the boot process, you may click the following link to return to the main page:<br><br>
    <a href="http://*IPADDR*">Return to settings</a><br>
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
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
    <H1>*VAR_APP_NAME* Configuration File</H1>
    The following is the raw JSON configuration file loaded <b><u>at last boot</u></b>.<br>
    It does <i>not</i> list current settings or any default settings changed but not yet saved.<br><br>
    <button type="button" id="btncancel" style="text-align: center; font-size: 16px; border-radius: 8px; width: 100px; height: 30px; background-color:#bbbbbb;"
               onclick="document.location='http://*IPADDR*'">&lt;&lt; Back</button><br><br>
    Config File:<br>
    <textarea id="filedump" rows="65" cols="80">*CONFIGJSON*</textarea><br><br>
    Discovery File (only present if Discovery enabled):<br>
    <textarea id="discdump" rows="15" cols="80">*DISCJSON*</textarea>
  </body>
  </html>
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
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #0000ff; }
    </style>
  </head>
  <body>
      <H1>Controller Resetting...</H1><br>
      <H3>After this process is complete, you <b>must</b> setup your controller again:</H3>
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

