#include <stdafx.h>

#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/file_sinks.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/logger.h"

struct AppConfig
{
	std::string connectionString;
	std::string clientId;
	uint32_t telemetryInterval;
};

void onNvmlDataReceived(const NvmlManager::Data& data)
{
	MqttManager::publish("gpu0", data.toJson());
}

void onTelemetry(int)
{
	NvmlManager::readAll();
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
	// TODO: read from JSON config file (local dir, appdata dir)
	AppConfig config;
	config.connectionString = "tcp://raspberrypi.lan:1883";
	config.clientId = "Graphmon";
	config.telemetryInterval = 10 * 1000;
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
	MqttManager::create(params);

	NvmlManager::create();
	NvmlManager::init();

	auto yield = new concurrency::call<int>(onTelemetry);

	auto telemetryTimer = new concurrency::timer<int>(cfg.telemetryInterval, 0, yield, true);

	MqttManager::connect()->then([=]() {
		telemetryTimer->start();
	});


	telemetryTimer->start();


	_executeMessageLoop();

	telemetryTimer->stop();
	delete telemetryTimer;

	return 0;
}
