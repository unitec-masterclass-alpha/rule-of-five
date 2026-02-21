# Rule of Five

## Set up the Environment

> I am copying this from repo "the-problem"

1. Create the repo in GitHub
   1. Name: rule-of-five-alpha
   2. Visibility: Public
   3. README.md: True
   4. License: MIT
2. Clone the Repo
3. Edit License to include your name in the (c) = ©
4. Create the file system structure with the following empty files

```
project-root/
│
├── .devcontainer/              # VS Code Dev Container configuration
│   ├── devcontainer.json       # Dev container settings
│   └── Dockerfile              # Container build instructions
│
├── include/                    # Header files
│   └── person.h
│
├── src/                        # Source files
│   ├── person.cpp
│   └── main.cpp
│
├── makefile                    # Build configuration
├── .gitignore                  # Git ignored files
├── README.md                   # Project documentation
└── LICENSE                     # License file


```

The following sequence of commands executed at the command line with the repo's directory achieves this structure:
```bash
mkdir -p .devcontainer include src
touch makefile include/person.h src/main.cpp src/person.cpp 
touch .devcontainer/Dockerfile .devcontainer/devcontainer.json
```

:pushpin: `TAG step-01-00-structure`


5. Write the following code on `.devcontainer/devcontainer.json`:
```json
{
  "name": "UNITEC C++ Workshop",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "customizations": {
    "vscode": {
      "settings": {
        "terminal.integrated.defaultProfile.linux": "bash",
        "C_Cpp.default.compilerPath": "/usr/bin/g++"
      },
      "extensions": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "ms-azuretools.vscode-docker"
      ]
    }
  },
  "remoteUser": "vscode",
  "postCreateCommand": "make doctor || true"
}
```

6. Write the following code on `.devcontainer/Dockerfile`:
```dockerfile
FROM mcr.microsoft.com/devcontainers/base:ubuntu
# Why not ubuntu:latest? Because the devcontainer base image has some optimizations and tools pre-installed that are useful for development.

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang \
    gdb \
    llvm \
    libclang-rt-18-dev \
    valgrind \
    make \
    git \
    vim \
    curl \
    ca-certificates \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Optional but nice: install GitHub CLI (gh)
RUN type -p curl >/dev/null || (apt-get update && apt-get install -y curl) \
 && curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg \
    | dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg \
 && chmod go+r /usr/share/keyrings/githubcli-archive-keyring.gpg \
 && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" \
    > /etc/apt/sources.list.d/github-cli.list \
 && apt-get update \
 && apt-get install -y gh \
 && apt-get clean && rm -rf /var/lib/apt/lists/*
```
:pushpin: `TAG step-01-00-structure`


## Coding
### Destructor Natural Crash

#### Goal
Introduce the idea of ownership and the first consequence of dynamic memory without lifecycle management:
* The class allocates heap memory (new[]) for _name
* Since there is no destructor, the memory is never released
* *Tools (Valgrind / ASan leak detection) reveal the leak

This sets up the motivation for:

> “If your class owns a resource, it must define how that resource is released.”

#### State of Code

* Person has:
  * _name as a char* allocated with new[]
  * _age, _id as plain ints
* Implemented:
  * Person(const char* name, int age, int id) allocates and copies name
  * SetName(const char* name) replaces the name (this will also leak in this step unless we free first—see note below)
  * getters
* Not implemented:
  * Destructor (intentionally missing)
  * copy/move operations (not needed for this first leak demo)

##### Important Note Step 01-01
To keep the lesson focused on “missing destructor causes leaks,” in SetName we’ll not delete the old name yet (so leaks are obvious and undeniable). We’ll fix that properly when we introduce the destructor and Rule of 3/5.

#### Code

`include/person.h`
```c++
#pragma once

class Person {
private:
    char* _name;
    int _age;
    int _id;

public:
    Person(const char* name, int age, int id);

    void SetName(const char* name);

    const char* GetName() const;
    int GetAge() const;
    int GetId() const;
};
```

