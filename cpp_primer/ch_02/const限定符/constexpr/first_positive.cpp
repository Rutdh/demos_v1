#include <type_traits>

template<class T>
constexpr auto hash_like(const T& value) {
    if constexpr (requires { value.size(); }) {
        return value.size();
    } else if constexpr (std::is_integral_v<T>) {
        return static_cast<std::size_t>(value * 2654435761u);
    } else {
        return 0u;
    }
}
static_assert(hash_like(42) == 11151230362u % (1ull<<32));
