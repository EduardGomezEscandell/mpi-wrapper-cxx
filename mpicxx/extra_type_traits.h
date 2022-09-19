# pragma once

#include <type_traits>

#include <vector>
#include <array>

// Returns true if T is the same type as any of Args
template<typename T, typename...Args>
constexpr bool same_as_any() {
    return (std::is_same_v<T, Args> || ...);
}

// Concept based on same_as_any
template<typename T, typename...Args>
concept AnyOf = same_as_any<T, Args...>();


// SFINAE class to detect container properties
template<typename C>
struct container_traits {
    static constexpr bool contiguous = false;
};

template<typename T, std::size_t S>
struct container_traits<std::array<T, S>> {
    static constexpr bool contiguous = true;
    using data = T;
    static constexpr auto size(std::array<T, S> const&)    noexcept ->  std::size_t    { return S; }
    static constexpr auto front(std::array<T, S> & c)      noexcept ->  T&             { return c.front(); }
    static constexpr auto front(std::array<T, S> const& C) noexcept ->  T const&       { return C.front(); }
    static constexpr void try_resize(std::array<T, S> const&, std::size_t sz) noexcept { assert(sz <= S); }
};

template<typename T, std::size_t S>
struct container_traits<T[S]> {
    static constexpr bool contiguous = true;
    using data = T;
    static constexpr auto size(const T[S])        noexcept ->  std::size_t { return S; }
    static constexpr auto front(T c[S])           noexcept ->  T&          { return c[0]; }
    static constexpr auto front(T const c[S])     noexcept ->  T const&    { return c[0]; }
    static constexpr void try_resize(const T[S], std::size_t sz) noexcept  { assert(sz <= S); }
};

template<typename T>
struct container_traits<std::vector<T>> {
    static constexpr bool contiguous = true;
    using data = T;
    static constexpr auto size(std::vector<T> const& c)  noexcept ->  std::size_t { return c.size(); }
    static constexpr auto front(std::vector<T> & c)      noexcept ->  T&          { return c.front(); }
    static constexpr auto front(std::vector<T> const& c) noexcept ->  T const&    { return c.front(); }
    static constexpr void try_resize(std::vector<T> & c, std::size_t sz)          { c.resize(sz); }
};

template<typename T>
struct container_traits<std::basic_string<T>> {
    static constexpr bool contiguous = true;
    using data = T;
    static constexpr auto size(std::vector<T> const& C)  noexcept ->  std::size_t   { return C.size(); }
    static constexpr auto front(std::vector<T> & C)      noexcept ->  T&            { return C.front(); }
    static constexpr auto front(std::vector<T> const& C) noexcept ->  T const&      { return C.front(); }
    static constexpr void try_resize(std::basic_string<T> & c, std::size_t sz)      { c.resize(sz); }
};

template<>
struct container_traits<std::vector<bool>> {
    static constexpr bool contiguous = false;
    using data = bool;
};


// Container with contiguous elements.
template<typename C>
concept ContiguousContainer = container_traits<C>::contiguous;

