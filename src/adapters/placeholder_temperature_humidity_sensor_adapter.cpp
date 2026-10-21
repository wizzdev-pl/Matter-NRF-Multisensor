#include "placeholder_temperature_humidity_sensor_adapter.h"

#include "task_executor.h"

namespace
{

/* The placeholder readings start at 21.00 °C / 45.00 %RH and go up by 0.30 °C / 0.50 %RH per sample,
 * wrapping back to the start after STEP_COUNT samples (26.70 °C / 54.50 %RH at the top). */
constexpr int32_t BASE_TEMPERATURE_CENTI_CELSIUS = 2100;
constexpr int32_t TEMPERATURE_STEP_CENTI_CELSIUS = 30;
constexpr int32_t BASE_HUMIDITY_CENTI_PERCENT = 4500;
constexpr int32_t HUMIDITY_STEP_CENTI_PERCENT = 50;
constexpr uint32_t STEP_COUNT = 20;

} /* namespace */

namespace adapters
{

PlaceholderTemperatureHumiditySensorAdapter::PlaceholderTemperatureHumiditySensorAdapter()
{
    k_timer_init(&m_sampleTimer, &sampleTimerExpired, nullptr);
    k_timer_user_data_set(&m_sampleTimer, this);
}

void PlaceholderTemperatureHumiditySensorAdapter::init(ports::TemperatureHumidityInputPort& temperatureHumidityUseCase)
{
    m_pTemperatureHumidityUseCase = &temperatureHumidityUseCase;

    k_timer_start(&m_sampleTimer, K_NO_WAIT, K_SECONDS(CONFIG_APP_TEMPERATURE_HUMIDITY_SAMPLE_PERIOD_S));
}

void PlaceholderTemperatureHumiditySensorAdapter::sampleTimerExpired(k_timer* pTimer)
{
    auto* pThis = static_cast<PlaceholderTemperatureHumiditySensorAdapter*>(k_timer_user_data_get(pTimer));

    /* Timer callbacks run in interrupt context; the domain and the Matter clusters are only touched from
     * the app thread. */
    Nrf::PostTask([pThis]
                  { pThis->sampleSensor(); });
}

void PlaceholderTemperatureHumiditySensorAdapter::sampleSensor()
{
    const auto step = static_cast<int32_t>(m_sampleCount % STEP_COUNT);
    ++m_sampleCount;

    domain::TemperatureHumidityReading reading;
    reading.temperatureCentiCelsius = BASE_TEMPERATURE_CENTI_CELSIUS + (step * TEMPERATURE_STEP_CENTI_CELSIUS);
    reading.humidityCentiPercent = BASE_HUMIDITY_CENTI_PERCENT + (step * HUMIDITY_STEP_CENTI_PERCENT);

    m_pTemperatureHumidityUseCase->reportReading(reading);
}

} /* namespace adapters */
