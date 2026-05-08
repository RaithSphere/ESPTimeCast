#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <SPI.h>
#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <Preferences.h>
#include <DNSServer.h>

String cleanTextForDisplay(String str);
void saveUptime();
unsigned long getTotalRuntimeSeconds();
String formatTotalRuntime();
void saveCustomMessageToConfig(const char *msg);
String formatUptime(unsigned long seconds);
char getWeatherIconChar(const String &iconCode);
bool handleTimerCommand(String cmd);
void executeAction(const String &action, const String &value);
void handleBrightnessChange(int newBrightness, bool isFromUI);
bool isModeAvailable(int mode);
bool saveCountdownConfig(bool enabled, time_t targetTimestamp, const String &label);
bool saveConfigRuntime();
void showTimerMode7();

#include "../ESPTimeCast_ESP32/ESPTimeCast_ESP32.ino"
