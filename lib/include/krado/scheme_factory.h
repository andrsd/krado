#pragma once

#include "krado/parameters.h"
#include "krado/scheme.h"
#include <list>

namespace krado {

class Scheme;
class Mesh;

using SchemePtr = Scheme *;

using BuildPtr = SchemePtr (*)(Mesh & mesh, const Parameters & parameters);

template <typename T>
SchemePtr
build_scheme(Mesh & mesh, const Parameters & parameters)
{
    return new T(mesh, parameters);
}

class SchemeFactory {
public:
    static SchemeFactory & instance();

    template <typename T>
    void
    add(const std::string & class_name)
    {
        Entry entry = { &build_scheme<T> };
        this->classes[class_name] = entry;
    }

    Scheme *
    create(const std::string & scheme_name, Mesh & mesh, Parameters & parameters)
    {
        auto entry = get_entry(scheme_name);
        auto * object = entry.build_ptr(mesh, parameters);
        this->objects.push_back(object);
        return object;
    }

    /// Destroy all object build by this factory
    void
    destroy()
    {
        while (!this->objects.empty()) {
            delete this->objects.front();
            this->objects.pop_front();
        }
    }

private:
    SchemeFactory();
    ~SchemeFactory() { destroy(); }

    /// Describes how to build a Scheme
    struct Entry {
        /// Function pointer that builds the scheme
        BuildPtr build_ptr;
    };

    const Entry &
    get_entry(const std::string & class_name) const
    {
        auto it = this->classes.find(class_name);
        if (it == this->classes.end())
            throw Exception("Unknown scheme '{}'.", class_name);
        return it->second;
    }

    /// All registered classes that we can build
    std::map<std::string, Entry> classes;
    /// All objects built by this factory
    std::list<Scheme *> objects;
};

} // namespace krado
