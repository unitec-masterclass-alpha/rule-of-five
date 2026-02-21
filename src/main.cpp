#include "person.h"
#include <iostream>
#include <utility>

static void PrintPerson(const char* label, const Person& p)
{
    std::cout << label
              << " Name=" << (p.GetName() ? p.GetName() : "(null)")
              << " Age=" << p.GetAge()
              << " ID=" << p.GetId()
              << " name_ptr=" << static_cast<const void*>(p.GetName())
              << "\n";
}

int main()
{
    Person a("Alice", 30, 1);
    Person b("Bob", 40, 2);

    PrintPerson("Before move-assign, a:", a);
    PrintPerson("Before move-assign, b:", b);

    a = std::move(b);   // Move assignment

    PrintPerson("After move-assign, a:", a);
    PrintPerson("After move-assign, b:", b);

    return 0;
}
