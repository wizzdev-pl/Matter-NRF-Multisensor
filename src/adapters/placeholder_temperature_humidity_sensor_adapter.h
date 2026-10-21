#pragma once

#include "temperature_humidity_input_port.h"

#include <zephyr/kernel.h>

#include <cstdint>

namespace adapters
{

/* Stand-in for a real temperature/humidity sensor until the part is chosen: every
 * CONFIG_APP_TEMPERATURE_HUMIDITY_SAMPLE_PERIOD_S seconds it "reads" a made-up value and reports it through
 * TemperatureHumidityInputPort, exactly as a real sensor adapter would. The values step slowly upwards
 * and wrap around, so changes are visible on endpoints 4/5. Replace this class (not the use case) when
 * the real sensor arrives. */
class PlaceholderTemperatureHumiditySensorAdapter
{
public:
    PlaceholderTemperatureHumiditySensorAdapter();
    ~PlaceholderTemperatureHumiditySensorAdapter() = default;

    PlaceholderTemperatureHumiditySensorAdapter(const PlaceholderTemperatureHumiditySensorAdapter&) = delete;
    PlaceholderTemperatureHumiditySensorAdapter& operator=(const PlaceholderTemperatureHumiditySensorAdapter&) = delete;
    PlaceholderTemperatureHumiditySensorAdapter(PlaceholderTemperatureHumiditySensorAdapter&&) = delete;
    PlaceholderTemperatureHumiditySensorAdapter& operator=(PlaceholderTemperatureHumiditySensorAdapter&&) = delete;

    /* Registers temperatureHumidityUseCase as the destination for readings and starts periodic
     * sampling, the first one immediately. Must be called exactly once. */
    void init(ports::TemperatureHumidityInputPort& temperatureHumidityUseCase);

private:
    static void sampleTimerExpired(k_timer* pTimer);

    /* "Reads" the next placeholder value and reports it. Runs on the app thread (posted from the timer). */
    void sampleSensor();

    ports::TemperatureHumidityInputPort* m_pTemperatureHumidityUseCase = nullptr;
    uint32_t m_sampleCount = 0;
    k_timer m_sampleTimer{};
};

} /* namespace adapters */
