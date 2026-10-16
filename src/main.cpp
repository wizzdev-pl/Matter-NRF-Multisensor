#include "button_adapter.h"
#include "contact_sensors_domain.h"
#include "matter_thread_adapter.h"
#include "reset_thread_network_use_case.h"
#include "task_executor.h"
#include "toggle_contact_sensor_use_case.h"

#include <cstdlib>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, CONFIG_CHIP_APP_LOG_LEVEL);  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int main()
{
    domain::ContactSensorsDomain contactSensorsDomain;
    usecases::ToggleContactSensorUseCase toggleContactSensorUseCase{contactSensorsDomain};

    adapters::MatterThreadAdapter matterThreadAdapter;
    usecases::ResetThreadNetworkUseCase resetThreadNetworkUseCase{matterThreadAdapter};

    toggleContactSensorUseCase.addObserver(matterThreadAdapter);

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

    /* Synchronize the Matter clusters with the domain's initial state (every sensor Open)
     * once the main loop is running, mirroring the deferral every button signal goes through. */
    Nrf::PostTask([&toggleContactSensorUseCase]
                  { toggleContactSensorUseCase.notifyObserversOfCurrentState(); });

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
