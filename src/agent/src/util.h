uint32_t getTimeMS();

/**
	Simple class for loop dt calculation, sets to current time on construction.
*/
class TimeCounter
{
private:
	uint32_t _prevTime;
public:
	TimeCounter() :
		_prevTime(getTimeMS())
	{}

	/**
	advances timer to current time and returns difference before previous advancement or start.
	\return passed time in milliseconds
	*/
	int32_t advance()
	{
		uint32_t now = getTimeMS();
		int32_t dt = now - _prevTime;
		_prevTime = now;

		return dt;
	}
};
