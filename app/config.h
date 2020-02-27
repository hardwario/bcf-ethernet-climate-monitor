#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <bc_atci.h>
#include <bc_config.h>
#include "bc_module_ethernet.h"
#include "../ioLibrary_Driver/Ethernet/wizchip_conf.h"

void config_init();

void config_get_wiznet_structure(wiz_NetInfo *wiznet);
void config_get_mqtt(uint8_t* ip, uint16_t *port);

#endif
