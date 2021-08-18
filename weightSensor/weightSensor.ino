#include "HX711.h"
#include "creds.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>

#define POLLING_INTERVAL 600000 // In milliseconds
#define DOUT 18
#define CLK 19

HX711 scale;

float calibration_factor = -7050;

// Use 0 for testing to make it easy to remove from the database
int sensor_id = 0;
int event_id = 0;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(NETWORK, PASS);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting...");
    delay(500);
  }
  Serial.println("Connected!");
  setupSensor();
}

void loop()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    // Serve status webpage
    // Log values
    HTTPClient http;
    uint16_t val = readSensor();
    Serial.print("Value: ");
    Serial.println(val);
    String server_name = SERVER_NAME;
    String query =
      server_name +
      "?val=" + val +
      "&sid=" + sensor_id +
      "&eid=" + event_id;

    http.begin(query.c_str());
    int httpResponse = http.GET();

    http.end();
    delay(POLLING_INTERVAL);
  }
}

// Here is where you should place all of your sensor setup code
void setupSensor()
{
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();

  delay(1000);

  scale.tare();
  Serial.println(scale.get_offset());
}

// Place your sensor reading code here
uint16_t readSensor()
{
  scale.set_scale(calibration_factor);

  float rawRead = scale.get_units();
  float slope = (0.605605 + 0.60194) / 2.0;
  float adjRead = rawRead * slope;
  Serial.println(adjRead);

  return (uint16_t)(adjRead * 100);
}
