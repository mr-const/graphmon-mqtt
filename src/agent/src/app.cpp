#include <stdafx.h>

#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/file_sinks.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/logger.h"

struct AppConfig
{
	utility::string_t connectionString;
	utility::string_t clientId;
	uint32_t telemetryInterval;
	utility::string_t mqttTopicName;
	uint32_t mqttMessageTimeout; // in seconds
};

void runTelemetry(int p)
{
	(void)p;

	if (MqttManager::isConnected()) {
		NvmlManager::readAll().then([](const std::vector<NvmlManager::Data>& data) {
			for (int i = 0; i < data.size(); i++)
			{
				MqttManager::publish("gpu" + std::to_string(i), utility::conversions::to_utf8string(data[i].toJson()));
			}
		});
	}
	else {
		logger->warn("Not connected. Read skipped, attempting reconnect");
		MqttManager::connect();
	}
}


void _initLoggers()
{
	std::vector<spdlog::sink_ptr> sinks;
	auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink<std::mutex>>();
	auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink<std::mutex>>();

	sinks.push_back(color_sink);
	sinks.push_back(msvc_sink);
	// Add more sinks here, if needed.
#undef logger
	auto combined_logger = std::make_shared<spdlog::logger>("COMMONLOG", begin(sinks), end(sinks));
	spdlog::register_logger(combined_logger);

	combined_logger->flush_on(spdlog::level::info);
	combined_logger->info("Logger initialized");
}

AppConfig _loadAppConfig()
{
	using namespace utility;
	
	AppConfig config;

	ifstream_t ifs;
	ifs.open(U("config.json"), ifstream_t::in);

	if (ifs)
	{
		try
		{
			auto jsonConfig = web::json::value::parse(ifs);
			config.connectionString = jsonConfig.at(U("connectionString")).as_string();
			config.clientId = jsonConfig.at(U("clientId")).as_string();
			config.telemetryInterval = jsonConfig.at(U("telemetryInterval")).as_number().to_uint32();
			config.mqttTopicName = jsonConfig.at(U("mqttTopicName")).as_string();
			config.mqttMessageTimeout = jsonConfig.at(U("mqttMessageTimeout")).as_number().to_uint32();
			spdlog::get("COMMONLOG")->info("Loaded config file");
			return config;
		}
		catch (std::exception& e)
		{
			spdlog::get("COMMONLOG")->info("error while reading config file: {}", e.what());
		}
	}
	else 
	{
		spdlog::get("COMMONLOG")->info("failed to open config file");
	}

	spdlog::get("COMMONLOG")->info("Loading sane defaults");

	config.connectionString = U("tcp://raspberrypi.lan:1883");
	config.clientId = U("Graphmon");
	config.telemetryInterval = 10;
	config.mqttTopicName = U("graphmon");
	config.mqttMessageTimeout = 10;

	return config;
}

void _executeMessageLoop()
{
	MSG msg;

	while (1)
	{
		// Main message loop:
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		Sleep(100);
	}
}

int main(int argc, char * const argv[])
{
	_initLoggers();

	AppConfig cfg = _loadAppConfig();

	MqttManager::InitParams params;
	params.connString = cfg.connectionString;
	params.clientId = cfg.clientId;
	params.mqttMessageTimeout = cfg.mqttMessageTimeout;
	params.topicName = cfg.mqttTopicName;
	MqttManager::create(params);

	NvmlManager::create();
	NvmlManager::init();

	auto yield = new concurrency::call<int>(runTelemetry);

	auto telemetryTimer = new concurrency::timer<int>(cfg.telemetryInterval*1000, 0, yield, true);

	MqttManager::connect().then([=](bool) {
		runTelemetry(0);
		telemetryTimer->start();
	});

	_executeMessageLoop();

	telemetryTimer->stop();
	delete telemetryTimer;

	MqttManager::disconnect().wait();
	MqttManager::destroy();

	NvmlManager::term();
	NvmlManager::destroy();

	return 0;
}
