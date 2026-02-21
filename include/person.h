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