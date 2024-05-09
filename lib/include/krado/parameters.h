// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include "krado/exception.h"

namespace krado {

/// Class for user-defined parameters
///
class Parameters {
public:
    Parameters() = default;
    Parameters(const Parameters & p) { *this = p; }

    virtual ~Parameters() { clear(); }

protected:
    /// Base class for parameter values
    class Value {
    public:
        virtual ~Value() = default;

        /// Return the type of this value as a string
        virtual std::string type() const = 0;

        /// Create a copy of this value
        virtual Value * copy() const = 0;

        /// Is parameter valid
        bool valid;
    };

    /// Parameter value
    template <typename T>
    class Parameter : public Value {
    public:
        /// @returns A read-only reference to the parameter value.
        const T &
        get() const
        {
            return this->value;
        }

        /// @returns A writable reference to the parameter value.
        T &
        set()
        {
            return this->value;
        }

        inline std::string
        type() const override
        {
            return std::string(typeid(T).name());
        }

        Value *
        copy() const override
        {
            auto * copy = new Parameter<T>;
            copy->value = this->value;
            copy->valid = this->valid;
            return copy;
        }

        /// Parameter value
        T value;
    };

public:
    /// Check if parameter exist
    template <typename T>
    bool
    has(const std::string & name) const
    {
        auto it = this->params.find(name);

        if (it != this->params.end())
            if (dynamic_cast<const Parameter<T> *>(it->second) != nullptr)
                return true;

        return false;
    }

    /// Get parameter value
    template <typename T>
    inline const T &
    get(const std::string & name) const
    {
        if (!this->has<T>(name))
            throw Exception("No parameter '{}' found.", name);

        auto it = this->params.find(name);
        return dynamic_cast<Parameter<T> *>(it->second)->get();
    }

    /// Set parameter
    template <typename T>
    inline T &
    set(const std::string & name)
    {
        if (!this->has<T>(name))
            this->params[name] = new Parameter<T>;

        this->params[name]->valid = true;
        return dynamic_cast<Parameter<T> *>(this->params[name])->set();
    }

    template <typename T, typename S>
    void add_param(const std::string & name, const S & value);

    template <typename T>
    void add_param(const std::string & name);

    bool
    is_param_valid(const std::string & name) const
    {
        return this->params.count(name) > 0 && this->params.at(name)->valid;
    }

    std::string
    type(const std::string & name) const
    {
        return this->params.at(name)->type();
    }

    typedef std::map<std::string, Parameters::Value *>::iterator iterator;
    typedef std::map<std::string, Parameters::Value *>::const_iterator const_iterator;

    Parameters::iterator
    begin()
    {
        return this->params.begin();
    }

    Parameters::const_iterator
    begin() const
    {
        return this->params.begin();
    }

    Parameters::iterator
    end()
    {
        return this->params.end();
    }

    Parameters::const_iterator
    end() const
    {
        return this->params.end();
    }

    Parameters &
    operator=(const Parameters & rhs)
    {
        this->clear();
        for (const auto & par : rhs)
            this->params[par.first] = par.second->copy();
        return *this;
    }

    /// Add `rhs` Parameters into this Parameters object
    Parameters &
    operator+=(const Parameters & rhs)
    {
        for (const auto & rpar : rhs) {
            auto jt = this->params.find(rpar.first);
            if (jt != this->params.end())
                delete jt->second;
            this->params[rpar.first] = rpar.second->copy();
        }
        return *this;
    }

    void
    clear()
    {
        for (auto & it : this->params)
            delete it.second;
        this->params.clear();
    }

private:
    /// The actual parameter data. Each Metadata object contains attributes for the corresponding
    /// parameter.
    std::map<std::string, Value *> params;
};

template <typename T>
void
Parameters::add_param(const std::string & name)
{
    if (!this->has<T>(name)) {
        auto * param = new Parameter<T>;
        param->valid = false;
        this->params[name] = param;
    }
}

template <typename T, typename S>
void
Parameters::add_param(const std::string & name, const S & value)
{
    if (!this->has<T>(name)) {
        auto * param = new Parameter<T>;
        param->value = value;
        param->valid = true;
        this->params[name] = param;
    }
}

} // namespace krado
