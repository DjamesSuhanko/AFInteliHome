const char login_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
    <style>
input[type=text], select {
  width: 100%;
  padding: 12px 20px;
  margin: 8px 0;
  display: inline-block;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
}

input[type=submit] {
  width: 100%;
  background-color: #4CAF50;
  color: white;
  padding: 14px 20px;
  margin: 8px 0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

input[type=submit]:hover {
  background-color: #45a049;
}

div {
  border-radius: 5px;
  background-color: #f2f2f2;
  padding: 20px;
}
</style>
  <title>Configurador</title>
  <meta name="viewport" content="width=device-width, initial-scale=1" charset="UTF-8">
  </head><body>
  <form action="/setup">
    <label for="fname">Sua rede (SSID):</label> <input type="text" name="input_ssid"><br>
    <label for="fname">Senha da sua rede:</label> <input type="text" name="input_passwd"><br>
    <label for="fname">Nome para rede de configuracao:</label><input type="text" name="input_apssid"><br>
    <label for="fname">Senha para a rede de configuracao:</label><input type="text" name="input_appasswd"><br>
    <label for="fname">Chamada para Alexa ("Alexa, ligar..."):</label> <input type="text" name="input_command"><br>
    <input type="submit" value="Submit">
  </form><br>
</body></html>)rawliteral";
