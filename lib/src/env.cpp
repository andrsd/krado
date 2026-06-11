// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "krado/env.h"

namespace krado {

namespace {

// runtime environment
auto & env = Env::instance();

} // namespace

Env::Env() : msgr_(Message::DefaultMessenger()), printers_(msgr_->Printers())
{
    // remove "printers" from open cascade, so it does not output stuff we don't want to see
    for (int idx = 0; idx < this->printers_.Size(); ++idx)
        this->msgr_->RemovePrinter(this->printers_.Value(idx + 1));
}

Env &
Env::instance()
{
    static Env env;
    return env;
}

} // namespace krado
