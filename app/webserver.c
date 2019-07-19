#include "webserver.h"
#include "httpServer.h"
#include "webpage.h"

#define MAX_HTTPSOCK	6
uint8_t socknumlist[] = {2, 3, 4, 5, 6, 7};

////////////////////////////////////////////////
// Shared Buffer Definition  				  //
////////////////////////////////////////////////
uint8_t RX_BUF[DATA_BUF_SIZE];
uint8_t TX_BUF[DATA_BUF_SIZE];

void webserver_task(void *param)
{
    (void) param;

    for (int i = 0; i < MAX_HTTPSOCK; i++)
    {
        httpServer_run(i); 	// HTTP Server Handler
    }

    bc_scheduler_plan_current_from_now(20);
}

void webserver_tick_task(void *param)
{
    (void) param;

    httpServer_time_handler();

    bc_scheduler_plan_current_from_now(1000);
}


void webserver_start()
{
    httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);		// Tx/Rx buffers (1kB) / The number of W5500 chip H/W sockets in use

    // Web content registration (web content in webpage.h, Example web pages)

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

    bc_scheduler_register(webserver_task, NULL, 0);
    bc_scheduler_register(webserver_tick_task, NULL, 0);

}
