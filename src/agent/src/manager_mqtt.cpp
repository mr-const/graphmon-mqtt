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

MqttManager::MqttManager(const InitParams& params) :
_disconnected(true),
_params(params)
{
	int rc;
	rc = MQTTClient_create(&_client, utility::conversions::to_utf8string(_params.connString).c_str(), utility::conversions::to_utf8string(_params.clientId).c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTCLIENT_SUCCESS)
	{
		logger->error("Failed to create MQTT client");
	}

	logger->info("MQTT Manager created");
}

MqttManager::~MqttManager()
{
	MQTTClient_destroy((MQTTClient*)_client);
}

pplx::task<bool> MqttManager::connect()
{
	return pplx::create_task([]() {
		int rc;

		MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
		conn_opts.keepAliveInterval = 20;
		conn_opts.cleansession = 1;

		if ((rc = MQTTClient_connect(_class->_client, &conn_opts)) != MQTTCLIENT_SUCCESS)
		{
			logger->info("Failed to connect: {}", rc);
			_class->_disconnected = true;
			return false;
		}

		logger->info("MQTT Connected to broker");
		_class->_disconnected = false;
		return true;
	});
}

pplx::task<bool> MqttManager::disconnect()
{
	return pplx::create_task([]() {
		int rc;
		if ((rc = MQTTClient_disconnect(_class->_client, _class->_params.mqttMessageTimeout)) != MQTTCLIENT_SUCCESS)
		{
			logger->error("failed to disconnect {}", rc);
			return false;
		}

		_class->_disconnected = true;
		return true;
	});
}

pplx::task<bool> MqttManager::publish(const std::string& topic, const std::string& payload)
{
	std::string fullTopic = "tele/" + utility::conversions::to_utf8string(_class->_params.topicName) + "/" + topic;
	logger->info("mqtt publish: {} - {}", fullTopic, payload);
	return pplx::create_task([=]() {
		MQTTClient_message pubmsg = MQTTClient_message_initializer;
		int rc;

		pubmsg.payload = (void*)payload.c_str();
		pubmsg.payloadlen = (int)payload.length();
		pubmsg.qos = 1;
		pubmsg.retained = 0;

		MQTTClient_deliveryToken token;

		if ((rc = MQTTClient_publishMessage(_class->_client, fullTopic.c_str(), &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
		{
			logger->error("Failed to publish message: {}", rc);
			_class->_disconnected = true;
			return false;
		}

		if ((rc = MQTTClient_waitForCompletion(_class->_client, token, _class->_params.mqttMessageTimeout * 1000)) != MQTTCLIENT_SUCCESS)
		{
			logger->error("Wait for completion failed: {}", rc);
			_class->_disconnected = true;
			return false;
		}
		logger->debug("Message with delivery token {} delivered", token);

		return true;
	});
}
