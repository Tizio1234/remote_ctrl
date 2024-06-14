#pragma once

#include <esp_err.h>
#include <espnow.h>
#include <espnow_ctrl.h>

#include <iot_button.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct for remote control over a single base attribute
 */
typedef struct __packed {
    espnow_attribute_t init_attr;
    uint16_t wait_ms;
    uint16_t resp_to_init_ms;
    int8_t rssi;
} remote_ctrl_bind_config_t;

esp_err_t remote_ctrl_set_data_cb(espnow_ctrl_data_cb_t cb);

esp_err_t remote_ctrl_init_for_all();
esp_err_t remote_ctrl_deinit_for_all();

esp_err_t remote_ctrl_bind(remote_ctrl_bind_config_t *config,
                           bool bind);

esp_err_t remote_ctrl_send(espnow_attribute_t init_attr,
                           espnow_attribute_t resp_attr,
                           uint32_t val);

esp_err_t remote_ctrl_register_button_events(const button_handle_t handle,
                                             remote_ctrl_bind_config_t *config);
esp_err_t remote_ctrl_unregister_button_events(const button_handle_t handle);

#ifdef __cplusplus
}
#endif
