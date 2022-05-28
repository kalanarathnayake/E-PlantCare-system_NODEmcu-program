#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
//sensor library
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <BH1750.h>

//Initializing ports
#define DHTTYPE DHT11  // temperature and humidity sensor 
#define dht_dpin 12 

//Defining ports
DHT dht(dht_dpin, DHTTYPE); 
BH1750 lightMeter(0x23);
//Capasitive moist sensor pin initialization
int sensor_pin = A0; 

//Firebase and WIFI info
#define FIREBASE_HOST "nodemcu-8bd8c-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "gHkRCKjR1zSWhwl40OAqgK6AxXJuHXkj1XXwcPCb"  
#define WIFI_PASSWORD "QBRGN13N47J"    //"Abcd1234" "QBRGN13N47J" "12345678"
#define WIFI_SSID "Dialog 4G"          //"SLT_FIBER_fSn3h" "Dialog 4G" "KG2"

void setup()
{ 
//  start the sensors
  dht.begin();
  Wire.begin(D3, D4);

//  Start the serial monitor
  Serial.begin(9600);
  
  Serial.println("Humidity and temperature\n\n");
  delay(100);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("connecting");
  
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Not connected!");
    delay(1000);
  }
  Serial.println();
  Serial.println("connected: ");
  Serial.println(WiFi.localIP());

  //connecting the firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  if(Firebase.failed())
  {
    Serial.println(Firebase.error());
  }
  else{
    Serial.println("Firebase Connected");
  }

  // begin returns a boolean that can be used to detect setup problems.
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else{
    Serial.println(F("Error initialising BH1750"));
  }
}

void loop()
 {
  float h = dht.readHumidity();// Reading temperature or humidity takes about 250 milliseconds!
  float t = dht.readTemperature();// Read temperature as Celsius (the default)
  float lux = lightMeter.readLightLevel();
  float m = analogRead(sensor_pin);
  
  //Read the Light intencity Sensor in every second
  if (lightMeter.measurementReady()) {
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");
  }

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {  
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  m = map(m,550,0,0,100);
    
  //convert integers 
  String fireHumid = String(h);  
  String fireTemp = String(t);  
  String LightIntence = String(lux);   
  String output_value= String(m);

  Serial.print("Humidity: ");  Serial.print(h);
  Serial.print("%  Temperature: ");  Serial.print(t);  Serial.print("°C ");
  Serial.print("%  LightIntencity: ");  Serial.print(lux);  Serial.print("lx");
  Serial.print("%  Soil Moisture: ");  Serial.print(output_value);  Serial.println("%");

  delay(100);

  //Sending the data to the firebase realtime database
  Firebase.setString("/DHT11/Humidity", fireHumid);
  Firebase.setString("/DHT11/Temperature", fireTemp);
  Firebase.setString("/GY30/LightIntence", LightIntence);
  Firebase.setString("/CapacitiveSoilMoist1_2v/soilMoist", output_value);

}
