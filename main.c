#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "dht.h"  // DHT22 sensor library (make sure you have this in your project)

#define DHT_GPIO 4  // GPIO pin connected to DHT22 data pin
static const char *TAG = "DHT_HTTP";

// This function sends temperature and humidity to the server using HTTP POST
void send_to_server(float temp, float hum) {
    char post_data[100];
    // Prepare JSON string with temperature and humidity values
    snprintf(post_data, sizeof(post_data),
             "{\"temperature\": %.2f, \"humidity\": %.2f}", temp, hum);

    esp_http_client_config_t config = {
        .url = "http://<YOUR_SERVER_IP>:5000/data",  // Replace with your Flask server IP
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Data sent successfully");
    } else {
        ESP_LOGE(TAG, "Failed to send data");
    }

    esp_http_client_cleanup(client);
}

void app_main(void) {
    while (1) {
        float temperature = 0.0f;
        float humidity = 0.0f;

        // Read temperature and humidity from DHT22 sensor
        if (dht_read_float_data(DHT_TYPE_DHT22, DHT_GPIO, &humidity, &temperature) == ESP_OK) {
            ESP_LOGI(TAG, "Temp: %.1f°C, Hum: %.1f%%", temperature, humidity);
            send_to_server(temperature, humidity);
        } else {
            ESP_LOGE(TAG, "Failed to read from DHT22");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait for 5 seconds before next reading
    }
}
