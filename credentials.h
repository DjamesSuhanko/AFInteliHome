const char* http_username = "admin";
const char* http_password = "admin";

char WIFI_SSID[50]     = {0};
char WIFI_PASS[50]     = {0};
char ALEXA_COMMAND[50] = {0};
char AP_WIFI_SSID[50]  = {0};
char AP_WIFI_PASS[50]  = {0};

void startCredentials(){

  strcpy(WIFI_SSID,"laboratorio");
  strcpy(WIFI_PASS,"laboratorio123");
  strcpy(ALEXA_COMMAND,"sem nome");
  
  strcpy(AP_WIFI_SSID,"New device to Alexa");
  strcpy(AP_WIFI_PASS,"afeletronica");
}