`src/person.cpp`
```c++
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
```

`src/main.cpp`
```c++
int main()
{
    Person p("Ahmed", 25, 1001);
    std::cout << p.GetName() << " age=" << p.GetAge() << " id=" << p.GetId() << "\n";

    // Make the leak obvious: allocate a new name without freeing the old one.
    p.SetName("Ahmed Mustafa");
    std::cout << p.GetName() << " age=" << p.GetAge() << " id=" << p.GetId() << "\n";

    return 0;
}
```


#### Expected Results

- make run:
  - Program runs normally and prints two lines
  - No visible crash (leaks are “silent”)
```bash
vscode ➜ /workspaces/rule-of-five (main) $ make run
mkdir -p build
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -c src/main.cpp -o build/main.o
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -c src/person.cpp -o build/person.o
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude build/main.o build/person.o -o app 
./app
Ahmed age=25 id=1001
Ahmed Mustafa age=25 id=1001
```

- make valgrind:
  - Valgrind reports definitely lost memory (at least one leaked allocation, likely two because of SetName)
  - Valgrind will point to the allocation sites in Person::Person and Person::SetName
```bash
vscode ➜ /workspaces/rule-of-five (main) $ make valgrind
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./app
==8003== Memcheck, a memory error detector
==8003== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==8003== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==8003== Command: ./app
==8003== 
Ahmed age=25 id=1001
Ahmed Mustafa age=25 id=1001
==8003== 
==8003== HEAP SUMMARY:
==8003==     in use at exit: 20 bytes in 2 blocks
==8003==   total heap usage: 4 allocs, 2 frees, 74,772 bytes allocated
==8003== 
==8003== 6 bytes in 1 blocks are definitely lost in loss record 1 of 2
==8003==    at 0x4886FFC: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==8003==    by 0x108C6F: Person::Person(char const*, int, int) (person.cpp:17)
==8003==    by 0x108A97: main (main.cpp:7)
==8003== 
==8003== 14 bytes in 1 blocks are definitely lost in loss record 2 of 2
==8003==    at 0x4886FFC: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==8003==    by 0x108CEB: Person::SetName(char const*) (person.cpp:32)
==8003==    by 0x108B2F: main (main.cpp:11)
==8003== 
==8003== LEAK SUMMARY:
==8003==    definitely lost: 20 bytes in 2 blocks
==8003==    indirectly lost: 0 bytes in 0 blocks
==8003==      possibly lost: 0 bytes in 0 blocks
==8003==    still reachable: 0 bytes in 0 blocks
==8003==         suppressed: 0 bytes in 0 blocks
==8003== 
==8003== For lists of detected and suppressed errors, rerun with: -s
==8003== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
```
- make run-asan:
  - The program runs
  - You may or may not see leak output by default (depends on ASan options/build)
  - If you want ASan to report leaks reliably, run:
    - `ASAN_OPTIONS=detect_leaks=1 ./app_asan`
