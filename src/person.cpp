#include "person.h"

#include <cstddef>   // size_t
#include <cstring>   // std::strlen, std::strcpy

Person::Person(const char* name, int age, int id)
    : _name(nullptr), _age(age), _id(id)
{
    if (!name) {
        // Allocate an empty string if name is null
        _name = new char[1];
        _name[0] = '\0';
        return;
    }

    std::size_t len = std::strlen(name);
    _name = new char[len + 1];
    std::strcpy(_name, name);
}

void Person::SetName(const char* name)
{
    // Intentionally leaks in this step (no destructor / no delete yet).
    // We will fix ownership and cleanup in the next steps.
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