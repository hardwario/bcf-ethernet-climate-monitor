
#include "mqtt_client.h"

//Socket number defines
#define TCP_SOCKET	0

//Receive Buffer Size define
#define BUFFER_SIZE	2048

//Global variables
unsigned char targetIP[4] = {192,168,1,112}; // mqtt server IP
unsigned int targetPort = 1883; // mqtt server port

unsigned char tempBuffer[BUFFER_SIZE];

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
} opts ={ (char*)"stdout-subscriber", 0, (char*)"\n", QOS0, NULL, NULL, (char*)targetIP, 1883, 0 };

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
	unsigned char testbuffer[100];
	MQTTMessage* message = md->message;
	if (opts.showtopics)
	{
		memcpy(testbuffer,(char*)message->payload,(int)message->payloadlen);
		*(testbuffer + (int)message->payloadlen + 1) = '\n';
		bc_log_debug("%s\r\n",testbuffer);
	}
	if (opts.nodelimiter)
		bc_log_debug("%.*s", (int)message->payloadlen, (char*)message->payload);
	else
		bc_log_debug("%.*s%s", (int)message->payloadlen, (char*)message->payload, opts.delimiter);
}

void mqtt_client_pub(char *topic, char *payload)
{
    MQTTMessage m;

    m.qos = QOS0;
    m.retained = 0;
    m.dup = 0;
    m.id = 0;
    m.payload = payload;
    m.payloadlen = strlen(payload);

    MQTTPublish(&c, topic, &m);
}

void mqtt_client_start()
{

    bc_tick_t x = bc_tick_get();

    while (bc_tick_get() < x + 1000);


    static unsigned char buf[100];

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
	bc_log_debug("Subscribing to %s\r\n", "hello/wiznet");
	rc = MQTTSubscribe(&c, "hello/wiznet", opts.qos, messageArrived);
	bc_log_debug("Subscribed %d\r\n", rc);

    bc_scheduler_register(mqtt_client_task, NULL, 0);

}
