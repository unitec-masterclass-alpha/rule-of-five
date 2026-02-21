#pragma once

class Person {
private:
    char* _name;
    int _age;
    int _id;

public:
    Person(const char* name, int age, int id);

    // Rule of 3
    ~Person();
    Person(const Person& other);
    Person& operator=(const Person& other);

    // NEW: Move Constructor (Rule of 5 begins)
    Person(Person&& other) noexcept;

    void SetName(const char* name);

    const char* GetName() const;
    int GetAge() const;
    int GetId() const;
};