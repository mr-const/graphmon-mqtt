#include <set>
#include <agents.h>
#include <cpprest/json.h>

#undef ERROR
#include "gflags/gflags.h"
#include "spdlog/spdlog.h"
#include "spdlog/logdef.h"

#include "MQTTClient.h"
#include "nvml.h"

#include "agent/inc/manager_mqtt.h"
#include "agent/inc/manager_nvml.h"