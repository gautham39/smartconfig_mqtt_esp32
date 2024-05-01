#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_events.h"
#include "smartconfig_tasks.h"
#include "sdkconfig.h"
#include "esp_smartconfig.h"
#include "cJSON.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "string.h"
#include "fota.h"

#define BLINK_GPIO GPIO_NUM_2

esp_mqtt_client_handle_t client1 ;

/*declaration of variables*/
char tx_buff[250]; 
char *TAG = "MQTT_EXAMPLE";
char data[25]="from char data mqtt";
void  mqtt_event_handler();

 void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}


/*set the brocker and start connection  of mqtt*/
void  mqtt_app_start();

 void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://test.mosquitto.org:1883"
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    
    esp_mqtt_client_start(client);
}





void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)

{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    client1 = client ;

    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) 
    {
    
    /*initialize a pub and sub topic to tx and rx data */
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/esp/led_status", "device connected ", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "esp32/led/control", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        
    case MQTT_EVENT_DISCONNECTED: 
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    /* Set the GPIO as a push/pull output */
        gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

/*print the received data from client*/
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);


/*parse cJSON data*/
        cJSON *json = cJSON_Parse(event->data); 
            if (json == NULL) { 
                const char *error_ptr = cJSON_GetErrorPtr(); 
            if (error_ptr != NULL) { 
                printf("Error: %s\n", error_ptr); 
            } 
            cJSON_Delete(json); 
            return ; 
            } 

        const cJSON *led = cJSON_GetObjectItemCaseSensitive(json, "LED");
        const cJSON *smartconfig = cJSON_GetObjectItemCaseSensitive(json, "smartconfig");
        const cJSON *update = cJSON_GetObjectItemCaseSensitive(json, "update");
    
    if (cJSON_IsString(led) && (led->valuestring != NULL)) {
        printf("led: %s\n", led->valuestring);
    }
       

/*check what command has been received */
        if (cJSON_IsString(led) && (led->valuestring != NULL)) {

        if (strncmp(led->valuestring, "on", strlen(led->valuestring)) == 0) {
                    gpio_set_level(BLINK_GPIO, 1);// Turn LED on
                    esp_mqtt_client_publish(client, "/esp/led_status", "LED is ON \n", 0, 0, 0);
                    printf("LED is ON\n");
                    break;
                    }

        else if (strncmp(led->valuestring, "off", strlen(led->valuestring)) == 0) {
                        gpio_set_level(BLINK_GPIO,0 );// Turn LED off
                        esp_mqtt_client_publish(client, "/esp/led_status", "LED is OFF\n", 0, 0, 0);
                         printf("LED is OFF\n");
                         break;
                    }
        }
            
            /*smartconfig command */
        if (cJSON_IsString(smartconfig) && (smartconfig->valuestring != NULL)) {
    
            if (strncmp(smartconfig->valuestring, "start", strlen(smartconfig->valuestring)) == 0) {
                    esp_mqtt_client_unsubscribe(client, "esp32/led/control");
                    esp_mqtt_client_disconnect(client);
                    vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms delay
                    esp_wifi_disconnect();
                    esp_smartconfig_stop();
                    vTaskDelay(pdMS_TO_TICKS(500)); // 500 ms delay
                    smartconfig_task();
             }
        }
        
        /*update command*/
        if (cJSON_IsString(update) && (update->valuestring != NULL)) {
                
                xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, update->valuestring, 5, NULL);   
            }
        break;
    
    
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
