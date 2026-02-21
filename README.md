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
touch makefile include/buffer.h src/main.cpp src/buffer.cpp 
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
#### State of Code
#### Code
#### Expected Results

:pushpin: `TAG step-01-02-copy-ctor`

### Copy Assignment Operator

#### Goal
#### State of Code
#### Code
#### Expected Results

:pushpin: `TAG step-01-03-copy-assign`

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

