const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Configurador</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/setup">
    Rede (SSID): <input type="text" name="input_ssid">
    Senha: <input type="password" name="input_passwd">
    Alexa, ligar...: <input type="text" name="input_command">
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";
