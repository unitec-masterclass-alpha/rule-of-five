#include "person.h"

#include <iostream>

int main()
{
    Person p("Ahmed", 25, 1001);
    std::cout << p.GetName() << " age=" << p.GetAge() << " id=" << p.GetId() << "\n";

    // Make the leak obvious: allocate a new name without freeing the old one.
    p.SetName("Ahmed Mustafa");
    std::cout << p.GetName() << " age=" << p.GetAge() << " id=" << p.GetId() << "\n";

    return 0;
}