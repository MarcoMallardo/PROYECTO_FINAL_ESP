#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 25; // Tu pin actual

// --- CONFIGURACIÓN AVANZADA PARA AIRES ACONDICIONADOS ---
// Usamos nombres propios para evitar conflictos con la librería
const uint16_t TAMANO_BUFFER = 1024; 
const uint8_t TIEMPO_ESPERA = 50;  // 50ms para que no corte la señal de Samsung
const bool GUARDAR_BUFFER = true;  // Fundamental para juntar señales largas

// Inicializamos el receptor
IRrecv irrecv(kRecvPin, TAMANO_BUFFER, TIEMPO_ESPERA, GUARDAR_BUFFER);

decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  Serial.println("==========================================");
  Serial.println("Lector de Estados AC Iniciado.");
  Serial.println("Apunta el control y presiona un botón...");
  Serial.println("==========================================");
}

void loop() {
  if (irrecv.decode(&results)) {
    
    // Verificamos si es un protocolo complejo de Aire Acondicionado
    if (hasACState(results.decode_type)) {
      Serial.println("\n--- Nueva Señal de AC Capturada ---");
      
      // 1. Mostramos la marca / protocolo
      Serial.print("Protocolo detectado: ");
      Serial.println(typeToString(results.decode_type));
      
      // 2. Calculamos el tamaño en bytes (bits / 8)
      uint16_t bytesEstado = results.bits / 8;
      
      Serial.print("Tamaño en Bytes: ");
      Serial.println(bytesEstado);
      
      // 3. Imprimimos el estado en formato de arreglo listo para copiar y pegar
      Serial.print("Código para tu proyecto:\n  uint8_t estado_capturado[");
      Serial.print(bytesEstado);
      Serial.print("] = {");
      
      for (uint16_t i = 0; i < bytesEstado; i++) {
        Serial.printf("0x%02X", results.state[i]);
        // Ponemos la coma solo si no es el último elemento
        if (i < bytesEstado - 1) {
          Serial.print(", ");
        }
      }
      Serial.println("};");
      
    } else {
      // Si apuntas con un control de TV o un protocolo simple
      Serial.println("\n--- Señal Simple Capturada ---");
      Serial.print("Protocolo: ");
      Serial.println(typeToString(results.decode_type));
      Serial.print("Valor Hexadecimal: 0x");
      Serial.println(uint64ToString(results.value, 16));
    }
    
    // Preparamos el receptor para la próxima lectura
    irrecv.resume();
  }
}