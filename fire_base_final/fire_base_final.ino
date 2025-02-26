/*
  Project: Send Data to Firebase Using Arduino Uno WiFi Rev2
  Board: Arduino Uno WiFi Rev2
   
  External libraries:
  - Arduino_LSM6DS3 by Arduino V1.0.0
  - Firebase Arduino based on WiFiNINA by Mobizt V1.1.4
 */

#include <Firebase_Arduino_WiFiNINA.h>
#include "DHT.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#define FIREBASE_HOST "sensor-9c153-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "qt7NWmOrk8oqNaeYa3fcyUrfg1bM1btHayaJydD0"
#define WIFI_SSID "kki"
#define WIFI_PASSWORD "0907308446"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

FirebaseData firebaseData;
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial pmsSerial(4, 3);

String path1 = "/DHT_11";
String jsonStr1;
String path2 = "/PMS_5003";
String jsonStr2;
int currentIndex = 0;
const int DATA_SIZE = 6;



void dht_SET(){
   //DHT11 sensor-----------------------------------------
  
  delay(1000);
  Serial.println();

  Serial.print("Initialize DHT11 sensor...");
  dht.begin();
  
  //sensor data
  Serial.println(" done");
  Serial.print("Humidity = ");
  Serial.println(dht.readHumidity());
  Serial.print("Temperature(C) = ");
  Serial.println(dht.readTemperature());
  Serial.print("Temperature(F) = ");
  Serial.println(dht.readTemperature(true));
  //sensor data
  //DHT11 sensor-------------------------------------------

}
void pms_SET(){
   //pms5003------------------------------------------------
   // our debugging output監控序列port
  pmsSerial.begin(9600);
  Serial.print("Initialize PMS5003 sensor...");
  // sensor baud rate is 9600 感測器序列port
  Serial.println(" done");
  Serial.print("PM 2.5 = "); Serial.println("warming up");
  Serial.print("PM 10 = "); Serial.println("warming up");
 

  
  //pms5003--------------------------------------------------

}
void setup()
{
  Serial.begin(115200);
  pms_SET();
  dht_SET();
  //connect wifi-------------------------------------------
  Serial.print("Connecting to WiFi...");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  //connect wifi------------------------------------------------


  //connect firebase--------------------------------------------
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
  //connect firebase---------------------------------------------
}


//pms5003結構-----------------------------------------------------
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
//------------------------------------------------------------------
struct pms5003data data;

void getPMS(){
    //PMS5003 firebase------------------------------------------------------------
   
      //float PM_25,PM_100;
      //PM_25 = data.pm25_env;
      //PM_100  = data.pm100_env;
      // Send data to Firebase with specific path
   // if( readPMSdata(&Serial))        // call it many times to collect data
    //{  
     // float PM25_in60sec[DATA_SIZE] = {0,0,0,0,0,0};
    float PM100_in60sec[DATA_SIZE]  = {0,0,0,0,0,0};
 
    Serial.println(i)  ;
    
        if (Firebase.setFloat(firebaseData, path2 + "/3-setFloat/PM_25", data.pm25_env)) {
          Serial.println(firebaseData.dataPath() + " = " + data.pm25_env);
        }
        if (Firebase.setFloat(firebaseData, path2 + "/3-setFloat/PM_100", data.pm100_env)) {
          Serial.println(firebaseData.dataPath() + " = " + data.pm100_env);
        }
      PM25_in60sec[i] = data.pm25_env;
      PM100_in60sec[i] =data.pm100_env;

     if (i == 5) {
      // Create JSON object
      DynamicJsonDocument jsonDoc(512);
      JsonArray PM25_Array = jsonDoc.createNestedArray("PM25");
      JsonArray PM100_Array = jsonDoc.createNestedArray("PM100");
     

      for (int j = 0; j < DATA_SIZE; j++) {
        PM25_Array.add(PM25_in60sec[i]);
        PM100_Array.add( PM100_in60sec[i]);
      }
      
      // Convert JSON object to string
      String jsonString;
      serializeJson(jsonDoc, jsonString);
  
      // Push data using pushJSON
      if (Firebase.pushJSON(firebaseData, path2 + "/4-pushJSON", jsonString)) {
        Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
      } else {
        Serial.println("Error: " + firebaseData.errorReason());
      }
    }
    
    }
  }
  
void loop()
{
  //if (readPMSdata(&pmsSerial)) {
    //Serial.print("PM 2.5: "); Serial.print(data.pm25_env);
    //Serial.print("PM 10: "); Serial.println(data.pm100_env);
  //}
    //getPMS();
    
    //delay(2000);
    if (readPMSdata(&pmsSerial)) {
    // reading data was successful!
    getPMS();
    getDHT();
  

  }
    //Serial.println();
    //delay(2000);//it can check the sensor values every 2 seconds 

}

void getDHT(){
  float H_in60sec[DATA_SIZE] = {0,0,0,0,0,0};
  float C_in60sec[DATA_SIZE]  = {0,0,0,0,0,0};
  float F_in60sec[DATA_SIZE] = {0,0,0,0,0,0};
  for(int i = 0;i<6;i++)
  {
  Serial.println(i)  ;
  float  H = dht.readHumidity() ;
   float F = dht.readTemperature(true) ;
   float C = dht.readTemperature() ;
//read humi&temp and put into variables
   
    // Send data to Firebase with specific path
    if (Firebase.setFloat(firebaseData, path1 + "/1-setFloat/H", H)) {
      Serial.println(firebaseData.dataPath() + " = " + H);
    }
    if (Firebase.setFloat(firebaseData, path1 + "/1-setFloat/C", C)) {
      Serial.println(firebaseData.dataPath() + " = " + C);
    }
    if (Firebase.setFloat(firebaseData, path1 + "/1-setFloat/F", F)) {
      Serial.println(firebaseData.dataPath() + " = " + F);
    }
    //改成陣列
    H_in60sec[i] = H;
    C_in60sec[i] = C;
    F_in60sec[i] = F;
 
      if (i == 5) {
      // Create JSON object
      DynamicJsonDocument jsonDoc(512);
      JsonArray hArray = jsonDoc.createNestedArray("H");
      JsonArray cArray = jsonDoc.createNestedArray("C");
      JsonArray fArray = jsonDoc.createNestedArray("F");

      for (int j = 0; j < DATA_SIZE; j++) {
        hArray.add(H_in60sec[j]);
        cArray.add(C_in60sec[j]);
        fArray.add(F_in60sec[j]);
      }
      
      // Convert JSON object to string
      String jsonString;
      serializeJson(jsonDoc, jsonString);
  
      // Push data using pushJSON
      if (Firebase.pushJSON(firebaseData, path1 + "/2-pushJSON", jsonString)) {
        Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
      } else {
        Serial.println("Error: " + firebaseData.errorReason());
      }
    }
    
  }
  }
  
  
  

//pms5003讀取資料-----------------------------------------------------------
boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
 
  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}
//--------------------------------------------------------------------------------
