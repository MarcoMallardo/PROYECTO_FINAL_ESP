#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h> 
#include <ir_Coolix.h> 

// Definición de tus pines
const uint16_t SENSOR_IR = 25;
const uint16_t LED_IR = 26;

// Instancias de los objetos
IRSamsungAc samsungAC(LED_IR);
IRCoolixAC bghAC(LED_IR);

void setup() {
  Serial.begin(115200);
  
  // Iniciamos emisores
  samsungAC.begin();
  bghAC.begin();
  
  // Configuración predeterminada para el encendido (Heat 27°C)
  samsungAC.setMode(kSamsungAcHeat);
  samsungAC.setTemp(27);
  samsungAC.setFan(kSamsungAcFanHigh);

  bghAC.setMode(kCoolixHeat);
  bghAC.setTemp(27);
  bghAC.setFan(kCoolixFanAuto);

  Serial.println("=========================================");
  Serial.println("Control Maestro AC Iniciado (Pin 26)");
  Serial.println("A -> Samsung ON  | a -> Samsung OFF");
  Serial.println("B -> Coolix ON   | b -> Coolix OFF");
  Serial.println("=========================================");
}

void loop() {
  if (Serial.available() > 0) {
    char entrada = Serial.read();

    switch (entrada) {
      // --- SAMSUNG (A / a) ---
      case 'A':
        samsungAC.on();
        samsungAC.send();
        Serial.println("[A] Samsung: ENCENDIDO (Heat 27°C)");
        break;
      
      case 'a':
        samsungAC.off();
        samsungAC.send();
        Serial.println("[a] Samsung: APAGADO");
        break;

      // --- COOLIX / BGH / SURREY (B / b) ---
      case 'B':
        bghAC.on();
        bghAC.send();
        Serial.println("[B] Coolix: ENCENDIDO (Heat 27°C)");
        break;

      case 'b':
        bghAC.off();
        bghAC.send();
        Serial.println("[b] Coolix: APAGADO");
        break;

      default:
        // Ignorar otros caracteres (como saltos de línea)
        break;
    }
  }
}