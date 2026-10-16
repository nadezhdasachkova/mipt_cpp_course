#include "os_source.h"
#include "os.h"
#include "os_handle.h"
#include <stdexcept>
#include <string>
#include <utility>
namespace nano_edr {
OsSource::OsSource(std::string scenario_path)
    : scenario_path_(std::move(scenario_path)) {}
void OsSource::Run(Agent* agent) {
    if (agent == nullptr) {
        throw std::invalid_argument("OsSource::Run: agent is null");
    }
    OsHandle handle(scenario_path_);
    handle.Subscribe(&Agent::Trampoline, agent);
    handle.Start();
    os_status status;
    while((status = handle.Wait(100)) == OS_TIMEOUT) {
    }
    if (status != OS_OK && status != OS_STOPPED) {
        throw std::runtime_error(std::string("os: ") + os_status_str(status));
    }
}
}