```bash
vscode ➜ /workspaces/rule-of-five (main) $ make run-asan
clang++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -fsanitize=address,undefined -fno-omit-frame-pointer src/main.cpp src/person.cpp -o app_asan 
ASAN_SYMBOLIZER_PATH=$(command -v llvm-symbolizer) \
ASAN_OPTIONS=symbolize=1 \
./app_asan
Ahmed age=25 id=1001
Ahmed Mustafa age=25 id=1001

=================================================================
==8141==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 14 byte(s) in 1 object(s) allocated from:
    #0 0xaaaacce614a8 in operator new[](unsigned long) (/workspaces/rule-of-five/app_asan+0x1114a8) (BuildId: 38ac7f5c2f9bd4cb11c090f1815de2536abe3073)
    #1 0xaaaacce64180 in Person::SetName(char const*) /workspaces/rule-of-five/src/person.cpp:32:13
    #2 0xaaaacce63950 in main /workspaces/rule-of-five/src/main.cpp:11:7
    #3 0xffffa9cb84c0 in __libc_start_call_main csu/../sysdeps/nptl/libc_start_call_main.h:58:16
    #4 0xffffa9cb8594 in __libc_start_main csu/../csu/libc-start.c:360:3
    #5 0xaaaaccd84eac in _start (/workspaces/rule-of-five/app_asan+0x34eac) (BuildId: 38ac7f5c2f9bd4cb11c090f1815de2536abe3073)

Direct leak of 6 byte(s) in 1 object(s) allocated from:
    #0 0xaaaacce614a8 in operator new[](unsigned long) (/workspaces/rule-of-five/app_asan+0x1114a8) (BuildId: 38ac7f5c2f9bd4cb11c090f1815de2536abe3073)
    #1 0xaaaacce63f28 in Person::Person(char const*, int, int) /workspaces/rule-of-five/src/person.cpp:17:13
    #2 0xaaaacce636d4 in main /workspaces/rule-of-five/src/main.cpp:7:12
    #3 0xffffa9cb84c0 in __libc_start_call_main csu/../sysdeps/nptl/libc_start_call_main.h:58:16
    #4 0xffffa9cb8594 in __libc_start_main csu/../csu/libc-start.c:360:3
    #5 0xaaaaccd84eac in _start (/workspaces/rule-of-five/app_asan+0x34eac) (BuildId: 38ac7f5c2f9bd4cb11c090f1815de2536abe3073)

SUMMARY: AddressSanitizer: 20 byte(s) leaked in 2 allocation(s).
make: *** [makefile:47: run-asan] Error 1


```

:pushpin: `TAG step-01-01-memory-leak-no-destructor`

---


### Copy Constructor

#### Goal

Fix the **shallow-copy bug** by implementing a proper **copy constructor** that performs a **deep copy** of `_name`.

This step teaches:

- The compiler-generated copy constructor performs a **memberwise copy**
- For raw pointers, that means two objects end up pointing to the **same heap memory**
- If a destructor deletes that memory, both objects will attempt to free the same pointer → **double free**
- The fix is to allocate new memory and copy the data

> Key idea: Each `Person` must **own its own copy** of `_name`.

---

#### State of Code

- `Person` dynamically allocates `_name` using `new[]`
- Destructor exists and deletes `_name`
- Copy constructor is now explicitly implemented
- Copy assignment operator is **not implemented yet**
- Move operations are not implemented yet

After this step:

- `Person b = a;` performs a **deep copy**
- `a._name` and `b._name` point to **different** heap allocations
- No double-free occurs at scope exit

---

#### Code

`include/person.h`

```cpp
#pragma once

class Person {
private:
    char* _name;
    int _age;
    int _id;

public:
    Person(const char* name, int age, int id);
    ~Person();

    // Copy Constructor (deep copy)
    Person(const Person& other);

    void SetName(const char* name);

    const char* GetName() const;
    int GetAge() const;
    int GetId() const;
};

`src/person.cpp`
```c++
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

