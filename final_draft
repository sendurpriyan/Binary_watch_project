#include <FastLED.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <time.h>
#include <esp_system.h>

#define LED_PIN 4
#define NUM_LEDS 16

CRGB leds[NUM_LEDS];
AsyncWebServer server(80);
Preferences preferences;

const char *default_ssid = "ESP32_Hotspot";
const char *default_password = "12345678";

struct RTCData {
  int red;
  int green;
  int blue;
  int t_format;
  struct tm timeinfo;  // To store the current time
};

RTC_DATA_ATTR RTCData rtcData;  // Use RTC_DATA_ATTR to retain data across resets

String ssid;
String password;
String current_password;

int red = 255;
int green = 0;
int blue = 0;

bool hasBeenReprogrammed;

time_t now;
struct tm timeinfo;

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();
  FastLED.show();

  preferences.begin("wifi-config", false);

  // Check if there's a saved time in preferences
  int savedHour = preferences.getInt("savedHour", -1);  // Default is -1 if not set
  int savedMinute = preferences.getInt("savedMinute", -1);
  int t_for = preferences.getUInt("t_for", 24);

  if (savedHour != -1 && savedMinute != -1) {
    rtcData.timeinfo.tm_hour = savedHour;
    rtcData.timeinfo.tm_min = savedMinute;
    rtcData.timeinfo.tm_sec = 0;

    time_t now = mktime(&rtcData.timeinfo);
    timeval tv = { now, 0 };
    settimeofday(&tv, nullptr);

  } else {
    configTime(0, 0, "pool.ntp.org");
  }

  // Load "hasReprogrammed" status and password from preferences
  hasBeenReprogrammed = preferences.getBool("hasReprogrammed", false);
  current_password = preferences.getString("password", default_password);

  // If the device has not been reprogrammed, reset the password to default
  if (!hasBeenReprogrammed) {
    preferences.putString("password", default_password);
    preferences.putBool("hasReprogrammed", true);  // Mark the device as reprogrammed
    current_password = default_password;  // Set the current password to default
  }

  // Initialize the RTC data
  rtcData.red = preferences.getUInt("red", 255);     // Get stored red value
  rtcData.green = preferences.getUInt("green", 0);   // Get stored green value
  rtcData.blue = preferences.getUInt("blue", 0);     // Get stored blue value

  WiFi.softAP(default_ssid, current_password.c_str()); // logging into device
  IPAddress IP = WiFi.softAPIP();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html;
    //only run on first bootup of the device
    if (current_password == default_password) {
        html = R"rawliteral(
        <html>
        <head>
          <title>Change Password</title>
          <style>
            body {
              font-family: Arial, sans-serif;
              background-color: #f4f4f4;
              margin: 0;
              padding: 20px;
              text-align: center;
            }
            h1 {
              color: #333;
            }
            form {
              background: #fff;
              padding: 20px;
              border-radius: 5px;
              box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
            }
          </style>
        </head>
        <body>
          <h1>Change Password</h1>
          <form action="/change_password" method="GET">
            <label for="new_password">New Password:</label>
            <input type="password" id="new_password" name="new_password" required>
            <button type="submit">Change Password</button>
          </form>
        </body>
        </html>
        )rawliteral";
    } else {
      html = R"rawliteral(
      <html>
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 Time Sync</title>
        <style>
          body {
            font-family: 'Roboto', sans-serif;
            margin: 0;
            padding: 0;
            background: linear-gradient(135deg, #6B73FF 0%, #000DFF 100%);
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: #fff;
          }
          h1 {
            text-align: center;
            color: #000000;
            margin-bottom: 20px;
          }

          #timeFormatSelect {
                  width: 200px;
                  padding: 5px;
                  font-size: 16px;
              }

          .container {
            max-width: 600px;
            width: 100%;
            background: #f4f4f4;
            border-radius: 10px;
            padding: 30px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);
            text-align: center;
            color: #333;
            position: relative;
          }

          .password-reset {
            margin-bottom: 20px;
            display: flex;
            align-items: center;
          }

          .password-reset input {
            flex: 1;
            padding: 10px;
            margin-right: 10px;
            margin-top: 20px;
            border-radius: 5px;
            border: 1px solid #ddd;
            box-sizing: border-box;
            font-size: 1rem;
            height: 40px; /* Set a fixed height for the input */
          }

          .password-reset button {
            background: #ff4757;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 0.9rem;
            height: 40px; /* Set the same height for the button */
            transition: background 0.3s ease;
          }

          .password-reset button:hover {
            background-color: #e84118;
          }

          label {
            display: block;
            margin-top: 20px;
            font-weight: bold;
            color: #333;
          }

          input[type=range], input[type=time] {
            width: 100%;
            margin-top: 10px;
            padding: 10px;
            border-radius: 5px;
            border: 1px solid #ddd;
            box-sizing: border-box;
            font-size: 1rem;
          }

          .color-preview {
            width: 100px;
            height: 100px;
            margin-top: 20px;
            border: 3px solid #ddd;
            border-radius: 50%;
            transition: background-color 0.3s ease;
          }

          .flex-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            width: 100%;
            margin-top: 10px;
          }

          button {
            width: 100%;
            margin-top: 20px;
            padding: 15px;
            background: #007BFF;
            color: #fff;
            border: none;
            border-radius: 10px;
            font-size: 1rem;
            cursor: pointer;
            transition: background 0.3s ease;
          }

          button:hover {
            background-color: #0056b3;
          }

          p {
            margin: 20px 0;
            font-size: 1.2rem;
            color: #333;
          }

          .current-time {
            font-size: 1.5rem;
            margin-bottom: 20px;
            color: #333;
            font-weight: bold;
          }

          /* Modal Styles */
          .modal-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: rgba(0, 0, 0, 0.5);
            display: none;
            justify-content: center;
            align-items: center;
          }

          .modal {
            background: #fff;
            padding: 20px;
            border-radius: 10px;
            width: 90%;
            max-width: 400px;
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.2);
            z-index: 2;
          }

          .modal label {
            color: #333;
          }

          .modal button {
            background: #ff4757;
            border: none;
            padding: 10px 15px;
            color: white;
            border-radius: 5px;
            cursor: pointer;
            margin-top: 10px;
          }

          .modal button:hover {
            background-color: #e84118;
          }

          .blurred {
            filter: blur(5px);
          }

          @media (max-width: 600px) {
            .container {
              padding: 20px;
            }
            input[type=range] {
              width: 80%;
            }
            .flex-row {
              flex-direction: column;
              align-items: flex-start;
            }
          }
        </style>
        <script>
          // Sync device time with system time
          function syncDeviceTime() {
            var currentDate = new Date();
            var hours = currentDate.getHours().toString().padStart(2, '0');
            var minutes = currentDate.getMinutes().toString().padStart(2, '0');
            var time = hours + ":" + minutes;
            document.getElementById('time').value = time;
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/update?time=' + time, true);
            xhr.send();
          }

          // Update time and color
          function updateTime() {
            var xhr = new XMLHttpRequest();
            var time = document.getElementById('time').value;
            var red = document.getElementById('red').value;
            var green = document.getElementById('green').value;
            var blue = document.getElementById('blue').value;
            var timeFormat = document.getElementById('timeFormatSelect').value;
            xhr.open('GET', '/update?time=' + time + '&r=' + red + '&g=' + green + '&b=' + blue + '&t_format=' + timeFormat, true);
            xhr.send();
          }

          // Update brightness
          function updateBrightness() {
            var brightness = document.getElementById('brightness').value;
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/update_brightness?brightness=' + brightness, true);
            xhr.send();
          }


          // Preview LED color based on RGB values
          function updateColorPreview() {
            var red = document.getElementById('red').value;
            var green = document.getElementById('green').value;
            var blue = document.getElementById('blue').value;
            document.getElementById('colorPreview').style.backgroundColor = 'rgb(' + red + ',' + green + ',' + blue + ')';
          }

          // Real-time clock sync
          setInterval(function() {
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/gettime', true);
            xhr.onreadystatechange = function() {
              if (xhr.readyState == 4 && xhr.status == 200) {
                document.getElementById('currentTime').innerHTML = xhr.responseText;
              }
            };
            xhr.send();
          }, 1000);

        </script>
      </head>
      <body>
        <div class="container">
          <h1>ESP32 Binary Clock</h1>
          <form action="/change_password" method="GET">
            <div class="password-reset">
              <input type="password" id="new_password" name="new_password" placeholder="Enter Current Password" required>
              <button type="submit">Change Password</button>
            </div>
          </form>

          <div class="current-time">
            Current Time: <span id="currentTime">00:00</span>
          </div>

          <h3>Set Time</h3>
          <button onclick="syncDeviceTime()">Sync with Device Time</button><br><br>
          <label for="time">Set Time Manually (HH:MM):</label>
          <input type="time" id="time" name="time">

          <h2>Select Time Format</h2>
          <label for="timeFormatSelect">Choose Time Format:</label>
          <select id="timeFormatSelect">
            <option value="24">24-hour</option>
            <option value="12">12-hour</option>
          </select>

          <h3>Set LED Color (RGB)</h3>
          <div class="flex-row">
            <label for="red">Red:</label>
            <input type="range" id="red" name="red" min="0" max="255" oninput="updateColorPreview()">
          </div>
          <div class="flex-row">
            <label for="green">Green:</label>
            <input type="range" id="green" name="green" min="0" max="255" oninput="updateColorPreview()">
          </div>
          <div class="flex-row">
            <label for="blue">Blue:</label>
            <input type="range" id="blue" name="blue" min="0" max="255" oninput="updateColorPreview()">
          </div>

          <div id="colorPreview" class="color-preview"></div>

          <h3>Adjust LED Brightness</h3>
          <div class="flex-row">
            <label for="brightness">Brightness:</label>
            <input type="range" id="brightness" name="brightness" min="0" max="255" value="255" oninput="updateBrightness()">
          </div>

          <button onclick="updateTime()">Update Time and Color</button>
        </div>
      </body>
      </html>
      )rawliteral";

    }
    request->send(200, "text/html", html);
  });

  server.on("/change_password", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("new_password")) {
      String new_password = request->getParam("new_password")->value();
    
      // Update the current password and store it in preferences
      current_password = new_password;
      preferences.putString("password", current_password);
      preferences.putBool("hasReprogrammed", true);  // Mark as reprogrammed to avoid resetting on reboot
      preferences.end();  // Ensure changes are written to NVS
      
      // Restart Wi-Fi AP with the new password
      WiFi.softAP(default_ssid, current_password.c_str());
      String response = "<html><head><script>window.location.href='http://192.168.4.1/';</script></head><body></body></html>";
      request->send(200, "text/html", response);
      
    } else {
      request->send(400, "text/plain", "Missing password parameter!");
    }
  });



  // Route to handle time and RGB color update
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("time")) {
      String timeStr = request->getParam("time")->value();
      int hour = timeStr.substring(0, 2).toInt();
      int minute = timeStr.substring(3, 5).toInt();

      if (request->hasParam("t_format")) {
        String t_format = request->getParam("t_format")->value();
        rtcData.t_format = t_format.toInt();

        if (rtcData.t_format == 12 && hour > 12) {
          hour -= 12;
        }

        if (rtcData.t_format == 12 && hour == 0) {
          hour = 12;
        }
      }
      rtcData.timeinfo.tm_hour = hour;
      rtcData.timeinfo.tm_min = minute;
      rtcData.timeinfo.tm_sec = 0;

      struct tm t;
      time(&now);
      localtime_r(&now, &t);

      time_t now = mktime(&rtcData.timeinfo);
      timeval tv = { now, 0 };
      settimeofday(&tv, nullptr);

      preferences.putInt("savedHour", rtcData.timeinfo.tm_hour);
      preferences.putInt("savedMinute", rtcData.timeinfo.tm_min);
      preferences.putUInt("t_for", rtcData.t_format);
    }

    // Handle RGB color update
    if (request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
      red = request->getParam("r")->value().toInt();
      green = request->getParam("g")->value().toInt();
      blue = request->getParam("b")->value().toInt();
      
      rtcData.red = red;
      rtcData.green = green;
      rtcData.blue = blue;
      
      // Store RGB color in preferences
      preferences.putUInt("red", rtcData.red);
      preferences.putUInt("green", rtcData.green);
      preferences.putUInt("blue", rtcData.blue);

      // Update LEDs with the new RGB color
      updateLEDsWithColor(rtcData.red, rtcData.green, rtcData.blue);
    }

    request->send(200, "text/plain", "Time and Color Updated");
  });

  server.on("/update_brightness", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasParam("brightness")) {
    int brightness = request->getParam("brightness")->value().toInt();
    
    // Update FastLED brightness
    FastLED.setBrightness(brightness);
    
    // Optionally store the brightness in preferences if you want to retain it across resets
    preferences.putInt("brightness", brightness);
    
    request->send(200, "text/plain", "Brightness Updated");
  } else {
    request->send(400, "text/plain", "Missing brightness parameter");
  }
  });

  server.on("/gettime", HTTP_GET, [](AsyncWebServerRequest *request) {
    getLocalTime(&timeinfo);
    char timeStr[6];
    snprintf(timeStr,sizeof(timeStr), "%02d:%02d", rtcData.timeinfo.tm_hour, rtcData.timeinfo.tm_min);
    request->send(200, "text/plain", String(timeStr));
  });

  server.begin();
}

