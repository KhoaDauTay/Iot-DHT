#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
// Connect Wifi
const char *ssid = "Greenwich-Student";
const char *password = "12345678";
String server = "https://iot-dashboard-khoa.herokuapp.com/api/node/1/";
// Set up DHT Senor
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float t = 0.0;
float h = 0.0;
// Domain Name with full URL Path for HTTP PUT Request
unsigned long lastTime = 0;
unsigned long timerDelay = 20000;

void setup()
{
  Serial.begin(115200);
  dht.begin();
  // Wifi connect
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
}

void loop()
{
  
  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay)
  {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      float newT = dht.readTemperature();
      if (isnan(newT))
      {
        Serial.println("Failed to read from DHT sensor!");
      }
      else
      {
        t = newT;
        Serial.println(t);
      }
      // Read Humidity
      float newH = dht.readHumidity();
      // if humidity read failed, don't change h value
      if (isnan(newH))
      {
        Serial.println("Failed to read from DHT sensor!");
      }
      else
      {
        h = newH;
        Serial.println(h);
      };

      int temp = (int)t;
      int hum = (int)h;

      HTTPClient http;
      WiFiClientSecure client;
      client.setInsecure(); //the magic line, use with caution
      client.connect(server, 100); 
      http.begin(client, server);
      http.addHeader("Content-Type", "application/json");
      String httpRequestData = "{ \"temperature\" : " + String(temp) + " , \"humidity\" : " + String(hum) + ", \"moisture\" : " + String(temp) + ", \"light\" : " + String(hum) + " }";
      Serial.println(httpRequestData);
      int httpResponseCode = http.PUT(httpRequestData);
      if (httpResponseCode > 0)
      {

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }
      else
      {

        Serial.print("Error on sending PUT Request: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}