#pragma once

#include <json/json.hpp>

#include <string>

namespace clrsync::core::json_utils
{
using json = nlohmann::json;

inline std::string normalize_hex_string(const std::string &value)
{
    if (value.size() == 7 && value.front() == '#')
        return value;
    if (value.size() == 6)
        return std::string("#") + value;
    return {};
}

inline bool parse_json_output(const std::string &out, json &doc)
{
    const auto begin = out.find('{');
    const auto end = out.rfind('}');
    if (begin == std::string::npos || end == std::string::npos || begin >= end)
        return false;

    doc = json::parse(out.substr(begin, end - begin + 1), nullptr, false);
    return !doc.is_discarded();
}
} // namespace clrsync::core::json_utils
