#pragma once

#include "contact_sensors_domain.h"

namespace ports
{

/* For domain state observers: implemented by anything that wants to react to
 * contact sensor state changes, notified by the use case.
 */
class ContactSensorObserver
{
public:
    ContactSensorObserver() = default;

    virtual ~ContactSensorObserver() = default;
    ContactSensorObserver(const ContactSensorObserver&) = delete;
    ContactSensorObserver& operator=(const ContactSensorObserver&) = delete;
    ContactSensorObserver(ContactSensorObserver&&) = delete;
    ContactSensorObserver& operator=(ContactSensorObserver&&) = delete;

    /* state is the sensor's state as held by the domain after the change. */
    virtual void onContactStateChanged(domain::ESensorId sensorId, domain::EContactState state) = 0;
};

} /* namespace ports */
