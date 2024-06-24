#include <smplcli/parameter.h>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Parameter::Create") {
    using namespace smplcli;

    Parameter arg1 = Parameter::Create<int>(std::vector<std::string>{"--arg"});
    REQUIRE(!arg1.IsFlag());

    Parameter arg2 = Parameter::Create<std::string>(std::vector<std::string>{"--v", "int"});
    REQUIRE(!arg2.IsFlag());

    Parameter arg3 = Parameter::Create<bool>(std::vector<std::string>{"--v", "int"});
    REQUIRE(arg3.IsFlag());

    Parameter arg4 = Parameter::Create<float>(std::vector<std::string>{"--v", "int"});
    REQUIRE(!arg4.IsFlag());
}

TEST_CASE("Parameter::DefaultValue && Get") {
    using namespace smplcli;

    Parameter arg1 = Parameter::Create<int>(std::vector<std::string>{"--arg"});
    arg1.DefaultValue(5);
    REQUIRE(arg1.Get<int>() == 5);
    REQUIRE_THROWS(arg1.Get<bool>());

    arg1.DefaultValue(10);
    REQUIRE(arg1.Get<int>() == 10);

    arg1.DefaultValue(true);
    REQUIRE(arg1.Get<int>() == 10);


    Parameter arg2 = Parameter::Create<int>(std::vector<std::string>{"--arg2"});
    arg2.DefaultValue(50);
    REQUIRE(arg2.Get<int>() == 50);

    arg2.DefaultValueFromString("123");
    REQUIRE(arg2.Get<int>() == 123);

    REQUIRE_THROWS(arg2.DefaultValueFromString("abcd"));
    REQUIRE_THROWS(arg2.DefaultValueFromString("1abc"));


    Parameter arg3 = Parameter::Create<float>(std::vector<std::string>{"--arg3"});
    arg3.DefaultValue(5.0f);
    REQUIRE(arg3.Get<float>() == 5.0f);

    arg3.DefaultValueFromString("32.4444");
    REQUIRE(arg3.Get<float>() == 32.4444f);

    arg3.DefaultValueFromString(".5");
    REQUIRE(arg3.Get<float>() == 0.5f);

    REQUIRE_THROWS(arg3.DefaultValueFromString("abcd"));
    REQUIRE_THROWS(arg3.DefaultValueFromString("1abc"));
    REQUIRE_THROWS(arg3.DefaultValueFromString("16.8.99"));
}

