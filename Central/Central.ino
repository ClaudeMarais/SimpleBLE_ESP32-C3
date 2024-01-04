// A simple example of setting up a BLE Central device to receive a value from a BLE Peripheral device
//
// Hardware:
//    XIAO ESP32-C3

#include "BLECentral.h"

void setup()
{
  // Open hardware serial port to communicate with PC
  Serial.begin(115200);
  delay(200);
  Serial.println("\n\n********* BLECentral with ESP32-C3 *********\n");

  BLECentral::Setup();
}

void loop()
{
  BLECentral::Loop();

  // NOTE: We can do something interesting with the received value, but in this simple example, we'll just check the Serial output for received notifications
  // if (BLECentral::isPeripheralConnected)
  // {
  //   unsigned char mostRecentPeripheralValue = BluetoothCentral::peripheralValue;
  // }
}


