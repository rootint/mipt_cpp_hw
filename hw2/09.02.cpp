/*
Реализуйте класс Tree для представления бинарного дерева. Реализуйте структуру Node для представления
узлов бинарного дерева как публичную вложенную структуру в классе Tree. Реализуйте в структуре Node по-
ле типа int для хранения значения текущего узла дерева, два стандартных указателя std::shared_ptr для
хранения адресов правого и левого дочерних узлов и стандартный указатель std::weak_ptr для хранения ад-
реса родительского узла. Реализуйте в классе Tree публичный стандартный указатель std::shared_ptr для
хранения адреса корневого узла дерева. Реализуйте в классе Tree публичные функции-члены traverse_v1
и traverse_v2 для вывода значений всех текущих узлов дерева в окно терминала через стандартный поток
std::cout при полном обходе дерева от корня алгоритмами поиска в ширину и поиска в глубину соответ-
ственно. Сконструируйте в функции main экземпляр класса Tree таким образом, чтобы дерево содержало
корневой узел, два дочерних узла на промежуточном уровне и четыре дочерних узла на последнем уровне.
Продемонстрируйте отсутствие неразрывных связей между узлами дерева и корректную работу деструкторов.
*/
#include <cassert>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>

class Tree
{
public:
    struct Node
    {
        explicit Node(int value) : value(value)
        {
            std::cout << "create node " << value << '\n';
        }

        ~Node()
        {
            std::cout << "destroy node " << value << '\n';
        }

        int value;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        std::weak_ptr<Node> parent;
    };

    std::shared_ptr<Node> root;

    void traverse_v1() const
    {
        std::cout << "breadth-first : ";

        if (!root)
        {
            std::cout << "<empty>\n";
            return;
        }

        std::queue<std::shared_ptr<Node>> nodes;
        nodes.push(root);

        while (!nodes.empty())
        {
            auto current = nodes.front();
            nodes.pop();

            std::cout << current->value << ' ';

            if (current->left)
            {
                nodes.push(current->left);
            }

            if (current->right)
            {
                nodes.push(current->right);
            }
        }

        std::cout << '\n';
    }

    void traverse_v2() const
    {
        std::cout << "depth-first   : ";

        if (!root)
        {
            std::cout << "<empty>\n";
            return;
        }

        std::stack<std::shared_ptr<Node>> nodes;
        nodes.push(root);

        while (!nodes.empty())
        {
            auto current = nodes.top();
            nodes.pop();

            std::cout << current->value << ' ';

            if (current->right)
            {
                nodes.push(current->right);
            }

            if (current->left)
            {
                nodes.push(current->left);
            }
        }

        std::cout << '\n';
    }
};

int main()
{
    Tree tree;

    tree.root = std::make_shared<Tree::Node>(1);

    tree.root->left = std::make_shared<Tree::Node>(2);
    tree.root->left->parent = tree.root;

    tree.root->right = std::make_shared<Tree::Node>(3);
    tree.root->right->parent = tree.root;

    tree.root->left->left = std::make_shared<Tree::Node>(4);
    tree.root->left->left->parent = tree.root->left;

    tree.root->left->right = std::make_shared<Tree::Node>(5);
    tree.root->left->right->parent = tree.root->left;

    tree.root->right->left = std::make_shared<Tree::Node>(6);
    tree.root->right->left->parent = tree.root->right;

    tree.root->right->right = std::make_shared<Tree::Node>(7);
    tree.root->right->right->parent = tree.root->right;

    tree.traverse_v1();
    tree.traverse_v2();

    auto leaf = std::weak_ptr<Tree::Node>(tree.root->right->right);

    assert(!tree.root->right->right->parent.expired());
    assert(tree.root->right->right->parent.lock()->value == 3);

    std::cout << std::boolalpha;
    std::cout << "leaf expired before reset : " << leaf.expired() << '\n';

    tree.root.reset();

    std::cout << "leaf expired after reset  : " << leaf.expired() << '\n';
    assert(leaf.expired());

    tree.traverse_v1();
    tree.traverse_v2();

    return 0;
}
