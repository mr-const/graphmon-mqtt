#ifndef manager_mqtt_h__
#define manager_mqtt_h__

class MqttManager
{
public:
	struct InitParams
	{
		utility::string_t connString;
		utility::string_t clientId;
		utility::string_t topicName;
		uint32_t mqttMessageTimeout;
	};

	typedef void(*ConnectHandler)();


	static bool create(const InitParams& params);
	static void destroy();

	static pplx::task<bool> connect();
	static pplx::task<bool> disconnect();

	static bool isConnected()
	{
		return !_class->_disconnected;
	}

	static pplx::task<bool> publish(const std::string& topic, const std::string& payload);
private:
	static MqttManager* _class;
	InitParams _params;
	void* _client;
	bool _disconnected;

	MqttManager(const InitParams& params);
	~MqttManager();
};

#endif // manager_mqtt_h__