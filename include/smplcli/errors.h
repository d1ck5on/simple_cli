#include <string>

namespace smplcli {
namespace errors {

void InvalidShortOption(std::string& name);
void ArgumentRequired(std::string& name);
void LotOfPosArgs();

}  // namespace errors

}  // namespace smplcli
