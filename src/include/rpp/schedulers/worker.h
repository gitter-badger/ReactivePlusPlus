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

#include <rpp/schedulers/constraints.h>
#include <rpp/schedulers/fwd.h>

namespace rpp::schedulers
{
template<typename T>
concept worker_strategy = std::copyable<T> && requires(const T t)
{
    t.defer_at(time_point{}, std::declval<void(*)()>());
};

template<worker_strategy Strategy>
class worker
{
public:
    template<typename ...Args>
    worker(Args&& ...args) : m_strategy{std::forward<Args>(args)...} {}

    void schedule(constraint::schedulable_fn auto&& fn) const
    {
        schedule(std::chrono::high_resolution_clock::now(), std::forward<decltype(fn)>(fn));
    }

    void schedule(time_point time_point, constraint::schedulable_fn auto&& fn) const
    {
        m_strategy.defer_at(time_point, scheduler_wrapper<std::decay_t<decltype(fn)>>{m_strategy, time_point, std::forward<decltype(fn)>(fn)});
    }

private:
    template<constraint::schedulable_fn Fn>
    struct scheduler_wrapper
    {
        scheduler_wrapper(const Strategy& strategy, time_point time_point, const Fn& fn)
            : m_strategy{strategy}
            , m_time_point{time_point}
            , m_fn{fn} {}

        scheduler_wrapper(const Strategy& strategy, time_point time_point, Fn&& fn)
            : m_strategy{strategy}
            , m_time_point{time_point}
            , m_fn{std::move(fn)} {}

        scheduler_wrapper(const scheduler_wrapper&)                                               = default; // LCOV_EXCL_LINE
        scheduler_wrapper(scheduler_wrapper&&) noexcept(std::is_nothrow_move_constructible_v<Fn>) = default;

        void operator()()
        {
            if (auto duration = m_fn())
            {
                m_time_point += duration.value();
                auto time_to_schedule = m_time_point;
                m_strategy.defer_at(time_to_schedule, std::move(*this));
            }
        }

        Strategy   m_strategy;
        time_point m_time_point;
        Fn         m_fn{};
    };
private:
    Strategy m_strategy;
};
} // namespace rpp::schedulers