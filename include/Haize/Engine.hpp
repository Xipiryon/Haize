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
	class HAIZE_API Engine : public m::helper::NonCopyable
	{
		Engine();
		~Engine();
	public:
		MUON_SINGLETON_GET(Engine);

		/*!
		* @brief Create a new Context
		* This will create a new context.
		* If a context already exists with this name, it will be returned instead
		* @param name Context name
		* @return Created Context (or the one associated with the given name)
		*/
		Context* createContext(const m::String& name);

		/*!
		* @brief Return a Context
		* Return the Context matching the name parameter.
		* Will return NULL if no Context exists.
		* @param name Context name
		* @return Context pointer if it exists, NULL otherwhise
		*/
		Context* getContext(const m::String& name);

		/*!
		* @brief Destroy a Context
		* Clear all the memory used by the specified Context.
		* If there is no context associated with the given name,
		* then the function will return false.
		* @param name Context name
		* @return True if Context correctly destroyed, false otherwhise
		*/
		bool destroyContext(const m::String& name);

	private:
		typedef std::map<m::String, Context*> ModuleContextMap;

		ModuleContextMap* m_moduleContext;
	};
}
#endif
