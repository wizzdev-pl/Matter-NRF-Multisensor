#pragma once

#include "contact_sensor_observer.h"
#include "contact_sensors_domain.h"
#include "thread_network_reset_output_port.h"

#include "identify.h"

#include <lib/core/CHIPError.h>
#include <lib/core/DataModelTypes.h>

#include <array>

namespace adapters
{

/* owns the Matter and Thread stack and publishes each contact sensor's state on its own Matter
 * Contact Sensor endpoint's BooleanState cluster. */
class MatterThreadAdapter : public ports::ContactSensorObserver, public ports::ThreadNetworkResetOutputPort
{
public:
    /* Prepares the Matter stack and registers the endpoints' Identify clusters. Must be called
     * before startServer(). */
    CHIP_ERROR init();

    /* Starts the Matter/Thread stack (CHIP thread) and blocks until it is up. */
    CHIP_ERROR startServer();

    void onContactStateChanged(domain::ESensorId sensorId, domain::EContactState state) override;

    /* Resets the Thread network connection. Implemented as a full Matter factory reset
     * (chip::Server::ScheduleFactoryReset()) - the same
     * effect as the `matter device factoryreset` minicom command */
    void resetThreadNetwork() override;

private:
    /* The Matter endpoint publishing one contact sensor. */
    struct SensorEndpoint
    {
        domain::ESensorId sensorId;
        chip::EndpointId endpointId;
    };

    static constexpr std::array<SensorEndpoint, domain::SENSOR_COUNT> SENSOR_ENDPOINTS = {{
        {domain::ESensorId::Sensor1, 1},
        {domain::ESensorId::Sensor2, 2},
        {domain::ESensorId::Sensor3, 3},
    }};

    static void setEndpointState(chip::EndpointId endpointId, bool closed);

    /* Sets the device's Basic Information cluster "NodeLabel" - the name ecosystems show for the
     * whole device. Written through the cluster itself rather than straight to storage:
     * the cluster only loads the stored value in its Startup(),
     * which has already run by the time post-server-init callbacks execute.
     * Passed to Nrf::Matter::InitData::mPostServerInitClbk, so its signature (CHIP_ERROR(*)(void))
     * is fixed by the SDK. */
    static CHIP_ERROR initializeDefaultNodeLabel();

    std::array<Nrf::Matter::IdentifyCluster, domain::SENSOR_COUNT> m_identifyClusters = {
        Nrf::Matter::IdentifyCluster(SENSOR_ENDPOINTS[0].endpointId),
        Nrf::Matter::IdentifyCluster(SENSOR_ENDPOINTS[1].endpointId),
        Nrf::Matter::IdentifyCluster(SENSOR_ENDPOINTS[2].endpointId),
    };
};

} /* namespace adapters */
