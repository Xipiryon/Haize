#ifndef INCLUDE_HAIZE_DEFINE_H_INCLUDED
#define INCLUDE_HAIZE_DEFINE_H_INCLUDED

#include <Muon/Core/Define.hpp>
#include <Muon/Core/Typedef.hpp>

//		--------------------------
//				EXPORT
//		--------------------------
#if defined(HAIZE_STATIC)
#	define HAIZE_API
#else
//Windows
#	ifdef _MSC_VER
#		if HAIZE_EXPORTS
#			define HAIZE_API __declspec(dllexport)
#		else
#			define HAIZE_API __declspec(dllimport)
#		endif
#	else
//Unix & Apple
#		if HAIZE_EXPORTS
#			define HAIZE_API __attribute__ ((visibility("default")))
#		else
#			define HAIZE_API
#		endif
#	endif
#endif

#endif //_HAIZE_DEFINE_H_INCLUDED
