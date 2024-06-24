#include <smplcli/errors.h>
#include <iostream>
#include <exception>

namespace smplcli {
namespace errors {
void InvalidShortOption(std::string& name) {
    throw std::invalid_argument(name);
}

void ArgumentRequired(std::string& name) {
    throw std::invalid_argument(name);
}

void LotOfPosArgs() {
    throw std::invalid_argument("lot of args");
}

}  // namespace errors
}  // namespace smplcli