#include "Arduino.h"

#include <WiFi.h> 
#include <AsyncTCP.h> 
#include <ESPAsyncWebServer.h> 

#define RX2 16
#define TX2 17

const char* ssid = "ESP32"; 
const char* password = "";

AsyncWebServer server(80); 
AsyncWebSocket ws("/ws");

String comdata = "";
String message = "";
String msg = "";

String index_html = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <title>BioSensor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html { font-family: Arial; text-align: center; }
    h2 { font-size: 2.0rem; }
    p { font-size: 2.0rem; }
    .value { font-weight: bold; color: darkblue; }
  </style>
</head>

<body>
  <h1>BIO SENSOR</h1>
  <p>
    <strong>Temperatura</strong> 
    <span id="temperature">%TEMP%</span>
  </p>
  
  <p>
    <strong>Umidade</strong>
    <span id="humidity">%HUMI%</span>
  </p>
</body>

<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;

  window.addEventListener('load', onLoad);

  function onLoad(event) {
    initWebSocket();
  }

  function initWebSocket() {
    websocket = new WebSocket(gateway);
    websocket.onopen = () => console.log('Conectado ao websocket');
    websocket.onclose = () => { 
      console.log('Tentando reconectar...');
      setTimeout(initWebsocket, 2000);
    }
    websocket.onmessage = (event) => {
      console.log("Dados recebidos: ", event.data);
      let data = JSON.parse(event.data);
      document.getElementById('temperature').innerHTML = data.temp;
      document.getElementById('humidity').innerHTML = data.humi;
    }
  }
</script>

</html>
)rawliteral";


void notifyClients() {
  //String json = "{\"message\": \"" + message + "\"}";
  //String json = "{\"temp\": \"30\", \"humi\": \"70\"}";
  ws.textAll(msg);
}

void setup() { 
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2); 

  WiFi.softAP(ssid, password); 
  Serial.println(WiFi.softAPIP());

  // WebSocket
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.println("Cliente conectado ao WebSocket");
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.println("Cliente desconectado");
    }
  });
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
    request->send(200, "text/html", index_html); 
  }); 

  server.begin(); 
} 

void loop() {
  if(comdata != ""){
    //message = comdata;
    //message.trim();
    notifyClients();
  }
  if(Serial2.available()) {
    msg = Serial2.readStringUntil('\n');
    Serial.println(msg);
    notifyClients();
  }
  comdata = "";
}

/*void serialEvent()
{
  comdata = "";
  while (Serial.available())
  {
    comdata += char(Serial.read());
    delay(2);
  }
}*/