#include <HLW8012.h>              //https://bitbucket.org/xoseperez/hlw8012
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>

#define PIN_LED                         3
#define PIN_BUTTON                      0
#define PIN_RELAY                       12
#define SEL_PIN                         5
#define CF1_PIN                         13
#define CF_PIN                          14

#define LED_WIFI_CONNECTING_TICK_FREQ         0.25

#define LED_RELAY_STATUS_TICK_FREQ            1       // 1s freq
#define LED_RELAY_STATUS_OFF_TIME             4       // the led is off for 4s
#define LED_RELAY_STATUS_BLINK_PERIOD         5       // 5s period of led blinking pattern

#define BUTTON_PRESSED()    (digitalRead(PIN_BUTTON) == 0)
#define LED_ON()            digitalWrite(PIN_LED, HIGH)
#define LED_OFF()           digitalWrite(PIN_LED, LOW)
#define LED_TOGGLE()        digitalWrite(PIN_LED, digitalRead(PIN_LED) ^ 0x01)

#define RELAY_ON()          ticker.detach(); \
                            relay_on = true; \
                            LED_ON(); \
                            digitalWrite(PIN_RELAY, HIGH)
#define RELAY_OFF(isBlink)  if (isBlink) {ticker.attach(LED_RELAY_STATUS_TICK_FREQ, ledBlinkTick);}\
                            relay_on = false; \
                            digitalWrite(PIN_RELAY, LOW)

#define OTA_PASS "admin"
#define OTA_PORT (8266)

// Set SEL_PIN to HIGH to sample current
// This is the case for Itead's Sonoff POW, where a
// the SEL_PIN drives a transistor that pulls down
// the SEL pin in the HLW8012 when closed
#define CURRENT_MODE                    HIGH

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 5 * 470000 ) // Real: 2280k
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 950 ) // Real 1.009k

HLW8012 hlw8012;
const char* ssid      = "zealot-wifi";
const char* password  = "pastebin309";
ESP8266WebServer server(80);

bool relay_on         = false;

Ticker ticker;

void ledBlinkTick()
{
  static uint8_t tickTimes = 0;
  tickTimes ++;

  if (tickTimes % LED_RELAY_STATUS_BLINK_PERIOD < LED_RELAY_STATUS_OFF_TIME) {
    digitalWrite(PIN_LED, 0);
  } else {
    digitalWrite(PIN_LED, 1);
  }
}

void ledWifiConnectingTick()
{
  LED_TOGGLE();
}

// When using interrupts we have to call the library entry point
// whenever an interrupt is triggered
void hlw8012_cf1_interrupt() {
  hlw8012.cf1_interrupt();
}
void hlw8012_cf_interrupt() {
  hlw8012.cf_interrupt();
}

// Library expects an interrupt on both edges
void setInterrupts() {
  attachInterrupt(CF1_PIN, hlw8012_cf1_interrupt, CHANGE);
  attachInterrupt(CF_PIN, hlw8012_cf_interrupt, CHANGE);
}

bool isButtonPressed() {
  if (BUTTON_PRESSED()) {
    delay(200);
    if (BUTTON_PRESSED()) {
      return true;
    }
  }
  return false;
}

// return true if hold a button more than x_sec
bool isButtonPressedXSec(int x_sec) {
  static int lastPress = 0;
  if (millis() - lastPress > x_sec*1000 && digitalRead(PIN_BUTTON) == 0) {
    return true;
  } else if (digitalRead(PIN_BUTTON) == 1) {
    lastPress = millis();
  }
  return false;
}

void ledToggle(uint32_t times, uint32_t delay_ms) {
  int curLedValue = digitalRead(PIN_LED);
  while (times--) {
    LED_TOGGLE();
    delay(delay_ms/2);
    LED_TOGGLE();
    delay(delay_ms/2);
  }
  digitalWrite(PIN_LED, curLedValue);
}

void handleRoot() {
  String relayStatus = (relay_on == true) ? "on" : "off";
  String buttonStatus = (BUTTON_PRESSED() == true) ? "pressed" : "released";
  String returnStr = "{\"relay_status\":\"" + relayStatus + "\","
    "\"button_status\":\"" + buttonStatus + "\","
    "\"voltage:\"" + hlw8012.getVoltage() + "\","
    "\"current:\"" + hlw8012.getCurrent() + "\","
    "\"active_power:\"" + hlw8012.getActivePower() + "\","
    "\"apparent_power:\"" + hlw8012.getApparentPower() + "\","
    "\"power_factor:\"" + 100*hlw8012.getPowerFactor() + "\""
    "}";
  server.send(200, "application/json", returnStr);
}

void returnError(String errorMsg) {
  String errorJson = "{\"error\":\"" + errorMsg + "\"}";
  server.send(401, "application/json", errorJson);
}

void handleControl() {
  String wantedArg = "relay";
  if (!server.hasArg(wantedArg)) {
    returnError("relay is required");
    return;
  }

  if (server.arg(wantedArg) == "on") {
    RELAY_ON();
  } else if (server.arg(wantedArg) == "off") {
    RELAY_OFF(true);
  } else {
    returnError("Unrecognize relay value: " + server.arg(wantedArg));
    return;
  }

  handleRoot();
}

void handleToggle() {
  if (relay_on == false) {
    RELAY_ON();
  } else {
    RELAY_OFF(true);
  }
  handleRoot();
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\r\nBooting");

  //set led pin as output
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  LED_OFF();
  RELAY_OFF(false);

  // tick indicate that we are connecting to wifi
  ticker.attach(LED_WIFI_CONNECTING_TICK_FREQ, ledWifiConnectingTick);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // already connected, stop tick
  ticker.detach();

  // ("esp8266_" + String(ESP.getChipId())).c_str())
  if (MDNS.begin(("esp8266_" + String(ESP.getChipId())).c_str())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/control", handleControl);
  server.onNotFound(handleNotFound);
  server.begin();

  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  hlw8012.begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, true);
  hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);
  setInterrupts();
  LED_OFF();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  ledToggle(20, 50);
  RELAY_OFF(true);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (isButtonPressed()) {
    if (relay_on == false) {
      RELAY_ON();
    } else {
      RELAY_OFF(true);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
    server.handleClient();
  }

  if (Serial.read() != -1) {
    Serial.printf("Receive keystroke: ");
    Serial.println(WiFi.localIP());
  }
}
