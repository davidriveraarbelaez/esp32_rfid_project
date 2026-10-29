
#include <SPI.h>
#include <MFRC522.h>

// Pines del lector RC522 para ESP32 DevKit
constexpr byte PIN_SS  = 12;
constexpr byte PIN_RST = 15;

MFRC522 rfid(PIN_SS, PIN_RST);

// Control de lecturas repetidas
String ultimoUID = "";
unsigned long ultimaLectura = 0;

constexpr unsigned long INTERVALO_LECTURA = 1500;

String obtenerUID() {
  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();
  return uid;
}

void setup() {
  Serial.begin(115200);

  // Configuración SPI explícita para ESP32
  SPI.begin(18, 19, 23, PIN_SS);

  rfid.PCD_Init();

  delay(100);

  Serial.println();
  Serial.println("================================");
  Serial.println(" SISTEMA DE LECTURA RFID");
  Serial.println(" ESP32 + RC522");
  Serial.println("================================");
  Serial.println("Acerque una tarjeta RFID...");
}

void loop() {
  // ¿Hay una tarjeta nueva disponible?
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // ¿Se pudo leer su UID?
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uidActual = obtenerUID();
  unsigned long ahora = millis();

  // Evitar repetir el mismo UID continuamente
  if (uidActual != ultimoUID ||
      ahora - ultimaLectura >= INTERVALO_LECTURA) {

    Serial.print("UID detectado: ");
    Serial.println(uidActual);

    ultimoUID = uidActual;
    ultimaLectura = ahora;
  }

  // Finalizar la comunicación con la tarjeta
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  delay(50);
}