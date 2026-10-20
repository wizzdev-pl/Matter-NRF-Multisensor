cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})

project(matter_multisensor)

# Enable GNU STD support.
include(${ZEPHYR_CONNECTEDHOMEIP_MODULE_DIR}/config/nrfconnect/app/enable-gnu-std.cmake)

# NORDIC SDK APP START

# Include all source files that are located in the Matter common directory.
include(${ZEPHYR_NRF_MODULE_DIR}/samples/matter/common/cmake/source_common.cmake)

# Include Data Model utils
include(${ZEPHYR_NRF_MODULE_DIR}/samples/matter/common/cmake/data_model.cmake)

include(${ZEPHYR_NRF_MODULE_DIR}/samples/matter/common/cmake/zap_helpers.cmake)
ncs_get_zap_parent_dir(ZAP_PARENT_DIR)

target_include_directories(app PRIVATE
  src
  src/domain
  src/ports
  src/usecases
  src/adapters
  ${ZEPHYR_NRF_MODULE_DIR}/samples/matter/common/src/board
  ${ZEPHYR_NRF_MODULE_DIR}/samples/matter/common/src/app
  ${ZEPHYR_NRF_MODULE_DIR}/samples/matter/common/src/clusters
  ${ZAP_PARENT_DIR}
)

target_sources(app PRIVATE
  src/domain/contact_sensors_domain.h
  src/domain/contact_sensors_domain.cpp
  src/domain/temperature_humidity_domain.h
  src/domain/temperature_humidity_domain.cpp
  src/ports/contact_sensor_input_port.h
  src/ports/contact_sensor_observer.h
  src/ports/temperature_humidity_input_port.h
  src/ports/temperature_humidity_observer.h
  src/ports/thread_network_reset_input_port.h
  src/ports/thread_network_reset_output_port.h
  src/usecases/toggle_contact_sensor_use_case.h
  src/usecases/toggle_contact_sensor_use_case.cpp
  src/usecases/update_temperature_humidity_use_case.h
  src/usecases/update_temperature_humidity_use_case.cpp
  src/usecases/reset_thread_network_use_case.h
  src/usecases/reset_thread_network_use_case.cpp
  src/adapters/button_adapter.h
  src/adapters/button_adapter.cpp
  src/adapters/placeholder_temperature_humidity_sensor_adapter.h
  src/adapters/placeholder_temperature_humidity_sensor_adapter.cpp
  src/adapters/matter_thread_adapter.h
  src/adapters/matter_thread_adapter.cpp
  src/main.cpp
)

ncs_configure_data_model()
# NORDIC SDK APP END
