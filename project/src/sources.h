#ifndef NANO_EDR_SOURCES_H
#define NANO_EDR_SOURCES_H
#include <string>
#include "event_source.h"
namespace nano_edr {
class FileSource : public IEventSource {
    public:
        explicit FileSource(std::string path);
        void Run(IEventHandler* handler) override;
    private:
        std::string path_;
};
class OsSource : public IEventSource {
    public:
        explicit OsSource(std::string scenario_path);
        void Run(IEventHandler* handler) override;
    private:
        std::string scenario_path_;
};
}
#endif