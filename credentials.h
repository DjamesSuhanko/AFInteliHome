#define IS_SSID     1
#define IS_PASSWD   2
#define IS_APSSID   3
#define IS_APPASSWD 4
#define IS_ALEXA    5
#define IS_ADMIN_U  6
#define IS_ADMIN_P  7

char WIFI_SSID[50]     = {0};
char WIFI_PASS[50]     = {0};
char ALEXA_COMMAND[50] = {0};
char AP_WIFI_SSID[50]  = {0};
char AP_WIFI_PASS[50]  = {0};
char LOGIN_USER[50]    = {0};
char LOGIN_PASS[50]    = {0};

unsigned long int  HTTP_PORT = 80;

IPAddress staticIP(10, 0, 0, 1); //ESP static ip
IPAddress gateway(0, 0, 0, 0);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 0, 0, 0);  //Subnet mask

void startCredentials(){

  strcpy(WIFI_SSID,"laboratorio");
  strcpy(WIFI_PASS,"laboratorio123");
  strcpy(ALEXA_COMMAND,"sala");
  
  strcpy(AP_WIFI_SSID,"New device");
  strcpy(AP_WIFI_PASS,"afintelihome");

  strcpy(LOGIN_USER,"admin");
  strcpy(LOGIN_PASS,"admin");
}
