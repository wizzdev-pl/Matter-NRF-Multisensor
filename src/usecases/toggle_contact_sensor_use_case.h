#pragma once

#include "contact_sensor_input_port.h"
#include "contact_sensor_observer.h"
#include "contact_sensors_domain.h"

#include <vector>

namespace usecases
{

/* Connection from adapters to inform domain about a contact sensor toggle. Holds every registered
 * ContactSensorObserver (Matter/Thread, ...) and notifies all of them with the state the
 * domain holds after each toggle.
 */
class ToggleContactSensorUseCase : public ports::ContactSensorInputPort
{
public:
    explicit ToggleContactSensorUseCase(domain::ContactSensorsDomain& contactSensorsDomain):
        m_contactSensorsDomain(contactSensorsDomain)
    {
    }

    void addObserver(ports::ContactSensorObserver& observer);

    /* Notifies observers of every sensor's current state unconditionally. Used once at startup to
     * sync observers with the initial state. */
    void notifyObserversOfCurrentState();

    void toggle(domain::ESensorId sensorId) override;

private:
    void notifyObservers(domain::ESensorId sensorId, domain::EContactState state);

    domain::ContactSensorsDomain& m_contactSensorsDomain;
    std::vector<ports::ContactSensorObserver*> m_pObservers;
};

} /* namespace usecases */
