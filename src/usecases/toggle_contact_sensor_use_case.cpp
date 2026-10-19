#include "toggle_contact_sensor_use_case.h"

#include <cstddef>

namespace usecases
{

void ToggleContactSensorUseCase::addObserver(ports::ContactSensorObserver& observer)
{
    m_pObservers.push_back(&observer);
}

void ToggleContactSensorUseCase::notifyObserversOfCurrentState()
{
    for (size_t index = 0; index < domain::SENSOR_COUNT; ++index)
    {
        const auto sensorId = static_cast<domain::ESensorId>(index);
        notifyObservers(sensorId, m_contactSensorsDomain.getState(sensorId));
    }
}

void ToggleContactSensorUseCase::toggle(domain::ESensorId sensorId)
{
    if (static_cast<size_t>(sensorId) >= domain::SENSOR_COUNT)
    {
        return;
    }

    notifyObservers(sensorId, m_contactSensorsDomain.toggle(sensorId));
}

void ToggleContactSensorUseCase::notifyObservers(domain::ESensorId sensorId, domain::EContactState state)
{
    for (auto* pObserver : m_pObservers)
    {
        pObserver->onContactStateChanged(sensorId, state);
    }
}

} /* namespace usecases */
