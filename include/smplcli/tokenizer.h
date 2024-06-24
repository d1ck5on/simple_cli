#pragma once

#include <string>
#include <variant>

namespace smplcli {
namespace details {
struct PosArgToken {
    std::string name;

    bool operator==(const PosArgToken& other) const;
};

struct ShortOptionToken {
    std::string name;

    bool operator==(const ShortOptionToken& other) const;
};

struct LongOptionToken {
    std::string name;

    bool operator==(const LongOptionToken& other) const;
};

using Token = std::variant<PosArgToken, ShortOptionToken, LongOptionToken>;

class Tokenizer {
public:
    Tokenizer(int argc, char **argv);
    bool IsEnd() const;
    void Next();
    Token GetToken() const;

private:
    int argc_;
    char **argv_;
    int i = 0;
};

}  // namespace details
}  // namespace smplcli
