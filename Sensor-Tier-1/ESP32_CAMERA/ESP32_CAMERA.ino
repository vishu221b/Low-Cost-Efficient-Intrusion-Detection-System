#include "secrets.h"
#include <ArduinoWebsockets.h>
#define CAMERA_MODEL_AI_THINKER
#include <stdio.h>
#include "camera_pins.h"
#include <WiFi.h>
#include "esp_camera.h"
#include <nvs_flash.h>
#include <Preferences.h>


#define FLASH_PIN 4

int flashlight                        = 0;
int LINE_DIV_FACTOR                   = 10;
unsigned long _timestamp              = 1000;

const char* ssid                      = NETWORK_NAME;
const char* password                  = PASSWORD;
const char* websocket_server_host     = WEBSOCK_SERVER_HOST;
const uint16_t websocket_server_port1 = WEBSOCK_SERVER_PORT;
const char* STORE                     = "ESP32_STORE";


Preferences storage;


using namespace websockets;

WebsocketsClient client;

void printEmptyLineIfApplicable(int *counter, int *factor);


// void createDestroyNVSPartition(){
//   ESP_ERROR_CHECK(nvs_flash_init());
// }

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connection Closed");
    ESP.restart();
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void onMessageCallback(WebsocketsMessage message) {
  String data = message.data();
  int index = data.indexOf("=");
  if (index != -1) {
    String key = data.substring(0, index);
    String value = data.substring(index + 1);

    if (key == "ON_BOARD_LED_1") {
      if (value.toInt() == 1) {
        flashlight = 1;
        digitalWrite(FLASH_PIN, HIGH);
      } else {
        flashlight = 0;
        digitalWrite(FLASH_PIN, LOW);
      }
    }
  }
}

void initStore(){

  bool store_open = storage.begin(STORE);
  Serial.println(storage.getString("WiFi_SSID"));
  storage.clear();
  Serial.println(storage.getString("WiFi_SSID"));
  Serial.println("====================");
  
  if(!store_open){
    Serial.print("Could not open the ESP32 Store, please retry again.");
    abort();
  }else{
    Serial.print("Nothing went wrong with STORAGE.");
    Serial.println(store_open);
  }
}

void connectclient(){}

void initServer(){
  /**
  Web server for allowing password reset.
  */
}

void setup() {
  Serial.begin(115200);
  /**
  * Create Camera Configuration
  */

  initStore();
  storage.putString("WiFi_SSID", ssid);

  Serial.println(storage.getString("WiFi_SSID"));
  
  // Serial.println(storage.remove("WiFi_SSID"));
  
  // Serial.println(storage.getString("WiFi_SSID"));

  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 19;
  config.fb_count = 2;

  /**
  * Initiate Camera Sensor using Config
  */
  esp_err_t err = esp_camera_init(&config);
  Serial.print("Configging!\n");
  if (err != ESP_OK) { 
    Serial.print("Not Configging :((\n");
    return; 
  }
  Serial.print("Configured!\n");

  sensor_t* s = esp_camera_sensor_get();

  s->set_raw_gma(s, 1);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi....");
  int c=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(String(c+1) + "...");
    c++;
  }

  Serial.println("WiFi connected!!");
  /** 
  * Initiate Pin modes
  */
  pinMode(FLASH_PIN, OUTPUT);

  Serial.println("Connecting to websockets server,");
  c=1;
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  Serial.println(websocket_server_host);
  Serial.println(websocket_server_port1);

  while (!client.connect(websocket_server_host, websocket_server_port1, "/")) { 
    delay(500);
    Serial.print(String(c) + ".. ");
    printEmptyLineIfApplicable(&c, &LINE_DIV_FACTOR);
    c++;
    if(c == 100){
      exit(1);
    }
  }
}


unsigned long current_timestamp = 0l;

void loop() {
  client.poll();
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    esp_camera_fb_return(fb);
    return;
  }

  client.sendBinary((const char*)fb->buf, fb->len);

  // String _img_ts = String(millis());
  // client.send(_img_ts);

  esp_camera_fb_return(fb);


  current_timestamp = millis();
  /**
  * Capturing sensor data every 2 seconds
  */
  String output = "{'ON_BOARD_LED_1': '" + String(flashlight) + "' }";
  if ((current_timestamp / 1000 - _timestamp / 1000) > 2) {
    Serial.println(output);
    _timestamp = current_timestamp;
  }

  client.send(output);
}

/**
* Empty line for better visibility in Serial Monitor
*/
void printEmptyLineIfApplicable(int *counter, int *factor){
  if (*counter % *factor == 0){
      Serial.println();
  }
}