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

  for (uint i = 0; i < m_additionalCharacteristics.size(); i++) {
    Serial.print("Adding additional characteristic ");
    Serial.println(m_additionalCharacteristics.at(i).uuid());
    m_ledService->addCharacteristic(m_additionalCharacteristics.at(i));
  }

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
  String allStyles = StringUtils::joinStrings(styleNames, ';');
  setStyleNames(allStyles);
}

void CommonPeripheral::setStyleNames(String styleNames) {
  Serial.print("All style names: ");
  Serial.println(styleNames);
  Serial.print("Style name string length: ");
  Serial.println(styleNames.length());

  m_styleNamesCharacteristic.writeValue(styleNames);
}

void CommonPeripheral::setPatternNames(std::vector<String> patternNames) {
  String allPatterns = StringUtils::joinStrings(patternNames, ';');
  setPatternNames(allPatterns);
}

void CommonPeripheral::setPatternNames(String patternNames) {
  Serial.print("All pattern names: ");
  Serial.println(patternNames);
  Serial.print("Pattern name string length: ");
  Serial.println(patternNames.length());

  m_patternNamesCharacteristic.writeValue(patternNames);
}

byte CommonPeripheral::getBrightness() {
  m_currentBrightness = readByteFromCharacteristic(m_brightnessCharacteristic, m_currentBrightness, "brightness");
  return m_currentBrightness;
}

void CommonPeripheral::setBrightness(byte brightness) {
  m_currentBrightness = brightness;
  m_brightnessCharacteristic.writeValue(brightness);
}

byte CommonPeripheral::getStyle() {
  m_currentStyle = readByteFromCharacteristic(m_styleCharacteristic, m_currentStyle, "style");
  return m_currentStyle;
}

void CommonPeripheral::setStyle(byte style) {
  m_currentStyle = style;
  m_styleCharacteristic.writeValue(style);
}

byte CommonPeripheral::getSpeed() {
  m_currentSpeed = readByteFromCharacteristic(m_speedCharacteristic, m_currentSpeed, "speed");
  return m_currentSpeed;
}

void CommonPeripheral::setSpeed(byte speed) {
  m_currentSpeed = speed;
  m_speedCharacteristic.writeValue(speed);
}

byte CommonPeripheral::getPattern() {
  m_currentPattern = readByteFromCharacteristic(m_patternCharacteristic, m_currentPattern, "pattern");
  return m_currentPattern;
}

void CommonPeripheral::setPattern(byte pattern) {
  m_currentPattern = pattern;
  m_patternCharacteristic.writeValue(pattern);
}

byte CommonPeripheral::getStep() {
  m_currentStep = readByteFromCharacteristic(m_stepCharacteristic, m_currentStep, "step");
  return m_currentStep;
}

void CommonPeripheral::setStep(byte step) {
  m_currentStep = step;
  m_stepCharacteristic.writeValue(step);
}

void CommonPeripheral::emitBatteryVoltage(float voltage) {
  m_batteryVoltageCharacteristic.writeValue(voltage);
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

String CommonPeripheral::readStringFromCharacteristic(BLEStringCharacteristic characteristic, String defaultValue, String name) {
  if (isConnected()) {
    if (characteristic.written()) {
      Serial.print("Reading new value for ");
      Serial.print(name);
      String value = characteristic.value();
      Serial.print(". Value received: ");
      Serial.println(value);
      return value;
    }
  }

  return defaultValue;
}