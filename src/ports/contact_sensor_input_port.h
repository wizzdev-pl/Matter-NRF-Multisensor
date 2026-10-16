#pragma once

#include "contact_sensors_domain.h"

namespace ports
{

/* Input about a contact sensor change to the use case.
 * Currently by buttons, can be switched for real sensors.
 */
class ContactSensorInputPort
{
public:
    ContactSensorInputPort() = default;

    virtual ~ContactSensorInputPort() = default;
    ContactSensorInputPort(const ContactSensorInputPort&) = delete;
    ContactSensorInputPort& operator=(const ContactSensorInputPort&) = delete;
    ContactSensorInputPort(ContactSensorInputPort&&) = delete;
    ContactSensorInputPort& operator=(ContactSensorInputPort&&) = delete;

    virtual void toggle(domain::ESensorId sensorId) = 0;
};

} /* namespace ports */
