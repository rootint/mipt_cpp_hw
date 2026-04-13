/*
Доработайте пример 09.15 таким образом, чтобы реализация использовала двунаправленные итераторы вместо
однонаправленных итераторов. Реализуйте класс List для представления двусвязного списка. Реализуйте в
структуре Node стандартный указатель std::weak_ptr для хранения адреса предыдущего узла списка.
Реализуйте в классе Iterator перегруженные операторы префиксного и постфиксного декремента. Замените в
классе List псевдоним категории итератора. Доработайте в классе List реализацию функции-члена push_back.
*/

#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

template <typename T>
class List
{
private:
    struct Node
    {
        explicit Node(T value) : value(std::move(value)) {}

        T value = T();
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> prev;
    };

public:
    class Iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        Iterator(
            std::shared_ptr<Node> node = nullptr,
            std::shared_ptr<Node> tail = nullptr)
            : m_node(std::move(node)), m_tail(std::move(tail))
        {
        }

        auto operator++(int) -> Iterator
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        auto &operator++()
        {
            assert(m_node);
            m_node = m_node->next;
            return *this;
        }

        auto operator--(int) -> Iterator
        {
            auto copy = *this;
            --(*this);
            return copy;
        }

        auto &operator--()
        {
            if (m_node)
            {
                m_node = m_node->prev.lock();
            }
            else
            {
                m_node = m_tail;
            }

            return *this;
        }

        auto &operator*() const
        {
            assert(m_node);
            return m_node->value;
        }

        auto operator->() const
        {
            assert(m_node);
            return &m_node->value;
        }

        friend auto operator==(Iterator const &lhs, Iterator const &rhs)
        {
            return lhs.m_node == rhs.m_node;
        }

    private:
        std::shared_ptr<Node> m_node;
        std::shared_ptr<Node> m_tail;
    };

    auto begin() const
    {
        return Iterator(m_head, m_tail);
    }

    auto end() const
    {
        return Iterator(nullptr, m_tail);
    }

    void push_back(T value)
    {
        auto node = std::make_shared<Node>(std::move(value));

        if (!m_head)
        {
            m_head = node;
            m_tail = node;
            return;
        }

        node->prev = m_tail;
        m_tail->next = node;
        m_tail = std::move(node);
    }

private:
    std::shared_ptr<Node> m_head;
    std::shared_ptr<Node> m_tail;
};

int main()
{
    List<int> list;

    for (auto value : {1, 2, 3, 4})
    {
        list.push_back(value);
    }

    std::vector<int> forward;

    for (auto iterator = std::begin(list); iterator != std::end(list); ++iterator)
    {
        forward.push_back(*iterator);
    }

    assert((forward == std::vector<int>{1, 2, 3, 4}));

    auto iterator = std::end(list);
    --iterator;
    assert(*iterator == 4);

    auto snapshot = iterator--;
    assert(*snapshot == 4);
    assert(*iterator == 3);

    std::vector<int> backward;

    for (auto reverse = std::end(list); reverse != std::begin(list);)
    {
        --reverse;
        backward.push_back(*reverse);
    }

    assert((backward == std::vector<int>{4, 3, 2, 1}));

    List<std::string> words;
    words.push_back("alpha");
    words.push_back("beta");

    auto word = std::begin(words);
    assert(word->size() == 5);

    for ([[maybe_unused]] auto const &element : list)
    {
    }

    std::cout << "09.05 : ok\n";

    return 0;
}
