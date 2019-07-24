#ifndef _MQTT_CLIENT_
#define _MQTT_CLIENT_

#include <bcl.h>
#include "MQTTClient.h"
#include "application.h"

void mqtt_client_start(char *node_alias);

void mqtt_client_pub_string(char *topic, char *payload);
void mqtt_client_pub_float(char *topic, float *value);
void mqtt_client_pub_int(char *topic, int *value);

#endif
