// A simple example of setting up a BLE Peripheral device to send a value to a BLE Central device
//
// Hardware:
//    XIAO ESP32-C3

#include "BLEPeripheral.h"

// The value that we'll send to Central
static unsigned char valueToSendToCentral = 0;

void setup()
{
  // Open hardware serial port to communicate with PC
  Serial.begin(115200);
  delay(200);
  Serial.println("\n\n********* BLEPeripheral with ESP32-C3 *********\n");

  BLEPeripheral::Setup();
}

void loop()
{
  if (BLEPeripheral::isCentralConnected)
  {
    BLEPeripheral::SendByteToCentral(valueToSendToCentral);

    // In this example we'll just update the value once per second
    delay(1000);
    valueToSendToCentral++;
  }
}
