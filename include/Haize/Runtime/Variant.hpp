#ifndef INCLUDE_HAIZE_VARIANT_HPP
#define INCLUDE_HAIZE_VARIANT_HPP

#include <Muon/Traits/Variant.hpp>
#include "Haize/Core/Define.hpp"

namespace hz
{
	typedef m::traits::Variant<bool, m::i32, m::f32, m::String, void*> Variant;
}
#endif
