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

	//espnow_ctrl_responder_clear_bindlist();
	ESP_LOGI(TAG, "Cleared remote control(espnow) bindlist");
}

static void button_single_click_cb(void *button_handle, void *usr_data)
{
	ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");

	//espnow_ctrl_initiator_send(ESPNOW_ATTRIBUTE_BASE, ESPNOW_ATTRIBUTE_BASE, esp_log_timestamp());
	ESP_LOGI(TAG, "Sending test message");
}

esp_err_t remote_ctrl_init_for_all()
{
	if (all_init)
		return ESP_ERR_INVALID_STATE;

	/* Initialize espnow storage */
	ESP_RETURN_ON_ERROR(espnow_storage_init(), TAG,
			    "Failed to init espnow storage");

	/* Initialize espnow with default config*/
	espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
	ESP_RETURN_ON_ERROR(espnow_init(&espnow_config), TAG,
			    "Failed to init espnow");

	all_init = true;

	return ESP_OK;
}
esp_err_t remote_ctrl_deinit_for_all()
{
	if (!all_init)
		return ESP_ERR_INVALID_STATE;

	ESP_RETURN_ON_ERROR(espnow_deinit(), TAG, "Failed to deinit espnow");

	all_init = false;

	return ESP_OK;
}

esp_err_t remote_ctrl_bind(remote_ctrl_bind_config_t *config, bool bind)
{
    ESP_PARAM_CHECK(config);

	ESP_RETURN_ON_ERROR(espnow_ctrl_responder_bind(config->wait_ms,
						       config->rssi, NULL),
			    TAG, "Failed to responder_bind");

	vTaskDelay(pdMS_TO_TICKS(config->resp_to_init_ms));

	ESP_RETURN_ON_ERROR(espnow_ctrl_initiator_bind(config->init_attr, bind),
			    TAG, "Failed to initiator_bind");

	return ESP_OK;
}

esp_err_t remote_ctrl_set_data_cb(espnow_ctrl_data_cb_t cb)
{
	return espnow_ctrl_responder_data(cb);
}

esp_err_t remote_ctrl_send(espnow_attribute_t init_attr,
			   espnow_attribute_t resp_attr, uint32_t val)
{
	return espnow_ctrl_initiator_send(init_attr, resp_attr, val);
}

esp_err_t remote_ctrl_register_button_events(const button_handle_t handle,
					     remote_ctrl_bind_config_t *config)
{
	ESP_PARAM_CHECK(handle);
	ESP_PARAM_CHECK(config);

	ESP_LOGI(TAG,
		 "Config{init_attr : %" PRIi16 ", rssi : %" PRIi8
		 ", wait_ms : %" PRIu16 ", resp_to_init_ms : %" PRIu16
		 "}, size of config: %u",
		 config->init_attr, config->rssi, config->wait_ms,
		 config->resp_to_init_ms, sizeof(*config));

	ESP_RETURN_ON_ERROR(iot_button_register_cb(handle, BUTTON_DOUBLE_CLICK,
						   button_double_click_cb,
						   config),
			    TAG, "Failed to register double click cb");
	ESP_RETURN_ON_ERROR(iot_button_register_cb(handle, BUTTON_SINGLE_CLICK,
						   button_single_click_cb,
						   config),
			    TAG, "Failed to register single click cb");
	ESP_RETURN_ON_ERROR(iot_button_register_cb(handle, BUTTON_LONG_PRESS_UP,
						   button_single_click_cb,
						   config),
			    TAG, "Failed to register long press up cb");
    
    button_event_config_t btn_evt_conf = {
        .event = BUTTON_MULTIPLE_CLICK,
        .event_data.multiple_clicks.clicks = 3
    };

    ESP_RETURN_ON_ERROR(iot_button_register_event_cb(handle, btn_evt_conf, button_multiple_click_cb, config), TAG, "Failed to register multiple clicks cb");

	return ESP_OK;
}

esp_err_t remote_ctrl_unregister_button_events(const button_handle_t handle)
{
    ESP_PARAM_CHECK(handle);
    return ESP_OK;
}
