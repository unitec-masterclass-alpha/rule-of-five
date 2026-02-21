#include "person.h"
#include <iostream>

int main()
{
    Person a("Piotr", 25, 1001);

    // Copy construction
    Person b("Sasha", 30, 1002);

    b = a; // Assignment operator (not defined, will use default shallow copy)

    std::cout << "a: " << a.GetName()
              << "Address of a[0]: " << static_cast<const void*>(a.GetName())
              << " age=" << a.GetAge()
              << " id=" << a.GetId() << "\n";

    std::cout << "b: " << b.GetName()
              << "Address of b[0]: " << static_cast<const void*>(b.GetName())
              << " age=" << b.GetAge()
              << " id=" << b.GetId() << "\n";

    return 0;
}