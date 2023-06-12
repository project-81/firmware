#pragma once

/* toolchain */
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace Project81
{

std::ostream &get_stdout(bool status);
void print_verb_name_condition(const std::string name, const std::string verb,
                               bool status, bool show_errno = true,
                               bool error_only = false);

template <typename data_t>
void populate_flag_strs(data_t data, std::vector<std::string> &flag_strs,
                        const std::map<std::string, data_t> &flags,
                        std::ostream *stream = nullptr, bool endl = true)
{
    data_t flags_found = 0;

    for (const auto &[key, value] : flags)
    {
        data_t masked = data & value;
        if (masked)
        {
            flag_strs.push_back(key + "=" + std::to_string(masked));
            flags_found |= masked;
        }
    }

    if (stream)
    {
        *stream << "0x" << std::hex << data;

        /*
         * Leave behind some info if the flags we iterated through don't
         * combine to produce the original value (this probably means that
         * the map of flags passed in is incomplete, or that there are "don't
         * care" bits).
         */
        if (flags_found != data)
        {
            *stream << " (!= " << std::hex << flags_found << ")";
        }

        for (auto &flag_str : flag_strs)
        {
            *stream << ", " << flag_str;
        }
        if (endl)
        {
            *stream << std::endl;
        }
    }
}

}; // namespace Project81
