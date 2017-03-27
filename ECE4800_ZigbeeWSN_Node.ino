#include <Adafruit_Sensor.h>  //using DHT11 (temp & humidity)
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2  // Pin which is connected to the DHT sensor.
#define LIGHTPIN 0 //Pin light sensor is connected to
#define DHTTYPE DHT11 // DHT11 (digital humidity & temp sensor)

const int lightSensor = 0x00;
const int tempSensor = 0x01;
const int humidSensor = 0x02;
const int gasSensor = 0x03;

int lightVal, tempVal, humidVal, gasVal, pLength;
long checksum;

boolean hasGas = false;

DHT_Unified dht(DHTPIN, DHTTYPE);

//**************************************************
void setup() {
    Serial.begin(9600);
    dht.begin();
    sensor_t sensor;
}

//**************************************************
void loop() {
  lightVal = tempVal = humidVal = gasVal = 0;
  pLength = 20;

  //only router node has gas sensor due to power consumption
  if(hasGas) {
    gasVal = getGas();
    pLength += 3;
  }

  lightVal = getLight();
  tempVal = getTemp();
  delay(1000); //delay between readings (same device has both sensors)
  humidVal = getHumid();
  
  //printValues(lightVal, tempVal, humidVal, gasVal); //debugging only
  
  Serial.write(0x7E); //start byte
  Serial.write(0x00); //MSB - pLength
  Serial.write(pLength); //LSB - pLength
  Serial.write(0x90); //frame type: Zigbee Rx Packet
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //64-bit addr of sender
  Serial.write(0xFF); //16-bit addr of sender
  Serial.write(0xFF); //16-bit addr of sender
  Serial.write(0x40); //Receive options (0x40 - packet sent from end device)
  
  checksum = 0x90 + 0x40 + 0x22;
  checksum += 0x9F6;
  
  //start of Received FR data
  Serial.write(0x22); //received data type: sensor data

  Serial.write(lightSensor); //analog light - tells which sensor data comes next
  checksum += send16(lightVal); //send 2 bytes for analog light value
  
  checksum += send8(tempSensor); //digital temp - tells which sensor data comes next
  checksum += send8(tempVal); //1 byte for digital temp val
  
  checksum += send8(humidSensor); //digital humidity - tells which sensor data comes next
  checksum += send8(humidVal); //1 byte for digital humid val
  
  if(hasGas) {
    checksum += send8(gasSensor); //analog gas - tells which sensor data comes next
    checksum += send16(gasVal); //send 2 bytes for analog gas value
  }
  
  checksum = 0xFF - (checksum & 0xFF);
  Serial.write(checksum);
  
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
}//end getHumidity()

//**************************************************
int getGas() {
  int g1 = 0;
  return g1;
}

//**************************************************
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
int send16(int value) {
  //used to send 10 bit ADC as two bytes over serial
  //returns sum of two bytes for checksum
  int high, low;
  low = value & 0xFF;
  high = (value >> 8) & 0xFF;
  Serial.write(high); //send high byte first
  Serial.write(low); //send low byte
  value = high + low;
  return value;
}//end send16()

//**************************************************
int send8(int value) {
  //makes sending byte and calculating checksum condensed into one function
  //returns byte for checksum calculations
  Serial.write(value);
  return (value);
}//end send8()
