#pragma once

class Person {
private:
    char* _name;
    int _age;
    int _id;

public:
    Person(const char* name, int age, int id);
    ~Person();

    Person(const Person& other);
    Person& operator=(const Person& other);

    Person(Person&& other) noexcept;
    Person& operator=(Person&& other) noexcept;   // NEW

    void SetName(const char* name);

    const char* GetName() const;
    int GetAge() const;
    int GetId() const;
};
