#ifndef logdef_h__
#define logdef_h__
#include <sstream>
#include <codecvt>
#include "spdlog/fmt/ostr.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__) || defined(__CYGWIN32__)
namespace spdlog {

	// A very useful logging function to log windows errors:
	inline std::string sysresult(DWORD result)
	{
		LPSTR message = NULL;
		LPSTR msg = reinterpret_cast<LPSTR>(&message);
		DWORD message_length = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			0, result, 0, msg, 100, NULL);
		std::stringstream ss;

		ss << "0x" << std::hex << result << " ";

		if (message_length > 0)
		{
			static std::locale loc("");
			ss.imbue(loc);
			ss << reinterpret_cast<const char*>(message);
			LocalFree(message);
		}

		return ss.str();
	}

	struct ToMbstr
	{
		const wchar_t* m_src;
		ToMbstr(const wchar_t* src)
		{
			m_src = src;
		}
	};

}

inline std::ostream& operator<<(std::ostream& out, const spdlog::ToMbstr& in) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
	out << utf16conv.to_bytes(in.m_src);
	return out;
}

inline std::ostream& operator<<(std::ostream& out, REFGUID guid) {
	out << std::uppercase;
	out.width(8);
	out << std::hex << guid.Data1 << '-';

	out.width(4);
	out << std::hex << guid.Data2 << '-';

	out.width(4);
	out << std::hex << guid.Data3 << '-';

	out.width(2);
	out << std::hex
		<< static_cast<short>(guid.Data4[0])
		<< static_cast<short>(guid.Data4[1])
		<< '-'
		<< static_cast<short>(guid.Data4[2])
		<< static_cast<short>(guid.Data4[3])
		<< static_cast<short>(guid.Data4[4])
		<< static_cast<short>(guid.Data4[5])
		<< static_cast<short>(guid.Data4[6])
		<< static_cast<short>(guid.Data4[7]);
	out << std::nouppercase;
	return out;
}

#endif


#define logger spdlog::get("COMMONLOG")
#define netLogger spdlog::get("NETPROTOLOG")

#endif // logdef_h__
