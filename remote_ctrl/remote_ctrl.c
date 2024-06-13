#include <remote_ctrl.h>

#include <esp_check.h>

static const char *TAG = "remote_ctrl";
static bool all_init = false;

/* button */
static void button_double_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_DOUBLE_CLICK");

    ESP_LOGI(TAG, "Doing bind operation");
}

static void button_long_press_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_LONG_PRESS_CLICK");

    ESP_LOGI(TAG, "Doing unbind operation");
}

static void button_multiple_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_MULTIPLE_CLICK");

    espnow_ctrl_responder_clear_bindlist();
    ESP_LOGI(TAG, "Cleared remote control(espnow) bindlist");
}

static void button_single_click_cb(void *button_handle, void *usr_data)
{
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");

    espnow_ctrl_initiator_send(ESPNOW_ATTRIBUTE_BASE, ESPNOW_ATTRIBUTE_BASE, esp_log_timestamp());
    ESP_LOGI(TAG, "Sending test message");
}

esp_err_t init_for_all()
{
    if (all_init)
        return ESP_ERR_INVALID_STATE;

    /* Initialize espnow storage */
    ESP_RETURN_ON_ERROR(espnow_storage_init(), TAG, "Failed to init espnow storage");

    /* Initialize espnow with default config*/
    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(espnow_init(&espnow_config), TAG, "Failed to init espnow");

    return ESP_OK;
}
esp_err_t deinit_for_all()
{
    if (!all_init)
        return ESP_ERR_INVALID_STATE;

    ESP_RETURN_ON_ERROR(espnow_deinit(), TAG, "Failed to deinit espnow");
    return ESP_OK;
}

esp_err_t bind(remote_ctrl_bind_config_t config,
               bool bind)
{
    ESP_RETURN_ON_ERROR(espnow_ctrl_responder_bind(config.wait_ms, config.rssi, NULL), TAG, "Failed to responder_bind");

    vTaskDelay(pdMS_TO_TICKS(config.resp_to_init_ms));

    ESP_RETURN_ON_ERROR(espnow_ctrl_initiator_bind(config.init_attr, bind), TAG, "Failed to initiator_bind");

    return ESP_OK;
}

esp_err_t set_data_cb(espnow_ctrl_data_cb_t cb)
{
    return espnow_ctrl_responder_data(cb);
}

esp_err_t send(espnow_attribute_t init_attr,
               espnow_attribute_t resp_attr,
               uint32_t val)
{
    return espnow_ctrl_initiator_send(init_attr, resp_attr, val);
}

esp_err_t register_button_events(const button_handle_t handle,
                                 espnow_attribute_t init_attr)
{
    ESP_PARAM_CHECK(handle);

    ESP_RETURN_ON_ERROR(iot_button_register_cb(handle, BUTTON_DOUBLE_CLICK, button_double_click_cb, (void *)init_attr), TAG, "Failed to register double click cb");

    return ESP_OK;
}
