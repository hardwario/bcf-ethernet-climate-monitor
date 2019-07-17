#include <application.h>

#include "w5500data.h"

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

int button_press_count = 0;

enum
{
    _BC_MODULE_ETHERNET_CS_PIN = BC_TCA9534A_PIN_P0,
    _BC_MODULE_ETHERNET_RSTN_PIN = BC_TCA9534A_PIN_P1
};

bc_tca9534a_t tca9534a;

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

unsigned char wizGetCtl8(uint16_t ctlregaddr);
uint16_t wizGetCtl16(uint16_t ctlregaddr);

#define wizWrite(addr, cb, buff, len) eth_write(addr,cb,buff,len)
#define wizRead(addr, cb, buff, len) eth_read(addr,cb,buff,len)

#define sendconst(x) send0((unsigned char*)x,sizeof(x)-1)

#define MAX_BUF 512
unsigned char buf[MAX_BUF];			//memory buffer for incoming & outgoing data


union WReg{ //used to retrieve a 16 bit value from the wiznet
	uint16_t i;
	unsigned char c[2];
};

void wizCmd(unsigned char cmd){ //send a command to socket 0 and wait for completion
	wizWrite(SnCR,WIZNET_WRITE_S0R,&cmd,1); //send the command
	while(wizRead(SnCR,WIZNET_READ_S0R,&cmd,1),cmd); //wait til command completes

}

void wizSetCtl8(unsigned int ctlreg, unsigned char val){//write to a socket 0 control register

    volatile uint8_t before;
    volatile uint8_t after;

    before = wizGetCtl8(ctlreg);

	wizWrite(ctlreg, WIZNET_WRITE_S0R,&val,1);

    after = wizGetCtl8(ctlreg);

    before++;
    after++;
}
unsigned char wizGetCtl8(uint16_t ctlregaddr){
  unsigned char regval; //spot to hold the register contents
  wizRead(ctlregaddr,WIZNET_READ_S0R,&regval,1);
  return regval;
}

void wizSetCtl16(uint16_t ctlreg, uint16_t val){
    volatile uint8_t before;
    volatile uint8_t after;

    uint8_t buff[2];
    buff[0] = val >> 8;
    buff[1] = val;

    before = wizGetCtl16(ctlreg);

    wizWrite(ctlreg,WIZNET_WRITE_S0R,(unsigned char *) buff,2);

    after = wizGetCtl16(ctlreg);

    before++;
    after++;
}

uint16_t wizGetCtl16(uint16_t ctlreg){
  //union WReg regval;
  uint8_t buff[2];

  wizRead(ctlreg,WIZNET_READ_S0R, buff,2);

  uint16_t ret = buff[0] << 8 | buff[1];
  return ret;
}
void wiz_Init(unsigned char ip_addr[]){// Ethernet Setup
  wizWrite(SIPR,WIZNET_WRITE_COMMON,ip_addr,4);// Set the Wiznet W5100 IP Address
}

void socket0_init(){ //initialize socket 0 for http server
	wizCmd(CR_CLOSE); //make sure port is closed
	wizSetCtl8(SnIR,0xFF); //reset interrupt reg
	wizSetCtl8(SnMR,MR_TCP); //set mode register to tcp
	wizSetCtl16(SnPORT,80); //set tcp port to 80
	wizCmd(CR_OPEN); //open the port
	wizCmd(CR_LISTEN); //listen for a conection
}

