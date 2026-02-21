#include "person.h"

#include <iostream>
#include <utility> // std::move

static void PrintPerson(const char* label, const Person& p)
{
    std::cout << label
              << " Name=" << (p.GetName() ? p.GetName() : "(null)")
              << " Age=" << p.GetAge()
              << " ID=" << p.GetId()
              << " name_ptr=" << static_cast<const void*>(p.GetName())
              << "\n";
}

static Person MakePerson()
{
    Person temp("TempName", 99, 999);
    return temp; // may use move (or NRVO). Either is fine.
}

int main()
{
    // 1) Move from a function result
    Person a = MakePerson();
    PrintPerson("a:", a);

    // 2) Force a move explicitly
    Person b("Bindi", 25, 1001);
    PrintPerson("b (before move):", b);

    Person c = std::move(b); // calls move constructor
    PrintPerson("c (moved-from b):", c);
    PrintPerson("b (after move):", b); // b should be safe (name may be null)

    return 0;
}