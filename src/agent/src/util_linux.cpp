uint32_t getTimeMS()
{
	struct tms t;

	float ticks = static_cast<float>(times(&t));
	float ticksPerMs = static_cast<float>(sysconf(_SC_CLK_TCK)) / 1000.0f;

	uint32_t uptime = ticksPerMs != 0.f ? static_cast<uint32_t>(ticks / ticksPerMs) : 0;

	return uptime - _timeAppStart;
}
