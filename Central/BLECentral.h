// Based on code snippets from here: https://randomnerdtutorials.com/esp32-ble-server-client/

#ifndef _BLE_CENTRAL
#define _BLE_CENTRAL

#include <BLEDevice.h>

#include "..\\SharedDefines.h"

namespace BLECentral
{
  // Variables read/written from callback functions
  volatile unsigned char peripheralValue = 0;
  volatile bool isPeripheralConnected = false;
  volatile bool isScanning = false;
  volatile bool tryToConnect = false;

  static BLEScan* pBLEScan;
  static BLEAddress* pPeripheralAddress;  // Address of the peripheral device which is found during scanning
  static BLERemoteCharacteristic* pMyCharacteristic;

  const unsigned int scanTimeout = 5;  // 5 seconds timeout

  // Activate notify
  const uint8_t notificationOn[] = { 0x1, 0x0 };
  const uint8_t notificationOff[] = { 0x0, 0x0 };

  void StartScanning()
  {
    Serial.println("Start scanning");
    isScanning = true;
    pBLEScan->start(scanTimeout);

    // start() will block until scanning is explicitly stopped or it timed out
    Serial.println("Scanning stopped or timed out");
    isScanning = false;
  }

  void StopScanning()
  {
    Serial.println("Stop scanning");
    pBLEScan->stop();
    isScanning = false;
  }

  // Callback for when a BLE device is found during scanning
  class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
  {
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      if (advertisedDevice.getName() == PERIPHERAL_NAME)
      {
        Serial.printf("Device '%s' found\n", PERIPHERAL_NAME);
        pPeripheralAddress = new BLEAddress(advertisedDevice.getAddress());
        tryToConnect = true;
        StopScanning();
      }
    }
  };

  // Callback for when a new value was received
  static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
  {
    peripheralValue = *((unsigned char*)pData);
    Serial.printf("New value received from peripheral: %d\n", peripheralValue);
    delay(10);
  }

  // Callbacks for when connecting/disconnecting
  class MyClientCallback : public BLEClientCallbacks
  {
    void onConnect(BLEClient* pClient)
    {
      Serial.println("MyClientCallback::onConnect() called");
    }

    void onDisconnect(BLEClient* pClient)
    {
      isPeripheralConnected = false;
      Serial.println("MyClientCallback::onDisconnect() called");
    }
  };

  bool ConnectToPeripheral(BLEAddress pAddress)
  {
    Serial.println("Trying to connect to peripheral");

    isPeripheralConnected = false;

    BLEClient* pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());

    pClient->connect(pAddress);
 
    BLERemoteService* pRemoteService = pClient->getService(MY_SERVICE_UUID);
    if (pRemoteService == nullptr)
    {
      Serial.println("Failed to find our service");
      pClient->disconnect();
      return false;
    }
  
    pMyCharacteristic = pRemoteService->getCharacteristic(MY_CHARACTERISTIC_UUID);
    if (pMyCharacteristic == nullptr)
    {
      Serial.println("Failed to find our characteristic");
      pClient->disconnect();
      return false;
    }

    pMyCharacteristic->registerForNotify(notifyCallback);
  
    isPeripheralConnected = true;
    return true;
  }

  void Setup()
  {
    Serial.println("Initializing BLE Central");

    BLEDevice::init("");
 
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    StartScanning();
  }

  void Loop()
  {
    if (tryToConnect)
    {
      if (ConnectToPeripheral(*pPeripheralAddress))
      {
        Serial.println("Connected to peripheral");

        // Activate the Notify property of the Characteristic
        pMyCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);     
      }
      else
      {
        Serial.println("Failed to connect to peripheral, start scanning again");
        StartScanning();
      }

      tryToConnect = false;
    }
    else
    {
      if (!isPeripheralConnected && !isScanning)
      {
        StartScanning();
      }
    }

    delay(100);
  }

} // namespace BLEServer

#endif  // _BLE_CENTRAL