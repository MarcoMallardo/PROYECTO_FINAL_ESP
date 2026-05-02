//#define API_KEY "AIzaSyCI682u5MblQpp4Cg3XBAPMnwRf-Uz-S9A"
//#define DATABASE_URL "proyectofinal5lcg6-default-rtdb.firebaseio.com"
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h> 
#include <ir_Coolix.h> 
#include <ir_LG.h>

const char* ssid = "Fibertel WiFi063 2.4GHz";
const char* password = "00425154222";

#define API_KEY "AIzaSyCI682u5MblQpp4Cg3XBAPMnwRf-Uz-S9A"
#define DATABASE_URL "proyectofinal5lcg6-default-rtdb.firebaseio.com" 

FirebaseData fbdoStream;    
FirebaseData fbdoEscribir;  

FirebaseAuth auth;
FirebaseConfig config;

const uint16_t LED_IR = 26;
IRSamsungAc samsungAC(LED_IR);
IRCoolixAC bghAC(LED_IR);
IRLgAc lgAC(LED_IR);

// --- CANDADO LÓGICO ---
bool esperandoReinicio = false; 

void setup() {
  Serial.begin(115200);
  
  samsungAC.begin(); bghAC.begin(); lgAC.begin();
  samsungAC.setMode(kSamsungAcHeat); samsungAC.setTemp(27); samsungAC.setFan(kSamsungAcFanHigh);
  bghAC.setMode(kCoolixHeat);        bghAC.setTemp(27);     bghAC.setFan(kCoolixFanAuto);
  lgAC.setMode(kLgAcHeat);           lgAC.setTemp(27);      lgAC.setFan(kLgAcFanHigh);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a Fibertel...");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi Conectado.");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.signer.test_mode = true; 
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (!Firebase.RTDB.beginStream(&fbdoStream, "/comando")) {
    Serial.println("Error al iniciar el stream");
  } else {
    Serial.println("ESP32 escuchando comandos desde la Nube...");
  }
}

void loop() {
  if (Firebase.ready() && Firebase.RTDB.readStream(&fbdoStream)) {
    
    if (fbdoStream.dataType() == "string") {
      String orden = fbdoStream.stringData();
      orden.replace("\"", "");

      // 1. Si Firebase dice ESPERANDO, abrimos el candado y reiniciamos el ciclo
      if (orden == "ESPERANDO") {
        esperandoReinicio = false; 
        return; 
      }

      // 2. TU LÓGICA: Si el candado está cerrado, forzamos agresivamente el "ESPERANDO" y cortamos
      if (esperandoReinicio == true) {
        Firebase.RTDB.setStringAsync(&fbdoEscribir, "/comando", "ESPERANDO");
        return; 
      }

      Serial.print("Comando recibido: ");
      Serial.println(orden);

      // --- DISPARO DE SEÑALES ---
      if (orden == "SAMSUNG_ON") { samsungAC.on(); samsungAC.send(); } 
      else if (orden == "SAMSUNG_OFF") { samsungAC.off(); samsungAC.send(); }
      else if (orden == "COOLIX_ON") { bghAC.on(); bghAC.send(); } 
      else if (orden == "COOLIX_OFF") { bghAC.off(); bghAC.send(); }
      else if (orden == "LG_ON") { lgAC.on(); lgAC.send(); } 
      else if (orden == "LG_OFF") { lgAC.off(); lgAC.send(); }
      
      // 3. Cerramos el candado post-disparo
      esperandoReinicio = true;
      
      // 4. Pedimos a Firebase que se limpie por primera vez
      Firebase.RTDB.setStringAsync(&fbdoEscribir, "/comando", "ESPERANDO");
    }
  }
}