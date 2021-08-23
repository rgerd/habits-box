#include "AnalogInput.h"
#include "Wifi.h"
#include "secrets/WifiConfig.h"
#include "Firebase_Arduino_WiFiNINA.h"

AnalogReading AnalogReadings[NUM_SENSOR_PINS];
bool LedButtonPressedDown = false;

char PressBuff[NUM_SENSOR_PINS * NUM_INPUTS + 1];
FirebaseData firebaseData;


void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(LED_BUTTON_LIGHT_PIN, OUTPUT);
  pinMode(LED_BUTTON_INPUT_PIN, INPUT_PULLUP);

  for (uint32_t i = 0; i < NUM_SENSOR_PINS; i++)
  {
    pinMode(SENSOR_PINS[i], INPUT);
    AnalogReadings[i].SetSensorPinIdx(i);
  }

  PressBuff[NUM_SENSOR_PINS * NUM_INPUTS] = 0;
}

bool ConnectedToWiFi = false;
bool ConnectToWiFi()
{
  int status = WL_IDLE_STATUS;

  // Check if the WiFi module works
  if (WiFi.status() == WL_NO_SHIELD) {
    // Wait until WiFi ready
    Serial.println("WiFi adapter not ready");
  }
  else if (WiFi.begin(ssid, pass) == WL_CONNECTED)
  {
    _PrintWiFiStatus();
    ConnectedToWiFi = true;

    _SetUpRTC();

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASS);
    Firebase.reconnectWiFi(true);

    return true;
  }

  return false;
}

void UpdateInputs()
{
  bool thinking = false;

  for (uint32_t i = 0; i < NUM_SENSOR_PINS; i++)
  {
    auto& reading = AnalogReadings[i];
    reading.Update();
    const auto currentReadingPresses = reading.GetPresses();
    _GetPressString(currentReadingPresses, i, PressBuff);
    thinking |= (currentReadingPresses & ANALOG_SETTLING_BIT) != 0;
  }

  if (thinking)
  {
    const auto thinkingIndication = static_cast<int>((sin(millis() * 0.01) + 1) * 0.5 * 255);
    analogWrite(LED_BUTTON_LIGHT_PIN, thinkingIndication);
    delay(1);
  }
  else
  {
    analogWrite(LED_BUTTON_LIGHT_PIN, 255);

    const bool ledButtonCurrentlyPressedDown = 1 - digitalRead(LED_BUTTON_INPUT_PIN);
    if (ledButtonCurrentlyPressedDown != LedButtonPressedDown && LedButtonPressedDown)
    {
      Serial.println(PressBuff);
      const auto dateString = _GetDateString();
      const auto pressesString = String(PressBuff);
      analogWrite(LED_BUTTON_LIGHT_PIN, 0);
      if (Firebase.setString(firebaseData, "/habits/" + dateString, pressesString))
      {
        Serial.println("Successfully pushed data!");
        analogWrite(LED_BUTTON_LIGHT_PIN, 255);
      }
      else
      {
        Serial.println("There was a problem pushing data:");
        Serial.println(firebaseData.errorReason());
        const auto startMillis = millis();
        while (millis() < startMillis + 10000)
        {
          analogWrite(LED_BUTTON_LIGHT_PIN, ((millis() / 500) % 2) * 255);
        }
      }
    }
    LedButtonPressedDown = ledButtonCurrentlyPressedDown;
    delay(5);
  }
}

void loop() {
  if (ConnectedToWiFi || ConnectToWiFi())
  {
    UpdateInputs();
  }
  else
  {
    Serial.println("Looking for wifi...");
    analogWrite(LED_BUTTON_LIGHT_PIN, ((millis() / 500) % 2) * 255);
    delay(10);
  }
}
