#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h> 
#include <ir_Coolix.h> 
#include <ir_LG.h>

const char* ssid = "Fibertel WiFi063 2.4GHz";
const char* password = "00425154222";

WebServer server(80);

const uint16_t LED_IR = 26;

IRSamsungAc samsungAC(LED_IR);
IRCoolixAC bghAC(LED_IR);
IRLgAc lgAC(LED_IR);

void setup() {
  Serial.begin(115200);
  
  samsungAC.begin(); bghAC.begin(); lgAC.begin();
  
  // Estados base (Calor, 27°C)
  samsungAC.setMode(kSamsungAcHeat); samsungAC.setTemp(27); samsungAC.setFan(kSamsungAcFanHigh);
  bghAC.setMode(kCoolixHeat);        bghAC.setTemp(27);     bghAC.setFan(kCoolixFanAuto);
  lgAC.setMode(kLgAcHeat);           lgAC.setTemp(27);      lgAC.setFan(kLgAcFanHigh);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Conectado! IP de la API:");
  Serial.println(WiFi.localIP()); // <-- Anotá esta IP para Flutter

  // --- RUTAS DE LA API ---
  // Al recibir la petición, disparan y devuelven un código HTTP 200 (Éxito)
  
  server.on("/samsung/on", []() {
    samsungAC.on(); samsungAC.send();
    server.send(200, "text/plain", "OK");
    Serial.println("API: Samsung ON");
  });
  server.on("/samsung/off", []() {
    samsungAC.off(); samsungAC.send();
    server.send(200, "text/plain", "OK");
    Serial.println("API: Samsung OFF");
  });

  server.on("/coolix/on", []() {
    bghAC.on(); bghAC.send();
    server.send(200, "text/plain", "OK");
    Serial.println("API: Coolix ON");
  });
  server.on("/coolix/off", []() {
    bghAC.off(); bghAC.send();
    server.send(200, "text/plain", "OK");
    Serial.println("API: Coolix OFF");
  });

  server.on("/lg/on", []() {
    lgAC.on(); lgAC.send();
    server.send(200, "text/plain", "OK");
    Serial.println("API: LG ON");
  });
  server.on("/lg/off", []() {
    lgAC.off(); lgAC.send();
    server.send(200, "text/plain", "OK");
    Serial.println("API: LG OFF");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}