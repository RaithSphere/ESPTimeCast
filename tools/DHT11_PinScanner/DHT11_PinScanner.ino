/*
  Temporary DHT11 pin scanner for ESP32.

  Upload this sketch, connect the DHT11 data wire to one candidate GPIO at a
  time, and watch Serial Monitor at 115200 baud.

  Skips MAX7219 pins: GPIO18, GPIO23, GPIO5.
  Also skips ESP32 flash pins GPIO6-GPIO11 and input-only GPIO34-GPIO39.
*/

#include <Arduino.h>

struct DhtReading {
  bool ok;
  uint8_t humidity;
  uint8_t temperatureC;
  uint8_t checksum;
  const char *reason;
};

const int CANDIDATE_PINS[] = {
  0, 2, 4,
  12, 13, 14, 15, 16, 17,
  19, 21, 22,
  25, 26, 27,
  32, 33
};

const size_t PIN_COUNT = sizeof(CANDIDATE_PINS) / sizeof(CANDIDATE_PINS[0]);

bool waitForLevel(int pin, int level, uint32_t timeoutMicros) {
  uint32_t started = micros();
  while (digitalRead(pin) != level) {
    if (micros() - started > timeoutMicros) return false;
    yield();
  }
  return true;
}

DhtReading readDht11(int pin) {
  uint8_t data[5] = {0, 0, 0, 0, 0};

  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(250);

  digitalWrite(pin, LOW);
  delay(20);
  digitalWrite(pin, HIGH);
  delayMicroseconds(40);
  pinMode(pin, INPUT_PULLUP);

  if (!waitForLevel(pin, LOW, 1000)) return {false, 0, 0, 0, "no response low"};
  if (!waitForLevel(pin, HIGH, 1000)) return {false, 0, 0, 0, "no response high"};
  if (!waitForLevel(pin, LOW, 1000)) return {false, 0, 0, 0, "bad preamble"};

  for (int bit = 0; bit < 40; bit++) {
    if (!waitForLevel(pin, HIGH, 1000)) return {false, 0, 0, 0, "bit start timeout"};

    uint32_t highStart = micros();
    if (!waitForLevel(pin, LOW, 1000)) return {false, 0, 0, 0, "bit end timeout"};
    uint32_t highTime = micros() - highStart;

    data[bit / 8] <<= 1;
    if (highTime > 45) data[bit / 8] |= 1;
  }

  uint8_t checksum = data[0] + data[1] + data[2] + data[3];
  if (checksum != data[4]) return {false, data[0], data[2], data[4], "checksum mismatch"};

  return {true, data[0], data[2], data[4], "ok"};
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println("DHT11 ESP32 pin scanner");
  Serial.println("Skipping GPIO18, GPIO23, GPIO5, GPIO6-GPIO11, GPIO34-GPIO39.");
  Serial.println("Move the DHT11 data wire between candidate pins, or leave it connected and wait.");
}

void loop() {
  Serial.println();
  Serial.println("Scanning...");

  for (size_t i = 0; i < PIN_COUNT; i++) {
    int pin = CANDIDATE_PINS[i];
    DhtReading reading = readDht11(pin);

    if (reading.ok) {
      Serial.printf("FOUND on GPIO%d: %uC, %u%% RH\n", pin, reading.temperatureC, reading.humidity);
    } else {
      Serial.printf("GPIO%d: %s\n", pin, reading.reason);
    }

    delay(1200);
  }

  Serial.println("Scan complete. Repeating in 5 seconds.");
  delay(5000);
}