unsigned int send0(unsigned char *buf,unsigned int buflen){
    unsigned int timeout,txsize,txfree;
    unsigned char crsend=CR_SEND,crreadback;
	unsigned int txwr;
    if (buflen <= 0) return 0; //make sure there is something to send
    //make sure there is room in the transmit buffer for what we want to send
    txfree=wizGetCtl16(SnTX_FSR); //this is the size of the available buffer area
    timeout=0;
    while (txfree < buflen) {
      delay(1);
     txfree=wizGetCtl16(SnTX_FSR);
     if (timeout++ > 1000) {// Timeout for approx 1 sec
       	printf("TX Free Size Error!\n");
		wizCmd(CR_DISCON);// Disconnect the connection
       	return 0;
     }
   }
	//space is available so we will send the buffer
   	txwr=wizGetCtl16(SnTX_WR);  // Read the Tx Write Pointer
   	wizWrite(txwr,WIZNET_WRITE_S0TX,buf, buflen); //write the outgoing data to the transmit buffer
   	wizSetCtl16(SnTX_WR,txwr+buflen);//update the buffer pointer
	wizCmd(CR_SEND); // Now Send the SEND command which tells the wiznet the pointer is updated
    return 1;
}

unsigned int recv0(unsigned char *buf,unsigned int buflen){
	unsigned int rxrd;
    if (buflen <= 0) return 1;
    if (buflen > MAX_BUF)	// If the request size > MAX_BUF,just truncate it
        buflen=MAX_BUF - 2;
    rxrd = wizGetCtl16(SnRX_RD); // get the address where the wiznet is holding the data
	wizRead(rxrd,WIZNET_READ_S0RX,buf,buflen); //read the data
    *(buf+buflen)='\0';        // terminate string
    return 1;
}

void flush(unsigned int rsize){ //this just gets rid of data that i don't want to process
	unsigned int rxrd;
	if (rsize>0){
  		rxrd=wizGetCtl16(SnRX_RD); //retrieve read data pointer
  		wizSetCtl16(SnRX_RD,rxrd+rsize); //replace read data pointer
		wizCmd(CR_RECV); //tell the wiznet we`ve retrieved the data
	}
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

    eth_init();

    bc_log_debug("app_init");

    bc_system_pll_enable();

    // Set MAC address
    uint8_t mac[] = {0xaa, 0xaa, 0xaa, 0xbb, 0xbb, 0xbb};
    eth_write(0x0009, 0x04, mac, 6);

    // Set IP address
    uint8_t buf[4];
    buf[0] = 192;
    buf[1] = 168;
    buf[2] = 1;
    buf[3] = 30;
    eth_write(0x000F, 0x04, buf, 4);

}

void sendresp(){
	static char hdr1[]="HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"
						"<html>"
						"<span style=\"color:#0000A0\">\r\n"
						"<center><h1>Hello from BigClown</h1>";
	sendconst(hdr1); 	// Now Send the HTTP Response first part

    static char buffer[64];
    snprintf(buffer, sizeof(buffer), "Button press: %d", button_press_count);
    sendconst(buffer);

	static char trlr[]="</center></body></html>\r\n\r\n";
	sendconst(trlr); 	// Now Send the rest of the page
}

void handlesession(){
	unsigned int rsize;
	rsize=wizGetCtl16(SnRX_RSR); //get the size of the received data
	if (rsize>0){
		sendresp(); //send a response
		flush(rsize);	//get rid of the received data
	}
	wizCmd(CR_DISCON);
}

void application_task(void)
{

		uint8_t socket0status=wizGetCtl8(SnSR); //socket 0 status
		switch (socket0status){
			case SOCK_CLOSED: //initial condition
				socket0_init();	//initialize socket 0
				break;
			case SOCK_ESTABLISHED: //someone wants to talk to the server
				handlesession();
				break;
			//following are cases where we have to reset and reopen the socket
			case SOCK_FIN_WAIT: case SOCK_CLOSING: case SOCK_TIME_WAIT:
			case SOCK_CLOSE_WAIT: case SOCK_LAST_ACK:
				wizCmd(CR_CLOSE);
				break;
		}

/*
    eth_read(0x0039, 0, buf, 4);
    bc_log_dump(buf, 4, "VersionR");


    eth_read(0x000F, 0, buf, 4);
    bc_log_dump(buf, 4, "IP");*/

/*
    // Allcapable + auto negotiation
    buf[0] = 0xFF;
    eth_write(0x002E, 0x04, buf, 1);
*/
    bc_scheduler_plan_current_from_now(100);
}
