#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Samsung.h> 
#include <ir_Coolix.h> // Agregamos la librería para BGH

const uint16_t LED_IR = 26; // El pin de tu emisor IR

// Instanciamos ambos objetos apuntando al mismo pin físico
IRSamsungAc samsungAC(LED_IR);
IRCoolixAC bghAC(LED_IR);

void setup() {
  Serial.begin(115200);
  
  // Iniciamos el hardware de ambos emisores
  samsungAC.begin();
  bghAC.begin();
  
  // --- CONFIGURACIÓN SAMSUNG ---
  samsungAC.on();                        
  samsungAC.setMode(kSamsungAcHeat);     
  samsungAC.setTemp(27);                 
  samsungAC.setFan(kSamsungAcFanHigh);   
  
  // --- CONFIGURACIÓN BGH (COOLIX) ---
  bghAC.on();
  bghAC.setMode(kCoolixHeat);     // Modo Calor específico de Coolix
  bghAC.setTemp(27);              // Temperatura a 27°C
  bghAC.setFan(kCoolixFanAuto);   // Ventilador en Automático
  
  Serial.println("Sistema Universal ESP32 Iniciado.");
  Serial.println("Escribe 'A' para encender el SAMSUNG (Calor, 27°C)");
  Serial.println("Escribe 'B' para encender el BGH (Calor, 27°C)");
}

void loop() {
  // Comprobamos si hay algún dato esperando a ser leído
  if (Serial.available() > 0) {
    
    char entrada = Serial.read();
    
    // Disparo para Samsung
    if (entrada == 'A' || entrada == 'a') {
      samsungAC.send();
      Serial.println("-> [A] Señal SAMSUNG enviada con éxito.");
    }
    // Disparo para BGH
    else if (entrada == 'B' || entrada == 'b') {
      bghAC.send();
      Serial.println("-> [B] Señal BGH (Coolix) enviada con éxito.");
    }
    
    delay(10); 
  }
}