void loop() {
  time(&now);
  localtime_r(&now, &timeinfo);

  // Print individual binary representations of each digit
  int hours = rtcData.timeinfo.tm_hour;
  int minutes = rtcData.timeinfo.tm_min;

  // Convert time to BCD
  uint16_t bcdTime = convertTimeToBCD(hours, minutes);

  // Update the WS2812B LEDs with the current time
  updateLEDs(bcdTime, rtcData.red, rtcData.green, rtcData.blue);
  delay(1000);
}

uint16_t reverseBits(uint8_t value) {
  uint8_t reversed = 0;
  for (int i = 0; i < 4; i++) {
    reversed |= ((value >> i) & 1) << (3 - i);  // Reverse the bits
  }
  return reversed;
}


uint16_t convertTimeToBCD(int hours, int minutes) {
  // Extract individual digits
  int hoursTens = hours / 10;    // Tens place of hours
  int hoursOnes = hours % 10;    // Ones place of hours
  int minutesTens = minutes / 10; // Tens place of minutes
  int minutesOnes = minutes % 10; // Ones place of minutes

  // Reverse the second 4 bits (minutesTens)
  hoursOnes = reverseBits(hoursOnes);
  minutesOnes = reverseBits(minutesOnes);

  // Convert each digit to BCD (4 bits each)
  uint16_t bcdTime = (hoursTens << 12) | (hoursOnes << 8) | (minutesTens << 4) | minutesOnes;

  return bcdTime;
}

void updateLEDs(uint16_t bcdTime, int red, int green, int blue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (bcdTime & (1 << (NUM_LEDS - 1 - i))) {
      leds[i] = CRGB(red, green, blue);  // Set LED color to RGB
    } else {
      leds[i] = CRGB::Black;  // Turn off the LED, we can set default LED colour here
    }
  }
  FastLED.show();
}

// Function to update LEDs immediately after color change
void updateLEDsWithColor(int red, int green, int blue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(red, green, blue);  // Set all LEDs to the selected RGB color
  }
  FastLED.show();
}
