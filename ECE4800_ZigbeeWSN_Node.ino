#include <Adafruit_Sensor.h>  //using DHT11 (temp & humidity)
#include <DHT.h>
#include <DHT_U.h>

//**************************************************
//**************************************************
//Configuration settings
//These values can be adjusted depending on hardware configuration
#define DHTPIN 2  // Pin which is connected to the DHT sensor.
#define LIGHTPIN 0 //Pin light sensor is connected to
#define DHTTYPE DHT11 // DHT11 (digital humidity & temp sensor)
boolean hasGas = false;
//**************************************************
//**************************************************

const int lightSensor = 0x00;
const int tempSensor = 0x01;
const int humidSensor = 0x02;
const int gasSensor = 0x03;

int lightVal, tempVal, humidVal, gasVal;
DHT_Unified dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    dht.begin();
    sensor_t sensor;
}

void loop() {
  lightVal = tempVal = humidVal = gasVal = 0;

  //only router node has gas sensor due to power consumption
  if(hasGas) {
    gasVal = getGas();
  }

  lightVal = getLight();
  tempVal = getTemp();
  delay(1000); //delay between readings (same device has both sensors)
  humidVal = getHumid();
  
  //printValues(lightVal, tempVal, humidVal, gasVal); //debugging only

  //start of Received FR data
  Serial.write(0x22); //FR data type indicator - arduino sensor data
  Serial.write(lightSensor); //indicates from which sensor data originates
  Send16(lightVal); //send 2 bytes for analog data
  Serial.write(tempSensor); //indicates from which sensor data originates
  Serial.write(tempVal); //send 1 byte for digital data
  Serial.write(humidSensor); //indicates from which sensor data originates
  Serial.write(humidVal); //send 1 byte for digital data
  
  if(hasGas) {
    Serial.write(gasSensor); //indicates from which sensor data originates
    Send16(gasVal); //send 2 bytes for analog data
  }
  
  delay(5000); //need to change to sleep mode instead of delay
} //end main loop
  
//**************************************************
int getLight() {
  int l1;
  l1 = analogRead(LIGHTPIN);
  return l1;
}//end getLight()

//**************************************************
int getTemp() {
  int t1;
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    t1 = -100;
  }
  else {
    t1 = event.temperature;
  }
  return t1;
}//end getTemp()

//**************************************************
int getHumid() {
  int h1;
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.temperature)) {
    h1 = -100;
  }
  else {
    h1 = event.relative_humidity;
  }
  return h1;
}//end getHumid()

//**************************************************
int getGas() {
  int g1 = 0;
  return g1;
}//end getGas()

//**************************************************
//for debugging purposes only
void printValues(int light, int temp, int humid, int gas) {
  Serial.print("L:");
  Serial.print(light);
  Serial.print("  T:");
  Serial.print(temp);
  Serial.print("*C");
  Serial.print("  T:");
  Serial.print( (temp * 9 / 5) + 32 );
  Serial.print("*F");
  Serial.print("  H:");
  Serial.print(humid);
  Serial.print("%");
  Serial.print("  G:");
  Serial.println(gas);
}//end printValues()

//**************************************************
int Send16(int value) {
  //Purpose is to send value as two bytes to serial
  //Used primarilly to send 10 bit ADC data
  int high, low;
  low = value & 0xFF;
  high = (value >> 8) & 0xFF;
  Serial.write(high); //send high byte first
  Serial.write(low); //send low byte
}//end Send16()
