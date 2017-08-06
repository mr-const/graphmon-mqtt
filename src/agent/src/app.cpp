#include <stdafx.h>
#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/file_sinks.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/logger.h"

void onNvmlDataReceived(const NvmlManager::Data& data)
{
	MqttManager::publish("gpu0", data.toJson());
}

void onConnected()
{
	NvmlManager::readAll();
}

int main(int argc, char * const argv[])
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

	MqttManager::InitParams params;
	params.connString = "tcp://raspberrypi.lan:1883";
	params.clientId = "Graphmon";
	MqttManager::create(params);

	NvmlManager::create();
	NvmlManager::init();

	NvmlManager::subscribeToData(onNvmlDataReceived);

	MqttManager::subscribeOnConnected(onConnected);
	MqttManager::connect();
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

	return 0;
}
