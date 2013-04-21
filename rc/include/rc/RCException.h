#pragma once

#include <exception>
#include <string>

namespace rc
{
	class RCException : public std::exception
	{
	public:
		RCException( const std::string& reason )
			: m_Reason( reason )
		{}

		const char* what() const { return m_Reason.c_str(); }
	private:
		std::string  m_Reason;
	};
}