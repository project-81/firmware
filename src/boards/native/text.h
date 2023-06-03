#pragma once

/* toolchain */
#include <string>

namespace Project81
{

std::ostream &get_stdout(bool status);
void print_verb_name_condition(const std::string name, const std::string verb,
                               bool status, bool show_errno = true,
                               bool error_only = false);

}; // namespace Project81
