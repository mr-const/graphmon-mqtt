#include <stdafx.h>
#include <functional>

MqttManager* MqttManager::_class = nullptr;

bool MqttManager::create(const InitParams& params)
{
	if (_class == nullptr)
	{
		_class = new MqttManager(params);
		return true;
	}

	return false;
}

void MqttManager::destroy()
{
	delete _class;
	_class = nullptr;
}

pplx::task<void> MqttManager::connect()
{
	int rc;

	concurrency::task_completion_event<void> tce;

	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.onSuccess = std::bind1st<void>([&tce](void* context, MQTTAsync_successData* response) {
	
	});
	conn_opts.onFailure = _onConnectFailure;
	conn_opts.context = _class->_client;

	if ((rc = MQTTAsync_connect(_class->_client, &conn_opts)) != MQTTASYNC_SUCCESS)
	{
		logger->info("Failed to start connect: {}", rc);
		_class->_disconnected = true;
		// TODO introduce custom exception
		throw std::exception("failed to start connect");
	}

	logger->info("MQTT Connection to broker started");
	return pplx::task<void>(tce);
}

void MqttManager::publish(const std::string& topic, const std::string& payload)
{
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	int rc;

	opts.context = _class->_client;

	pubmsg.payload = (void*)payload.c_str();
	pubmsg.payloadlen = (int)payload.length();
	pubmsg.qos = 1;
	pubmsg.retained = 0;

	if ((rc = MQTTAsync_sendMessage(_class->_client, ("tele/graphmon/" + topic).c_str(), &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
	{
		logger->error("Failed to start sendMessage: {}", rc);
		// exit(EXIT_FAILURE);
	}

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
	logger->info("Connected to MQTT Broker at: {} version {}", response->alt.connect.serverURI, response->alt.connect.MQTTVersion);
	_class->_disconnected = false;
	if (_class->_connectHandler)
		_class->_connectHandler();
}

int MqttManager::_messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
	logger->info("Message arrived ({}): {}", topicName, message->struct_id);
	return true;
}
