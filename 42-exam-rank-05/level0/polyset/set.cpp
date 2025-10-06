#include "set.hpp"

set::set() : bag(nullptr)
{
}

set::set(searchable_bag& s_bag) : bag(&s_bag)
{
}

set::set(const set& source) : bag(source.bag)
{
}

set& set::operator=(const set& source)
{
    if (this != &source)
    {
        bag = source.bag;
    }
    return *this;
}

bool set::has(int value) const
{
    if (bag)
        return bag->has(value);
    return false;
}

void set::insert(int value)
{
    if (bag && !(this->has(value)))
        bag->insert(value);
}

void set::insert(int* data, int size)
{
    if (bag)
    {
        for (int i = 0; i < size; i++)
        {
            this->insert(data[i]);
        }
    }
}

void set::print() const
{
    if (bag)
        bag->print();
}

void set::clear()
{
    if (bag)
        bag->clear();
}

const searchable_bag& set::get_bag() const
{
    return *bag;
}

set::~set()
{
}
