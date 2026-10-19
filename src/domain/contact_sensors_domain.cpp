#include "contact_sensors_domain.h"

namespace domain
{

EContactState ContactSensorsDomain::toggle(ESensorId sensorId)
{
    const auto index = static_cast<size_t>(sensorId);
    if (index >= SENSOR_COUNT)
    {
        return EContactState::Open;
    }

    EContactState& state = m_states.at(index);
    state = (state == EContactState::Open) ? EContactState::Closed : EContactState::Open;
    return state;
}

EContactState ContactSensorsDomain::getState(ESensorId sensorId) const
{
    const auto index = static_cast<size_t>(sensorId);
    if (index >= SENSOR_COUNT)
    {
        return EContactState::Open;
    }

    return m_states.at(index);
}

} /* namespace domain */
