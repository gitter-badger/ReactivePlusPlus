//                  ReactivePlusPlus library
//
//          Copyright Aleksey Loginov 2022 - present.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/victimsnino/ReactivePlusPlus
//

#pragma once

#include <rpp/observers/constraints.h>
#include <rpp/utils/constraints.h>

namespace rpp::details
{
struct subscriber_tag;

template<constraint::decayed_type Type>
class subscriber_base;
} // namespace rpp::details

namespace rpp
{
template<constraint::decayed_type Type, constraint::decayed_observer Observer>
class specific_subscriber;

template<constraint::decayed_type Type>
class dynamic_subscriber;
} // namespace rpp
