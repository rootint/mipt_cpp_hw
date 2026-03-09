
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
