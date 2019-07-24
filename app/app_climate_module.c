
#include "app_climate_module.h"
#include "mqtt_client.h"

#define CLIMATE_MODULE_UPDATE_INTERVAL             (10 * 1000)

void climate_module_event_handler(bc_module_climate_event_t event, void *event_param)
{
    (void) event_param;

    float value;

    if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_THERMOMETER)
    {
        if (bc_module_climate_get_temperature_celsius(&value))
        {
            mqtt_client_pub_float("thermometer/0:1/temperature", &value);
        }
    }
    else if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_HYGROMETER)
    {
        if (bc_module_climate_get_humidity_percentage(&value))
        {
            mqtt_client_pub_float("hygrometer/0:4/relative-humidity", &value);
        }
    }
    else if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_LUX_METER)
    {
        if (bc_module_climate_get_illuminance_lux(&value))
        {
            mqtt_client_pub_float("lux-meter/0:0/illuminance", &value);
        }
    }
    else if (event == BC_MODULE_CLIMATE_EVENT_UPDATE_BAROMETER)
    {
        if (bc_module_climate_get_pressure_pascal(&value))
        {
            mqtt_client_pub_float("barometer/0:0/pressure", &value);
        }
    }
}

void app_climate_module_start(void)
{
    // Initialize climate module
    bc_module_climate_init();
    bc_module_climate_set_event_handler(climate_module_event_handler, NULL);
    bc_module_climate_set_update_interval_thermometer(CLIMATE_MODULE_UPDATE_INTERVAL);
    bc_module_climate_set_update_interval_hygrometer(CLIMATE_MODULE_UPDATE_INTERVAL);
    bc_module_climate_set_update_interval_lux_meter(CLIMATE_MODULE_UPDATE_INTERVAL);
    bc_module_climate_set_update_interval_barometer(CLIMATE_MODULE_UPDATE_INTERVAL);
    bc_module_climate_measure_all_sensors();
}
