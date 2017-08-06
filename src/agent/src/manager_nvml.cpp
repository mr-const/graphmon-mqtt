#include <stdafx.h>

NvmlManager* NvmlManager::_class = nullptr;

inline std::string nvresult(nvmlReturn_t result)
{
	const char* strErr = nvmlErrorString(result);

	std::stringstream ss;
	ss << strErr << "(" << result << ")";
	return ss.str();
}

bool NvmlManager::create()
{
	if (_class == nullptr)
	{
		_class = new NvmlManager();
		return true;
	}

	return false;
}

void NvmlManager::destroy()
{
	delete _class;
	_class = nullptr;
}

bool NvmlManager::init()
{
	nvmlReturn_t result = nvmlInit();
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed to initialize NVML: {}", nvresult(result));
		return false;
	}

	char version[NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE];

	nvmlSystemGetNVMLVersion(version, NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed to read NVML version: {}", nvresult(result));
		return false;
	}

	logger->info("Initialized NVML version {}", version);

	result = nvmlSystemGetDriverVersion(version, NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed retrieving NVidia driver version: {}", nvresult(result));
		return false;
	}

	logger->info("NVidia Driver: {}", version);

	result = nvmlDeviceGetCount(&_class->_gpuCount);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed enumerate NVidia GPUs: {}", nvresult(result));
		return false;
	}

	logger->info("Total NVidia GPUs detected: {}", _class->_gpuCount);

	return true;
}

void NvmlManager::_notifyData(const Data& data)
{
	std::for_each(_dataSubscribers.begin(), _dataSubscribers.end(), [=](auto sub) 
	{
		sub(data);
	});
}

NvmlManager::NvmlManager()
{
}

bool NvmlManager::readAll()
{
	for (uint32_t nGpu = 0; nGpu < _class->_gpuCount; nGpu++)
	{
		readByIndex(nGpu);
	}

	return true;
}

bool NvmlManager::readByIndex(uint32_t idx)
{
	nvmlDevice_t handle;
	nvmlReturn_t result = nvmlDeviceGetHandleByIndex(idx, &handle);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed obtain NVidia device handle: {}", nvresult(result));
		return false;
	}

	return readByHandle(handle);
}

bool NvmlManager::readByHandle(nvmlDevice_t handle)
{
	Data data;
	char name[NVML_DEVICE_NAME_BUFFER_SIZE];
	nvmlReturn_t result = nvmlDeviceGetName(handle, name, NVML_DEVICE_NAME_BUFFER_SIZE);
	data.name = name;

	result = nvmlDeviceGetTemperature(handle, nvmlTemperatureSensors_t::NVML_TEMPERATURE_GPU, &data.temperature);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed to retrieve GPU temperature: {}", nvresult(result));
	}
	result = nvmlDeviceGetClockInfo(handle, nvmlClockType_t::NVML_CLOCK_GRAPHICS, &data.graphicClock);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed to retrieve GPU graphics clock: {}", nvresult(result));
	}
	result = nvmlDeviceGetClockInfo(handle, nvmlClockType_t::NVML_CLOCK_MEM, &data.memClock);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed to retrieve GPU memory clock: {}", nvresult(result));
	}
	result = nvmlDeviceGetFanSpeed(handle, &data.fanSpeed);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed to retrieve fan speed: {}", nvresult(result));
	}
	result = nvmlDeviceGetPowerUsage(handle, &data.power);
	if (result != NVML_SUCCESS)
	{
		logger->error("Failed to retrieve power use: {}", nvresult(result));
	}

	_class->_notifyData(data);

	return true;
}

void NvmlManager::subscribeToData(DataReceiver receiver)
{
	_class->_dataSubscribers.insert(receiver);
}

void NvmlManager::unsubscribeToData(DataReceiver receiver)
{
	_class->_dataSubscribers.erase(receiver);
}

std::string NvmlManager::Data::toJson() const
{
	std::stringstream ss;
	ss << "{ ";
	ss << "temperature: " << temperature << ", ";
	ss << "graphClock: " << graphicClock << ", ";
	ss << "memClock: " << memClock << ", ";
	ss << "fanSpeed: " << fanSpeed << ", ";
	ss << "power: " << power << ", ";
	ss << "name : \"" << name << "\"";
	ss << " }";

	return ss.str();
}