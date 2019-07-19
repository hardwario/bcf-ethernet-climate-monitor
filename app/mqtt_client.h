#ifndef _MQTT_CLIENT_
#define _MQTT_CLIENT_

#include <bcl.h>
#include "MQTTClient.h"

void mqtt_client_start();
void mqtt_client_pub(char *topic, char *payload);

#endif
