#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "Adafruit_DRV2605.h"
#include <Adafruit_NeoPixel.h>

#define SERVICE_UUID        "1adc2787-3cfd-43be-a363-701bea5b59f9"
#define CHARACTERISTIC_UUID "cfdef092-e196-440e-938e-981f97692a78"
#define CONNECTION_UUID     "fe113624-8cd7-48f0-808e-ebb4f257ebd9"
#define NUMPIXELS           1

BLEServer* pServer = NULL;
BLECharacteristic *pCharacteristic;
BLECharacteristic *pConnection;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);


//BLECharacteristic* pCharacteristicRX = NULL;
bool deviceConnected = false;
Adafruit_DRV2605 drv; 
uint8_t r_effect = 60;
uint8_t front_effect = 58;
uint8_t back_effect = 62;
String rxValue = "";
String connect = "";



class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    //pServer->getAdvertising()->start();
  }
};



void setup() {
  Serial.begin(115200);

#if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  drv.begin(&Wire1);

  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);
  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32_Test");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pConnection = pService->createCharacteristic(
                                         CONNECTION_UUID ,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  //pCharacteristic->setValue("Hello!");

  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright
}

void loop() {
  //check if client is disconnected
  if (!deviceConnected){
    //turn of led
    //pixels.fill(0xFF0000);
    pixels.fill(0x000000);
    pixels.show();
    //reconnect to the client
    pServer->getAdvertising()->start();
  }

  connect = pConnection->getValue();
  if (connect == "Connected"){
    pixels.fill(0xFF0000);
    pixels.show();
  }
  else if (connect == "Disconnected"){
    pixels.fill(0x000000);
    pixels.show();
  }

  //get value from client
  rxValue = pCharacteristic->getValue();
  //print received value from client
  Serial.println(rxValue);
  //test condition for haptic
  if (rxValue == "right"){
    pixels.fill(0x00ffff);
    pixels.show();
    drv.setWaveform(0, r_effect);  // play effect 
    drv.setWaveform(1, 0);       // end waveform
    drv.go();
  }
  else if (rxValue == "front"){
    pixels.fill(0x00ffff);
    pixels.show();
    drv.setWaveform(0, front_effect);  // play effect 
    drv.setWaveform(1, 0);       // end waveform
    drv.go();
  }
  else if (rxValue == "back"){
    pixels.fill(0x00ffff);
    pixels.show();
    drv.setWaveform(0, back_effect);  // play effect 
    drv.setWaveform(1, 0);       // end waveform
    drv.go();
  }
  delay(500);
}