#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <Wire.h>
#include <SHTSensor.h>

#define ESP32LOCALNAME "FJM0001"
#define SENSOR_SERVICE "181A"
#define CHARACTERISTIC_TEMP "2A6E"
#define CHARACTERISTIC_HUMIDITY "2A6F"

SHTSensor sht;

BLECharacteristic *pCharacteristic_temp_compost, *pCharacteristic_humidity_compost;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
    digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
    };

    void onDisconnect(BLEServer* pServer) {
    }
};

void setup() {
  pinMode(4, OUTPUT);

  Wire.begin(21, 22);
  Serial.begin(115200);
  delay(1000);

  BLEDevice::init(ESP32LOCALNAME);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService_compost = pServer->createService(SENSOR_SERVICE);
  pCharacteristic_temp_compost = pService_compost->createCharacteristic(
    CHARACTERISTIC_TEMP, BLECharacteristic::PROPERTY_READ);
  pCharacteristic_humidity_compost = pService_compost->createCharacteristic(
    CHARACTERISTIC_HUMIDITY, BLECharacteristic::PROPERTY_READ);  

  pCharacteristic_temp_compost->setValue("0");
  pCharacteristic_humidity_compost->setValue("0");

  pService_compost->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SENSOR_SERVICE);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  if(sht.init()){
    Serial.print("init(): success\n");
  }
  else{
    Serial.print("init(): failed\n");
  }
  delay(1000);
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
  delay(1000);
  Serial.println("Bluetooth device active, waiting for connections...");
}
 
void loop() {
  char temp_str[16];
  char humidity_str[16];

  if(sht.readSample()){
    Serial.print("SHT:\n");
    Serial.print("  RH: ");
    Serial.print(sht.getHumidity(), 2);
    Serial.print(" %\n");
    Serial.print("  T:  ");
    Serial.print(sht.getTemperature(), 2);
    Serial.print(" *C\n");
  }
  else{
    Serial.print("Error in readSample()\n");
  }
  sprintf(temp_str, "%.1f", sht.getTemperature());
  sprintf(humidity_str, "%.1f", sht.getHumidity());
  pCharacteristic_temp_compost->setValue(temp_str);
  pCharacteristic_humidity_compost->setValue(humidity_str);
  delay(60000);
}