// Copy Constructor (deep copy)
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
```

`scr/main.cpp`
```c++
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
```

#### Expected Results

make run
* Program runs normally.
* Both a and b print identical values.
* No crash at scope exit.
```bash
vscode ➜ /workspaces/rule-of-five (main) $ make run
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -c src/main.cpp -o build/main.o
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude build/main.o build/person.o -o app 
./app
a: PiotrAddress of a[0]: 0xaaab26b992b0 age=25 id=1001
b: PiotrAddress of b[0]: 0xaaab26b992d0 age=25 id=1001
vscode ➜ /workspaces/rule-of-five (main) $ 
```
make run-asan
* No double-free reported.
* ASan should be clean for this scenario.
```bash
vscode ➜ /workspaces/rule-of-five (main) $ make run asan
./app
a: PiotrAddress of a[0]: 0xaaaaf76222b0 age=25 id=1001
b: PiotrAddress of b[0]: 0xaaaaf76222d0 age=25 id=1001
clang++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -fsanitize=address,undefined -fno-omit-frame-pointer src/main.cpp src/person.cpp -o app_asan 
vscode ➜ /workspaces/rule-of-five (main) $ ./app_asan 
a: PiotrAddress of a[0]: 0x502000000010 age=25 id=1001
b: PiotrAddress of b[0]: 0x502000000030 age=25 id=1001
```

make valgrind
* No “definitely lost” memory (assuming destructor correctly frees memory).
* If leaks appear, Valgrind will point to the relevant allocation site.
```bash
vscode ➜ /workspaces/rule-of-five (main) $ make valgrind
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -c src/main.cpp -o build/main.o
src/main.cpp: In function ‘int main()’:
src/main.cpp:11:9: warning: implicitly-declared ‘constexpr Person& Person::operator=(const Person&)’ is deprecated [-Wdeprecated-copy]
   11 |     b = a; // Assignment operator (not defined, will use default shallow copy)
      |         ^
In file included from src/main.cpp:1:
include/person.h:11:5: note: because ‘Person’ has user-provided ‘Person::Person(const Person&)’
   11 |     Person(const Person& other); // Here
      |     ^~~~~~
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude build/main.o build/person.o -o app 
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./app
==26867== Memcheck, a memory error detector
==26867== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==26867== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==26867== Command: ./app
==26867== 
a: PiotrAddress of a[0]: 0x4e03080 age=25 id=1001
b: PiotrAddress of b[0]: 0x4e03080 age=25 id=1001
==26867== Invalid free() / delete / delete[] / realloc()
==26867==    at 0x488A834: operator delete[](void*) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==26867==    by 0x108F6B: Person::~Person() (person.cpp:22)
==26867==    by 0x108E1B: main (main.cpp:24)
==26867==  Address 0x4e03080 is 0 bytes inside a block of size 6 free'd
==26867==    at 0x488A834: operator delete[](void*) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==26867==    by 0x108F6B: Person::~Person() (person.cpp:22)
==26867==    by 0x108E13: main (main.cpp:24)
==26867==  Block was alloc'd at
==26867==    at 0x4886FFC: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==26867==    by 0x108F1F: Person::Person(char const*, int, int) (person.cpp:16)
==26867==    by 0x108C57: main (main.cpp:6)
==26867== 
==26867== 
==26867== HEAP SUMMARY:
==26867==     in use at exit: 6 bytes in 1 blocks
==26867==   total heap usage: 4 allocs, 4 frees, 74,764 bytes allocated
==26867== 
==26867== 6 bytes in 1 blocks are definitely lost in loss record 1 of 1
==26867==    at 0x4886FFC: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==26867==    by 0x108F1F: Person::Person(char const*, int, int) (person.cpp:16)
==26867==    by 0x108C73: main (main.cpp:9)
==26867== 
==26867== LEAK SUMMARY:
==26867==    definitely lost: 6 bytes in 1 blocks
==26867==    indirectly lost: 0 bytes in 0 blocks
==26867==      possibly lost: 0 bytes in 0 blocks
==26867==    still reachable: 0 bytes in 0 blocks
==26867==         suppressed: 0 bytes in 0 blocks
==26867== 
==26867== For lists of detected and suppressed errors, rerun with: -s
==26867== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
vscode ➜ /workspaces/rule-of-five (main) $ 
```


:pushpin: `TAG step-01-02-copy-assg-op`


### Copy Constructor

#### Goal

Create a **Copy Constructor disaster** by relying on the **compiler-generated copy constructor** (which performs a shallow copy of `_name`).

This step demonstrates:

- The compiler will automatically generate a copy constructor if you don’t write one.
- That default copy constructor copies **the pointer value**, not the heap data.
- After copying, **two `Person` objects point to the same `_name` buffer**.
- When the objects are destroyed, both destructors attempt to `delete[]` the same pointer → **double free**.
- ASan and Valgrind will reveal the bug and point to the exact lines.

> Key idea: If your class owns dynamic memory, the compiler-generated copy constructor is almost always wrong.

---

#### State of Code

- `Person` owns `_name` (allocated with `new[]`) and stores `_age`, `_id`.
- `Person` has a **constructor** that allocates/copies `_name`.
- `Person` has a **destructor** that does `delete[] _name;`.
- `Person` does **NOT** implement a copy constructor.
  - Therefore, the compiler provides a default one (shallow copy).

In this step, we intentionally copy-construct a `Person`:

```cpp
Person a("Bindi", 25, 1001);
Person b = a;  // Copy construction -> shallow copy of _name (DISASTER)
```

#### Code
`include/person.h`
```c++
#pragma once

