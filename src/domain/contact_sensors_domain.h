#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace domain
{

/* Identifies one of the device's independent contact sensors. Count is not a sensor - it is the
 * number of sensors, so adding a sensor means adding one value before it. */
enum class ESensorId : uint8_t
{
    Sensor1,
    Sensor2,
    Sensor3,
    Count
};

constexpr size_t SENSOR_COUNT = static_cast<size_t>(ESensorId::Count);

enum class EContactState : uint8_t
{
    Open,
    Closed
};

/* holds the current state of every contact sensor. All sensors start Open. */
class ContactSensorsDomain
{
public:
    ContactSensorsDomain() = default;

    /* Flips the sensor between Open and Closed and returns its new state. An id outside
     * [Sensor1, Count) changes nothing and returns Open. */
    EContactState toggle(ESensorId sensorId);

    /* Returns the sensor's current state. An id outside [Sensor1, Count) returns Open. */
    [[nodiscard]] EContactState getState(ESensorId sensorId) const;

private:
    std::array<EContactState, SENSOR_COUNT> m_states = {EContactState::Open, EContactState::Open, EContactState::Open};
};

} /* namespace domain */
