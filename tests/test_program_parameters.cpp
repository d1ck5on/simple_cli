#include <smplcli/program_parameters.h>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <wordexp.h>

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

TEST_CASE("Program Parameters Constructor") {
    using namespace smplcli;

    ProgramParameters program1("my_program", "1.3");
    ProgramParameters program2("my_program");
    ProgramParameters program3;
}

TEST_CASE("Add Parameter") {
    using namespace smplcli;

    ProgramParameters program;

    program.AddParameter<int>(Parameter::ParameterType::Argument, "number");
    program.AddParameter<std::string>(Parameter::ParameterType::Argument, "string");
    program.AddParameter<float>(Parameter::ParameterType::Argument, "float");

    program.AddParameter<int>(Parameter::ParameterType::Option, "--number");
    program.AddParameter<std::string>(Parameter::ParameterType::Option, "--string");
    program.AddParameter<float>(Parameter::ParameterType::Option, "-f", "--float");
    program.AddParameter<bool>(Parameter::ParameterType::Option, "--bool");

    Cmd cmd("./main --number=10 --string abcd -f 15.0 --bool 1337 lalala 20.4");
    program.Parse(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(program["number"].Get<int>() == 1337);
    REQUIRE(program["string"].Get<std::string>() == "lalala");
    REQUIRE(program["float"].Get<float>() == 20.4f);

    REQUIRE(program[0].Get<int>() == 1337);
    REQUIRE(program[1].Get<std::string>() == "lalala");
    REQUIRE(program[2].Get<float>() == 20.4f);

    REQUIRE(program["--number"].Get<int>() == 10);
    REQUIRE(program["--string"].Get<std::string>() == "abcd");
    REQUIRE(program["-f"].Get<float>() == 15.0f);
    REQUIRE(program["--bool"].Get<bool>());
}

TEST_CASE("Subprogramms") {
    using namespace smplcli;

    ProgramParameters program("Image Editor");
    program.Description("simple image editor");

    program.AddParameter<std::string>(Parameter::ParameterType::Argument, "input_file").Help("path to input file");
    program.AddParameter<std::string>(Parameter::ParameterType::Argument, "output_file").Help("path to output file");

    auto& blur = program.AddSubprog("-blur").Description("blurs the image");
    blur.AddParameter<float>(Parameter::ParameterType::Argument, "sigma").Help(" degree of blurring");

    auto& gs = program.AddSubprog("-gs").Description("grayscale the image");

    auto& crop = program.AddSubprog("-crop").Description("crop the image");
    crop.AddParameter<int>(Parameter::ParameterType::Argument, "width").Help("width of crop");
    crop.AddParameter<int>(Parameter::ParameterType::Argument, "height").Help("height of crop");

    Cmd cmd("./main input.bmp /tmp/output.bmp -crop 800 600 -gs -blur 0.5");
    program.Parse(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(program["input_file"].Get<std::string>() == "input.bmp");
    REQUIRE(program["output_file"].Get<std::string>() == "/tmp/output.bmp");

    REQUIRE(blur["sigma"].Get<float>() == 0.5f);

    REQUIRE(crop["width"].Get<int>() == 800);
    REQUIRE(crop["height"].Get<int>() == 600);

    auto it = program.OrderedSubprogsBegin();
    REQUIRE((*it)->GetName() == "-crop");

    ++it;
    REQUIRE((*it)->GetName() == "-gs");

    ++it;
    REQUIRE((*it)->GetName() == "-blur");
    
    ++it;
    REQUIRE(it == program.OrderedSubprogsEnd());
}

TEST_CASE("Required and Optional Parameters") {
    using namespace smplcli;

    ProgramParameters program;

    program.AddParameter<std::string>(Parameter::ParameterType::Argument, "input_file");
    program.AddParameter<std::string>(Parameter::ParameterType::Argument, "output_file");

    Cmd cmd("./main input.bmp");
    program.Parse(cmd.GetArgc(), cmd.GetArgv());

    REQUIRE(program["input_file"].Get<std::string>() == "input.bmp");

    REQUIRE(program["input_file"].HasValue());
    REQUIRE(!program["output_file"].HasValue());
}