// Based on code snippets from here: https://randomnerdtutorials.com/esp32-ble-server-client/

#ifndef _BLE_PERIPHERAL
#define _BLE_PERIPHERAL

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "..\\SharedDefines.h"

namespace BLEPeripheral
{
  static bool isCentralConnected = false;

  // Central is allowed to read values and get notified when values are received
  BLECharacteristic myCharacteristics(MY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  class MyServerCallbacks: public BLEServerCallbacks
  {
      void onConnect(BLEServer* pServer) override
      {
          Serial.println("Central connected");
          isCentralConnected = true;
      }

      void onDisconnect(BLEServer* pServer) override
      {
          Serial.println("Central disconnected");
          isCentralConnected = false;
          BLEDevice::startAdvertising();  // When disconnected, start advertising again
          Serial.println("Peripheral started advertising");
      }
  };

  void Setup()
  {
    Serial.println("Initializing BLE Peripheral");

    // Create BLE device
    BLEDevice::init(PERIPHERAL_NAME);

    // Create the BLE Server
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Add a service to the server
    BLEService* pService = pServer->createService(MY_SERVICE_UUID);

    // Add a characteristic to the service
    pService->addCharacteristic(&myCharacteristics);

    // Add BLE2902 descriptor to the characteristic to use Notify property
    BLE2902* pBLE2902 = new BLE2902();
    pBLE2902->setNotifications(true);
    myCharacteristics.addDescriptor(pBLE2902);

    // Configure advertising
    pServer->getAdvertising()->addServiceUUID(MY_SERVICE_UUID);

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();

    Serial.println("Peripheral started advertising");
    Serial.println("Waiting for Central to connect...");
  }
  
  void SendByteToCentral(unsigned char value)
  {
    if (isCentralConnected)
    {
      // Set the new value and notify the central device
      myCharacteristics.setValue(&value, 1);
      myCharacteristics.notify();
      Serial.printf("Sent value %d\n", value);
    }
  }

} // namespace BLEPeripheral

#endif  // _BLE_PERIPHERAL