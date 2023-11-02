#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";
const char* timeZoneAPI = "http://worldtimeapi.org/api/timezone/America/Cancun";
const int bellPin = 2;  // Change to the GPIO pin connected to the LED

void setup() {
    pinMode(bellPin, OUTPUT);
    connectToWiFi();
}

void loop() {
    // Check the current time and ring the bell if it matches your schedule
    if (shouldRingBell()) {
        digitalWrite(bellPin, HIGH);
        delay(1000);  // Ring for 1 second
        digitalWrite(bellPin, LOW);
    }

    // Fetch timezone information every minute
    delay(60000);  // 60,000 milliseconds = 1 minute
}

bool shouldRingBell() {
    HTTPClient http;
    http.begin(timeZoneAPI);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        const char* tz = doc["timezone"];
        const char* datetime = doc["datetime"];

        // Check the timezone and datetime to determine if the bell should ring
        if (strcmp(tz, "America/Cancun") == 0) {
            // Parse the datetime string
            int year, month, day, hour, minute, second;
            float millisecond;
            int offsetHours, offsetMinutes;

            if (sscanf(datetime, "%d-%d-%dT%d:%d:%d.%f-%d:%d", &year, &month, &day, &hour, &minute, &second, &millisecond, &offsetHours, &offsetMinutes) == 9) {
                // Compare the components to decide if it's time to ring the bell
                if ((hour == 8 && minute == 45) || (hour == 9 && minute == 45)) {
                    return true;
                }

                if ((day >= 1 && day <= 3 && hour == 15 && minute == 45) ||
                    (day == 4 && hour == 16 && minute == 45)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}
