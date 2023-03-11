#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
//#define _DISABLE_TLS_


#include "time.h"
const char* ntpServer = "pool.ntp.org";
float epochTime; 
float getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}
//DHT11
#include "DHT.h"
#define DHT11PIN 16
DHT dht(DHT11PIN, DHT11);

//Soilmoisture sensor
#define AOUT_PIN 34
const int AirValue = 2990;
const int WaterValue = 1067;
int soilmoisturepercent=0;

//MQ135 sensor
#include "MQ135.h"
#define pinA 33
MQ135 senzorMQ = MQ135(pinA);

//BH1750
#include <Wire.h>
#include <BH1750.h>
BH1750 lightMeter(0x23);

//BME280
#include <Adafruit_BMP280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP280 bmp(0x76);

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
//WiFi
#define WIFI_SSID "Deco"
#define WIFI_PASSWORD "e9x84rtknphst5bf"
//Firebase
#define API_KEY "AIzaSyCinHGYcCWxVMi9TgJMkrd6yNye-PoJMGo"
#define DATABASE_URL "https://chytre-zemedelstvi-bp-default-rtdb.europe-west1.firebasedatabase.app/" 
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  dht.begin();
  lightMeter.begin();
  bmp.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  configTime(0, 0, ntpServer);

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


}

void loop() {
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("ºC ");
  Serial.print("Humidity: ");
  Serial.println(humi);

  int hum_soil = analogRead(AOUT_PIN);
  soilmoisturepercent = map(hum_soil, AirValue, WaterValue, 0, 100);
  Serial.print("Moisture value: ");
  Serial.println(soilmoisturepercent);

  float ppm = senzorMQ.getPPM();
  Serial.print("Koncentrace plynů: ");
  Serial.print(ppm);

//-----------------------------------------------------------------------
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

//--------------------------------------------------------------------
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  float pressure = (bmp.readPressure() / 100.0F);
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  delay(3000);

//Sending values to Firebase
if (Firebase.RTDB.setFloat(&fbdo, "DHT11/temp", temp)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
if (Firebase.RTDB.setFloat(&fbdo, "DHT11/hum", humi)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
if (Firebase.RTDB.setFloat(&fbdo, "soil_sensor/soil_hu", soilmoisturepercent)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
if (Firebase.RTDB.setFloat(&fbdo, "BH1750/lux", lux)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
if (Firebase.RTDB.setFloat(&fbdo, "BMP280/pressure", pressure)){
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
if (Firebase.RTDB.setFloat(&fbdo, "MQ135/quality", ppm)){
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }

  epochTime = getTime();
  Firebase.RTDB.setFloat(&fbdo, "DHT11/measures/" + epochTime);
  Firebase.RTDB.setFloat(&fbdo, "DHT11/measures/temp", temp);
  Firebase.RTDB.setFloat(&fbdo, "DHT11/measures/hum", humi);
  Firebase.RTDB.setFloat(&fbdo, "DHT11/measures/timestamp", epochTime);
  delay(30000);
}