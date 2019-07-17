#include <application.h>

#include "wizchip_conf.h"

#include "httpServer.h"
#include "webpage.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
///////////////////////////////////////
// Debugging Message Printout enable //
///////////////////////////////////////
#define _MAIN_DEBUG_

///////////////////////////
// Demo Firmware Version //
///////////////////////////
#define VER_H		1
#define VER_L		00

//////////////////////////////////////////////////
// Socket & Port number definition for Examples //
//////////////////////////////////////////////////
#define SOCK_TCPS       0
#define SOCK_UDPS       1
#define PORT_TCPS		5000
#define PORT_UDPS       3000

#define MAX_HTTPSOCK	6
uint8_t socknumlist[] = {2, 3, 4, 5, 6, 7};

////////////////////////////////////////////////
// Shared Buffer Definition  				  //
////////////////////////////////////////////////
uint8_t RX_BUF[DATA_BUF_SIZE];
uint8_t TX_BUF[DATA_BUF_SIZE];

///////////////////////////
// Network Configuration //
///////////////////////////
wiz_NetInfo gWIZNETINFO = { .mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},	// Mac address
                            .ip 	= {192, 168, 1, 30},					// IP address
                            .sn 	= {255, 255, 255, 0},					// Subnet mask
                            .gw 	= {192, 168, 1, 254},						// Gateway address
                            .dns 	= {8, 8, 8, 8},							// DNS server
                            .dhcp 	= NETINFO_DHCP/*NETINFO_STATIC*/ };						// DHCP enable / disable




// Button instance
bc_button_t button;

int button_press_count = 0;

enum
{
    _BC_MODULE_ETHERNET_CS_PIN = BC_TCA9534A_PIN_P0,
    _BC_MODULE_ETHERNET_RSTN_PIN = BC_TCA9534A_PIN_P1
};

bc_tca9534a_t tca9534a;

void http_tick_task(void *param);

void delay(int ms)
{
    bc_tick_t timestamp = bc_tick_get() + ms;

    while (bc_tick_get() <= timestamp);
}

void eth_init()
{
    bc_system_pll_enable();

    if (!bc_tca9534a_init(&tca9534a, BC_I2C_I2C0, 0x24))
    {
        bc_log_debug("TCA init err");

    }

    // NRST + CS to log 1
    if (!bc_tca9534a_write_port(&tca9534a, _BC_MODULE_ETHERNET_RSTN_PIN | _BC_MODULE_ETHERNET_CS_PIN))
    {
        return ;
    }

    if (!bc_tca9534a_write_pin(&tca9534a, _BC_MODULE_ETHERNET_CS_PIN, 0))
    {
        bc_log_debug("CS ERR0");
    }

    // all outputs
    if (!bc_tca9534a_set_port_direction(&tca9534a, 0x00))
    {
        return;
    }

    bc_spi_init(BC_SPI_SPEED_4_MHZ, BC_SPI_MODE_0);
    bc_spi_set_manual_cs_control(true);

    bc_log_debug("TCA init OK");

    // We need to do a reset so chip would detect the default SPI levels and use MODE0
    // Activate RESET
    if (!bc_tca9534a_write_pin(&tca9534a, _BC_MODULE_ETHERNET_RSTN_PIN, 0))
    {
        bc_log_debug("CS ERR0");
    }

    bc_tick_t tick = bc_tick_get() + 50;
    while(bc_tick_get() < tick);

    // Deactivate RESET
    if (!bc_tca9534a_write_pin(&tca9534a, _BC_MODULE_ETHERNET_RSTN_PIN, 1))
    {
        bc_log_debug("CS ERR0");
    }

    tick = bc_tick_get() + 50;
    while(bc_tick_get() < tick);
}


void eth_spi_cs(bool chip_select)
{
    if (chip_select)
    {
        GPIOB->BSRR = GPIO_BSRR_BR_12;
    }
    else
    {
        GPIOB->BSRR = GPIO_BSRR_BS_12;
    }

    /*
    if (chip_select)
    {
        if (!bc_tca9534a_write_pin(&tca9534a, _BC_MODULE_ETHERNET_CS_PIN, 1))
        {
            bc_log_debug("CS ERR1");
        }
    }
    else
    {
        if (!bc_tca9534a_write_pin(&tca9534a, _BC_MODULE_ETHERNET_CS_PIN, 0))
        {
            bc_log_debug("CS ERR0");
        }
    }
    */
}

