#include "BluetoothCommon.h"

std::vector<BLECharacteristic> emptyCharacteristicList;

void BluetoothCommon::initialize(String uuid, String localName) {
  initialize(uuid, localName, emptyCharacteristicList);
}

void BluetoothCommon::initialize(String uuid, String localName, std::vector<BLECharacteristic> additionalCharacteristics) {
  // if (!BLE.begin())
  // {
  //   Serial.println("Failed to start BLE!");
  //   return;
  // }

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

  for(uint i = 0; i < additionalCharacteristics.size(); i++) {
    m_ledService->addCharacteristic(additionalCharacteristics.at(i));
  }

  BLE.addService(*m_ledService);
  BLE.advertise();
}

void BluetoothCommon::stop() {
  // Disconnect any clients and stop advertising.
  BLE.disconnect();
  BLE.stopAdvertise();
}

void BluetoothCommon::resume() {
  BLE.advertise();
}

void BluetoothCommon::setStyleNames(std::vector<String> styleNames) {
  String allStyles = *joinStrings(styleNames);

  Serial.print("All style names: ");
  Serial.println(allStyles);
  Serial.print("Style name string length: ");
  Serial.println(allStyles.length());

  m_styleNamesCharacteristic.setValue(allStyles);
}

void BluetoothCommon::setPatternNames(std::vector<String> patternNames) {
  String allPatterns = *joinStrings(patternNames);

  Serial.print("All pattern names: ");
  Serial.println(allPatterns);
  Serial.print("Pattern name string length: ");
  Serial.println(allPatterns.length());

  m_patternNamesCharacteristic.setValue(allPatterns);
}

byte BluetoothCommon::getBrightness() {
  m_currentBrightness = readByteFromCharacteristic(m_brightnessCharacteristic, m_currentBrightness, "brightness");
  return m_currentBrightness;
}

void BluetoothCommon::setBrightness(byte brightness) {
  m_currentBrightness = brightness;
  m_brightnessCharacteristic.setValue(brightness);
}

byte BluetoothCommon::getStyle() {
  m_currentStyle = readByteFromCharacteristic(m_styleCharacteristic, m_currentStyle, "style");
  return m_currentStyle;
}

void BluetoothCommon::setStyle(byte style) {
  m_currentStyle = style;
  m_styleCharacteristic.setValue(style);
}

byte BluetoothCommon::getSpeed() {
  m_currentSpeed = readByteFromCharacteristic(m_speedCharacteristic, m_currentSpeed, "speed");
  return m_currentSpeed;
}

void BluetoothCommon::setSpeed(byte speed) {
  m_currentSpeed = speed;
  m_speedCharacteristic.setValue(speed);
}

byte BluetoothCommon::getPattern() {
  m_currentPattern = readByteFromCharacteristic(m_patternCharacteristic, m_currentPattern, "pattern");
  return m_currentPattern;
}

void BluetoothCommon::setPattern(byte pattern) {
  m_currentPattern = pattern;
  m_patternCharacteristic.setValue(pattern);
}

byte BluetoothCommon::getStep() {
  m_currentStep = readByteFromCharacteristic(m_stepCharacteristic, m_currentStep, "step");
  return m_currentStep;
}

void BluetoothCommon::setStep(byte step) {
  m_currentStep = step;
  m_stepCharacteristic.setValue(step);
}

void BluetoothCommon::emitBatteryVoltage(float voltage) {
  m_batteryVoltageCharacteristic.setValue(voltage);
}

byte BluetoothCommon::readByteFromCharacteristic(BLEByteCharacteristic characteristic, byte defaultValue, String name) {
  // We don't really care what central device is connected, just if there's one there.
  BLEDevice central = BLE.central();
  if (central) {
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

String* BluetoothCommon::joinStrings(std::vector<String> strings) {
  String* joinedStrings = new String();
  for (int i = 0; i < strings.size(); i++) {
    joinedStrings->concat(strings.at(i));
    if (i < strings.size()-1) {
      joinedStrings->concat(";");
    }
  }

  return joinedStrings;
}
