#include <iostream>
#include <Muon/Memory/Allocator.hpp>
#include "Haize/VM/GCObject.hpp"

namespace hz
{
	GCObject::GCObject()
		: _refCount(1)
	{
	}

	GCObject::~GCObject()
	{
	}

	u32 GCObject::incCount()
	{
		++_refCount;
		return _refCount;
	}

	u32 GCObject::decCount()
	{
		--_refCount;
		return _refCount;
	}

	u32 GCObject::getCount() const
	{
		return _refCount;
	}

	std::ostream& operator<<(std::ostream& o, const GCObject& gc)
	{
		o << "(RefCount: " << gc.getCount() << ")";
		return o;
	}

	GC::GC()
		: _objects(MUON_CNEW(std::deque<GCObject*>))
	{
	}

	GC::~GC()
	{
		delete _objects;
	}

	GCObject* GC::add()
	{
		GCObject* gc = MUON_CNEW(GCObject);
		_objects->push_back(gc);
		return gc;
	}

	u32 GC::garbage()
	{
		i32 aliveInRow = 0;
		i32 count = 0;
		while (_objects->size() > 0 && aliveInRow < GC_CHECK_ALIVE_IN_ROW)
		{
			++aliveInRow;
			i32 i = (rand() % _objects->size());
			if (_objects->at(i)->getCount() == 0)
			{
				++count;
				aliveInRow = 0;
				delete _objects->at(i);
				_objects->erase(_objects->begin() + i);
			}
		}
		return count;
	}

	u32 GC::size() const
	{
		return _objects->size();
	}
}
