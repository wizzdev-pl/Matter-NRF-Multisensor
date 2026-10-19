#include "matter_thread_adapter.h"

#include "board.h"
#include "matter_init.h"

#include <access/SubjectDescriptor.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/AttributeValueDecoder.h>
#include <app/ConcreteAttributePath.h>
#include <app/clusters/basic-information/BasicInformationCluster.h>
#include <app/clusters/boolean-state-server/CodegenIntegration.h>
#include <app/data-model-provider/OperationTypes.h>
#include <app/persistence/AttributePersistence.h>
#include <app/persistence/AttributePersistenceProviderInstance.h>
#include <app/server/Server.h>
#include <clusters/BasicInformation/AttributeIds.h>
#include <clusters/BasicInformation/ClusterId.h>
#include <cstddef>
#include <lib/core/TLV.h>

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

namespace adapters
{

CHIP_ERROR MatterThreadAdapter::initializeDefaultNodeLabel()
{
    /* Device name advertised until a controller renames it.
     * Char buffer instead of string as Matter function requires this type. */
    constexpr char DEFAULT_NODE_LABEL[] = "Matter multisensor";

    /* Basic Information NodeLabel maximum length, per the Matter spec. */
    constexpr size_t MAX_NODE_LABEL_SIZE = 32;

    /* Room for the TLV control byte, tag and length prefix around an encoded NodeLabel string. */
    constexpr size_t TLV_STRING_OVERHEAD_SIZE = 8;

    namespace BasicInformation = chip::app::Clusters::BasicInformation;

    auto* pPersistenceProvider = chip::app::GetAttributePersistenceProvider();
    if (pPersistenceProvider == nullptr)
    {
        return CHIP_NO_ERROR;
    }

    const chip::app::ConcreteDataAttributePath path(chip::kRootEndpointId,
                                                    BasicInformation::Id,
                                                    BasicInformation::Attributes::NodeLabel::Id);

    /* Check if there already is a stored label as in a name set through smart home app.
     * If there is we exit early to not overwrite it. */
    chip::app::Storage::String<MAX_NODE_LABEL_SIZE> storedLabel;
    chip::app::AttributePersistence persistence(*pPersistenceProvider);
    if (persistence.LoadString(path, storedLabel))
    {
        return CHIP_NO_ERROR;
    }

    /* Encode the label as a TLV string and feed it through the cluster's own write path, which
     * both updates the value the cluster serves and persists it. */
    uint8_t buffer[MAX_NODE_LABEL_SIZE + TLV_STRING_OVERHEAD_SIZE];
    chip::TLV::TLVWriter writer;
    writer.Init(buffer);
    ReturnErrorOnFailure(writer.PutString(chip::TLV::AnonymousTag(), DEFAULT_NODE_LABEL));
    ReturnErrorOnFailure(writer.Finalize());

    chip::TLV::TLVReader reader;
    reader.Init(buffer, writer.GetLengthWritten());
    ReturnErrorOnFailure(reader.Next());

    chip::Access::SubjectDescriptor subjectDescriptor;
    chip::app::AttributeValueDecoder decoder(reader, subjectDescriptor);

    chip::app::DataModel::WriteAttributeRequest request;
    request.path = path;

    return chip::app::Clusters::BasicInformationCluster::Instance().WriteAttribute(request, decoder).GetUnderlyingError();
}

CHIP_ERROR MatterThreadAdapter::init()
{
    Nrf::Matter::InitData initData;
    initData.mPostServerInitClbk = &initializeDefaultNodeLabel;
    ReturnErrorOnFailure(Nrf::Matter::PrepareServer(initData));

    /* Controls the connectivity status LED based on the captured Matter network state. */
    ReturnErrorOnFailure(Nrf::Matter::RegisterEventHandler(Nrf::Board::DefaultMatterEventHandler, 0));

    for (auto& identifyCluster : m_identifyClusters)
    {
        ReturnErrorOnFailure(identifyCluster.Init());
    }

    return CHIP_NO_ERROR;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
CHIP_ERROR MatterThreadAdapter::startServer()
{
    return Nrf::Matter::StartServer();
}

void MatterThreadAdapter::onContactStateChanged(domain::ESensorId sensorId, domain::EContactState state)
{
    for (const SensorEndpoint& sensorEndpoint : SENSOR_ENDPOINTS)
    {
        if (sensorEndpoint.sensorId == sensorId)
        {
            setEndpointState(sensorEndpoint.endpointId, state == domain::EContactState::Closed);
            return;
        }
    }
}

void MatterThreadAdapter::resetThreadNetwork()
{
    chip::Server::GetInstance().ScheduleFactoryReset();
}

void MatterThreadAdapter::setEndpointState(chip::EndpointId endpointId, bool closed)
{
    auto* pBooleanState = chip::app::Clusters::BooleanState::FindClusterOnEndpoint(endpointId);
    if (pBooleanState == nullptr)
    {
        LOG_ERR("Updating contact sensor state on endpoint %u failed", static_cast<unsigned>(endpointId));
        return;
    }

    /* StateValue=true renders as "Closed" in Apple Home, false as "Open" (verified empirically). */
    pBooleanState->SetStateValue(closed);
}

} /* namespace adapters */
