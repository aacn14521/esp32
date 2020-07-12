#include <Wire.h>
#include <SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define ESP32LOCALNAME "FJM0002"
#define SENSOR_SERVICE "181A"
#define CHARACTERISTIC_TEMP "2A6E"
#define CHARACTERISTIC_EC "2A58"

#define ONE_WIRE_BUS 32
#define SSD1306_ADDR 0x3c

BLECharacteristic *pCharacteristic_temp, *pCharacteristic_ec;

int ECPin = 34;
int ECGround = 25;
int ECPower = 33;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

SSD1306 display(SSD1306_ADDR, 21, 22, GEOMETRY_128_32);

void setup(void)
{
  Serial.begin(115200);
  pinMode(ECPin, ANALOG);
  pinMode(ECPower, OUTPUT);
  pinMode(ECGround, OUTPUT);
  digitalWrite(ECGround, LOW);

  delay(100);

  BLEDevice::init(ESP32LOCALNAME);
  BLEServer *pServer = BLEDevice::createServer();
  //pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SENSOR_SERVICE);
  pCharacteristic_temp = pService->createCharacteristic(
    CHARACTERISTIC_TEMP, BLECharacteristic::PROPERTY_READ);
  pCharacteristic_ec = pService->createCharacteristic(
    CHARACTERISTIC_EC, BLECharacteristic::PROPERTY_READ);
  pCharacteristic_temp->setValue("0");
  pCharacteristic_ec->setValue("0");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SENSOR_SERVICE);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();
  sensors.begin();

  display.init();    //ディスプレイを初期化
  display.setFont(ArialMT_Plain_10);   //フォントを設定
  display.displayOn();
}

/*
 * Main function, get and show the temperature
 */
void loop(void)
{ 

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    char temp_str[256],ec_str[256];
    float EC = getEC(tempC);
    sprintf(temp_str, "Water temp: %.1f*C", tempC);
    sprintf(ec_str, "EC: %.3fmS", EC);
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(temp_str);
    Serial.print("EC is:");
    Serial.println(ec_str);
    display.clear();
    display.drawString(0, 0, temp_str);
    display.drawString(0, 16, ec_str);
    display.display();   //指定された情報を描画

  sprintf(temp_str, "%.1f", tempC);
  sprintf(ec_str, "%.1f", EC);
  pCharacteristic_temp->setValue(temp_str);
  pCharacteristic_ec->setValue(ec_str);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
    display.drawString(0, 0, "Temp Error");
    display.display();   //指定された情報を描画
  }
  delay(10000);
}

float getEC(float temp){
  int R1 = 1000;
  int Ra = 22;


  float PPMconversion = 0.5;
  float TempCoef = 0.02;
  float K = 2.88;

  float EC = 0;
  float EC25 = 0;
  int ppm = 0;
  float raw = 0;
  float Vin = 3.3;
  float Vdrop = 0;
  float Rc = 0;
  
  digitalWrite(ECPower, HIGH);
  raw = analogRead(ECPin);
  delay(2);
  raw = analogRead(ECPin);
  digitalWrite(ECPower, LOW);
  Vdrop = (Vin * raw) / 4096.0;
  Rc = (Vdrop * (R1 - Ra)) / (Vin - Vdrop);
  Rc -= Ra;
  EC = 1000 / (Rc * K);
  EC25 = EC / (1 + TempCoef * (temp - 25.0));
  ppm = (EC25) * (PPMconversion * 1000);

  return EC25;
}
