#pragma once

#include <optional>
#include <vector>
#include <string>
#include <boost/lexical_cast.hpp>
inline std::optional<int> try_parse_int(std::string str)
{
  try {
    return boost::lexical_cast<int>(str);
  } catch (...) {
    return {};
  }
}
inline std::optional<double> try_parse_double(std::string str)
{
  try {
    return boost::lexical_cast<double>(str);
  } catch (...) {
    return {};
  }
}
inline bool starts_with(std::string str, std::string prefix) {
        return str.rfind(prefix, 0) == 0;
}

