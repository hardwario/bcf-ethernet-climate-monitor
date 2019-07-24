
#include "mqtt_client.h"

#include "app_relay_module.h"

//Socket number defines
#define TCP_SOCKET	0

//Receive Buffer Size define
#define BUFFER_SIZE	2048

#define TOPIC_FORMAT "node/%s/"

//Global variables
unsigned char targetIP[4] = {192,168,1,112}; // mqtt server IP
unsigned int targetPort = 1883; // mqtt server port
unsigned char tempBuffer[BUFFER_SIZE];

char *mqtt_topic_alias;

struct opts_struct
{
	char* clientid;
	int nodelimiter;
	char* delimiter;
	enum QoS qos;
	char* username;
	char* password;
	char* host;
	int port;
	int showtopics;
} opts ={ (char*)"stdout-subscriber", 1, (char*)"\n", QOS0, NULL, NULL, (char*)targetIP, 1883, 0 };

Network n;
MQTTClient c;

MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

// 1ms timer
void HAL_SYSTICK_Callback(void)
{
    MilliTimer_Handler();
}

void mqtt_client_task(void *param)
{
    (void) param;

    MQTTYield(&c, data.keepAliveInterval);
    bc_scheduler_plan_current_from_now(20);
}


// @brief messageArrived callback function
void messageArrived(MessageData* md)
{
	unsigned char payload[64];
    unsigned char topic[64];
	MQTTMessage* message = md->message;

    memcpy(topic,(char*)md->topicName->lenstring.data, (int)md->topicName->lenstring.len);
    *(topic + (int)md->topicName->lenstring.len) = '\0';

    memcpy(payload,(char*)message->payload,  (int)message->payloadlen);
    *(payload + (int)message->payloadlen) = '\0';

    bc_log_debug("topic: %s, payload: %s.", topic, payload);

    // Relay module toggle
    if (strstr((const char*)topic, "relay/0:0/state/set"))
    {
        bc_module_relay_set_state(&relay, (payload[0] == 't') ? true : false);
    }

    // Core Module LED
    if (strstr((const char*)topic, "led/-/state/set"))
    {
        bc_led_set_mode(&led, (payload[0] == 't') ? BC_LED_MODE_ON : BC_LED_MODE_OFF);
    }
}

void mqtt_client_pub_string(char *topic, char *payload)
{
    MQTTMessage m;

    m.qos = QOS0;
    m.retained = 0;
    m.dup = 0;
    m.id = 0;
    m.payload = payload;
    m.payloadlen = strlen(payload);

    char full_topic[64];
    snprintf(full_topic, sizeof(full_topic), "node/%s/%s", mqtt_topic_alias, topic);

    MQTTPublish(&c, full_topic, &m);
}

void mqtt_client_pub_float(char *topic, float *value)
{
    char payload[16];
    snprintf(payload, sizeof(payload), "%f", *value);
    mqtt_client_pub_string(topic, payload);
}

void mqtt_client_pub_int(char *topic, int *value)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%i", *value);
    mqtt_client_pub_string(topic, buffer);
}

void mqtt_client_start(char *node_alias)
{

    bc_tick_t x = bc_tick_get();
    while (bc_tick_get() < x + 1000);
    static unsigned char buf[100];

    mqtt_topic_alias = node_alias;

	NewNetwork(&n, TCP_SOCKET);
	ConnectNetwork(&n, targetIP, targetPort);
	MQTTClientInit(&c, &n, 1000, buf, sizeof(buf), tempBuffer, sizeof(tempBuffer));
	data.willFlag = 0;
	data.MQTTVersion = 3;
	data.clientID.cstring = opts.clientid;
	data.username.cstring = opts.username;
	data.password.cstring = opts.password;
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	int rc = MQTTConnect(&c, &data);
	bc_log_debug("Connected %d\r\n", rc);
	opts.showtopics = 1;

    static char subscribe_topic_relay[64];
    snprintf(subscribe_topic_relay, sizeof(subscribe_topic_relay), "node/%s/relay/#", mqtt_topic_alias);
	bc_log_debug("Subscribing to %s\r\n", subscribe_topic_relay);
	rc = MQTTSubscribe(&c, subscribe_topic_relay, opts.qos, messageArrived);

    bc_log_debug("Subscribed %d\r\n", rc);

    static char subscribe_topic_led[64];
    snprintf(subscribe_topic_led, sizeof(subscribe_topic_led), "node/%s/led/#", mqtt_topic_alias);
	bc_log_debug("Subscribing to %s\r\n", subscribe_topic_led);
	rc = MQTTSubscribe(&c, subscribe_topic_led, opts.qos, messageArrived);

	bc_log_debug("Subscribed %d\r\n", rc);

    bc_scheduler_register(mqtt_client_task, NULL, 0);

}
