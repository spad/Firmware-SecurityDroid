// Test I2C: scansione del bus e verifica del PCA9685 a 0x40.
// XIAO ESP32-S3: SDA = D4 (GPIO5), SCL = D5 (GPIO6).

#include <Arduino.h>
#include <Wire.h>

const uint8_t PCA_ADDR  = 0x40;
const uint8_t REG_MODE1 = 0x00;
const uint8_t REG_MODE2 = 0x01;

int readReg(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return -1;
  if (Wire.requestFrom(addr, (uint8_t)1) != 1) return -1;
  return Wire.read();
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}

  Wire.begin(SDA, SCL);
  Wire.setClock(100000);
  Serial.printf("\nI2C su SDA=GPIO%d, SCL=GPIO%d\n", SDA, SCL);
}

void loop() {
  int found = 0;
  Serial.println("Scansione...");
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  dispositivo a 0x%02X\n", addr);
      found++;
    }
  }
  Serial.printf("Trovati: %d\n", found);

  // Dopo il reset MODE1 = 0x11 (SLEEP + ALLCALL), MODE2 = 0x04 (OUTDRV totem-pole)
  int m1 = readReg(PCA_ADDR, REG_MODE1);
  int m2 = readReg(PCA_ADDR, REG_MODE2);
  if (m1 < 0 || m2 < 0) {
    Serial.println("PCA9685 a 0x40: NESSUNA RISPOSTA");
  } else {
    Serial.printf("PCA9685 a 0x40: MODE1=0x%02X (atteso 0x11), MODE2=0x%02X (atteso 0x04)\n", m1, m2);
  }

  Serial.println();
  delay(3000);
}
