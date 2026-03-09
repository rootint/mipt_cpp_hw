
// chapter : Number Processing

// section : Chrono Management

// content : Timing

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <print>
#include <string>
#include <vector>

template <typename D = std::chrono::duration<double>>
class Timer
{
public:
    Timer(std::string const &scope) : m_scope(scope), m_running(false) {}

    ~Timer()
    {
        if (!m_durations.empty())
        {
            std::print("{} : average = {:.6f} over {} run(s)\n",
                       m_scope, average().count(), m_durations.size());
        }
    }

    void start()
    {
        assert(!m_running);

        m_running = true;

        m_begin = clock_t::now();
    }

    void stop()
    {
        assert(m_running);

        m_durations.push_back(
            std::chrono::duration_cast<D>(clock_t::now() - m_begin));

        m_running = false;
    }

    auto average() const
    {
        assert(!m_durations.empty());

        auto sum = std::accumulate(
            m_durations.begin(),
            m_durations.end(),
            D::zero());

        return D(sum.count() / m_durations.size());
    }

private:
    using clock_t = std::chrono::steady_clock;

    std::string m_scope;
    clock_t::time_point m_begin;
    bool m_running;
    std::vector<D> m_durations;
};

auto calculate(std::size_t size)
{
    auto x = 0.0;
    for (auto i = 0uz; i < size; ++i)
    {
        x += std::pow(std::sin(i), 2) + std::pow(std::cos(i), 2);
    }
    return x;
}

auto equal(double x, double y, double epsilon = 1e-6)
{
    return std::abs(x - y) < epsilon;
}

int main()
{
    Timer timer("main : timer");
    for (auto i = 0; i < 5; ++i)
    {
        timer.start();

        assert(equal(calculate(1'000'000), 1'000'000));

        timer.stop();
    }
    return 0;
}

/*
main : timer : average = 0.055347 over 5 run(s)
*/