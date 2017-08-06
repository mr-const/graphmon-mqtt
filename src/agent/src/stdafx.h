#include <set>

#undef ERROR
#include "gflags/gflags.h"
#include "spdlog/spdlog.h"
#include "spdlog/logdef.h"

#include "MQTTAsync.h"
#include "nvml.h"

#include "agent/inc/manager_mqtt.h"
#include "agent/inc/manager_nvml.h"