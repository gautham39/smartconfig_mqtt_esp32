#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "smartconfig_tasks.h"
#include"driver/uart.h"
#include "freertos/semphr.h"
#include "mqtt_client.h"
#include "mqtt_events.h"
#include "driver/gpio.h"
#include "cJSON.h"

#define EXAMPLE_ESP_MAXIMUM_RETRY (4)

extern SemaphoreHandle_t xSemaphore ;
extern  EventGroupHandle_t s_wifi_event_group;
extern const int CONNECTED_BIT ;
extern const int ESPTOUCH_DONE_BIT ;
extern const  int  WIFI_FAIL_BIT;
extern const char *TAG2 ;
extern int s_retry_num2;
wifi_config_t wifi_config;

void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {   
     
     /*check if it is wifi event and start smartconfig task*/
     xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
    
    }
    /*if disconnected try to reconnect or enter password again*/
     else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
     {
       
        if (s_retry_num2 < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num2++;
            ESP_LOGI(TAG2, "retry to connect to the AP");
        }
        else
        {
            printf("enter password again");

            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG2, "connect to the AP fail");
        
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG2, "Scan done \n enter passowrd on  app now ");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG2, "Found channel");


    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG2, "Got SSID and password");
    /*here after entering data in esp touch app retrive the data and initialize and store da ta in wifi config ,

    and try to connect with the provided credentils */
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
       
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
       
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
       
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
       
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
       
        ESP_LOGI(TAG2, "SSID:%s", ssid);
        ESP_LOGI(TAG2, "PASSWORD:%s", password);
      

        ESP_ERROR_CHECK( esp_wifi_disconnect());
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        
        esp_wifi_connect();

    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}
    
    /*here initailize the smartconfig type  and check if wifi data is received and try to establish connection , 
        
    after which start mqtt initialization process */

void smartconfig_task()
{
    EventBits_t uxBits;
    
    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
    
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    
    ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
    
    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT | WIFI_FAIL_BIT, true, false, portMAX_DELAY);
        
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG2, "WiFi Connected to ap");
            
            mqtt_app_start();

            s_retry_num2=0;
            vTaskDelete(NULL);
        }
       
        
        if(uxBits & WIFI_FAIL_BIT) {
            ESP_LOGI(TAG2, "\n WRONG PASSWORD \n enter correct password  ");
            esp_smartconfig_stop();
            s_retry_num2=0;
            
            esp_smartconfig_start(&cfg);
            
            }
    }
}
