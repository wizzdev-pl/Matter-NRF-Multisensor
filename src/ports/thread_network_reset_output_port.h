#pragma once

namespace ports
{

/* requests a Thread network reset. Currently triggered by holding a button
 * Usecase to Adapter port
 */
class ThreadNetworkResetOutputPort
{
public:
    ThreadNetworkResetOutputPort() = default;

    virtual ~ThreadNetworkResetOutputPort() = default;
    ThreadNetworkResetOutputPort(const ThreadNetworkResetOutputPort&) = delete;
    ThreadNetworkResetOutputPort& operator=(const ThreadNetworkResetOutputPort&) = delete;
    ThreadNetworkResetOutputPort(ThreadNetworkResetOutputPort&&) = delete;
    ThreadNetworkResetOutputPort& operator=(ThreadNetworkResetOutputPort&&) = delete;

    virtual void resetThreadNetwork() = 0;
};

} /* namespace ports */
