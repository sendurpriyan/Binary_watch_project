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

String ssid;
String password;
String current_password;

int red = 255;
int green = 0;
int blue = 0;

RTC_DATA_ATTR bool hasBeenReprogrammed = false; // Stored in RTC memory

time_t now;
struct tm timeinfo;

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.clear();
  FastLED.show();

  preferences.begin("wifi-config", false);

    // Check if the ESP32 was reprogrammed
  if (!hasBeenReprogrammed) {
    Serial.println("Device reprogrammed, resetting password.");
    preferences.putString("password", default_password); // Reset the password
    hasBeenReprogrammed = true; // Set flag to indicate that the ESP32 was reprogrammed
  } else {
    Serial.println("Device not reprogrammed, keeping saved password.");
  }
  
  // Load the password from preferences
  current_password = preferences.getString("password", default_password);

  red = preferences.getUInt("red", 255);     // Get stored red value
  green = preferences.getUInt("green", 255); // Get stored green value
  blue = preferences.getUInt("blue", 255);   // Get stored blue value

  WiFi.softAP(default_ssid, current_password.c_str());
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  configTime(0, 0, "pool.ntp.org");
  setenv("TZ", "UTC0", 1);
  tzset();

  // Wait for time to synchronize
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html;
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
            <input type="text" id="new_password" name="new_password" required>
            <button type="submit">Change Password</button>
          </form>
        </body>
        </html>
        )rawliteral";
    } else {
        html = R"rawliteral(
        <html>
        <head>
          <title>ESP32 Clock</title>
          <style>
            body {
              font-family: Arial, sans-serif;
              margin: 0;
              padding: 20px;
              background-color: #f4f4f4;
              display: flex;
              flex-direction: column;
              align-items: center;
            }
            h1 {
              color: #333;
            }
            .container {
              max-width: 600px;
              width: 100%;
              background: #fff;
              border-radius: 5px;
              padding: 20px;
              box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
            }
            label {
              margin-top: 10px;
              color: #555;
            }
            input[type=range], input[type=time] {
              width: 100%;
              margin-top: 10px;
            }
            .color-preview {
              width: 100px;
              height: 100px;
              margin-top: 20px;
              border: 1px solid #000;
              display: inline-block;
            }
            .flex-row {
              display: flex;
              justify-content: space-between;
              width: 100%;
            }
            button {
              margin-top: 20px;
              padding: 10px 15px;
              border: none;
              background-color: #007BFF;
              color: white;
              border-radius: 5px;
              cursor: pointer;
            }
            button:hover {
              background-color: #0056b3;
            }
          </style>
          <script>
            function updateTime() {
              var xhr = new XMLHttpRequest();
              var time = document.getElementById('time').value;
              var red = document.getElementById('red').value;
              var green = document.getElementById('green').value;
              var blue = document.getElementById('blue').value;
              xhr.open('GET', '/update?time=' + time + '&r=' + red + '&g=' + green + '&b=' + blue, true);
              xhr.send();
            }

            function fetchTime() {
              var xhr = new XMLHttpRequest();
              xhr.open('GET', '/gettime', true);
              xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                  document.getElementById('currentTime').innerHTML = xhr.responseText;
                }
              };
              xhr.send();
            }

            function updateColorPreview() {
              var red = document.getElementById('red').value;
              var green = document.getElementById('green').value;
              var blue = document.getElementById('blue').value;
              document.getElementById('colorPreview').style.backgroundColor = 'rgb(' + red + ',' + green + ',' + blue + ')';
            }

            setInterval(fetchTime, 1000);  // Update time every second
          </script>
        </head>
        <body>
          <div class="container">
            <h1>ESP32 Binary Clock</h1>
            <p>Current Time: <span id="currentTime"></span></p>

            <label for="time">Set Time (HH:MM):</label>
            <input type="time" id="time" name="time"><br><br>

            <h3>Set LED Color (RGB):</h3>

            <div class="flex-row">
              <label for="red">Red:</label>
              <input type="range" id="red" name="red" min="0" max="255" value=red oninput="updateColorPreview()">
            </div>
            
            <div class="flex-row">
              <label for="green">Green:</label>
              <input type="range" id="green" name="green" min="0" max="255" value=green oninput="updateColorPreview()">
            </div>
            
            <div class="flex-row">
              <label for="blue">Blue:</label>
              <input type="range" id="blue" name="blue" min="0" max="255" value=blue oninput="updateColorPreview()">
            </div>

            <div id="colorPreview" class="color-preview"></div>

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
      current_password = new_password;
      preferences.putString("password", current_password);
      request->send(200, "text/plain", "Password changed successfully! Refresh to apply changes.");
    } else {
      request->send(400, "text/plain", "Missing password parameter!");
    }
  });

 // Route to handle time and RGB color update
