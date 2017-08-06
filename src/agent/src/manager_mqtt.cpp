#include <stdafx.h>

MqttManager* MqttManager::_class = nullptr;

bool MqttManager::init(const InitParams& params)
{
	if (_class == nullptr)
	{
		_class = new MqttManager(params);
		return true;
	}

	return false;
}

bool MqttManager::connect()
{
	int rc;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.onSuccess = _onConnect;
	conn_opts.onFailure = _onConnectFailure;
	conn_opts.context = _class->_client;
	if ((rc = MQTTAsync_connect(_class->_client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		logger->info("Failed to start connect: {}", rc);
		_class->_disconnected = true;
		return false;
	}
	logger->info("MQTT Connection to broker started");
	return true;
}

MqttManager::MqttManager(const InitParams& params) :
_disconnected(true)
{
	int rc;
	rc = MQTTAsync_create(&_client, params.connString.c_str(), params.clientId.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		logger->error("Failed to create MQTT Async client");
	}
	rc = MQTTAsync_setCallbacks(_client, this, _onConnectionLost, _messageArrived, NULL);
	if (rc != MQTTASYNC_SUCCESS)
	{
		logger->error("Failed to set MQTT Async callbacks");
	}

	logger->info("MQTT Manager created");
}

void MqttManager::_onConnectionLost(void *context, char *cause)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	int rc;

	logger->info("Connection lost: {}", cause);

	logger->info("Reconnecting...");

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		logger->error("Failed to start reconnect: {}", rc);
		_class->_disconnected = true;
	}
}

void MqttManager::_onConnectFailure(void* context, MQTTAsync_failureData* response)
{
	logger->error("Connect failed: {}", response ? response->code : 0);
	_class->_disconnected = true;
}

void MqttManager::_onConnect(void* context, MQTTAsync_successData* response)
{
	MQTTAsync client = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
//	int rc;

	logger->info("Connected to MQTT Broker at: {} version {}", response->alt.connect.serverURI, response->alt.connect.MQTTVersion);

//	opts.onSuccess = onSend;
// 	opts.context = client;
// 
// 	pubmsg.payload = PAYLOAD;
// 	pubmsg.payloadlen = strlen(PAYLOAD);
// 	pubmsg.qos = QOS;
// 	pubmsg.retained = 0;
// 	deliveredtoken = 0;
// 
// 	if ((rc = MQTTAsync_sendMessage(client, TOPIC, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
// 	{
// 		logger->error("Failed to start sendMessage: {}", rc);
// 		// exit(EXIT_FAILURE);
// 	}
}

int MqttManager::_messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
	logger->info("Message arrived ({}): {}", topicName, message->struct_id);
	return true;
}