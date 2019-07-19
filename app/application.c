#include <application.h>

#include "bc_module_ethernet.h"
#include "webserver.h"
#include "mqtt_client.h"

// For adding new services from ioLibrary_Driver/Internet, you have to add the folder paths to the Makefile

// Network Configuration
wiz_NetInfo gWIZNETINFO = { .mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
                            .ip 	= {192, 168, 1, 30},					// IP address
                            .sn 	= {255, 255, 255, 0},					// Subnet mask
                            .gw 	= {192, 168, 1, 254},					// Gateway address
                            .dns 	= {8, 8, 8, 8},							// DNS server
                            .dhcp 	= NETINFO_DHCP/*NETINFO_STATIC*/ };		// DHCP enable / disable

// Button instance
bc_button_t button;
// LED instance
bc_led_t led;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        static int button_press_count = 0;
        bc_led_pulse(&led, 200);
        button_press_count++;

        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%d", button_press_count);
        mqtt_client_pub("node/ethernet-module/button/-/event-count", buffer);
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

    bc_log_debug("app_init");

    bc_module_ethernet_init(&gWIZNETINFO);

    webserver_start();
    mqtt_client_start();
}
