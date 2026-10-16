#pragma once

#include "thread_network_reset_input_port.h"
#include "thread_network_reset_output_port.h"

namespace usecases
{

class ResetThreadNetworkUseCase : public ports::ThreadNetworkResetInputPort
{
public:
    explicit ResetThreadNetworkUseCase(ports::ThreadNetworkResetOutputPort& threadNetworkController):
        m_threadNetworkController(threadNetworkController)
    {
    }

    void requestThreadNetworkReset() override;

private:
    ports::ThreadNetworkResetOutputPort& m_threadNetworkController;
};

} /* namespace usecases */
