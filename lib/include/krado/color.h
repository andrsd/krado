// SPDX-FileCopyrightText: 2024 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

namespace krado {

class Color {
public:
    Color();
    Color(int r, int g, int b);

    [[nodiscard]] int red() const;
    [[nodiscard]] double redF() const;
    [[nodiscard]] int green() const;
    [[nodiscard]] double greenF() const;
    [[nodiscard]] int blue() const;
    [[nodiscard]] double blueF() const;

private:
    int r_, g_, b_;
};

} // namespace krado
