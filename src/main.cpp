#include "person.h"
#include <iostream>

void PrintPersonA(const Person& p)
{
    std::cout << "Name: " << p.GetName()
              << ", Age: " << p.GetAge()
              << ", ID: " << p.GetId()
              << " name_ptr=" << static_cast<const void*>(p.GetName()) << "\n";
}
void PrintPersonB(const Person p)
{
    std::cout << "Name: " << p.GetName()
              << ", Age: " << p.GetAge()
              << ", ID: " << p.GetId()
              << " name_ptr=" << static_cast<const void*>(p.GetName()) << "\n";
}
int main()
{
    Person a("Bindi", 25, 1001);

    // Copy constructor call (compiler-generated):
    Person b = a;

    std::cout << "a: " << a.GetName()
              << " age=" << a.GetAge()
              << " id=" << a.GetId()
              << " name_ptr=" << static_cast<const void*>(a.GetName()) << "\n";

    std::cout << "b: " << b.GetName()
              << " age=" << b.GetAge()
              << " id=" << b.GetId()
              << " name_ptr=" << static_cast<const void*>(b.GetName()) << "\n";


    Person c("Ahmose", 30, 1003);
    std::cout << "c: " << c.GetName()
              << " age=" << c.GetAge()
              << " id=" << c.GetId()
              << " name_ptr=" << static_cast<const void*>(c.GetName()) << "\n";

    PrintPersonA(c); // Pass by reference: no copy constructor call.
    PrintPersonB(c); // Pass by value: copy constructor call -> shallow copy -> same name_ptr address.

    // Notice: a and b will print the SAME pointer address for name_ptr.
    // End of scope: ~Person runs for both -> double free.
    return 0;
}