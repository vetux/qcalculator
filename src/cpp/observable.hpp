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
