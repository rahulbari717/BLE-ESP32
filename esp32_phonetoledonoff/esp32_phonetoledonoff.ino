/*
   This is Esp32 BLE Receive data from phone.
   run this code then open serial monitor 
   press en button on esp32 open nrf connect app scan and connect. 
*/
//Libraries we needed for BLE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define LED_BUILTIN 2

//BLE charcterstics 
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;

//Defining the services and charcterstics 

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" //UART service UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
class MyServerCallbacks: public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
       deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer)
    {
       deviceConnected = false;
    }
};
class MyCallbacks: public BLECharacteristicCallbacks 
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();
    if(rxValue.length()> 0)
    {
        Serial.println("=====START RECEIVE====");
        Serial.print("Received Value: ");

        for(int i =0;i<rxValue.length();i++)
          {
            Serial.print(rxValue[i]);
          }
        Serial.println();
        //Do stuff based on command received from app 
        if(rxValue.find("1") != -1)
        {
           Serial.println("Turning ON!");
           digitalWrite(LED_BUILTIN,HIGH);
        }
        else if(rxValue.find("0") != -1)
        {
          Serial.println("Turning OFF !");
          digitalWrite(LED_BUILTIN,LOW);
        }

      Serial.println();
      Serial.println("====END RECEIVE====");
      
    }
  }
};
void setup(void) 
{
  Serial.begin(9600);
//  pinMode(LED_BUILTIN, OUTPUT); //gives error 
  Serial.println("Starting BLE work!");
  
  //Create the BLE device 
  BLEDevice::init("Rahul ESP32(BLE)");//Bluetooth Name 
  
  //used  for create BLE server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  //Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  //Create BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_TX,
                                        // BLECharacteristic::PROPERTY_READ 
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );

//BLE needed to notify 
  pCharacteristic->addDescriptor(new BLE2902());
  
  //Characteristic for the receiving end
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX, 
    BLECharacteristic::PROPERTY_WRITE
    );
    
  pCharacteristic->setCallbacks(new MyCallbacks());
  //start the service ;
  pService->start();
  
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("open nrf connect app scan and connect!");
  Serial.println("Characteristic defined! Now you can read it in your phone!");

}


void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
