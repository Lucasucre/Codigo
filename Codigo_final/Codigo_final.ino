
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
 
#define DHTPIN D4 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

DHT dht(DHTPIN, DHTTYPE);
  
const char* ssid = "xxxxx"; //Coloque o nome da sua Rede
const char* password = "xxxxx"; //Coloque a senha da sua Rede
const char* host = "smartcampus.inatel.br";   //Manter assim
const int httpsPort = 4433;   //Manter assim
const char* device_token = "rNRP827K_SF0WpM2zDCxaNXW";  //Altere para o token do Dispositivo
const char* device_id = "5d25394b6a4890001a850094";   //Altere para o id do device
const char* aplication_id = "5d2539326a4890001a850091"; //Altere para o id da aplicação

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println();
  Serial.print("connecting to ");
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

}

void post_https(String PostData){
   // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/api/userapps/"+String(aplication_id)+"/devices/"+String(device_id)+"/meas";
  
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.println(String("POST ") + url + " HTTP/1.1");
  client.println(String("Host: ") + host);
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/json");
  client.println("x-access-token: "+String(device_token));
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData);
  client.println();
  client.println("Connection: close");

 unsigned long timeout = millis();
  
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  String payload = "";
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    payload += line;
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  
}

void loop() {

  char t_buffer[10];
  
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  String temp=dtostrf(t,2,0,t_buffer);

    if (isnan(t)) // || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    //Serial.print(t);
    post_https("{\"temperature\":" + temp + "}");
    delay(20000);
  }

}
