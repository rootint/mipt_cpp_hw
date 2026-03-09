
// chapter : Basics of Programming

// section : Functions and Algorithms
////////////////////////////////////////////////////////////////////////////////////
// content : Quicksort Algorithm (Hoare Partition Scheme)
//
// content : Median-of-Three Pivot Selection
//
// content : Algorithmic Complexities O(N^2) and O(N*log(N))
//
// content : Cache Lines
//
// content : Function std::midpoint
//
// content : Function Templates
//
// content : Performance Profiling with Google.Benchmark
////////////////////////////////////////////////////////////////////////////////////
// support : www.cs.usfca.edu/~galles/visualization/ComparisonSort.html
////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>

template <typename T>

void order(std::vector<T> &vector, std::size_t left, std::size_t right)
{
    for (auto i = left + 1; i < right; ++i)
    {
        for (auto j = i; j > left; --j)
        {
            if (vector[j - 1] > vector[j])
            {
                std::swap(vector[j], vector[j - 1]);
            }
        }
    }
}

template <typename T>

std::size_t partition(std::vector<T> &vector, std::size_t left, std::size_t right)
{
    auto middle = std::midpoint(left, right - 1);

    auto last = right - 1;

    if (vector[left] > vector[middle])
        std::swap(vector[left], vector[middle]);

    if (vector[left] > vector[last])
        std::swap(vector[left], vector[last]);

    if (vector[middle] > vector[last])
        std::swap(vector[middle], vector[last]);

    auto pivot = vector[middle];

    auto i = left;

    auto j = last;

    while (true)
    {
        while (vector[i] < pivot)
        {
            ++i;
        }

        while (vector[j] > pivot)
        {
            --j;
        }

        if (i >= j)
        {
            return j + 1;
        }

        std::swap(vector[i], vector[j]);

        ++i;
        --j;
    }
}

template <typename T>

void split(
    std::vector<T> &vector,

    std::size_t left,
    std::size_t right,
    std::size_t threshold)
{
    if (right - left > threshold)
    {
        auto middle = partition(vector, left, right);

        split(vector, left, middle, threshold);

        split(vector, middle, right, threshold);
    }
    else
    {
        order(vector, left, right);
    }
}

template <typename T>

void sort(std::vector<T> &vector, std::size_t threshold)
{
    split(vector, 0, std::size(vector), threshold);
}

void test(benchmark::State &state)
{
    auto size = 10'000uz;

    auto threshold = static_cast<std::size_t>(state.range(0));

    std::vector<double> source(size, 0.0);

    for (auto i = 0uz; i < size; ++i)
    {
        source[i] = static_cast<double>(size - i);
    }

    for (auto _ : state)
    {
        auto vector = source;

        sort(vector, threshold);

        benchmark::DoNotOptimize(vector);
    }
}

BENCHMARK(test)->DenseRange(1, 64, 1);
BENCHMARK_MAIN();

/*
root@44051:~/mipt_cpp_hw# ./07.05
2026-03-09T19:59:24+03:00
Running ./07.05
Run on (32 X 5084.89 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 512 KiB (x16)
  L3 Unified 32768 KiB (x2)
Load Average: 3.26, 2.66, 2.02
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-----------------------------------------------------
Benchmark           Time             CPU   Iterations
-----------------------------------------------------
test/1         482414 ns       482395 ns         1453
test/2         388508 ns       388498 ns         1802
test/3         352622 ns       352609 ns         1991
test/4         349456 ns       349439 ns         2002
test/5         332476 ns       332467 ns         2105
test/6         332189 ns       332184 ns         2107
test/7         332227 ns       332218 ns         2109
test/8         332330 ns       332324 ns         2107
test/9         338187 ns       338178 ns         2070
test/10        380973 ns       380968 ns         1836
test/11        381133 ns       381122 ns         1838
test/12        381208 ns       381198 ns         1837
test/13        381125 ns       381106 ns         1837
test/14        380687 ns       380674 ns         1835
test/15        381157 ns       381146 ns         1837
test/16        381143 ns       381134 ns         1837
test/17        380960 ns       380951 ns         1837
test/18        380672 ns       380667 ns         1835
test/19        435132 ns       435126 ns         1608
test/20        498945 ns       498933 ns         1403
test/21        498753 ns       498741 ns         1403
test/22        498450 ns       498417 ns         1396
test/23        498911 ns       498899 ns         1403
test/24        498895 ns       498883 ns         1404
test/25        498080 ns       498070 ns         1404
test/26        498593 ns       498581 ns         1403
test/27        498312 ns       498296 ns         1404
test/28        498605 ns       498594 ns         1405
test/29        498634 ns       498623 ns         1407
test/30        497797 ns       497790 ns         1403
test/31        498612 ns       498604 ns         1404
test/32        498047 ns       498040 ns         1403
test/33        497561 ns       497553 ns         1403
test/34        497292 ns       497285 ns         1404
test/35        498295 ns       498285 ns         1416
test/36        497329 ns       497322 ns         1407
test/37        498545 ns       498538 ns         1403
test/38        498025 ns       497996 ns         1409
test/39        766116 ns       766105 ns          922
test/40        785325 ns       785297 ns          892
test/41        785723 ns       785699 ns          892
test/42        783392 ns       783375 ns          892
test/43        785940 ns       785917 ns          890
test/44        783935 ns       783917 ns          892
test/45        771829 ns       771818 ns          889
test/46        772825 ns       772814 ns          906
test/47        772714 ns       772702 ns          906
test/48        773306 ns       773286 ns          906
test/49        772743 ns       772726 ns          906
test/50        772667 ns       772635 ns          906
test/51        772848 ns       772828 ns          906
test/52        772481 ns       772470 ns          906
test/53        772700 ns       772667 ns          906
test/54        772536 ns       772515 ns          906
test/55        772568 ns       772557 ns          906
test/56        772686 ns       772675 ns          909
test/57        772804 ns       772777 ns          906
test/58        772972 ns       772947 ns          906
test/59        772617 ns       772587 ns          906
test/60        772784 ns       772772 ns          906
test/61        772330 ns       772310 ns          906
test/62        773145 ns       773138 ns          905
test/63        772864 ns       772841 ns          906
test/64        772363 ns       772347 ns          906
*/
