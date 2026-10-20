#include "temperature_humidity_domain.h"

namespace domain
{

namespace
{

/* Returns value if it lies within [minimum, maximum], otherwise unknown. */
std::optional<int32_t> withinRange(const std::optional<int32_t>& value, int32_t minimum, int32_t maximum)
{
    if ((!value.has_value()) || (*value < minimum) || (*value > maximum))
    {
        return std::nullopt;
    }

    return value;
}

} /* namespace */

TemperatureHumidityReading TemperatureHumidityDomain::update(const TemperatureHumidityReading& reading)
{
    m_reading.temperatureCentiCelsius =
        withinRange(reading.temperatureCentiCelsius, MIN_TEMPERATURE_CENTI_CELSIUS, MAX_TEMPERATURE_CENTI_CELSIUS);
    m_reading.humidityCentiPercent =
        withinRange(reading.humidityCentiPercent, MIN_HUMIDITY_CENTI_PERCENT, MAX_HUMIDITY_CENTI_PERCENT);
    return m_reading;
}

TemperatureHumidityReading TemperatureHumidityDomain::getReading() const
{
    return m_reading;
}

} /* namespace domain */
