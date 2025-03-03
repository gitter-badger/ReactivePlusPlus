#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <rxcpp/rx.hpp>


auto MakeSpecificObserver()
{
    return rxcpp::make_observer<int>( [](int) {});
}

auto MakeDynamicObserver()
{
    return rxcpp::make_observer_dynamic<int>([](int){});
}

auto MakeSpecificObservable()
{
    return rxcpp::observable<>::create<int>([](const auto& sub){});
}

auto MakeDynamicObservable()
{
    return MakeSpecificObservable().as_dynamic();
}

TEST_CASE("Observable construction", "[benchmark]")
{
    BENCHMARK("Specific observable construction")
    {
        return MakeSpecificObservable();
    };

    BENCHMARK("Specific observable construction + as_dynamic")
    {
        return MakeSpecificObservable().as_dynamic();
    };

}

TEST_CASE("Observable subscribe #2", "[benchmark]")
{
    auto specific = MakeSpecificObservable();
    BENCHMARK("Specific observable subscribe lambda")
    {
        return specific.subscribe([](const auto&) {});
    };

    auto dynamic = MakeDynamicObservable();
    BENCHMARK("Dynamic observable subscribe lambda")
    {
        return dynamic.subscribe([](const auto&) {});
    };

    BENCHMARK("Specific observable subscribe lambda without subscription")
    {
        return specific.subscribe([](const auto&) {});
    };

    BENCHMARK("Dynamic observable subscribe lambda without subscription")
    {
        return dynamic.subscribe([](const auto&) {});
    };

    auto subscriber = rxcpp::make_subscriber<int>([](const int&) {} );
    BENCHMARK("Specific observable subscribe specific subscriber")
    {
        return specific.subscribe(subscriber);
    };

    BENCHMARK("Dynamic observable subscribe specific subscriber")
    {
        return dynamic.subscribe(subscriber);
    };

    auto dynamic_subscriber = subscriber.as_dynamic();
    BENCHMARK("Specific observable subscribe dynamic observer")
    {
        return specific.subscribe(dynamic_subscriber);
    };

    BENCHMARK("Dynamic observable subscribe dynamic observer")
    {
        return dynamic.subscribe(dynamic_subscriber);
    };
}

TEST_CASE("Observer construction", "[benchmark]")
{
    BENCHMARK("Specific observer construction")
    {
        return MakeSpecificObserver();
    };

    BENCHMARK("Dynamic observer construction")
    {
        return MakeDynamicObserver();
    };

    BENCHMARK("Specific observer construction + as_dynamic")
    {
        return MakeSpecificObserver().as_dynamic();
    };
}

TEST_CASE("OnNext", "[benchmark]")
{
    auto specific_observer = MakeSpecificObserver();
    auto dynamic_observer = MakeDynamicObserver();

    BENCHMARK("Specific observer OnNext", i)
    {
        specific_observer.on_next(i);
        return i;
    };

    BENCHMARK("Dynamic observer OnNext", i)
    {
        dynamic_observer.on_next(i);
        return i;
    };

}

TEST_CASE("Subscriber construction", "[benchmark]")
{
    BENCHMARK("Make subsriber")
    {
        return rxcpp::make_subscriber<int>([](const int&) {});
    };

    auto sub = rxcpp::make_subscriber<int>([](const int&) {});

    BENCHMARK("Make copy of subscriber")
    {
        auto second = sub;
        return second;
    };

    BENCHMARK("Transform subsriber to dynamic")
    {
        return sub.as_dynamic();
    };
}

TEST_CASE("Observable subscribe", "[benchmark]")
{
    auto validate_observable = [](auto observable, const std::string& observable_prefix)
    {
        auto validate_with_observer = [&](const auto& observer, const std::string& observer_prefix)
        {
            BENCHMARK(observable_prefix + " observable subscribe " + observer_prefix + " observer")
            {
                return observable.subscribe(observer);
            };
        };
        validate_with_observer(MakeSpecificObserver(), "specific");
        validate_with_observer(MakeDynamicObserver(), "dynamic");
    };

    validate_observable(MakeSpecificObservable(), "Specific");
    validate_observable(MakeDynamicObservable(), "Dynamic");
}

TEST_CASE("Observable lift", "[benchmark]")
{
    auto validate_observable = [](auto observable, const std::string& observable_prefix)
    {
        auto validate_with_observer = [&](const auto& observer, const std::string& observer_prefix)
        {
            BENCHMARK(observable_prefix + " observable lift " + observer_prefix + " observer")
            {
                auto res_observable = observable.template lift<int>([](const auto& v) { return v; });
                return res_observable.subscribe(observer);
            };
        };
        validate_with_observer(MakeSpecificObserver(), "specific");
        validate_with_observer(MakeDynamicObserver(), "dynamic");
    };

    validate_observable(MakeSpecificObservable(), "Specific");
    validate_observable(MakeDynamicObservable(), "Dynamic");
}

TEST_CASE("Operators", "[benchmark]")
{
    auto obs = rxcpp::observable<>::create<int>([](const auto& sub)
        {
            sub.on_next(1);
        });
    auto sub = rxcpp::make_subscriber<int>([](const int&) {});
    BENCHMARK("map construction from observable via dot + subscribe")
    {
        return obs.map([](const auto& v)
        {
            return v * 100;
        }).subscribe(sub);
    };
}

TEST_CASE("Subscription", "[benchmark]")
{
    BENCHMARK("composite_subscription create")
    {
        return rxcpp::composite_subscription{};
    };

    rxcpp::composite_subscription sub_1{};
    rxcpp::composite_subscription sub_2{};

    BENCHMARK("composite_subscription add")
    {
        return sub_1.add(sub_2);
    };

    BENCHMARK("composite_subscription unsubscribe")
    {
        sub_1.unsubscribe();
    };
}

TEST_CASE("foundamental sources", "[benchmark]")
{
    auto sub = rxcpp::make_subscriber<int>();

    auto err = std::make_exception_ptr(std::runtime_error{""});

    BENCHMARK("empty")
    {
        return rxcpp::sources::empty<int>().subscribe(sub);
    };
    BENCHMARK("error")
    {
        return rxcpp::sources::error<int>(err).subscribe(sub);
    };
    BENCHMARK("never")
    {
        return rxcpp::sources::never<int>().subscribe(sub);
    };
}

TEST_CASE("just", "[benchmark]")
{
    auto sub = rxcpp::make_subscriber<int>();

    BENCHMARK("just send int")
    {
        return rxcpp::sources::just(1).subscribe(sub);
    };

    BENCHMARK("just send variadic")
    {
        return rxcpp::sources::from(1, 2, 3, 4, 5, 6, 7, 8, 9, 10).subscribe(sub);
    };
}

TEST_CASE("from", "[benchmark]")
{
    auto sub = rxcpp::make_subscriber<int>();

    std::vector vec{ 1 };
    BENCHMARK("from vector with int")
    {
        return rxcpp::sources::iterate(vec).subscribe(sub);
    };
}

TEST_CASE("merge", "[benchmark]")
{
    auto sub = rxcpp::make_subscriber<int>();

    BENCHMARK("merge")
    {
        return from(rxcpp::sources::just(1),
                    rxcpp::sources::just(2))
               .merge()
               .subscribe(sub);
    };
    BENCHMARK("merge_with")
    {
        return rxcpp::sources::just(1)
               .merge(rxcpp::sources::just(2))
               .subscribe(sub);
    };
}
