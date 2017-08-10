#ifndef manager_mqtt_h__
#define manager_mqtt_h__

class MqttManager
{
public:
	struct InitParams
	{
		std::string connString;
		std::string clientId;
		uint32_t mqttMessageTimeout;
	};

	typedef void(*ConnectHandler)();


	static bool create(const InitParams& params);
	static void destroy();

	static pplx::task<bool> connect();
	static bool disconnect();

	static pplx::task<bool> publish(const std::string& topic, const std::string& payload);
private:
	static MqttManager* _class;
	InitParams _params;
	void* _client;
	bool _disconnected;

	MqttManager(const InitParams& params);
};

#endif // manager_mqtt_h__