#ifndef QCALC_UIDGENERATOR_HPP
#define QCALC_UIDGENERATOR_HPP

#include <vector>

#include "uid.hpp"

/**
 * Generates unique identifiers which are guaranteed to be unique for each generator instance.
 * Users may optionally return identifiers to make them available in future generation requests.
 */
class UIDGenerator {
public:
    UID get() {
        if (availableIdentifiers.empty()) {
            if (counter == std::numeric_limits<UID>::max() - 1)
                throw std::runtime_error("Maximum amount of identifiers reached");
            return ++counter;
        } else {
            UID ret = *availableIdentifiers.begin();
            availableIdentifiers.erase(availableIdentifiers.begin());
            return ret;
        }
    }

    void put(UID id) {
        if (id == UID_NULL) {
            throw std::runtime_error("UID_NULL passed");
        }

        if (id >= counter) {
            throw std::runtime_error("Invalid id passed");
        }

        if (std::find(availableIdentifiers.begin(), availableIdentifiers.end(), id) != availableIdentifiers.end()) {
            throw std::runtime_error("UID already stored");
        }

        availableIdentifiers.emplace_back(id);
    }

private:
    UID counter = UID_NULL;
    std::vector<UID> availableIdentifiers;
};

#endif //QCALC_UIDGENERATOR_HPP
