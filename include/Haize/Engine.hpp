
#ifndef INCLUDE_HAIZE_ENGINE_INCLUDED
#define INCLUDE_HAIZE_ENGINE_INCLUDED

#include <unordered_map>
#include <Muon/Helper/Singleton.hpp>
#include <Muon/Variant.hpp>
#include "Haize/Context.hpp"

/*!
* @brief Haize namespace
*/
namespace hz
{
	/*!
	* @brief Main class of the API
	* Use the Engine to manipulate Context.
	*/
	class HAIZE_API Engine : public muon::helper::Singleton<Engine>
	{
		friend class muon::helper::Singleton<Engine>;
		Engine();
		~Engine();
	public:


	private:
		typedef std::map<muon::String, Context> ModuleContextMap;

		ModuleContextMap* m_moduleContext;
	};
}
#endif

