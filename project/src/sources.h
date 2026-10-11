#pragma once
#include <string>
#include "agent.h"
namespace nano_edr {
int RunFileSource(const std::string& path, Agent& agent);
int RunOsSource(const std::string& scenario_path, Agent& agent);
}  