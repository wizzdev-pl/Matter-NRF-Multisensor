#include "button_adapter.h"

#include "task_executor.h"

#include <array>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

namespace
{

struct SensorButton
{
    domain::ESensorId sensorId;
    Nrf::ButtonMask buttonMask;
};

/* DK_BTN2..4 are the buttons printed BUTTON1..3 on the nRF54L15 DK. */
constexpr std::array<SensorButton, domain::SENSOR_COUNT> SENSOR_BUTTONS = {{
    {domain::ESensorId::Sensor1, DK_BTN2_MSK},
    {domain::ESensorId::Sensor2, DK_BTN3_MSK},
    {domain::ESensorId::Sensor3, DK_BTN4_MSK},
}};
constexpr Nrf::ButtonMask THREAD_RESET_BUTTON_MASK = DK_BTN1_MSK;
constexpr uint32_t THREAD_RESET_HOLD_DURATION_MS = 3000;

/* Board::Init() only accepts a plain C function pointer for the button callback - Zephyr's
 * button_handler_t carries no user-data/context argument - so there is no way to reach the
 * ButtonAdapter constructed in main() from it directly. Set once, in init(), purely to bridge that
 * one call; everything else about ButtonAdapter is a plain object with no public global-access
 * point. */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
adapters::ButtonAdapter* s_pButtonAdapter = nullptr;

} /* namespace */

namespace adapters
{

ButtonAdapter::ButtonAdapter()
{
    k_timer_init(&m_resetHoldTimer, &holdTimerExpired, nullptr);
    k_timer_user_data_set(&m_resetHoldTimer, this);
}

bool ButtonAdapter::init(ports::ContactSensorInputPort& contactSensorUseCase,
                         ports::ThreadNetworkResetInputPort& threadNetworkResetUseCase)
{
    m_pContactSensorUseCase = &contactSensorUseCase;
    m_pThreadNetworkResetUseCase = &threadNetworkResetUseCase;
    s_pButtonAdapter = this;

    if (!Nrf::GetBoard().Init(handleButtonEvent))
    {
        LOG_ERR("User interface initialization failed.");
        return false;
    }

    return true;
}

void ButtonAdapter::handleButtonEvent(Nrf::ButtonState state, Nrf::ButtonMask hasChanged)
{
    ButtonAdapter* pThis = s_pButtonAdapter;

    // called on button press, first check if there was a change
    // in the signal from last time, and if currently the button is pressed

    for (const SensorButton& sensorButton : SENSOR_BUTTONS)
    {
        if (((sensorButton.buttonMask & hasChanged) != 0U) && ((sensorButton.buttonMask & state) != 0U))
        {
            const domain::ESensorId sensorId = sensorButton.sensorId;
            LOG_INF("Contact sensor %u toggle (button pressed)", static_cast<unsigned>(sensorId) + 1U);
            auto* pContactSensorUseCase = pThis->m_pContactSensorUseCase;
            Nrf::PostTask([pContactSensorUseCase, sensorId]
                          { pContactSensorUseCase->toggle(sensorId); });
        }
    }
    if ((THREAD_RESET_BUTTON_MASK & hasChanged) != 0U)
    {
        if ((THREAD_RESET_BUTTON_MASK & state) != 0U)
        {
            k_timer_start(&pThis->m_resetHoldTimer, K_MSEC(THREAD_RESET_HOLD_DURATION_MS), K_NO_WAIT);
        }
        else
        {
            k_timer_stop(&pThis->m_resetHoldTimer);
        }
    }
}

void ButtonAdapter::holdTimerExpired(k_timer* pTimer)
{
    auto* pThis = static_cast<ButtonAdapter*>(k_timer_user_data_get(pTimer));
    auto* pThreadNetworkResetUseCase = pThis->m_pThreadNetworkResetUseCase;

    LOG_INF("Thread network reset requested (button held)");
    Nrf::PostTask([pThreadNetworkResetUseCase]
                  { pThreadNetworkResetUseCase->requestThreadNetworkReset(); });
}

} /* namespace adapters */
