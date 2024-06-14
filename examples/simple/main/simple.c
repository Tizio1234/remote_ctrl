#include <stdio.h>
#include <remote_ctrl.h>

#include <esp_wifi.h>
#include <nvs_flash.h>

static const char *TAG = "main";

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

static void app_wifi_init()
{
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static const remote_ctrl_bind_config_t config = {
    .init_attr = ESPNOW_ATTRIBUTE_BASE,
    .rssi = -35,
    .resp_to_init_ms = 400,
    .wait_ms = 1000
};

static void app_remote_control_data_cb(espnow_attribute_t init_attr, espnow_attribute_t resp_attr, uint32_t resp_val)
{
    ESP_LOGI(TAG, "init_attr: %" PRIu16 ", resp_attr: %" PRIu16 ", resp_val: %" PRIu32, init_attr, resp_attr, resp_val);
}

void app_main(void)
{
    initialize_nvs();
    app_wifi_init();

    ESP_ERROR_CHECK(remote_ctrl_init());

    button_config_t btn_conf = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .active_level = 0,
            .gpio_num = GPIO_NUM_0
        }
    };

    button_handle_t btn_handle = iot_button_create(&btn_conf);

    ESP_ERROR_CHECK(remote_ctrl_set_data_cb(app_remote_control_data_cb));
    ESP_ERROR_CHECK(remote_ctrl_register_button_events(btn_handle, &config));
}
