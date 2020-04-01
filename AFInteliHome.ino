#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESPAsyncUDP.h>
//#include <ESPAsyncTCP.h>
#include "fauxmoESP.h"
#include "credentials.h"
#include "afwebpage.h"
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESP8266WiFiMulti.h>
#define IS_SSID   1
#define IS_PASSWD 2
#define IS_ALEXA  3

/*O server é async, aceita multiplas conexoes. O tratamento é feito com request.
  Na função serverSetup são tratados os callbacks do Alexa. A página de interação
  deve ser declarada dentro, mandando os dados pela URL.

  Objetivo:
  1 - Receber as credenciais por request
  2 - Salvar no spiffs ou EEPROM

  Se for SPIFFS, salvar ssid.txt e passwd.txt
  Se for EEPROM, guardar endereço 1 e endereço 2 do SSID e senha.

  Criar chave de acesso para configuração. O padrao será afeletronica em key.txt
  Se trocar a key e esquecer, só gravando o firmware novamente, melhor não mexer.
*/

ESP8266WiFiMulti WiFiMulti;

AsyncWebServer server(80);

using namespace fs;
// ws://ip/ws
AsyncWebSocket ws("/ws");
//envio de evento do servidor quando requisitado nesse local
AsyncEventSource events("/events");

bool shouldReboot = false;

fauxmoESP fauxmo;

void deleteFile(fs::FS &fs, char *filename);

void loadCredentials() {
  String recipe1 = readFile(LittleFS, "/ssid.txt");
  strcpy(WIFI_SSID,recipe1.c_str());

  String recipe2 = readFile(LittleFS, "/passwd.txt");
  strcpy(WIFI_PASS,recipe2.c_str());

  String recipe3 = readFile(LittleFS, "/alexa.txt");
  strcpy(ALEXA_COMMAND,recipe3.c_str());

  Serial.println("---------------------------");
  Serial.println("L I D O   D O   A R Q U I V O");
  Serial.println(WIFI_SSID);
  Serial.println(WIFI_PASS);
  Serial.println(ALEXA_COMMAND);
  Serial.println("---------------------------");
}

void setValuesToVars(uint8_t target, char *content) {
  if (target == IS_SSID) {
    writeFile(LittleFS, "/ssid.txt", content);
  }
  else if (target == IS_PASSWD) {
    writeFile(LittleFS, "/passwd.txt", content);
  }
  else if (target == IS_ALEXA) {
    writeFile(LittleFS, "/alexa.txt", content);
  }
}



//NAO ESQUECER DE ADICIONAR A BARRA ANTES DO NOME DO ARQUIVO
void writeFile(fs::FS &fs, char *filename, char *content) {
  Serial.println(content);
  File myFile = LittleFS.open(filename, "w");
  if (!myFile) {
    Serial.println("Problema ao tentar ler, sorry...");
    return;
  }

  if (myFile.print(content)) {
    Serial.println("conteudo salvo");
  }
  else {
    Serial.println("nao foi possivel salvar");
  }
  myFile.close();
}

String readFile(fs::FS &fs, char *filename) {
  const char *path = filename;
  Serial.printf("Reading file: %s\n", path);

  if (path[0] != '/') {
    Serial.println("File path needs start with /. Change it.");
    return "ouch! slash forgoten";
  }

  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open file for reading");
    return "ouch! You cant read it.";
  }

  Serial.println("Reading from file... ");
  char buf[50];
  memset(buf, 0, 50);

  while (file.available()) {
    file.readBytesUntil(0, buf, 49);
  }
  file.close();
  Serial.println(buf);
  return buf;
}

void deleteFile(fs::FS &fs, char *filename) {
  if (filename[0] != '/') {
    Serial.println("File path needs start with /. Change it.");
  }
  if (fs.remove(filename)) {
    return;
  }
}

void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  Serial.println("Implementar pagina aqui ou no 'login success?'");
}


