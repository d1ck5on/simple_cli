#include <smplcli/program_parameters.h>
#include <iostream>

int main(int argc, char** argv) {
    using ParamType = smplcli::Parameter::ParameterType;

    smplcli::ProgramParameters program("MyProgram");

    program.AddParameter<std::string>(ParamType::Argument, "input_file");

    try {
        program.Parse(argc, argv);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        exit(1);
    }

    if (program["input_file"].HasValue()) {
        std::cout << "Input file: " << program["input_file"].Get<std::string>() << std::endl;
    } else {
        std::cerr << "Input file required" << std::endl;
    }
}