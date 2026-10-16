#pragma once

namespace ports
{

/* requests a Thread network reset. Currently triggered by holding a button
 * Adapter to Usecase port
 */
class ThreadNetworkResetInputPort
{
public:
    ThreadNetworkResetInputPort() = default;

    virtual ~ThreadNetworkResetInputPort() = default;
    ThreadNetworkResetInputPort(const ThreadNetworkResetInputPort&) = delete;
    ThreadNetworkResetInputPort& operator=(const ThreadNetworkResetInputPort&) = delete;
    ThreadNetworkResetInputPort(ThreadNetworkResetInputPort&&) = delete;
    ThreadNetworkResetInputPort& operator=(ThreadNetworkResetInputPort&&) = delete;

    virtual void requestThreadNetworkReset() = 0;
};

} /* namespace ports */
