/*
Реализуйте систему мошеннического изменения приватного поля экземпляра некоторого класса внешним поль-
зователем, не являющимся другом этого класса. Реализуйте класс Entity_v1. Реализуйте в классе Entity_v1
приватное поле типа int. Реализуйте класс Entity_v2. Реализуйте в классе Entity_v2 публичное поле типа
int. Используйте оператор reinterpret_cast для преобразования ссылки на экземпляр класса Entity_v1 в
ссылку на экземпляр класса Entity_v2. Реализуйте хотя бы одну другую систему мошеннического изменения
приватного поля экземпляра некоторого класса внешним пользователем, не являющимся другом этого класса.
*/

#include <iostream>

// reinterpret_cast

class Entity_v1
{
private:
    int m_value = 42;

public:
    int get_value() const { return m_value; }
};

class Entity_v2
{
public:
    int m_value;
};

void reinterpret_cast_hack()
{
    Entity_v1 entity;
    std::cout << "reinterpret_cast method" << '\n';
    std::cout << "Before:" << entity.get_value() << '\n';

    Entity_v2 &hack = reinterpret_cast<Entity_v2 &>(entity);
    hack.m_value = 999;

    std::cout << "After:" << entity.get_value() << '\n';

    return;
}

// pointer arithmetics

class Secret
{
private:
    int m_hidden = 100;

public:
    int get_hidden() const { return m_hidden; }
};

void pointer_arithmetic_hack()
{
    Secret secret;
    std::cout << "Pointer arithmetic" << '\n';
    std::cout << "Before:" << secret.get_hidden() << '\n';

    // getting a pointer and writing there
    int *ptr = reinterpret_cast<int *>(&secret);
    *ptr = 777;

    std::cout << "After:" << secret.get_hidden() << '\n';

    return;
}

int main()
{
    reinterpret_cast_hack();
    pointer_arithmetic_hack();

    return 0;
}