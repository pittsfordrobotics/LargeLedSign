#include "CommonPeripheral.h"

void CommonPeripheral::initialize(String uuid, String localName) {
  Serial.print("Initializing BLE peripheral service for UUID ");
  Serial.print(uuid);
  Serial.println(".");
  
  m_ledService = new BLEService(uuid.c_str());
  BLE.setLocalName(localName.c_str());
  BLE.setAdvertisedService(*m_ledService);
  m_ledService->addCharacteristic(m_brightnessCharacteristic);
  m_ledService->addCharacteristic(m_styleCharacteristic);
  m_ledService->addCharacteristic(m_styleNamesCharacteristic);
  m_ledService->addCharacteristic(m_speedCharacteristic);
  m_ledService->addCharacteristic(m_stepCharacteristic);
  m_ledService->addCharacteristic(m_patternCharacteristic);
  m_ledService->addCharacteristic(m_patternNamesCharacteristic);
  m_ledService->addCharacteristic(m_batteryVoltageCharacteristic);

  BLE.addService(*m_ledService);
  BLE.advertise();
}

void CommonPeripheral::stop() {
  // Disconnect any clients and stop advertising.
  BLE.disconnect();
  BLE.stopAdvertise();
}

void CommonPeripheral::resume() {
  BLE.advertise();
}

bool CommonPeripheral::isConnected() {
  BLEDevice central = BLE.central();
  if (central) {
    return true;
  }
  return false;
}

void CommonPeripheral::setStyleNames(std::vector<String> styleNames) {
  String allStyles = *joinStrings(styleNames);

  Serial.print("All style names: ");
  Serial.println(allStyles);
  Serial.print("Style name string length: ");
  Serial.println(allStyles.length());

  m_styleNamesCharacteristic.setValue(allStyles);
}

void CommonPeripheral::setPatternNames(std::vector<String> patternNames) {
  String allPatterns = *joinStrings(patternNames);

  Serial.print("All pattern names: ");
  Serial.println(allPatterns);
  Serial.print("Pattern name string length: ");
  Serial.println(allPatterns.length());

  m_patternNamesCharacteristic.setValue(allPatterns);
}

byte CommonPeripheral::getBrightness() {
  m_currentBrightness = readByteFromCharacteristic(m_brightnessCharacteristic, m_currentBrightness, "brightness");
  return m_currentBrightness;
}

void CommonPeripheral::setBrightness(byte brightness) {
  m_currentBrightness = brightness;
  m_brightnessCharacteristic.setValue(brightness);
}

byte CommonPeripheral::getStyle() {
  m_currentStyle = readByteFromCharacteristic(m_styleCharacteristic, m_currentStyle, "style");
  return m_currentStyle;
}

void CommonPeripheral::setStyle(byte style) {
  m_currentStyle = style;
  m_styleCharacteristic.setValue(style);
}

byte CommonPeripheral::getSpeed() {
  m_currentSpeed = readByteFromCharacteristic(m_speedCharacteristic, m_currentSpeed, "speed");
  return m_currentSpeed;
}

void CommonPeripheral::setSpeed(byte speed) {
  m_currentSpeed = speed;
  m_speedCharacteristic.setValue(speed);
}

byte CommonPeripheral::getPattern() {
  m_currentPattern = readByteFromCharacteristic(m_patternCharacteristic, m_currentPattern, "pattern");
  return m_currentPattern;
}

void CommonPeripheral::setPattern(byte pattern) {
  m_currentPattern = pattern;
  m_patternCharacteristic.setValue(pattern);
}

byte CommonPeripheral::getStep() {
  m_currentStep = readByteFromCharacteristic(m_stepCharacteristic, m_currentStep, "step");
  return m_currentStep;
}

void CommonPeripheral::setStep(byte step) {
  m_currentStep = step;
  m_stepCharacteristic.setValue(step);
}

void CommonPeripheral::emitBatteryVoltage(float voltage) {
  m_batteryVoltageCharacteristic.setValue(voltage);
}

byte CommonPeripheral::readByteFromCharacteristic(BLEByteCharacteristic characteristic, byte defaultValue, String name) {
  if (isConnected()) {
    if (characteristic.written()) {
      Serial.print("Reading new value for ");
      Serial.print(name);
      byte valByte = characteristic.value();
      Serial.print(". Byte received: ");
      Serial.println(valByte, HEX);
      return valByte;
    }
  }

  return defaultValue;
}

String* CommonPeripheral::joinStrings(std::vector<String> strings) {
  String* joinedStrings = new String();
  for (uint i = 0; i < strings.size(); i++) {
    joinedStrings->concat(strings.at(i));
    if (i < strings.size()-1) {
      joinedStrings->concat(";");
    }
  }

  return joinedStrings;
}