void serverSetup() {

  // Custom entry point (not required by the library, here just as an example)
  //como nao eh utilizado pela lib, podemos jogar o index como config: (https://github.com/me-no-dev/ESPAsyncWebServer)
  //na pagina da lib tem exemplo de auth.
  /*
     Declarar a pagina antes dessa funcao usando progmem:
     const char index_html[] PROGMEM = "..."; // large char array, tested with 14k
     request->send_P(200, "text/html", index_html);

  */

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hello, world");
  });

  // These two callbacks are required for gen1 and gen3 compatibility
  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), String((char *)data))) return;
    // Handle any other body request here...
  });
  server.onNotFound([](AsyncWebServerRequest * request) {
    String body = (request->hasParam("body", true)) ? request->getParam("body", true)->value() : String();
    if (fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), body)) return;
    // Handle not found request here...
  });



  /*  CONFIGURACAO DE PAGINA DE LOGIN*/
  //---------------------------------------------------------------------------------------
  // attach AsyncWebSocket
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // attach AsyncEventSource
  server.addHandler(&events);

  server.on("/login",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send(200, "text/plain", "Login Ok");
  }
           );

  server.on("/config",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();

    //AsyncResponseStream *response = request->beginResponseStream("text/html");
    //response->print(config_html);
    //request->send(response);
    //text/plain ou text/html

    request->send_P(200, "text/html", login_html);
  }
           );

  server.on("/credenciais",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();

    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<html><body>SSID: ");
    response->print(String(WIFI_SSID));
    Serial.print("Isso tem que aparecer: ");
    Serial.println(WIFI_SSID);
    response->print("<br>");

    response->print("PASSWORD: ");
    response->print(String(WIFI_PASS));
    response->print("<br>");

    response->print("IDENTIFICADOR: ");
    response->print(String(ALEXA_COMMAND));
    response->print("<br></body></html>");
    
    request->send(response);
    //text/plain ou text/html

    request->send_P(200, "text/html", login_html);
  }
           );

  //http://192.168.1.207/config
  //?input_ssid=teste&input_passwd=um+&input_command=dois
  /*
     Acessando a url /config abre-se um formulario. Se não tiver ido para /login primeiro, será solicitado login aqui.
    Depois de clicar em submit, os dados sao enviados para /setup.
    os campos são:
    input_ssid
    input_passwd
    input_command

    Essas definicões estão no form, no arquivo afwebpage.h
  */
  server.on("/setup",
            HTTP_GET,
  [](AsyncWebServerRequest * request) {
    if (!request->authenticate(http_username, http_password))
      return request->requestAuthentication();

    String converter;
    if (request->hasParam("input_ssid")) {
      memset(WIFI_SSID, 0, 51);
      converter = request->getParam("input_ssid")->value();
      strcpy(WIFI_SSID, converter.c_str());
      Serial.println(WIFI_SSID);
      setValuesToVars(IS_SSID, WIFI_SSID);
    }

    if (request->hasParam("input_passwd")) {
      memset(WIFI_PASS, 0, 51);
      converter = request->getParam("input_passwd")->value();
      strcpy(WIFI_PASS, converter.c_str());
      Serial.println(WIFI_PASS);
      setValuesToVars(IS_PASSWD, WIFI_PASS);
    }

    if (request->hasParam("input_command")) {
      memset(ALEXA_COMMAND, 0, 51);
      strcpy(ALEXA_COMMAND, request->getParam("input_command")->value().c_str());
      Serial.println(ALEXA_COMMAND);
      setValuesToVars(IS_ALEXA, ALEXA_COMMAND);
    }

    request->send(200, "text/plain", "Aplicado. Reiniciando...");
    ESP.restart();

  }
           );


  //----------------------------------------------------------------------------------------




  // Start the server
  server.begin();

}

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE                 9600
#define RELE0                             0

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

  startCredentials();
  loadCredentials();
  // Set WIFI module to STA mode
  WiFi.mode(WIFI_AP_STA);
  WiFiMulti.addAP(AP_WIFI_SSID, AP_WIFI_PASS);

  // Connect
  Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait
  unsigned long int timeout = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    if ((millis()-timeout) > 15000){
      break; 
    }
    
  }
  Serial.println();
  delay(2000);

  // Connected!
  if (WiFi.status() == WL_CONNECTED){
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    Serial.println(WiFi.macAddress());
  }


}

void setup() {

  // Init serial port and clean garbage
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  Serial.println();

  // RELES
  pinMode(RELE0, OUTPUT);
  digitalWrite(RELE0, HIGH); // Our LED has inverse logic (high for OFF, low for ON)

  if (!LittleFS.begin()) {
    Serial.println("Couldn't mount the filesystem.");
  }
  if (LittleFS.exists("/ssid.txt") && LittleFS.exists("/passwd.txt") && LittleFS.exists("/alexa.txt")) {
    Serial.println("exists all 3 files!!!!!");
  }



  // Wifi
  wifiSetup();

  // Web server
  serverSetup();

  // Set fauxmoESP to not create an internal TCP server and redirect requests to the server on the defined port
  // The TCP port must be 80 for gen3 devices (default is 1901)
  // This has to be done before the call to enable()
  fauxmo.createServer(false);
  fauxmo.setPort(80); // This is required for gen3 devices

  // You have to call enable(true) once you have a WiFi connection
  // You can enable or disable the library at any moment
  // Disabling it will prevent the devices from being discovered and switched
  fauxmo.enable(true);

  // You can use different ways to invoke alexa to modify the devices state:
  // "Alexa, turn kitchen on" ("kitchen" is the name of the first device below)
  // "Alexa, turn on kitchen"
  // "Alexa, set kitchen to fifty" (50 means 50% of brightness)

  // Add virtual devices
  fauxmo.addDevice("luz da bancada");


  // You can add more devices
  //fauxmo.addDevice("light 3");
  //fauxmo.addDevice("light 4");
  //fauxmo.addDevice("light 5");
  //fauxmo.addDevice("light 6");
  //fauxmo.addDevice("light 7");
  //fauxmo.addDevice("light 8");

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {

    // Callback when a command from Alexa is received.
    // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
    // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
    // Just remember not to delay too much here, this is a callback, exit as soon as possible.
    // If you have to do something more involved here set a flag and process it in your main loop.

    // if (0 == device_id) digitalWrite(RELAY1_PIN, state);
    // if (1 == device_id) digitalWrite(RELAY2_PIN, state);
    // if (2 == device_id) analogWrite(LED1_PIN, value);

    Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

    // For the example we are turning the same LED on and off regardless fo the device triggered or the value
    //digitalWrite(RELE0, !state); // we are nor-ing the state because our LED has inverse logic.

    if (device_id == 0) digitalWrite(RELE0, !state); // we are nor-ing the state because our LED has inverse logic.

  });

}

void loop() {

  // fauxmoESP uses an async TCP server but a sync UDP server
  // Therefore, we have to manually poll for UDP packets
  fauxmo.handle();

  // This is a sample code to output free heap every 5 seconds
  // This is a cheap way to detect memory leaks
  static unsigned long last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
  }

  if ((WiFiMulti.run() == WL_CONNECTED)){
    uint8_t ok = 1;
  }

}
