#ifndef NANO_EDR_OS_SOURCE_H
#define NANO_EDR_OS_SOURCE_H
#include <string>
#include "agent.h"
namespace nano_edr {
class OsSource {
    public:
        explicit OsSource(std::string scenario_path);
        void Run(Agent* agent);
    private:
        std::string scenario_path_;
};
}
#endif