#pragma once

#include "temperature_humidity_domain.h"

namespace ports
{

/* For domain state observers: implemented by anything that wants to react to
 * temperature/humidity readings, notified by the use case.
 */
class TemperatureHumidityObserver
{
public:
    TemperatureHumidityObserver() = default;

    virtual ~TemperatureHumidityObserver() = default;
    TemperatureHumidityObserver(const TemperatureHumidityObserver&) = delete;
    TemperatureHumidityObserver& operator=(const TemperatureHumidityObserver&) = delete;
    TemperatureHumidityObserver(TemperatureHumidityObserver&&) = delete;
    TemperatureHumidityObserver& operator=(TemperatureHumidityObserver&&) = delete;

    /* reading is the reading as validated and held by the domain: every value present is within
     * the domain's MIN_/MAX_ limits. */
    virtual void onTemperatureHumidityChanged(const domain::TemperatureHumidityReading& reading) = 0;
};

} /* namespace ports */
