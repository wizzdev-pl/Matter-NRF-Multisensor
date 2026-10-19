#include "reset_thread_network_use_case.h"

namespace usecases
{

void ResetThreadNetworkUseCase::requestThreadNetworkReset()
{
    m_threadNetworkController.resetThreadNetwork();
}

} /* namespace usecases */
