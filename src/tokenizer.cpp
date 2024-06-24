#include <smplcli/tokenizer.h>

namespace smplcli {
namespace details {

Tokenizer::Tokenizer(int argc, char** argv) : argc_(argc), argv_(argv){};

Token Tokenizer::GetToken() const {
    std::string_view current_parameter = argv_[i];

    if (current_parameter[0] != '-') {
        return PosArgToken{std::string(current_parameter)};
    }

    current_parameter.remove_prefix(1);

    if (current_parameter[0] != '-') {
        return ShortOptionToken{std::string(current_parameter)};
    }

    current_parameter.remove_prefix(1);
    return LongOptionToken{std::string(current_parameter)};
}

void Tokenizer::Next() {
    if (IsEnd()) {
        return;
    }
    ++i;
}

bool Tokenizer::IsEnd() const {
    return i >= argc_;
}

bool PosArgToken::operator==(const PosArgToken& other) const {
    return this->name == other.name;
}

bool ShortOptionToken::operator==(const ShortOptionToken& other) const {
    return this->name == other.name;
}

bool LongOptionToken::operator==(const LongOptionToken& other) const {
    return this->name == other.name;
}

}  // namespace details
}  // namespace smplcli