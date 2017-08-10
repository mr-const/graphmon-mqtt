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
		Data():
			temperature(0),
			graphicClock(0),
			memClock(0),
			fanSpeed(0),
			power(0),
			name("")
		{
		}
	};

	static bool create();
	static void destroy();

	static bool init();
	static void term();

	static pplx::task< std::vector<Data> > readAll();

	static Data readByIndex(uint32_t idx);
	static Data readByHandle(nvmlDevice_t handle);

private:
	static NvmlManager* _class;
	uint32_t _gpuCount;

	NvmlManager();

};

#endif // manager_nvml_h__
