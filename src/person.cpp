#include "person.h"

#include <cstddef>
#include <cstring>

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

const char* Person::GetName() const
{
    return _name;
}

int Person::GetAge() const
{
    return _age;
}

int Person::GetId() const
{
    return _id;
}