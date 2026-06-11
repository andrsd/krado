// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include "Message.hxx"

namespace krado {

/// Environment class
///
/// @note This is a singleton
class Env {
private:
    Env();
    Env(const Env &) = delete;
    Env & operator=(const Env &) = delete;

    const opencascade::handle<Message_Messenger> & msgr_;
    const Message_SequenceOfPrinters & printers_;

public:
    static Env & instance();
};

} // namespace krado
