#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DHT_TYPE DHT11   // DHT11 Sensor Type
#define DHT_PIN D5 // Pin for Temperature Sensor
#define PIR_PIN D4   // Pin for Motion Sensor
#define TRIGGER_PIN D3  // Trigger pin - Ultrasonic Sensor
#define ECHO_PIN D2  // Echo pin - Ultrasonic Sensor

DHT dht(DHT_PIN, DHT_TYPE);
const char* ssid     = "Yusuf";
const char* password = "yusuf@567";
const String hostname = "http://172.20.10.4";
const String port = "8080";
const String tempEndpoint = "tempsensor";
//const String motionPort = "8081";
const String motionEndpoint = "motionsensor";
//const String ultrasonicPort = "8082";
const String ultrasonicEndpoint = "ultrasonicsensor";

String requestUrlTemperature = hostname + ":" + port + "/" + tempEndpoint;
String requestUrlMotion = hostname + ":" + port + "/" + motionEndpoint;
String requestUrlUltrasonic = hostname + ":" + port + "/" + ultrasonicEndpoint;

HTTPClient tempHttp;
HTTPClient motionHttp;
HTTPClient ultrasonicHttp;
int httpCode = -1;
String response = "";
int pirValue;
long duration;
int distance;

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  dht.begin();

  // Call REST API
  tempHttp.begin(requestUrlTemperature);
  Serial.println("POST URL>" + requestUrlTemperature + "<");
  motionHttp.begin(requestUrlMotion);
  Serial.println("POST URL>" + requestUrlMotion + "<");
  ultrasonicHttp.begin(requestUrlUltrasonic);
  Serial.println("POST URL>" + requestUrlUltrasonic + "<");

  // Motion Sensor
  pinMode(PIR_PIN, INPUT);

  // Ultrasonic sensor
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
}

void loop() {

  Serial.println("\n<--------------------------------------->");
  Serial.println("\n<----START--------START-------START----->");
  Serial.println("\n<--------------------------------------->");
  
  // Wait a few seconds between measurements.
  delay(1000);

  Serial.println("\n<--------------------------------------->");
  Serial.println("\n<----TEMPERATURE--------HUMIDITY-------->");
  Serial.println("\n<--------------------------------------->");

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  // Temperature Request Body
  String requestBody = "{\"temperature\":\""+ (String)t +"\",\"humidity\":\""+ (String)h +"\",\"timestamp\":1500285818}";
  httpCode = tempHttp.POST(requestBody);
  if(httpCode > 0) {
      response = tempHttp.getString();
      Serial.printf("response code:%d",httpCode);
      Serial.println(response);

  } else {
      Serial.printf("POST... failed, error: %s\n\n", tempHttp.errorToString(httpCode).c_str());
  }
  tempHttp.end();

  Serial.println("\n<--------------------------------------->");
  Serial.println("\n<----MOTION-----MOTION-----MOTION------->");
  Serial.println("\n<--------------------------------------->");

  pirValue = digitalRead(PIR_PIN);
  Serial.print("Motion Sensor Output: " + (String)pirValue);

  // Intrusion Request Body
  requestBody = "{\"intrusion\":\"" + (String)pirValue + "\"}";
  Serial.print("\nMotion Sensor Request Body: " + requestBody);
  httpCode = motionHttp.POST(requestBody);
  if(httpCode > 0) {
      response = motionHttp.getString();
      Serial.printf("response code:%d",httpCode);
      Serial.println(response);

  } else {
      Serial.printf("POST... failed, error: %s\n\n", motionHttp.errorToString(httpCode).c_str());
  }
  motionHttp.end();

  Serial.println("\n<--------------------------------------->");
  Serial.println("\n<------ULTRASONIC------ULTRASONIC------->");
  Serial.println("\n<--------------------------------------->");

  // Clears the trigPin
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  Serial.print("\nDuration: " + (String)duration);
  // Calculating the distance
  distance= duration*0.034/2;

  // Ultrasonic Distance Request Body
  requestBody = "{\"distance\":\"" + (String)distance + "\"}";
  Serial.print("\nUltrasonic Sensor Request Body: " + requestBody);
  httpCode = ultrasonicHttp.POST(requestBody);
  if(httpCode > 0) {
      response = ultrasonicHttp.getString();
      Serial.printf("response code:%d",httpCode);
      Serial.println(response);

  } else {
      Serial.printf("POST... failed, error: %s\n\n", ultrasonicHttp.errorToString(httpCode).c_str());
  }
  ultrasonicHttp.end();
  
  Serial.println("\n<--------------------------------------->");
  Serial.println("\n<------END--------END---------END------->");
  Serial.println("\n<--------------------------------------->");
}
