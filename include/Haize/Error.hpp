#ifndef INCLUDE_HAIZE_ERROR_INCLUDED
#define INCLUDE_HAIZE_ERROR_INCLUDED

#include <Muon/String.hpp>

#include "Haize/Core/Define.hpp"

namespace hz
{
	struct HAIZE_API Error
	{
		enum eState
		{
			SUCCESS,
			ERROR,
		} state;

		enum eStep
		{
			LOADING,
			COMPILATION,
			PREPARATION,
			EXECUTION,
		} step;

		m::String message;
		m::String section;
		m::u32 line;
		m::u32 column;

		void clear()
		{
			state = SUCCESS;
			step = LOADING;
			message.clear();
			section.clear();
			line = 0;
			column = 0;
		}
	};
}
#endif
