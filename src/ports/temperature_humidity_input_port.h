#pragma once

#include "temperature_humidity_domain.h"

namespace ports
{

/* Input of a new temperature/humidity reading to the use case.
 * Currently from a Zephyr sensor device, can be switched for any other source.
 */
class TemperatureHumidityInputPort
{
public:
    TemperatureHumidityInputPort() = default;

    virtual ~TemperatureHumidityInputPort() = default;
    TemperatureHumidityInputPort(const TemperatureHumidityInputPort&) = delete;
    TemperatureHumidityInputPort& operator=(const TemperatureHumidityInputPort&) = delete;
    TemperatureHumidityInputPort(TemperatureHumidityInputPort&&) = delete;
    TemperatureHumidityInputPort& operator=(TemperatureHumidityInputPort&&) = delete;

    /* reading is raw and not yet validated; an empty value means it could not be read. */
    virtual void reportReading(const domain::TemperatureHumidityReading& reading) = 0;
};

} /* namespace ports */