class Person {
private:
    char* _name;
    int _age;
    int _id;

public:
    Person(const char* name, int age, int id);
    ~Person();

    // Intentionally missing copy constructor in this tag:
    // Person(const Person& other);

    void SetName(const char* name);

    const char* GetName() const;
    int GetAge() const;
    int GetId() const;
};
```

`src/person.h`
```c++
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
```

`src/main.cpp`
```c++
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
```
#### Expected Results
- make run
  - Program may crash at the end of `main` (undefined behavior).
  - Output should show:

    - `a` and `b` have the **same `name_ptr` address** (evidence of shallow copy).
    - `c` has a different `name_ptr` from `a` and `b`.
    - `PrintPersonA(c)` (pass by reference) does **not** create a copy.
      - The printed `name_ptr` will match `c` exactly.
    - `PrintPersonB(c)` (pass by value) **does create a copy**.
      - The printed `name_ptr` inside `PrintPersonB` will be the **same address as `c`**, proving that the compiler-generated copy constructor performed a shallow copy.

  - At scope exit:
    - Destructors run for `a`, `b`, and `c`.
    - The shallow copies cause multiple objects to attempt to `delete[]` the same `_name`.
    - This results in a **double-free** (or multiple double-frees).
```bash
vscode ➜ /workspaces/rule-of-five (main) $ make run
mkdir -p build
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -c src/main.cpp -o build/main.o
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -c src/person.cpp -o build/person.o
g++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude build/main.o build/person.o -o app 
./app
a: Bindi age=25 id=1001 name_ptr=0xaaaaf61e02b0
b: Bindi age=25 id=1001 name_ptr=0xaaaaf61e02b0
c: Ahmose age=30 id=1003 name_ptr=0xaaaaf61e06e0
Name: Ahmose, Age: 30, ID: 1003 name_ptr=0xaaaaf61e06e0
Name: Ahmose, Age: 30, ID: 1003 name_ptr=0xaaaaf61e06e0
free(): double free detected in tcache 2
make: *** [makefile:40: run] Aborted
vscode ➜ /workspaces/rule-of-five (main) $ 
```
---

- make run-asan
  - ASan should reliably report:

    - `ERROR: AddressSanitizer: attempting double-free`

  - The stack trace should point to:

    - `Person::~Person()` (where `delete[] _name` is executed)
    - `main` (scope exit for `a`, `b`, and `c`)
    - Potentially also `PrintPersonB` (because passing by value triggered an additional shallow copy)

  - The allocation trace should point back to:

    - `Person::Person(const char*, int, int)` (where `_name` was allocated)

This clearly demonstrates:
- Copy construction is occurring in two places:
  - `Person b = a;`
  - `PrintPersonB(c);`
- The compiler-generated copy constructor is performing a shallow copy.

```bash
vscode ➜ /workspaces/rule-of-five (main) $ make run-asan
clang++ -std=c++20 -g -O0 -Wall -Wextra -pedantic -Iinclude -fsanitize=address,undefined -fno-omit-frame-pointer src/main.cpp src/person.cpp -o app_asan 
ASAN_SYMBOLIZER_PATH=$(command -v llvm-symbolizer) \
ASAN_OPTIONS=symbolize=1 \
./app_asan
a: Bindi age=25 id=1001 name_ptr=0x502000000010
b: Bindi age=25 id=1001 name_ptr=0x502000000010
c: Ahmose age=30 id=1003 name_ptr=0x502000000030
Name: Ahmose, Age: 30, ID: 1003 name_ptr=0x502000000030
Name: Ahmose, Age: 30, ID: 1003 name_ptr=0x502000000030
=================================================================
==31722==ERROR: AddressSanitizer: attempting double-free on 0x502000000030 in thread T0:
    #0 0xaaaae85d2ac4 in operator delete[](void*) (/workspaces/rule-of-five/app_asan+0x112ac4) (BuildId: c4d8df763a9f2d93d8a052853b943516bf9689c7)
    #1 0xaaaae85d610c in Person::~Person() /workspaces/rule-of-five/src/person.cpp:22:5
    #2 0xaaaae85d5ab4 in main /workspaces/rule-of-five/src/main.cpp:48:1
    #3 0xffff949384c0 in __libc_start_call_main csu/../sysdeps/nptl/libc_start_call_main.h:58:16
    #4 0xffff94938594 in __libc_start_main csu/../csu/libc-start.c:360:3
    #5 0xaaaae84f5c2c in _start (/workspaces/rule-of-five/app_asan+0x35c2c) (BuildId: c4d8df763a9f2d93d8a052853b943516bf9689c7)

