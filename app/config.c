
#include "config.h"


bool _config_at_ip_read(void);
bool _config_at_ip_set(bc_atci_param_t *param);

bool _config_at_subnet_read(void);
bool _config_at_subnet_set(bc_atci_param_t *param);

bool _config_at_mqtt_ip_read(void);
bool _config_at_mqtt_ip_set(bc_atci_param_t *param);

bool _config_at_mqtt_port_read(void);
bool _config_at_mqtt_port_set(bc_atci_param_t *param);

bool _config_at_gateway_read(void);
bool _config_at_gateway_set(bc_atci_param_t *param);

bool _config_at_dns_read(void);
bool _config_at_dns_set(bc_atci_param_t *param);

bool _config_at_write(void);
bool _config_at_factory(void);


#define CONFIG_COMMANDS {"$IP", NULL, _config_at_ip_set, _config_at_ip_read, NULL,  "IP address"}, \
                        {"$SUBNET", NULL, _config_at_subnet_set, _config_at_subnet_read, NULL,  "Subnet mask"}, \
                        {"$MQTT_IP", NULL, _config_at_mqtt_ip_set, _config_at_mqtt_ip_read, NULL, "MQTT broker IP"}, \
                        {"$MQTT_PORT", NULL, _config_at_mqtt_port_set, _config_at_mqtt_port_read, NULL, "MQTT broker port"}, \
                        {"$GATEWAY", NULL, _config_at_gateway_set, _config_at_gateway_read, NULL, "Gateway IP"}, \
                        {"$DNS", NULL, _config_at_dns_set, _config_at_dns_read, NULL, "DNS IP"}, \
                        {"&W", _config_at_write, NULL, NULL, NULL, "Write config to flash"}, \
                        {"&F", _config_at_factory, NULL, NULL, NULL, "Load default configuration"} \

// Example structure that save configuration of PIR detector
typedef struct config_t
{
    char ip[16];
    char subnet[16];
    char mqtt_ip[16];
    int mqtt_port;
    char gateway[16];
    char dns[16];

} config_t;

config_t config;

config_t default_config = {
    .ip = "192.168.1.30",
    .subnet = "255.255.255.0",
    .mqtt_ip = "192.168.1.112",
    .mqtt_port = 1883,

    .gateway = "192.168.1.1",
    .dns = "8.8.8.8"
};

void config_init()
{
    // Initialize AT command interface
    static const bc_atci_command_t commands[] = {
            CONFIG_COMMANDS,
            BC_ATCI_COMMAND_CLAC,
            BC_ATCI_COMMAND_HELP
    };
    bc_atci_init(commands, BC_ATCI_COMMANDS_LENGTH(commands));

    // Load configuration
    bc_config_init(0x12345678, &config, sizeof(config), &default_config);
}

bool _config_at_ip_read(void)
{
    bc_atci_printf("$IP:%s", config.ip);
    return true;
}

bool _config_at_ip_set(bc_atci_param_t *param)
{
    //snprintf(config.ip, sizeof(config.ip), "%s", )
    strncpy(config.ip, param->txt, sizeof(config.ip));
    return true;
}

bool _config_at_subnet_read(void)
{
    bc_atci_printf("$SUBNET:%s", config.subnet);
    return true;
}

bool _config_at_subnet_set(bc_atci_param_t *param)
{
    strncpy(config.subnet, param->txt, sizeof(config.subnet));
    return true;
}

bool _config_at_mqtt_ip_read(void)
{
    bc_atci_printf("$MQTT_IP:%s", config.mqtt_ip);
    return true;
}

bool _config_at_mqtt_ip_set(bc_atci_param_t *param)
{
    strncpy(config.mqtt_ip, param->txt, sizeof(config.mqtt_ip));
    return true;
}

bool _config_at_mqtt_port_read(void)
{
    bc_atci_printf("$MQTT_PORT:%d", config.mqtt_port);
    return true;
}

bool _config_at_mqtt_port_set(bc_atci_param_t *param)
{
    config.mqtt_port = atoi(param->txt);
    return true;
}

bool _config_at_gateway_read(void)
{
    bc_atci_printf("$GATEWAY:%s", config.gateway);
    return true;
}

bool _config_at_gateway_set(bc_atci_param_t *param)
{
    strncpy(config.gateway, param->txt, sizeof(config.gateway));
    return true;
}

bool _config_at_dns_read(void)
{
    bc_atci_printf("$DNS:%s", config.dns);
    return true;
}

bool _config_at_dns_set(bc_atci_param_t *param)
{
    strncpy(config.dns, param->txt, sizeof(config.dns));
    return true;
}

bool _config_at_write(void)
{
    bc_config_save();
    bc_system_reset();
    return true;
}

bool _config_at_factory(void)
{
    bc_config_reset();
    return true;
}


void config_get_wiznet_structure(wiz_NetInfo *w)
{
    int ip[4];
    sscanf(config.ip, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
    w->ip[0] = ip[0];
    w->ip[1] = ip[1];
    w->ip[2] = ip[2];
    w->ip[3] = ip[3];

    int subnet[4];
    sscanf(config.ip, "%d.%d.%d.%d", &subnet[0], &subnet[1], &subnet[2], &subnet[3]);
    w->sn[0] = subnet[0];
    w->sn[1] = subnet[1];
    w->sn[2] = subnet[2];
    w->sn[3] = subnet[3];

    int gateway[4];
    sscanf(config.gateway, "%d.%d.%d.%d", &gateway[0], &gateway[1], &gateway[2], &gateway[3]);
    w->gw[0] = gateway[0];
    w->gw[1] = gateway[1];
    w->gw[2] = gateway[2];
    w->gw[3] = gateway[3];

    int dns[4];
    sscanf(config.dns, "%d.%d.%d.%d", &dns[0], &dns[1], &dns[2], &dns[3]);
    w->dns[0] = dns[0];
    w->dns[1] = dns[1];
    w->dns[2] = dns[2];
    w->dns[3] = dns[3];
}

void config_get_mqtt(uint8_t* ip, uint16_t *port)
{
    int mqtt[4];
    sscanf(config.mqtt_ip, "%d.%d.%d.%d", &mqtt[0], &mqtt[1], &mqtt[2], &mqtt[3]);
    ip[0] = mqtt[0];
    ip[1] = mqtt[1];
    ip[2] = mqtt[2];
    ip[3] = mqtt[3];

    *port = config.mqtt_port;
}