server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
  String newTime; 
  
  // Handle time update
  if (request->hasParam("time")) {
    newTime = request->getParam("time")->value();
    setTimeFromWeb(newTime);
  }

  // Handle RGB color update
  if (request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
    red = request->getParam("r")->value().toInt();
    green = request->getParam("g")->value().toInt();
    blue = request->getParam("b")->value().toInt();
    
    // Store RGB color in preferences
    preferences.putUInt("red", red);
    preferences.putUInt("green", green);
    preferences.putUInt("blue", blue);

    // Update LEDs with the new RGB color
    updateLEDsWithColor(red, green, blue);
  }

  request->send(200, "text/plain", "Time and Color Updated");
});


  server.on("/gettime", HTTP_GET, [](AsyncWebServerRequest *request) {
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    request->send(200, "text/plain", timeStr);
  });

  server.begin();
}

void loop() {
  time(&now);
  localtime_r(&now, &timeinfo);

  // Print individual binary representations of each digit
  int hours = timeinfo.tm_hour;
  int minutes = timeinfo.tm_min;

  // Convert time to BCD
  uint16_t bcdTime = convertTimeToBCD(hours, minutes);
  
  Serial.print("Current Time: ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.println("");
  Serial.print("BCD Time: ");
  printBCDTime(bcdTime);
  Serial.print(red);
  Serial.print(",");
  Serial.print(green);
  Serial.print(",");
  Serial.println(blue);

  // Update the WS2812B LEDs with the current time
  updateLEDs(bcdTime, red, green, blue);
  
  delay(1000);
}

uint16_t convertTimeToBCD(int hours, int minutes) {
  // Extract individual digits
  int hoursTens = hours / 10;    // Tens place of hours
  int hoursOnes = hours % 10;    // Ones place of hours
  int minutesTens = minutes / 10; // Tens place of minutes
  int minutesOnes = minutes % 10; // Ones place of minutes

  // Convert each digit to BCD (4 bits each)
  uint16_t bcdTime = (hoursTens << 12) | (hoursOnes << 8) | (minutesTens << 4) | minutesOnes;

  return bcdTime;
}

void updateLEDs(uint16_t bcdTime, int red, int green, int blue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (bcdTime & (1 << (NUM_LEDS - 1 - i))) {
      leds[i] = CRGB(red, green, blue);  // Set LED color to RGB
    } else {
      leds[i] = CRGB::Black;  // Turn off the LED
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

void setTimeFromWeb(String newTime) {
  int hour = newTime.substring(0, 2).toInt();
  int minute = newTime.substring(3, 5).toInt();

  struct tm t;
  time(&now);
  localtime_r(&now, &t);
  
  t.tm_hour = hour;
  t.tm_min = minute;
  time_t updatedTime = mktime(&t);
  timeval tv = { updatedTime, 0 };
  settimeofday(&tv, nullptr);
}

// Function to print BCD time in binary to Serial Monitor
void printBCDTime(uint16_t bcdTime) {
  for (int i = 15; i >= 0; i--) {
    Serial.print((bcdTime >> i) & 1);
  }
  Serial.println();
}

void resetPasswordOnReprogram() {
  // This function resets the password only after reprogramming
  hasBeenReprogrammed = false; // Reset the RTC flag to trigger a password reset on reprogram
}