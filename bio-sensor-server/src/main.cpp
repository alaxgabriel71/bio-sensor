#include "Arduino.h"

#include <WiFi.h> 
#include <AsyncTCP.h> 
#include <ESPAsyncWebServer.h> 

#define led_alert 15

const char* ssid = "ESP32"; 
const char* password = "";


AsyncWebServer server(80); 
AsyncWebSocket ws("/ws");

String comdata = "";
String alert = "\"alert\": \"1\"";

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
    <span>🌡Temperatura</span> 
    <strong id="temperature"></strong>
  </p>
  
  <p>
    <span>💧Umidade</span>
    <strong id="humidity"></strong>
  </p>

  <div id="fire-alert">
    <h2>🔥ALERTA DE INCÊNDIO🔥</h2>
  </div>

  <div id="temp-alert">
    <h2>🔥🌡️ALERTA DE ALTA TEMPERATURA🌡️🔥</h2>
  </div>

  <div id="humi-alert">
    <h2>⬇️💧ALERTA DE BAIXA UMIDADE💧⬇️</h2>
  </div>
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
      //let data = JSON.parse(event.data);
      let data;
      try {
        data = JSON.parse(event.data);
      } catch(err) {
        console.warn("Pacote inválido!", event.data);
        return;
      }

      let alert = (typeof data.alert !== "undefined") ? data.alert : 0;
      let temp  = (typeof data.temp  !== "undefined") ? data.temp  : 0;
      let humi  = (typeof data.humi  !== "undefined") ? data.humi  : 100;

      document.getElementById('temperature').innerHTML = `${temp} °C`;
      document.getElementById('humidity').innerHTML = `${humi} %`;

      document.getElementById('fire-alert').hidden = alert != 1;
      document.getElementById('temp-alert').hidden = temp < 50;
      document.getElementById('humi-alert').hidden = humi > 30;
    }
  }
</script>

</html>
)rawliteral";


void notifyClients() {
  ws.textAll(comdata);
}

void setup() { 
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2);

  pinMode(led_alert, OUTPUT);
  digitalWrite(led_alert, LOW);

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
  if(Serial2.available()) {
    comdata = Serial2.readStringUntil('\n');
    comdata.trim();
    if(comdata.length() > 0 && comdata.startsWith("{")){
      Serial.println(comdata);
      notifyClients();
    }
  }
  if(comdata.indexOf(alert) == -1){
    digitalWrite(led_alert, LOW);
  } else {
    digitalWrite(led_alert, HIGH);
  }
}
