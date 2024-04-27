#include <Arduino.h> 
#include <atomic>
#include <esp_now.h>
#include <stripLED.hpp>
#include <WiFi.h>

#define REAR_CONTROLLER
const uint16_t num_leds = 300;

#ifdef FRONT_CONTROLLER
  uint8_t host_mac_addr[] = {0xDC, 0x54, 0x75, 0xF1, 0xE2, 0x20};
#endif

#ifdef REAR_CONTROLLER
  uint8_t host_mac_addr[] = {0xDC, 0x54, 0x75, 0xF1, 0xE2, 0x38}; 
#endif

typedef struct led_data_t
{
  uint8_t led_state; 
  uint8_t led_brightness;
} led_data_t;

esp_now_peer_info_t host_info;

LED::led_state_t DEFAULT_STARTUP_STATE = LED::led_state_t::white;

void on_receive_callback(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  led_data_t data_recieved;
  memcpy(&data_recieved, data, sizeof(data));
  LED::setState(static_cast<LED::led_state_t>(data_recieved.led_state));
  LED::setBrightness(data_recieved.led_brightness);

  Serial.printf("State: %i, Brightness: %i \n", data_recieved.led_state, data_recieved.led_brightness);
}

esp_now_peer_info_t peer_information;;

void setup() 
{
  Serial.begin(115200);

  LED::startLEDs(num_leds);
  LED::setState(DEFAULT_STARTUP_STATE);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) 
  {
    //Safety, if ESPNOW fails, LEDs will stil run in white state
    Serial.println("Error initializing ESP-NOW");
    vTaskDelay(3000);
  }

  esp_now_register_recv_cb(on_receive_callback);

  memcpy(peer_information.peer_addr, &host_mac_addr, sizeof(host_mac_addr));
  peer_information.channel = 0;
  peer_information.encrypt = false;
  esp_now_add_peer(&peer_information);
}

void loop() 
{
  vTaskSuspend(NULL);
}