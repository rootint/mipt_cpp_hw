/*
07.04 ( 07.22 )
Доработайте Ваше предыдущее решение задачи 04.01 таким образом, чтобы пользователь мог выполнять те-
стирование алгоритма сортировки. Используйте автоматизированные тесты на основе библиотеки Google.Test.
*/
////////////////////////////////////////////////////////////////////////////////////
// chapter : Basics of Programming
////////////////////////////////////////////////////////////////////////////////////
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
// content : Automated Testing with Google.Test
////////////////////////////////////////////////////////////////////////////////////
// support : www.cs.usfca.edu/~galles/visualization/ComparisonSort.html
////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>


template < typename T >

void order(std::vector < T > & vector, std::size_t left, std::size_t right)
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


template < typename T >

std::size_t partition(std::vector < T > & vector, std::size_t left, std::size_t right)
{
	auto middle = std::midpoint(left, right - 1);

	auto last = right - 1;

	if (vector[left]   > vector[middle]) std::swap(vector[left],   vector[middle]);

	if (vector[left]   > vector[last]  ) std::swap(vector[left],   vector[last]  );

	if (vector[middle] > vector[last]  ) std::swap(vector[middle], vector[last]  );

	auto pivot = vector[middle];

	auto i = left;

	auto j = last;

	while (true)
	{
		while (vector[i] < pivot) { ++i; }

		while (vector[j] > pivot) { --j; }

		if (i >= j) 
		{ 
			return j + 1; 
		}

		std::swap(vector[i], vector[j]);

		++i; --j;
	}
}


template < typename T >

void split(std::vector < T > & vector, std::size_t left, std::size_t right)
{
	if (right - left > 16)
	{
		auto middle = partition(vector, left, right);

		split(vector, left,   middle);
		
		split(vector, middle, right );
	}
	else
	{
		order(vector, left,   right );
	}
}


template < typename T >

void sort(std::vector < T > & vector)
{
	split(vector, 0, std::size(vector));
}


TEST(Sort, Empty)
{
	std::vector < int > vector;

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Single)
{
	std::vector < int > vector = { 1 };

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Two_Sorted)
{
	std::vector < int > vector = { 1, 2 };

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Two_Unsorted)
{
	std::vector < int > vector = { 2, 1 };

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Sorted)
{
	std::vector < int > vector(1'000, 0);

	std::iota(std::begin(vector), std::end(vector), 1);

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Reversed)
{
	auto size = 1'000uz;

	std::vector < int > vector(size, 0);

	for (auto i = 0uz; i < size; ++i)
	{
		vector[i] = size - i;
	}

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Equal)
{
	std::vector < int > vector(1'000, 5);

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Duplicates)
{
	std::vector < int > vector = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5 };

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Small_Insertion_Sort)
{
	std::vector < int > vector = { 8, 3, 7, 1, 5, 2, 6, 4 };

	sort(vector);

	ASSERT_EQ(std::size(vector), 8uz);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Threshold_Boundary)
{
	for (auto size = 15uz; size <= 18uz; ++size)
	{
		std::vector < int > vector(size, 0);

		for (auto i = 0uz; i < size; ++i)
		{
			vector[i] = size - i;
		}

		sort(vector);

		ASSERT_TRUE(std::ranges::is_sorted(vector));
	}
}


TEST(Sort, Random)
{
	auto size = 10'000uz;

	std::vector < int > vector(size, 0);

	std::mt19937 engine(42);

	std::uniform_int_distribution distribution(1, 1'000'000);

	for (auto i = 0uz; i < size; ++i)
	{
		vector[i] = distribution(engine);
	}

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Negative)
{
	std::vector < int > vector = { -5, 3, -1, 0, -8, 7, 2, -3 };

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, Preservation)
{
	std::vector < int > vector = { 5, 3, 1, 4, 2 };

	auto sorted = vector;

	std::ranges::sort(sorted);

	sort(vector);

	ASSERT_EQ(vector, sorted);
}


TEST(Sort, Double)
{
	std::vector < double > vector = { 3.14, 1.41, 2.72, 0.58, 1.73 };

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


TEST(Sort, String)
{
	std::vector < std::string > vector = { "delta", "alpha", "echo", "bravo", "charlie" };

	sort(vector);

	ASSERT_TRUE(std::ranges::is_sorted(vector));
}


int main(int argc, char ** argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
