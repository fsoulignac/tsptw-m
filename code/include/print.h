/*
 * @author Francisco Soulignac
 * @brief Formatters for standard structures (for debugging purposes mostly)
 * 
 * Code adapted from different sources
 */

#ifndef PRINT_H
#define PRINT_H

#include <format>
#include <bitset>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>
    
namespace {
    //format a collection
    template<typename Collection>
    auto do_format(std::format_context& ctx, const Collection& C, char prefix='{', char suffix='}') {
        auto out = ctx.out();
        out = std::format_to(out, "{}", prefix);
        bool comma = false;
        for (auto& v : C)
        {
            if(comma) out = std::format_to(out, ",");
            out = std::format_to(out, "{}", v);
            comma = true;
        }
        return std::format_to(out, "{}", suffix);
    } 
};   

template <class T1, class T2>
struct std::formatter<std::pair<T1, T2>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::pair<T1, T2>& p, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "({},{})", p.first, p.second);
    }
};

template <class... Ts>
struct std::formatter<std::tuple<Ts...>> {

    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}

    template <std::size_t... I>
    static auto format_impl(const std::tuple<Ts...>& tup, std::format_context& ctx, std::index_sequence<I...>)
    {
        auto out = ctx.out();
        out = std::format_to(out, "(");

        ((out = std::format_to(out, "{}", std::get<I>(tup)),
          out = std::format_to(out, "{}", (I + 1 < sizeof...(Ts) ? ", " : ""))), ...);

        return std::format_to(out, ")");
    }

    auto format(const std::tuple<Ts...>& tup, std::format_context& ctx) const
    {
        return format_impl(tup, ctx, std::index_sequence_for<Ts...>{});
    }
};

template <typename T>
struct std::formatter<std::vector<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::vector<T>& v, std::format_context& ctx) const {return do_format(ctx, v, '[', ']');}
};

template <typename T>
struct std::formatter<std::deque<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::deque<T>& q, std::format_context& ctx) const {return do_format(ctx, q, '[', ']');}
};

template<typename T, typename C>
struct std::formatter<std::set<T, C>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::set<T, C>& s, std::format_context& ctx) const {return do_format(ctx, s);}
};

template<typename T>
struct std::formatter<std::unordered_set<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::unordered_set<T>& s, std::format_context& ctx) const {return do_format(ctx, s);}
};

template<typename K, typename V>
struct std::formatter<std::map<K,V>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::map<K,V>& m, std::format_context& ctx) const {return do_format(ctx, m);}
};

template<typename K, typename V>
struct std::formatter<std::unordered_map<K,V>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::unordered_map<K,V>& m, std::format_context& ctx) const {return do_format(ctx, m);}
};

// Prints the list in the ostream os.
// The output format is "[ element1, element2, ..., elementn ]".
// Precondition: The type T must have the <<(ostream&) operator implemented.
// Returns: the modified ostream.
template <typename T>
struct std::formatter<std::list<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::list<T>& l, std::format_context& ctx) const {return do_format(ctx, l, '[', ']');}
};

template <unsigned long N>
struct std::formatter<std::bitset<N>> {
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    auto format(const std::bitset<N>& b, std::format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "(");
        bool comma = false;
        for (unsigned long i = 0; i < N; ++i)
        if (b.test(i))
        {
            if (comma) out = std::format_to(out, ",");
            comma = true;
            out = std::format_to(out, "{}", i);
        }
        return std::format_to(out, ")");
    }
};

#endif //PRINT_H
