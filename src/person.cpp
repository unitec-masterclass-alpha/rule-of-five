#include "person.h"

#include <cstddef>
#include <cstring>
#include <utility> // std::move (optional)

Person::Person(const char* name, int age, int id)
    : _name(nullptr), _age(age), _id(id)
{
    if (!name) {
        _name = new char[1];
        _name[0] = '\0';
        return;
    }

    std::size_t len = std::strlen(name);
    _name = new char[len + 1];
    std::strcpy(_name, name);
}

Person::~Person()
{
    delete[] _name;
    _name = nullptr;
}

Person::Person(const Person& other)
    : _name(nullptr), _age(other._age), _id(other._id)
{
    if (!other._name) {
        _name = new char[1];
        _name[0] = '\0';
        return;
    }

    std::size_t len = std::strlen(other._name);
    _name = new char[len + 1];
    std::strcpy(_name, other._name);
}

Person& Person::operator=(const Person& other)
{
    if (this == &other) {
        return *this;
    }

    delete[] _name;
    _name = nullptr;

    _age = other._age;
    _id  = other._id;

    if (!other._name) {
        _name = new char[1];
        _name[0] = '\0';
        return *this;
    }

    std::size_t len = std::strlen(other._name);
    _name = new char[len + 1];
    std::strcpy(_name, other._name);

    return *this;
}

// NEW: Move Constructor
Person::Person(Person&& other) noexcept
    : _name(other._name), _age(other._age), _id(other._id)
{
    // Leave other in a safe state
    other._name = nullptr;
    other._age = 0;
    other._id = 0;
}

void Person::SetName(const char* name)
{
    delete[] _name;
    _name = nullptr;

    if (!name) {
        _name = new char[1];
        _name[0] = '\0';
        return;
    }

    std::size_t len = std::strlen(name);
    _name = new char[len + 1];
    std::strcpy(_name, name);
}

const char* Person::GetName() const { 
    return _name; 
}
int Person::GetAge() const { 
    return _age; 
}
int Person::GetId() const { 
    return _id; 
}