0x502000000030 is located 0 bytes inside of 7-byte region [0x502000000030,0x502000000037)
freed by thread T0 here:
    #0 0xaaaae85d2ac4 in operator delete[](void*) (/workspaces/rule-of-five/app_asan+0x112ac4) (BuildId: c4d8df763a9f2d93d8a052853b943516bf9689c7)
    #1 0xaaaae85d610c in Person::~Person() /workspaces/rule-of-five/src/person.cpp:22:5
    #2 0xaaaae85d5aa8 in main /workspaces/rule-of-five/src/main.cpp:43:5
    #3 0xffff949384c0 in __libc_start_call_main csu/../sysdeps/nptl/libc_start_call_main.h:58:16
    #4 0xffff94938594 in __libc_start_main csu/../csu/libc-start.c:360:3
    #5 0xaaaae84f5c2c in _start (/workspaces/rule-of-five/app_asan+0x35c2c) (BuildId: c4d8df763a9f2d93d8a052853b943516bf9689c7)

previously allocated by thread T0 here:
    #0 0xaaaae85d2228 in operator new[](unsigned long) (/workspaces/rule-of-five/app_asan+0x112228) (BuildId: c4d8df763a9f2d93d8a052853b943516bf9689c7)
    #1 0xaaaae85d5fac in Person::Person(char const*, int, int) /workspaces/rule-of-five/src/person.cpp:16:13
    #2 0xaaaae85d5658 in main /workspaces/rule-of-five/src/main.cpp:36:12
    #3 0xffff949384c0 in __libc_start_call_main csu/../sysdeps/nptl/libc_start_call_main.h:58:16
    #4 0xffff94938594 in __libc_start_main csu/../csu/libc-start.c:360:3
    #5 0xaaaae84f5c2c in _start (/workspaces/rule-of-five/app_asan+0x35c2c) (BuildId: c4d8df763a9f2d93d8a052853b943516bf9689c7)

