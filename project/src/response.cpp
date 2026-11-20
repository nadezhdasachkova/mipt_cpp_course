#include "response.h"
#include <print>
#include <cctype>
#include <algorithm>
#include "fields.h"
namespace nano_edr {
namespace {
bool StartsWithIgnoreCase(const std::string& str, const std::string& prefix) {
    if (str.size() < prefix.size()) return false;
    for (std::size_t i = 0; i < prefix.size(); ++i) {
        if (std::tolower(str[i]) != std::tolower(prefix[i])) return false;
    }
    return true;
}
bool IsSystemFile(const std::string& path) {
    std::string lower;
    for (char c : path) lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    return lower.starts_with("c:\\windows\\") || 
           lower.starts_with("c:\\program files") ||
           lower.starts_with("c:/windows/") || 
           lower.starts_with("c:/program files");
}
}
ResponsePlanner::ResponsePlanner(const EntityModel* model, std::string quarantine_dir)
    : model_(model), quarantine_dir_(std::move(quarantine_dir)) {}
std::string ResponsePlanner::QuarantinePath(const std::string& path, const std::string& pid) const {
    if (quarantine_dir_.empty()) return "";
    size_t pos = path.find_last_of("\\/");
    std::string filename = (pos == std::string::npos) ? path : path.substr(pos + 1);
    return quarantine_dir_ + "\\" + pid + "-" + filename;
}
ResponsePlan ResponsePlanner::Plan(const Detection& detection) const {
    ResponsePlan plan;
    auto tree = model_->GetProcessTree(detection.pid);
    for (const auto& name : detection.actions) {
        if (name == "kill_process") {
            for (auto it = tree.rbegin(); it != tree.rend(); ++it) {
                const ProcessRecord* p = model_->FindProcess(*it);
                if (p) {
                    Action a;
                    a.kind = ActionKind::kKillProcess;
                    a.pid = p->pid;
                    a.expected_start = p->start;
                    
                    if (p->pid == detection.pid) {
                        a.reason = detection.rule;
                    } else {
                        a.reason = detection.rule + ", потомок " + detection.pid;
                    }
                    plan.Add(std::move(a));
                }
            }
        } else if (name == "quarantine_source") {
            const FileRecord* source = model_->FindSpawnSource(detection.pid);
            if (source && !IsSystemFile(source->path)) {
                Action a;
                a.kind = ActionKind::kQuarantineFile;
                a.path = source->path;
                a.to = QuarantinePath(source->path, detection.pid);
                if (!a.to.empty()) {
                    a.reason = detection.rule + ", источник запуска";
                    plan.Add(std::move(a));
                }
            }
        } else if (name == "delete_dropped") {
            for (const auto& p_pid : tree) {
                auto files = model_->GetFilesCreatedBy(p_pid, true);
                for (const auto& file : files) {
                    if (!IsSystemFile(file)) {
                        Action a;
                        a.kind = ActionKind::kDeleteFile;
                        a.path = file;
                        a.reason = detection.rule + ", сброшенный файл";
                        plan.Add(std::move(a));
                    }
                }
            }
        } else if (name == "quarantine_image") {
            const ProcessRecord* p = model_->FindProcess(detection.pid);
            if (p && !p->image.empty() && !IsSystemFile(p->image)) {
                Action a;
                a.kind = ActionKind::kQuarantineFile;
                a.path = p->image;
                a.to = QuarantinePath(p->image, detection.pid);
                if (!a.to.empty()) {
                    a.reason = detection.rule + ", образ процесса";
                    plan.Add(std::move(a));
                }
            }
        } else if (name == "delete_evidence") {
            for (const auto& ev : detection.evidence) {
                const char* key = ev->event_type() == EventType::kFileMove ? "to" : "path";
                const std::string* path = FindField(*ev, key);
                if (path && !path->empty() && !IsSystemFile(*path)) {
                Action a;
                a.kind = ActionKind::kDeleteFile;
                a.path = *path;
                a.reason = detection.rule + ", файл из детекта";
                plan.Add(std::move(a));
                }
            }
        }
    }
    return plan;
}
Responder::Responder(OsHandle* os, bool dry_run) : os_(os), dry_run_(dry_run) {}
void Responder::Execute(const ResponsePlan& plan) {
    for (const Action& action : plan.actions()) {
        ++stats_.planned;
        bool repeat = false;
        for (const std::string& target : done_) {
            if (target == action.target()) { repeat = true; break; }
        }
        if (repeat) {
            ++stats_.skipped_repeat;
            continue;
        }
        done_.push_back(action.target());

        if (dry_run_ || os_ == nullptr) {
            std::print("[PLAN] {}\n", ToString(action));
            continue;
        }
        switch (action.kind) {
            case ActionKind::kKillProcess:    Kill(action); break;
            case ActionKind::kQuarantineFile: MoveToQuarantine(action); break;
            case ActionKind::kDeleteFile:     Delete(action); break;
        }
    }
}
}  // namespace nano_edr
