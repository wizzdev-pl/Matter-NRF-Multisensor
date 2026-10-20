#pragma once

#include <cstdint>
#include <optional>

namespace domain
{

/* Valid measurement ranges. A value outside them is treated as unknown. Kept equal to the
 * Min/MaxMeasuredValue attributes of endpoints 4/5 in matter_endpoint_zap/multisensor.zap. */
constexpr int32_t MIN_TEMPERATURE_CENTI_CELSIUS = -4000;
constexpr int32_t MAX_TEMPERATURE_CENTI_CELSIUS = 12500;
constexpr int32_t MIN_HUMIDITY_CENTI_PERCENT = 0;
constexpr int32_t MAX_HUMIDITY_CENTI_PERCENT = 10000;

/* One temperature/humidity reading in fixed point: hundredths of a degree Celsius and hundredths
 * of a percent relative humidity. An empty value means unknown (no sensor, failed read or out of
 * range). The fields are wider than the Matter attribute types on purpose, so an out-of-range raw
 * value reaches TemperatureHumidityDomain's range check intact instead of wrapping around on the way. */
struct TemperatureHumidityReading
{
    std::optional<int32_t> temperatureCentiCelsius;
    std::optional<int32_t> humidityCentiPercent;
};

/* holds the latest validated temperature/humidity reading. Starts with both values unknown. */
class TemperatureHumidityDomain
{
public:
    TemperatureHumidityDomain() = default;

    /* Stores and returns a validated copy of reading: a value outside its valid range becomes
     * unknown. Every value in the returned reading is within the MIN_/MAX_ limits above. */
    TemperatureHumidityReading update(const TemperatureHumidityReading& reading);

    /* Returns the latest validated reading. */
    [[nodiscard]] TemperatureHumidityReading getReading() const;

private:
    TemperatureHumidityReading m_reading;
};

} /* namespace domain */
