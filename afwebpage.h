const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Configurador</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/setup">
    <b>Sua rede (SSID):</b> <input type="text" name="input_ssid"><br>
    <b>Senha da sua rede:</b> <input type="text" name="input_passwd"><br>
    <b>Nome para rede de configuração:</b><input type="text" name="input_apssid"><br>
    <b>Senha para a rede de configuração:</b><input type="text" name="input_appasswd"><br>
    <b>Chamada para Alexa ("Alexa, ligar..."):</b> <input type="text" name="input_command">
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";