SUMMARY: AddressSanitizer: double-free (/workspaces/rule-of-five/app_asan+0x112ac4) (BuildId: c4d8df763a9f2d93d8a052853b943516bf9689c7) in operator delete[](void*)
==31722==ABORTING
make: *** [makefile:47: run-asan] Error 1
vscode ➜ /workspaces/rule-of-five (main) $ 
```


---

- Run with Valgrind
  - Valgrind should report:

    - An **invalid free** or **double free** near program exit.

  - The error will trace back to:

    - `Person::~Person()`

  - Valgrind may also indicate that the same memory block was freed multiple times.

This confirms that:
- The shallow copy of `_name` leads to multiple `delete[]` calls on the same heap allocation.
- The compiler-generated copy constructor is unsafe for classes that own dynamic memory.

```bash
vscode ➜ /workspaces/rule-of-five (main) $ valgrind --leak-check=full ./app 
==32029== Memcheck, a memory error detector
==32029== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==32029== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==32029== Command: ./app
==32029== 
a: Bindi age=25 id=1001 name_ptr=0x4e03080
b: Bindi age=25 id=1001 name_ptr=0x4e03080
c: Ahmose age=30 id=1003 name_ptr=0x4e03510
Name: Ahmose, Age: 30, ID: 1003 name_ptr=0x4e03510
Name: Ahmose, Age: 30, ID: 1003 name_ptr=0x4e03510
==32029== Invalid free() / delete / delete[] / realloc()
==32029==    at 0x488A834: operator delete[](void*) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==32029==    by 0x109233: Person::~Person() (person.cpp:22)
==32029==    by 0x1090BB: main (main.cpp:48)
==32029==  Address 0x4e03510 is 0 bytes inside a block of size 7 free'd
==32029==    at 0x488A834: operator delete[](void*) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==32029==    by 0x109233: Person::~Person() (person.cpp:22)
==32029==    by 0x1090AF: main (main.cpp:43)
==32029==  Block was alloc'd at
==32029==    at 0x4886FFC: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==32029==    by 0x1091E7: Person::Person(char const*, int, int) (person.cpp:16)
==32029==    by 0x108FCB: main (main.cpp:36)
==32029== 
==32029== Invalid free() / delete / delete[] / realloc()
==32029==    at 0x488A834: operator delete[](void*) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==32029==    by 0x109233: Person::~Person() (person.cpp:22)
==32029==    by 0x1090CB: main (main.cpp:48)
==32029==  Address 0x4e03080 is 0 bytes inside a block of size 6 free'd
==32029==    at 0x488A834: operator delete[](void*) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==32029==    by 0x109233: Person::~Person() (person.cpp:22)
==32029==    by 0x1090C3: main (main.cpp:48)
==32029==  Block was alloc'd at
==32029==    at 0x4886FFC: operator new[](unsigned long) (in /usr/libexec/valgrind/vgpreload_memcheck-arm64-linux.so)
==32029==    by 0x1091E7: Person::Person(char const*, int, int) (person.cpp:16)
==32029==    by 0x108E1F: main (main.cpp:20)
==32029== 
==32029== 
==32029== HEAP SUMMARY:
==32029==     in use at exit: 0 bytes in 0 blocks
==32029==   total heap usage: 4 allocs, 6 frees, 74,765 bytes allocated
==32029== 
==32029== All heap blocks were freed -- no leaks are possible
==32029== 
==32029== For lists of detected and suppressed errors, rerun with: -s
==32029== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 0 from 0)
vscode ➜ /workspaces/rule-of-five (main) $ 
```
:pushpin: `TAG step-01-03-copy-ctor`

### Move Constructor

#### Goal
#### State of Code
#### Code
#### Expected Results

:pushpin: `TAG step-01-04-move-ctor`

### Move Copy Assignment Operator

#### Goal
#### State of Code
#### Code
#### Expected Results

:pushpin: `TAG step-01-05-move-assign`

### Rule of Five Done

#### Goal
#### State of Code
#### Code
#### Expected Results

:pushpin: `step-01-06-rule-of-5-done`

