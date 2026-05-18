/*
Реализуйте алгоритм решения задачи коммивояжера для неориентированного полносвязного графа, содержа-
щего 10 вершин. Используйте библиотеку Boost.Graph для реализации графа. Используйте матрицу смежности
для хранения графа. Используйте тип int для значений весов ребер графа. Инициализируйте веса ребер гра-
фа случайными значениями от 1 до 10. Используйте стандартный источник энтропии std::random_device.
Используйте стандартный генератор std::default_random_engine. Используйте стандартное распределение
std::uniform_int_distribution для реализации процесса генерации весов ребер графа. Используйте стан-
дартный алгоритм std::next_permutation для перебора всех перестановок последовательности обхода вер-
шин, включающей в себя каждую вершину графа только один раз. Учтите, что обход вершин графа должен
завершаться в начальной вершине. Используйте стандартный поток std::cout для вывода через терминал
матрицы инцидентности, оптимальной последовательности обхода вершин, а также ее суммарной стоимости.
*/

#include <algorithm>
#include <climits>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include <boost/graph/adjacency_matrix.hpp>

constexpr std::size_t k_vertex_count = 10;

using edge_weight_t = boost::property<boost::edge_weight_t, int>;

using graph_t = boost::adjacency_matrix<
    boost::undirectedS,
    boost::no_property,
    edge_weight_t>;

auto build_graph()
{
    graph_t graph(k_vertex_count);

    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_int_distribution<int> distribution(1, 10);

    auto weight_map = boost::get(boost::edge_weight, graph);

    for (std::size_t i = 0; i < k_vertex_count; ++i)
    {
        for (std::size_t j = i + 1; j < k_vertex_count; ++j)
        {
            auto const [edge, inserted] = boost::add_edge(i, j, graph);
            weight_map[edge] = distribution(engine);
        }
    }

    return graph;
}

void print_matrix(graph_t const &graph)
{
    auto const weight_map = boost::get(boost::edge_weight, graph);

    std::cout << "adjacency matrix:\n     ";

    for (std::size_t j = 0; j < k_vertex_count; ++j)
    {
        std::cout << std::setw(4) << j;
    }

    std::cout << "\n";

    for (std::size_t i = 0; i < k_vertex_count; ++i)
    {
        std::cout << std::setw(4) << i << " ";

        for (std::size_t j = 0; j < k_vertex_count; ++j)
        {
            if (i == j)
            {
                std::cout << std::setw(4) << 0;
            }
            else
            {
                auto const [edge, exists] = boost::edge(i, j, graph);
                std::cout << std::setw(4) << (exists ? weight_map[edge] : 0);
            }
        }

        std::cout << '\n';
    }
}

auto cost(graph_t const &graph, std::vector<std::size_t> const &order) -> int
{
    auto const weight_map = boost::get(boost::edge_weight, graph);

    int total = 0;

    for (std::size_t i = 0; i + 1 < order.size(); ++i)
    {
        auto const [edge, exists] = boost::edge(order[i], order[i + 1], graph);

        if (!exists)
        {
            return std::numeric_limits<int>::max();
        }

        total += weight_map[edge];
    }

    return total;
}

int main()
{
    auto const graph = build_graph();

    print_matrix(graph);

    std::vector<std::size_t> permutation(k_vertex_count - 1);
    std::iota(std::begin(permutation), std::end(permutation), 1);

    std::vector<std::size_t> best_tour;
    int best_cost = std::numeric_limits<int>::max();

    do
    {
        std::vector<std::size_t> tour;
        tour.reserve(k_vertex_count + 1);
        tour.push_back(0);
        tour.insert(std::end(tour), std::cbegin(permutation), std::cend(permutation));
        tour.push_back(0);

        auto const current = cost(graph, tour);

        if (current < best_cost)
        {
            best_cost = current;
            best_tour = std::move(tour);
        }
    }
    while (std::next_permutation(std::begin(permutation), std::end(permutation)));

    std::cout << "\noptimal tour: ";

    for (std::size_t i = 0; i < best_tour.size(); ++i)
    {
        std::cout << best_tour[i];

        if (i + 1 != best_tour.size())
        {
            std::cout << " -> ";
        }
    }

    std::cout << "\ntotal cost : " << best_cost << '\n';

    return 0;
}
