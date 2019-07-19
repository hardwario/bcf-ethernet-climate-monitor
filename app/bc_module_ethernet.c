#include "bc_module_ethernet.h"

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


void bc_module_ethernet_init(wiz_NetInfo *wiznet_info)
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
    while(bc_tick_get() < tick)
    {
    }

    // Read unique ID from security chip and use it as a MAC
    //bc_atsha

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

    ctlnetwork(CN_SET_NETINFO, (void*) wiznet_info);
}
