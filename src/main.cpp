#include "button_adapter.h"
#include "contact_sensors_domain.h"
#include "matter_thread_adapter.h"
#include "placeholder_temperature_humidity_sensor_adapter.h"
#include "reset_thread_network_use_case.h"
#include "task_executor.h"
#include "temperature_humidity_domain.h"
#include "toggle_contact_sensor_use_case.h"
#include "update_temperature_humidity_use_case.h"

#include <cstdlib>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, CONFIG_CHIP_APP_LOG_LEVEL);  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main()
{
    domain::ContactSensorsDomain contactSensorsDomain;
    usecases::ToggleContactSensorUseCase toggleContactSensorUseCase{contactSensorsDomain};

    domain::TemperatureHumidityDomain temperatureHumidityDomain;
    usecases::UpdateTemperatureHumidityUseCase updateTemperatureHumidityUseCase{temperatureHumidityDomain};

    adapters::MatterThreadAdapter matterThreadAdapter;
    usecases::ResetThreadNetworkUseCase resetThreadNetworkUseCase{matterThreadAdapter};

    toggleContactSensorUseCase.addObserver(matterThreadAdapter);
    updateTemperatureHumidityUseCase.addObserver(matterThreadAdapter);

    CHIP_ERROR err = matterThreadAdapter.init();
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("Matter initialization failed: %" CHIP_ERROR_FORMAT, err.Format());
        return EXIT_FAILURE;
    }

    adapters::ButtonAdapter buttonAdapter;
    if (!buttonAdapter.init(toggleContactSensorUseCase, resetThreadNetworkUseCase))
    {
        return EXIT_FAILURE;
    }

    /* Synchronize the Matter clusters with the domains' initial state (every contact sensor Open,
     * temperature/humidity reading unknown) once the main loop is running, mirroring the deferral every button
     * signal and sensor reading goes through. Posted before the temperature/humidity sensor starts sampling, so
     * the first reading always lands after it. */
    Nrf::PostTask(
        [&toggleContactSensorUseCase, &updateTemperatureHumidityUseCase]
        {
            toggleContactSensorUseCase.notifyObserversOfCurrentState();
            updateTemperatureHumidityUseCase.notifyObserversOfCurrentState();
        });

    adapters::PlaceholderTemperatureHumiditySensorAdapter temperatureHumiditySensorAdapter;
    temperatureHumiditySensorAdapter.init(updateTemperatureHumidityUseCase);

    err = matterThreadAdapter.startServer();
    if (err != CHIP_NO_ERROR)
    {
        LOG_ERR("Matter server start failed: %" CHIP_ERROR_FORMAT, err.Format());
        return EXIT_FAILURE;
    }

    while (true)
    {
        Nrf::DispatchNextTask();
    }

    return EXIT_SUCCESS;
}
