/*
Реализуйте алгоритм моделирования игры Жизнь по стандартным правилам на игровом поле размером 10 на 10
клеток. Используйте библиотеку Boost.MultiArray для реализации двумерного хранилища состояний игрового поля.
Установите вручную некоторый паттерн начального состояния игрового поля. Используйте стандартный поток
std::cout для вывода через терминал состояний игрового поля на всех итерациях моделирования.
*/

#include <cassert>
#include <cstddef>
#include <iostream>

#include <boost/multi_array.hpp>

using Board = boost::multi_array<int, 2>;

auto make_board()
{
    return Board(boost::extents[10][10]);
}

void print(Board const &board, std::size_t generation)
{
    std::cout << "generation " << generation << '\n';

    for (std::size_t row = 0; row < board.shape()[0]; ++row)
    {
        for (std::size_t column = 0; column < board.shape()[1]; ++column)
        {
            std::cout << (board[row][column] ? '#' : '.');
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}

auto count_neighbors(Board const &board, std::size_t row, std::size_t column)
{
    auto alive = 0;

    for (auto dr = -1; dr <= 1; ++dr)
    {
        for (auto dc = -1; dc <= 1; ++dc)
        {
            if (dr == 0 && dc == 0)
            {
                continue;
            }

            auto const nr = static_cast<int>(row) + dr;
            auto const nc = static_cast<int>(column) + dc;

            if (nr >= 0 &&
                nr < static_cast<int>(board.shape()[0]) &&
                nc >= 0 &&
                nc < static_cast<int>(board.shape()[1]))
            {
                alive += board[static_cast<std::size_t>(nr)][static_cast<std::size_t>(nc)];
            }
        }
    }

    return alive;
}

auto next_generation(Board const &board)
{
    auto next = make_board();

    for (std::size_t row = 0; row < board.shape()[0]; ++row)
    {
        for (std::size_t column = 0; column < board.shape()[1]; ++column)
        {
            auto const neighbors = count_neighbors(board, row, column);

            if (board[row][column])
            {
                next[row][column] = neighbors == 2 || neighbors == 3;
            }
            else
            {
                next[row][column] = neighbors == 3;
            }
        }
    }

    return next;
}

int main()
{
    auto board = make_board();

    board[5][4] = 1;
    board[5][5] = 1;
    board[5][6] = 1;

    print(board, 0);

    board = next_generation(board);
    print(board, 1);

    assert(board[4][5] == 1);
    assert(board[5][5] == 1);
    assert(board[6][5] == 1);
    assert(board[5][4] == 0);
    assert(board[5][6] == 0);

    board = next_generation(board);
    print(board, 2);

    assert(board[5][4] == 1);
    assert(board[5][5] == 1);
    assert(board[5][6] == 1);
    assert(board[4][5] == 0);
    assert(board[6][5] == 0);

    for (std::size_t generation = 3; generation <= 5; ++generation)
    {
        board = next_generation(board);
        print(board, generation);
    }

    std::cout << "10.03 : ok\n";

    return 0;
}
