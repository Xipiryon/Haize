#ifndef _HAIZE_GCOBJECT_INCLUDE
#define _HAIZE_GCOBJECT_INCLUDE

#include <vector>
#include <deque>

#include "Haize/Core/Config.hpp"

namespace hz
{
	class GC;
	class HAIZE_API GCObject
	{
	public:
		u32 incCount();
		u32 decCount();
		u32 getCount() const;

	protected:

	private:
		friend GC;

		GCObject();
		virtual ~GCObject();
		GCObject(const GCObject&);
		GCObject& operator=(const GCObject&);

		u32 _refCount;
	};

	class HAIZE_API GC
	{
	public:
		static GC& get()
		{
			static GC instance;
			return instance;
		}

		GCObject* add();
		u32 garbage();
		u32 size() const;

	protected:
		GC();
		~GC();

		std::deque<GCObject*>* _objects;
	};
}
#endif
