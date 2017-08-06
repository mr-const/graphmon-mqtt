#ifndef manager_nvml_h__
#define manager_nvml_h__

class NvmlManager
{
public:
	struct Data
	{
		uint32_t temperature;
		uint32_t graphicClock;
		uint32_t memClock;
		uint32_t fanSpeed;
		uint32_t power;
		std::string name;

		std::string toJson() const;
	};

	typedef void (*DataReceiver)(const NvmlManager::Data&);

	static bool create();
	static void destroy();

	static bool init();

	static bool readAll();

	static bool readByIndex(uint32_t idx);
	static bool readByHandle(nvmlDevice_t handle);

	static void subscribeToData(DataReceiver receiver);
	static void unsubscribeToData(DataReceiver receiver);

private:
	static NvmlManager* _class;
	uint32_t _gpuCount;

	std::set<DataReceiver> _dataSubscribers;

	void _notifyData(const Data& data);

	NvmlManager();

};

#endif // manager_nvml_h__
