#pragma once

#include "board.h"
#include "contact_sensor_input_port.h"
#include "thread_network_reset_input_port.h"

#include <zephyr/kernel.h>

namespace adapters
{

/* Buttons 2/3/4 each toggle contact sensor 1/2/3; holding button 1 for
 * THREAD_RESET_HOLD_DURATION_MS (button_adapter.cpp) requests a Thread network reset. */
class ButtonAdapter
{
public:
    ButtonAdapter();
    ~ButtonAdapter() = default;

    ButtonAdapter(const ButtonAdapter&) = delete;
    ButtonAdapter& operator=(const ButtonAdapter&) = delete;
    ButtonAdapter(ButtonAdapter&&) = delete;
    ButtonAdapter& operator=(ButtonAdapter&&) = delete;

    /* Registers contactSensorUseCase/threadNetworkResetUseCase as the destinations for button
     * signals and registers this adapter with Board::Init(). Must be called exactly once, before
     * any button press is expected to be handled. Returns false if board/button initialization
     * itself failed. */
    bool init(ports::ContactSensorInputPort& contactSensorUseCase,
              ports::ThreadNetworkResetInputPort& threadNetworkResetUseCase);

private:
    static void handleButtonEvent(Nrf::ButtonState state, Nrf::ButtonMask hasChanged);
    static void holdTimerExpired(k_timer* pTimer);

    ports::ContactSensorInputPort* m_pContactSensorUseCase = nullptr;
    ports::ThreadNetworkResetInputPort* m_pThreadNetworkResetUseCase = nullptr;
    k_timer m_resetHoldTimer{};
};

} /* namespace adapters */
