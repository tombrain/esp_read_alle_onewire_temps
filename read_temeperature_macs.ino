

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS        4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#define TEMPERATURE_PRECISION 12
#define MAX_SENSORS 5

DeviceAddress Thermometer[MAX_SENSORS];
byte numberOfDevices;
byte Limit;
#if !defined(DEVICE_DISCONNECTED)
#define DEVICE_DISCONNECTED -127
#endif

void printAddress(DeviceAddress deviceAddress){
  Serial.print(F("{ "));
  for (uint8_t i = 0; i < 8; i++){
    // zero pad the address if necessary
    Serial.print(F("0x"));
    if (deviceAddress[i] < 16) {Serial.print("0");};
    Serial.print(deviceAddress[i], HEX);
    if (i<7) {Serial.print(F(", "));};
  };
  Serial.print(F(" }"));
};

void printMAC(DeviceAddress deviceAddress){
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) {Serial.print("0");};
    Serial.print(deviceAddress[i], HEX);
    if (i<7) {Serial.print("-");};
  };
};

void setup(void)
{
  Serial.begin(115200);
  // delay for terminal
  delay(2000);
  sensors.begin();

  Serial.print(F("Locating devices..."));
  numberOfDevices = sensors.getDeviceCount();
  Serial.print(F("Found "));
  Serial.print(numberOfDevices, DEC);
  Serial.println(F(" devices."));

  Serial.print(F("Parasite power is: ")); 
  if (sensors.isParasitePowerMode()){
    Serial.println(F("ON"));
  }
  else{
    Serial.println(F("OFF"));
  };
  if (numberOfDevices > MAX_SENSORS){
    Limit = MAX_SENSORS;
  }
  else{
    Limit = numberOfDevices;
  };
  for(byte index=0; index<Limit; index++){
    if(sensors.getAddress(Thermometer[index], index)){
      Serial.print(F("Found device "));
      Serial.print(index, DEC);
      Serial.print(F(" with address: "));
      printAddress(Thermometer[index]);
      Serial.println();

      Serial.print(F("Setting resolution to "));
      Serial.println(TEMPERATURE_PRECISION,DEC);
      sensors.setResolution(Thermometer[index], TEMPERATURE_PRECISION);
      delay(750/ (1 << (12-TEMPERATURE_PRECISION))); 
      Serial.print(F("Resolution actually set to: "));
      Serial.print(sensors.getResolution(Thermometer[index]), DEC); 
      Serial.println();
    }
    else{
      Serial.print(F("Found ghost device at "));
      Serial.print(index, DEC);
      Serial.print(F(" but could not detect address. Check power and cabling"));
    };
  };

  Serial.println(F("Getting temperatures..."));
}

float printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED) {
    Serial.print(F("Error getting temperature"));
  } 
  else {
    Serial.print(tempC);
    /*
    Serial.print(F(" F: "));
     Serial.print(DallasTemperature::toFahrenheit(tempC));
     */
  };
  return tempC;
};

void loop(void)
{ 
  float tempC;
  sensors.requestTemperatures(); //can be omitted?

  for(byte index=0; index<Limit; index++){
    Serial.print(F("Temperature "));
    printMAC(Thermometer[index]);
    Serial.print(F(" : "));
    tempC = printTemperature(Thermometer[index]);
    Serial.println(F(" \xB0\x43"));

    switch (TEMPERATURE_PRECISION){
    case 9:
      delay(94);
      break;
    case 10:
      delay(188);
      break;
    case 11:
      delay(375);
      break;
    case 12:
    default:
      delay(750);
      break;
    };
  };
  delay(10000);
};
