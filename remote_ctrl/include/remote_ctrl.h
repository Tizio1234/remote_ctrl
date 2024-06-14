#pragma once

#include <esp_err.h>
#include <espnow.h>
#include <espnow_ctrl.h>

#include <iot_button.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct for remote control bind configuration
 */
typedef struct __packed {
	espnow_attribute_t init_attr;
	uint16_t wait_ms;
	uint16_t resp_to_init_ms;
	int8_t rssi;
} remote_ctrl_bind_config_t;

/**
 * @brief Set rx data callback
 * @param cb callback
 */
esp_err_t remote_ctrl_set_data_cb(espnow_ctrl_data_cb_t cb);

/**
 * @brief Init espnow for remote ctrl
 */
esp_err_t remote_ctrl_init();
/**
 * @brief Deinit espnow
 */
esp_err_t remote_ctrl_deinit();

/**
 * @brief espnow_ctrl_responder_bind, waits config->resp_to_init_ms milliseconds, espnow_ctrl_initiator_bind
 * @param config pointer to config structure, the structure can go out of scope after function execution
 * @param bind true: bind, false: unbind
 */
esp_err_t remote_ctrl_bind(const remote_ctrl_bind_config_t *config, bool bind);

/**
 * @brief Send some data using espnow_ctrl_initiator_send
 */
esp_err_t remote_ctrl_send(espnow_attribute_t init_attr,
			   espnow_attribute_t resp_attr, uint32_t val);

/**
 * @brief register button events callbacks for easily do bind related operations with the touch of a button:
 * @paragraph 
 */
esp_err_t
remote_ctrl_register_button_events(const button_handle_t handle,
				   const remote_ctrl_bind_config_t *config);
/**
 * @brief Unregister previously registered button events
 */
esp_err_t remote_ctrl_unregister_button_events(const button_handle_t handle);

#ifdef __cplusplus
}
#endif
