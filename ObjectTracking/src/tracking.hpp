#pragma once
#include "data_loader.hpp"
#include "utils.hpp"

// Handles data corresponding to a tracked object, i.e. a vector of Objects
struct TrackedObject {
    std::vector<ObjectData> m_objects;
    ObjectData m_last_object;

    TrackedObject(const ObjectData& object);
    ~TrackedObject();

    double getMatchingCoefficient(const ObjectData& newObject);
};
