#include "update_temperature_humidity_use_case.h"

namespace usecases
{

void UpdateTemperatureHumidityUseCase::addObserver(ports::TemperatureHumidityObserver& observer)
{
    m_pObservers.push_back(&observer);
}

void UpdateTemperatureHumidityUseCase::notifyObserversOfCurrentState()
{
    notifyObservers(m_temperatureHumidityDomain.getReading());
}

void UpdateTemperatureHumidityUseCase::reportReading(const domain::TemperatureHumidityReading& reading)
{
    notifyObservers(m_temperatureHumidityDomain.update(reading));
}

void UpdateTemperatureHumidityUseCase::notifyObservers(const domain::TemperatureHumidityReading& reading)
{
    for (auto* pObserver : m_pObservers)
    {
        pObserver->onTemperatureHumidityChanged(reading);
    }
}

} /* namespace usecases */
