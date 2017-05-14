#include <HLW8012.h> //https://bitbucket.org/xoseperez/hlw8012
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

#define PIN_LED 15//3
#define PIN_BUTTON 0
#define PIN_RELAY 12
#define SEL_PIN                         5
#define CF1_PIN                         13
#define CF_PIN                          14

#define LED_ON() digitalWrite(PIN_LED, HIGH)
#define LED_OFF() digitalWrite(PIN_LED, LOW)
#define LED_TOGGLE() digitalWrite(PIN_LED, digitalRead(PIN_LED) ^ 0x01)
#define RELAY_ON() digitalWrite(PIN_RELAY, HIGH)
#define RELAY_OFF() digitalWrite(PIN_RELAY, LOW)

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
const char* ssid = "ThanhNienNghiemTuc";
const char* password = "taodangchoi";
ESP8266WebServer server(80);

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

void handleRoot() {
  LED_OFF();
  server.send(200, "text/plain", "hello from esp8266!");
  LED_ON();
}

void handleNotFound(){
  LED_OFF();
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
  LED_ON();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\r\nBooting");

  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // ("esp8266_" + String(ESP.getChipId())).c_str())
  if (MDNS.begin(("esp8266_" + String(ESP.getChipId())).c_str())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();

  //set led pin as output
  pinMode(PIN_LED, OUTPUT);

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

  LED_ON();
  RELAY_ON();
  hlw8012.begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, true);
  hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);
  setInterrupts();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

bool relay_on = false;
int last_check = 0;

void loop() {
  // put your main code here, to run repeatedly:

  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
    server.handleClient();
  }

  if (millis() - last_check > 10000) {
    Serial.printf("Time: %d - IP address: ", last_check);
    last_check = millis();
    Serial.println(WiFi.localIP());
  }

  if (Serial.read() != -1) {
    Serial.printf("Receive keystroke: ");
    Serial.println(WiFi.localIP());
  }
}
