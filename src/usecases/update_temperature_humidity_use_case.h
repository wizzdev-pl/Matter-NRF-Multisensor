#pragma once

#include "temperature_humidity_domain.h"
#include "temperature_humidity_input_port.h"
#include "temperature_humidity_observer.h"

#include <vector>

namespace usecases
{

/* Connection from adapters to inform domain about a new temperature/humidity reading. Holds every
 * registered TemperatureHumidityObserver (Matter/Thread, ...) and notifies all of them with the reading the
 * domain holds after validating it.
 */
class UpdateTemperatureHumidityUseCase : public ports::TemperatureHumidityInputPort
{
public:
    explicit UpdateTemperatureHumidityUseCase(domain::TemperatureHumidityDomain& temperatureHumidityDomain):
        m_temperatureHumidityDomain(temperatureHumidityDomain)
    {
    }

    void addObserver(ports::TemperatureHumidityObserver& observer);

    /* Notifies observers of the current reading unconditionally. Used once at startup to sync
     * observers with the initial (unknown) state. */
    void notifyObserversOfCurrentState();

    void reportReading(const domain::TemperatureHumidityReading& reading) override;

private:
    void notifyObservers(const domain::TemperatureHumidityReading& reading);

    domain::TemperatureHumidityDomain& m_temperatureHumidityDomain;
    std::vector<ports::TemperatureHumidityObserver*> m_pObservers;
};

} /* namespace usecases */
