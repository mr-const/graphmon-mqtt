#include <stdafx.h>
#include "spdlog/sinks/wincolor_sink.h"
#include "spdlog/sinks/file_sinks.h"
#include "spdlog/logger.h"

int main(int argc, char* argv[])
{
	std::vector<spdlog::sink_ptr> sinks;
	auto stdout_sink = spdlog::sinks::stdout_sink_mt::instance();
	auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink<std::mutex>>();
	sinks.push_back(color_sink);
	// Add more sinks here, if needed.
#undef logger
	auto combined_logger = std::make_shared<spdlog::logger>("COMMONLOG", begin(sinks), end(sinks));
	spdlog::register_logger(combined_logger);

	combined_logger->flush_on(spdlog::level::info);
	combined_logger->info("Logger initialized");

}
