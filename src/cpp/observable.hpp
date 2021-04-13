/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_OBSERVABLE_HPP
#define QCALC_OBSERVABLE_HPP

#include <vector>
#include <functional>

/**
 * Generic observable which supports a single std::function as observer.
 *
 * @tparam T The type of the object to observe.
 */
template<typename T>
class Observable {
public:
    typedef std::function<void(T)> Observer;

    void setObserver(Observer obs) {
        this->observer = obs;
    }

    void clearObserver() {
        this->observer = {};
    }

    void set(const T &v) {
        value = v;
        observer(value);
    }

    T get() {
        return value;
    }

private:
    T value;
    Observer observer;
};

#endif //QCALC_OBSERVABLE_HPP
