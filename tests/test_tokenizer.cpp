#include <smplcli/tokenizer.h>
#include <catch2/catch_test_macros.hpp>

#include <wordexp.h>
#include <string>
#include <iostream>

class Cmd {
public:
    Cmd(const std::string& input) {
        wordexp(input.c_str(), &wexp, 0);
    }

    ~Cmd() {
        wordfree(&wexp);
    }

    int GetArgc() const {
        return wexp.we_wordc;
    }

    char** GetArgv() const {
        return wexp.we_wordv;
    }

private:
    wordexp_t wexp;
};

TEST_CASE("Base case") {
    using namespace smplcli::details;

    Cmd cmd("./main -a --sss -sda \"kfk fkf\" ssss");
    Tokenizer tokenizer(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(!tokenizer.IsEnd());

    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"./main"}});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == Token{ShortOptionToken{"a"}});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == Token{LongOptionToken{"sss"}});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == Token{ShortOptionToken{"sda"}});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"kfk fkf"}});

    tokenizer.Next();
    REQUIRE(!tokenizer.IsEnd());
    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"ssss"}});

    tokenizer.Next();
    REQUIRE(tokenizer.IsEnd());
}

TEST_CASE("GetToken is not moving") {
    using namespace smplcli::details;

    Cmd cmd("./main -a --sss -sda kfkfkf ssss");
    Tokenizer tokenizer(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"./main"}});
    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"./main"}});
}

TEST_CASE("Just name") {
    using namespace smplcli::details;

    Cmd cmd("./main");
    Tokenizer tokenizer(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"./main"}});
    tokenizer.Next();
    REQUIRE(tokenizer.IsEnd());
}

TEST_CASE("A lot of spaces") {
    using namespace smplcli::details;

    Cmd cmd("./main              fffff");
    Tokenizer tokenizer(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"./main"}});

    tokenizer.Next();
    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"fffff"}});

    tokenizer.Next();
    REQUIRE(tokenizer.IsEnd());
}

TEST_CASE("A lot of -") {
    using namespace smplcli::details;

    Cmd cmd("./main ----------fffff");
    Tokenizer tokenizer(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(tokenizer.GetToken() == Token{PosArgToken{"./main"}});

    tokenizer.Next();
    REQUIRE(tokenizer.GetToken() == Token{LongOptionToken{"--------fffff"}});

    tokenizer.Next();
    REQUIRE(tokenizer.IsEnd());
}

TEST_CASE("Empty") {
    using namespace smplcli::details;

    Cmd cmd("");
    Tokenizer tokenizer(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(tokenizer.IsEnd());
}

