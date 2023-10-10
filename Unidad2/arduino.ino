#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHTesp.h"

#define DHTPIN 12 // Pin del ESP32 al que está conectado el sensor DHT11 (cambia según tu conexión)
#define DHTTYPE DHTesp::DHT11 //define que tipo de sensor de temperatura estamos utilizando

const char *ssid = "ULTRARED_1404"; 
const char *password = "144MR15M";

String serverName = "http://612a-45-188-167-254.ngrok-free.app";

unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

// Define los pines para los botones
const int buttonPinAsc = 27; // Cambia al número de pin que estás utilizando
const int buttonPinDesc = 33; // Cambia al número de pin que estás utilizando

const int ledPin = 13; //pin del led

bool ascRequested = false; // Bandera para controlar la solicitud "asc"
bool descRequested = false; // Bandera para controlar la solicitud "desc"


DHTesp dht;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a la red WiFi con dirección IP: ");
  Serial.println(WiFi.localIP());

  // Configura los pines de los botones como entradas
  pinMode(buttonPinAsc, INPUT_PULLUP);
  pinMode(buttonPinDesc, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  dht.setup(DHTPIN, DHTTYPE);

  Serial.println("Temporizador configurado a 5 segundos (variable timerDelay). Tomará 5 segundos antes de enviar la primera lectura.");
}

void loop() {
  // Verifica si el botón "asc" se ha presionado
  if (digitalRead(buttonPinAsc) == LOW && !ascRequested) {
    ascRequested = true;
  }

  // Verifica si el botón "desc" se ha presionado
  if (digitalRead(buttonPinDesc) == LOW && !descRequested) {
    descRequested = true;
  }
  // Leer temperatura y humedad
  float humedad = dht.getHumidity();
  Serial.println("Humedad: " + String(humedad) + "%");
  float temperatura = dht.getTemperature();
  Serial.println("Temperatura: " + String(temperatura) + "°C");

  // Enviar datos al servidor independientemente de las solicitudes de botones
  sendTemperatureToServer(temperatura);

  // Enviar datos al servidor independientemente de las solicitudes de botones
  sendHumedadToServer(humedad);

  delay(2000); // Esperar 2 segundos antes de tomar otra lectura

  HTTPClient http;

  String serverPathLed = serverName + "/led";

  http.begin(serverPathLed.c_str());

  // Send HTTP GET request
  int httpResponseCodeGET = http.GET();

  if (httpResponseCodeGET > 0) {
    Serial.print("Código de respuesta HTTP: ");
    Serial.println(httpResponseCodeGET);
    String payload = http.getString();
    Serial.println(payload);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    int led = doc["status"];
    Serial.println(led);

    if (led == false) {
      digitalWrite(ledPin, LOW);
    }
    if (led == true) {
      digitalWrite(ledPin, HIGH);
    }
  } else {
    Serial.print("Código de error: ");
    Serial.println(httpResponseCodeGET);
  }

  http.end();

  if (ascRequested || descRequested) {
    if ((millis() - lastTime) > timerDelay) {
      if (WiFi.status() == WL_CONNECTED) {
        String serverPath = serverName + "/led";

        http.begin(serverPath.c_str());

        DynamicJsonDocument jsonDoc(64);

        if (ascRequested) {
          jsonDoc["action"] = "asc";
          ascRequested = false; // Restablece la bandera después de la solicitud
        } else if (descRequested) {
          jsonDoc["action"] = "desc";
          descRequested = false; // Restablece la bandera después de la solicitud
        }
        jsonDoc["quantity"] = 1; // Cambia la cantidad según tu necesidad

        String jsonString;
        serializeJson(jsonDoc, jsonString);

        http.addHeader("Content-Type", "application/json");

        int httpResponseCode = http.POST(jsonString);

        if (httpResponseCode > 0) {
          Serial.print("Código de respuesta HTTP: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
          Serial.println(payload);
        } else {
          Serial.print("Código de error: ");
          Serial.println(httpResponseCode);
        }

        http.end();
      } else {
        Serial.println("WiFi Desconectado");
      }
      lastTime = millis();
    }
  }
}

void sendTemperatureToServer(float temperatura) {
  if (WiFi.status() == WL_CONNECTED) {
    String serverPath = serverName + "/temp";

    HTTPClient http;
    http.begin(serverPath.c_str());

    DynamicJsonDocument jsonDoc(64);
    jsonDoc["temperatura"] = temperatura;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.print("Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Código de error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Desconectado");
  }
}

void sendHumedadToServer(float humedad) {
  if (WiFi.status() == WL_CONNECTED) {
    String serverPath = serverName + "/hum";

    HTTPClient http;
    http.begin(serverPath.c_str());

    DynamicJsonDocument jsonDoc(64);
    jsonDoc["humedad"] = humedad;

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.print("Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Código de error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Desconectado");
  }
}