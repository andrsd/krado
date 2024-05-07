// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <ostream>
#include <fmt/format.h>

namespace krado {

class UVParam {
public:
    UVParam();
    UVParam(double u, double v);

    UVParam & operator+=(const UVParam & a);
    UVParam & operator/=(double alpha);

    /// Add two UVParams
    inline UVParam
    operator+(const UVParam & other) const
    {
        return UVParam(this->u + other.u, this->v + other.v);
    }

    /// Subtract two UVParams
    inline UVParam
    operator-(const UVParam & other)
    {
        return UVParam(this->u - other.u, this->v - other.v);
    }

    /// Multiply by scalar
    inline UVParam
    operator*(double alpha)
    {
        return UVParam(this->u * alpha, this->v * alpha);
    }

    // Unary minus operator
    UVParam
    operator-() const
    {
        return UVParam(-this->u, -this->v);
    }

    double u, v;
};

inline UVParam
operator*(double alpha, const UVParam & param)
{
    return UVParam(param.u * alpha, param.v * alpha);
}

inline double
determinant(const UVParam & a, const UVParam & b)
{
    return a.u * b.v - a.v * b.u;
}

} // namespace krado

inline std::ostream &
operator<<(std::ostream & stream, const krado::UVParam & param)
{
    stream << "(" << param.u << ", " << param.v << ")";
    return stream;
}

/// Format `UVParam` using fmt library
template <>
struct fmt::formatter<krado::UVParam> {
    constexpr auto
    parse(format_parse_context & ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    // Define how the object is formatted
    template <typename FormatContext>
    auto
    format(const krado::UVParam & obj, FormatContext & ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "({}, {})", obj.u, obj.v);
    }
};