void eth_read(uint16_t addr, uint8_t cb, uint8_t *buffer, uint16_t len)
{
    uint8_t cmd[8];

    eth_spi_cs(0);

    cmd[0] = addr >> 8;
    cmd[1] = addr;
    cmd[2] = cb;

    bc_spi_transfer(cmd, NULL, 3);

    memset(buffer, 0, len);
    bc_spi_transfer(NULL, buffer, len);

    eth_spi_cs(1);
}

void eth_write(uint16_t addr, uint8_t cb, uint8_t *buffer, uint16_t len)
{
    uint8_t cmd[8];

    eth_spi_cs(0);

    cmd[0] = addr >> 8;
    cmd[1] = addr;
    cmd[2] = cb;

    bc_spi_transfer(cmd, NULL, 3);
    bc_spi_transfer(buffer, NULL, len);

    eth_spi_cs(1);
}

/* W5500 Call Back Functions */
static void  wizchip_select(void)
{
	eth_spi_cs(0);	// SSEL(CS)
}

static void  wizchip_deselect(void)
{
	eth_spi_cs(1);	// SSEL(CS)
}

static uint8_t wizchip_read()
{
	uint8_t rb;
	bc_spi_transfer(NULL, &rb, 1);
	return rb;
}

static void  wizchip_write(uint8_t wb)
{
	bc_spi_transfer(&wb, NULL, 1);
}

static void wizchip_readburst(uint8_t* pBuf, uint16_t len)
{
	bc_spi_transfer(NULL, pBuf, len);
}

static void  wizchip_writeburst(uint8_t* pBuf, uint16_t len)
{
	bc_spi_transfer(pBuf, NULL, len);
}


void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        bc_led_set_mode(&led, BC_LED_MODE_TOGGLE);
        button_press_count++;
    }

    // Logging in action
    bc_log_info("Button event handler - event: %i", event);
}

void application_init(void)
{
    // Initialize logging
    bc_log_init(BC_LOG_LEVEL_DUMP, BC_LOG_TIMESTAMP_ABS);

    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_log_debug("app_init");

    bc_system_pll_enable();

    eth_init();

    /* Register Call back function */
	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);

	/* W5500 Chip Initialization */
    uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		printf("WIZCHIP Initialized fail.\r\n");
		while (1);
	}

    ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

    httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);		// Tx/Rx buffers (1kB) / The number of W5500 chip H/W sockets in use
    bc_scheduler_register(http_tick_task, NULL, 0);

    /* Web content registration (web content in webpage.h, Example web pages) */

    // Index page and netinfo / base64 image demo
    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);				// index.html 		: Main page example
    reg_httpServer_webContent((uint8_t *)"netinfo.html", (uint8_t *)netinfo_page);			// netinfo.html 	: Network information example page
    reg_httpServer_webContent((uint8_t *)"netinfo.js", (uint8_t *)wiz550web_netinfo_js);	// netinfo.js 		: JavaScript for Read Network configuration 	(+ ajax.js)
    reg_httpServer_webContent((uint8_t *)"img.html", (uint8_t *)img_page);					// img.html 		: Base64 Image data example page

    // Example #1
    reg_httpServer_webContent((uint8_t *)"dio.html", (uint8_t *)dio_page);					// dio.html 		: Digital I/O control example page
    reg_httpServer_webContent((uint8_t *)"dio.js", (uint8_t *)wiz550web_dio_js);			// dio.js 			: JavaScript for digital I/O control 	(+ ajax.js)

    // Example #2
    reg_httpServer_webContent((uint8_t *)"ain.html", (uint8_t *)ain_page);					// ain.html 		: Analog input monitor example page
    reg_httpServer_webContent((uint8_t *)"ain.js", (uint8_t *)wiz550web_ain_js);			// ain.js 			: JavaScript for Analog input monitor	(+ ajax.js)

    // Example #3
    reg_httpServer_webContent((uint8_t *)"ain_gauge.html", (uint8_t *)ain_gauge_page);		// ain_gauge.html 	: Analog input monitor example page; using Google Gauge chart
    reg_httpServer_webContent((uint8_t *)"ain_gauge.js", (uint8_t *)ain_gauge_js);			// ain_gauge.js 	: JavaScript for Google Gauge chart		(+ ajax.js)

    // AJAX JavaScript functions
    reg_httpServer_webContent((uint8_t *)"ajax.js", (uint8_t *)wiz550web_ajax_js);			// ajax.js			: JavaScript for AJAX request transfer

}

void application_task(void)
{
    for (int i = 0; i < MAX_HTTPSOCK; i++)
    {
        httpServer_run(i); 	// HTTP Server Handler
    }

    bc_scheduler_plan_current_from_now(20);
}

void http_tick_task(void *param)
{
    (void) param;

    httpServer_time_handler();

    bc_scheduler_plan_current_from_now(1000);
}
