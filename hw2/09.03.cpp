/*
Доработайте примеры 05.01, 05.03, 05.04, 05.09 и 05.13 таким образом, чтобы реализации использовали
стандартные интеллектуальные указатели вместо обычных указателей. Используйте стандартные указатели
std::shared_ptr и std::unique_ptr. Не используйте стандартный указатель std::weak_ptr. Подумайте над
тем, какой стандартный интеллектуальный указатель будет уместно использовать в каждом из примеров.
*/

#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

namespace builder_example
{
struct Entity
{
    int x = 0;
    int y = 0;
};

class Builder
{
public:
    virtual ~Builder() = default;

    auto make_entity() const -> std::unique_ptr<Entity>
    {
        auto entity = std::make_unique<Entity>();

        set_x(*entity);
        set_y(*entity);

        return entity;
    }

private:
    virtual void set_x(Entity &entity) const = 0;
    virtual void set_y(Entity &entity) const = 0;
};

class Builder_Client : public Builder
{
private:
    void set_x(Entity &entity) const override
    {
        entity.x = 1;
    }

    void set_y(Entity &entity) const override
    {
        entity.y = 10;
    }
};

class Builder_Server : public Builder
{
private:
    void set_x(Entity &entity) const override
    {
        entity.x = 2;
    }

    void set_y(Entity &entity) const override
    {
        entity.y = 20;
    }
};

void run()
{
    std::unique_ptr<Builder> builder = std::make_unique<Builder_Client>();

    auto entity = builder->make_entity();

    assert(entity->x == 1);
    assert(entity->y == 10);

    builder = std::make_unique<Builder_Server>();

    entity = builder->make_entity();

    assert(entity->x == 2);
    assert(entity->y == 20);
}
}

namespace abstract_factory_example
{
class Entity
{
public:
    virtual ~Entity() = default;
    virtual auto kind() const -> std::string_view = 0;
};

class Client : public Entity
{
public:
    auto kind() const -> std::string_view override
    {
        return "client";
    }
};

class Server : public Entity
{
public:
    auto kind() const -> std::string_view override
    {
        return "server";
    }
};

class Factory
{
public:
    virtual ~Factory() = default;
    virtual auto make_entity() const -> std::unique_ptr<Entity> = 0;
};

class Factory_Client : public Factory
{
public:
    auto make_entity() const -> std::unique_ptr<Entity> override
    {
        return std::make_unique<Client>();
    }
};

class Factory_Server : public Factory
{
public:
    auto make_entity() const -> std::unique_ptr<Entity> override
    {
        return std::make_unique<Server>();
    }
};

void run()
{
    std::unique_ptr<Factory> factory = std::make_unique<Factory_Client>();

    auto entity = factory->make_entity();
    assert(entity->kind() == "client");

    factory = std::make_unique<Factory_Server>();

    entity = factory->make_entity();
    assert(entity->kind() == "server");
}
}

namespace prototype_example
{
class Entity
{
public:
    virtual ~Entity() = default;
    virtual auto copy() const -> std::unique_ptr<Entity> = 0;
    virtual auto test() const -> int = 0;
};

class Client : public Entity
{
public:
    auto copy() const -> std::unique_ptr<Entity> override
    {
        return std::make_unique<Client>(*this);
    }

    auto test() const -> int override
    {
        return 1;
    }
};

class Server : public Entity
{
public:
    auto copy() const -> std::unique_ptr<Entity> override
    {
        return std::make_unique<Server>(*this);
    }

    auto test() const -> int override
    {
        return 2;
    }
};

class Prototype
{
public:
    Prototype()
    {
        m_entities.push_back(std::make_unique<Client>());
        m_entities.push_back(std::make_unique<Server>());
    }

    auto make_client() const -> std::unique_ptr<Entity>
    {
        return m_entities.at(0)->copy();
    }

    auto make_server() const -> std::unique_ptr<Entity>
    {
        return m_entities.at(1)->copy();
    }

private:
    std::vector<std::unique_ptr<Entity>> m_entities;
};

void run()
{
    Prototype prototype;

    auto client = prototype.make_client();
    auto server = prototype.make_server();

    assert(client->test() == 1);
    assert(server->test() == 2);
}
}

namespace composite_example
{
class Entity
{
public:
    virtual ~Entity() = default;
    virtual auto test() const -> int = 0;
};

class Client : public Entity
{
public:
    auto test() const -> int override
    {
        return 1;
    }
};

class Server : public Entity
{
public:
    auto test() const -> int override
    {
        return 2;
    }
};

class Composite : public Entity
{
public:
    void add(std::shared_ptr<Entity> entity)
    {
        m_entities.push_back(std::move(entity));
    }

    auto test() const -> int override
    {
        auto result = 0;

        for (auto const &entity : m_entities)
        {
            if (entity)
            {
                result += entity->test();
            }
        }

        return result;
    }

private:
    std::vector<std::shared_ptr<Entity>> m_entities;
};

auto make_composite(std::size_t client_count, std::size_t server_count)
    -> std::shared_ptr<Entity>
{
    auto composite = std::make_shared<Composite>();

    for (std::size_t i = 0; i < client_count; ++i)
    {
        composite->add(std::make_shared<Client>());
    }

    for (std::size_t i = 0; i < server_count; ++i)
    {
        composite->add(std::make_shared<Server>());
    }

    return composite;
}

void run()
{
    auto composite = std::make_shared<Composite>();

    for (std::size_t i = 0; i < 5; ++i)
    {
        composite->add(make_composite(1, 1));
    }

    std::shared_ptr<Entity> entity = composite;

    assert(entity->test() == 15);
}
}

namespace observer_example
{
class Observer
{
public:
    virtual ~Observer() = default;
    virtual void test(int x) = 0;
};

class Entity
{
public:
    void add(std::shared_ptr<Observer> observer)
    {
        m_observers.push_back(std::move(observer));
    }

    void set(int x)
    {
        m_x = x;
        notify_all();
    }

private:
    void notify_all()
    {
        for (auto const &observer : m_observers)
        {
            if (observer)
            {
                observer->test(m_x);
            }
        }
    }

    int m_x = 0;
    std::vector<std::shared_ptr<Observer>> m_observers;
};

class Client : public Observer
{
public:
    void test(int x) override
    {
        m_values.push_back(x);
        std::cout << "Client::test : x = " << x << '\n';
    }

    auto values() const -> std::vector<int> const &
    {
        return m_values;
    }

private:
    std::vector<int> m_values;
};

class Server : public Observer
{
public:
    void test(int x) override
    {
        m_values.push_back(x);
        std::cout << "Server::test : x = " << x << '\n';
    }

    auto values() const -> std::vector<int> const &
    {
        return m_values;
    }

private:
    std::vector<int> m_values;
};

void run()
{
    Entity entity;

    auto client = std::make_shared<Client>();
    auto server = std::make_shared<Server>();

    entity.add(client);
    entity.add(server);

    for (auto value = 1; value <= 2; ++value)
    {
        entity.set(value);
    }

    assert((client->values() == std::vector<int>{1, 2}));
    assert((server->values() == std::vector<int>{1, 2}));
}
}

int main()
{
    builder_example::run();
    abstract_factory_example::run();
    prototype_example::run();
    composite_example::run();
    observer_example::run();

    std::cout << "09.03 : ok\n";

    return 0;
}
