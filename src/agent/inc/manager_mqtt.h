#ifndef manager_mqtt_h__
#define manager_mqtt_h__

class MqttManager
{
public:
	struct InitParams
	{
		std::string connString;
		std::string clientId;
	};

	typedef void(*ConnectHandler)();


	static bool create(const InitParams& params);
	static void destroy();

	static bool connect();
	static bool disconnect();

	static void subscribeOnConnected(ConnectHandler handler)
	{
		_class->_connectHandler = handler;
	}

	static void publish(const std::string& topic, const std::string& payload);
private:
	static MqttManager* _class;
	MQTTAsync _client;
	bool _disconnected;

	ConnectHandler _connectHandler;

	MqttManager(const InitParams& params);
	static void _onConnect(void* context, MQTTAsync_successData* response);
	static int _messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message);
	static void _onConnectionLost(void *context, char *cause);
	static void _onConnectFailure(void* context, MQTTAsync_failureData* response);
};

#endif // manager_mqtt_h__