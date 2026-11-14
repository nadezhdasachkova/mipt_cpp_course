#ifndef NANO_EDR_FUNCTION_H
#define NANO_EDR_FUNCTION_H
#include <memory>
#include <utility>
namespace nano_edr {
template <typename Signature>
class Function;
template <typename R, typename... Args>
class Function<R(Args...)> {
 private:
    struct CallableBase {
        virtual ~CallableBase() = default;
        virtual R Invoke(Args... args) = 0;
    };
    template <typename F>
    struct CallableImpl : CallableBase {
        F func;
        explicit CallableImp(F : f) : func(std::move(f)) {}
        R Invoke(Args... args) override {
            return func(std::forward<Args>(args)...);
        }
    };
    std::unique_ptr<CallableBase> storage_;

 public:
    Function() = default;
    template <typename F>
    Function(F callable)
        : storage_(std::make_unique<CallableImpl<F>>(std::move(callable))) {}
    Function(Function&&) noexcept = default;
    Function& operator=(Function&&) noexcept = default;
    Function(const Function&) = delete;
    Function& operator=(const Function&) = delete;
    R operator()(Args... args) const {
        return storage_->Invoke(std::forward<Args>(args)...);
    }
    explicit operator bool() const {
        return storage_ != nullptr;
    }
};
}  // namespace nano_edr
#endif