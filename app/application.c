#include <application.h>
#include "bc_module_ethernet.h"
#include "webserver.h"
#include "mqtt_client.h"
#include "app_climate_module.h"
#include "app_relay_module.h"

// For adding new services from ioLibrary_Driver/Internet, you have to add the folder paths to the Makefile

// Network Configuration
wiz_NetInfo gWIZNETINFO = { .ip 	= {192, 168, 1, 30},					// IP address
                            .sn 	= {255, 255, 255, 0},					// Subnet mask
                            .gw 	= {192, 168, 1, 254},					// Gateway address
                            .dns 	= {8, 8, 8, 8},							// DNS server
                            .dhcp 	= NETINFO_STATIC };		                // DHCP enable / disable

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        static int button_press_count = 0;
        bc_led_pulse(&led, 200);
        button_press_count++;

        mqtt_client_pub_int("push-button/-/event-count", &button_press_count);
    }
}

void application_init(void)
{
    // Initialize logging
    bc_log_init(BC_LOG_LEVEL_DUMP, BC_LOG_TIMESTAMP_ABS);

    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_pulse(&led, 2000);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_module_ethernet_init(&gWIZNETINFO);

    mqtt_client_start("ethernet-node");
    // Run and publish data from Climate Module
    app_climate_module_start();
    // Support to control Relay Module over MQTT
    app_relay_module_start();
    // Run webserver example
    //webserver_start();
}
