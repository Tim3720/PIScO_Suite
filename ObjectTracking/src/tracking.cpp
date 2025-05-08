#include "tracking.hpp"

TrackedObject::TrackedObject(const ObjectData& object) : m_last_object(object)
{
    // preallocate memory for a few elements. As an object is not large (40bytes), this
    // should not lead to large memory consumption but increase the speed
    m_objects.reserve(10);
    m_objects.push_back(object);
}

TrackedObject::~TrackedObject()
{
    m_objects.clear();
}

double calculatePositionMatching(uint16_t xNew, uint16_t yNew, uint16_t xOld,
    uint16_t yOld)
{
    double distance = (xNew - xOld) * (xNew - xOld) + (yNew - yOld) * (yNew - yOld);
    if (distance >= e_maxMatchingDistance)
        return 0;
    return 1 - distance / e_maxMatchingDistance;
}

double calculateTimeMatching(int imageIdNew, int imageIdOld)
{
    int timeDiff = imageIdNew - imageIdOld;
    if (timeDiff > e_maxMatchingTimeDifference)
        return 0;
    // minimal value is 1 -> subtract 1 to get possible values in the range 0 to 1
    return 1 - double(timeDiff - 1) / e_maxMatchingTimeDifference;
}

double TrackedObject::getMatchingCoefficient(const ObjectData& newObject)
{
    double matchingCoefficient = 0;
    return matchingCoefficient;
}
