#ifndef NANO_EDR_SOURCES_H
#define NANO_EDR_SOURCES_H
#include <string>
#include "agent.h"
namespace nano_edr {
class FileSource {
    public:
        explicit FileSource(std::string path);
        void Run(Agent* agent);
    private:
        std::string path_;
};
class OsSource {
    public:
        explicit OsSource(std::string scenario_path);
        void Run(Agent* agent);
    private:
        std::string scenario_path_;
};
} 
#endif