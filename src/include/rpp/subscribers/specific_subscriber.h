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

#include <rpp/fwd.h>
#include <rpp/observers/specific_observer.h>
#include <rpp/subscribers/subscriber_base.h>
#include <rpp/utils/function_traits.h>

namespace rpp
{
/**
 * \brief specific version of subscriber which stores type of observer used inside to prevent extra allocations
 * \tparam Type type of values expected by this subscriber
 * \tparam Observer observer which was wrapped by this subscriber
 */
template<constraint::decayed_type Type, constraint::decayed_observer Observer>
class specific_subscriber : public details::subscriber_base<Type>
{
public:
    template<typename ...Types>
    specific_subscriber(Types&&...vals) requires std::constructible_from<Observer, Types...>
        : details::subscriber_base<Type>{  }
        , m_observer{ std::forward<Types>(vals)... } {}

    template<typename ...Types>
    specific_subscriber(constraint::decayed_same_as<composite_subscription> auto&& sub, Types&&...vals) requires std::constructible_from<Observer, Types...>
        : details::subscriber_base<Type>{ std::forward<decltype(sub)>(sub )}
        , m_observer{ std::forward<Types>(vals)... } {}

    // ************* Copy/Move ************************* //
    specific_subscriber(const specific_subscriber&) = default;
    specific_subscriber(specific_subscriber&&) noexcept = default;


    const Observer& get_observer() const
    {
        return m_observer;
    }

    auto as_dynamic() const & { return dynamic_subscriber<Type>{*this}; }
    auto as_dynamic() && { return dynamic_subscriber<Type>{this->get_subscription(), std::move(m_observer)}; }
protected:
    void on_next_impl(const Type& val) const final
    {
        m_observer.on_next(val);
    }

    void on_next_impl(Type&& val) const final
    {
        m_observer.on_next(std::move(val));
    }

    void on_error_impl(const std::exception_ptr& err) const final
    {
        m_observer.on_error(err);
    }

    void on_completed_impl() const final
    {
        m_observer.on_completed();
    }

private:
    Observer m_observer{};
};

template<constraint::observer TObs>
specific_subscriber(TObs observer) -> specific_subscriber<utils::extract_observer_type_t<TObs>, TObs>;

template<constraint::observer TObs>
specific_subscriber(composite_subscription, TObs observer) -> specific_subscriber<utils::extract_observer_type_t<TObs>, TObs>;

template<typename OnNext,
         typename ...Args,
         typename Type = std::decay_t<utils::function_argument_t<OnNext>>>
specific_subscriber(composite_subscription, OnNext, Args ...) -> specific_subscriber<Type, details::deduce_specific_observer_type_t<Type, OnNext, Args...>>;

template<typename OnNext,
         typename ...Args,
         typename Type = std::decay_t<utils::function_argument_t<OnNext>>>
specific_subscriber(OnNext, Args ...) -> specific_subscriber<Type, details::deduce_specific_observer_type_t<Type, OnNext, Args...>>;


/**
 * \brief Creation of rpp::specific_subscriber with manual providing of type of subscriber. In case of ability to determine type of subscriber by function -> use constructor
 */
template<typename Type, typename ...Args>
auto make_specific_subscriber(Args&& ...args) -> specific_subscriber<Type, decltype(rpp::make_specific_observer<Type>(std::declval<Args>()...))>
{
    auto observer = rpp::make_specific_observer<Type>(std::forward<Args>(args)...);
    return rpp::specific_subscriber<Type, decltype(observer)>(std::move(observer));
}

template<typename Type, typename ...Args>
auto make_specific_subscriber(constraint::decayed_same_as<composite_subscription> auto&& sub, Args&& ...args)  -> specific_subscriber<Type, decltype(rpp::make_specific_observer<Type>(std::declval<Args>()...))>
{
    auto observer = rpp::make_specific_observer<Type>(std::forward<Args>(args)...);
    return rpp::specific_subscriber<Type, decltype(observer)>(std::forward<decltype(sub)>(sub), std::move(observer));
}

} // namespace rpp