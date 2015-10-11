/*************************************************************************
* Muon
* C++ Core Library
*------------------------------------------------------------------------
* Copyright (c) 2015 Louis Schnellbach
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would
*    be appreciated but is not required.
*
* 2. Altered source versions must be plainly marked as such, and must not
*    be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source
*    distribution.
*
*************************************************************************/

#ifndef INCLUDE_MUON_VARIANT_HPP
#define INCLUDE_MUON_VARIANT_HPP

#include <cstring>
#include "Muon/System/Assert.hpp"
#include "Muon/Memory/PoolAllocator.hpp"
#include "Muon/Meta/MetaDatabase.hpp"
#include "Muon/Meta/TypeInfo.hpp"

namespace muon
{
	class MUON_API Variant
	{
	public:
		template<typename T> explicit Variant(const T& value);
		explicit Variant(meta::MetaData* meta, void* data);
		Variant(const Variant& rhs);
		Variant();

		Variant& operator=(const Variant& rhs);
		Variant& set(const Variant& rhs);

		meta::MetaData* getMeta() const;

		template<typename T> Variant& set(const typename std::enable_if<meta::MemCopyable<T>::value, T>::type& rhs);
		template<typename T> Variant& set(const typename std::enable_if<!meta::MemCopyable<T>::value, T>::type& rhs);

		template<typename T> Variant& operator=(const T& rhs);

		template<typename T> T& get();
		template<typename T> const T& get() const;

	protected:
		meta::MetaData* m_meta;
		void* m_data;
	};

	template <typename T>
	Variant::Variant(const T& value)
	{
		set<T>(value);
	}

	template <typename T>
	T& Variant::get()
	{
		return *reinterpret_cast<T*>(m_data);
	}

	template <typename T>
	const T& Variant::get() const
	{
		return *reinterpret_cast<T*>(m_data);
	}

	template<typename T>
	Variant& Variant::operator=(const T& rhs)
	{
		return set<T>(rhs);
	}

	template<typename T>
	Variant& Variant::set(const typename std::enable_if<meta::MemCopyable<T>::value, T>::type& rhs)
	{
		meta::MetaData* m = MUON_META(T);
		MUON_ASSERT(m, "Cannot copy an NULL MetaData!");
		if (m == NULL)
		{
			// Resetting our Variant
			::free(m_data);
			m_meta = MUON_META(void);
			return *this;
		}

		// Meta are different, erase the stored one and replace by the new
		if (m_meta != MUON_META(T))
		{
			::free(m_data);
			m_meta = m;
			m_data = ::malloc(m_meta->size());
			::memcpy(m_data, &rhs, m_meta->size());
		}
		else
		{
			// They are the same, just copy the value
			::memcpy(m_data, &rhs, m_meta->size());
		}

		return *this;
	}

	template<typename T>
	Variant& Variant::set(const typename std::enable_if<!meta::MemCopyable<T>::value, T>::type& rhs)
	{
		meta::MetaData* m = MUON_META(T);
		MUON_ASSERT(m, "Cannot copy an NULL MetaData!");
		if (m == NULL)
		{
			// Resetting our Variant
			::free(m_data);
			m_meta = MUON_META(void);
			return *this;
		}

		// As we are non copyable, erase and re-allocate in any cases
		if (m_data)
		{
			memory::PoolAllocator::destroy<T>((T*)m_data);
			memory::PoolAllocator::deallocate<T>(1, (T*)m_data);
		}
		m_meta = m;
		// Allocate memory
		m_data = memory::PoolAllocator::allocate<T>(1);
		new (m_data)T(rhs);

		return *this;
	}
}

MUON_TRAITS(muon::Variant)

#endif
