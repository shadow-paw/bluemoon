#ifndef KERNEL_STD_COPYABLE_H_
#define KERNEL_STD_COPYABLE_H_

namespace kernel {
template <class T>
class NonCopyable {
 protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
}  // namespace kernel

#endif  // KERNEL_STD_COPYABLE_H_
