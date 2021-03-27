#ifndef QCALC_UUIDGENERATOR_HPP
#define QCALC_UUIDGENERATOR_HPP

#include <vector>

#include "uuid.hpp"

class UUIDGenerator {
public:
    UUID get() {
        if (availableUUIDS.empty()) {
            return ++counter;
        } else {
            UUID ret = *availableUUIDS.begin();
            availableUUIDS.erase(availableUUIDS.begin());
            return ret;
        }
    }

    void put(UUID id) {
        if (id == UUID_NULL) {
            throw std::runtime_error("UUID_NULL passed");
        }

        if (std::find(availableUUIDS.begin(), availableUUIDS.end(), id) != availableUUIDS.end()) {
            throw std::runtime_error("UUID already stored");
        }

        availableUUIDS.emplace_back(id);
    }

private:
    UUID counter = UUID_NULL;
    std::vector<UUID> availableUUIDS;
};

#endif //QCALC_UUIDGENERATOR_HPP
