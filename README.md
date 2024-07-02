# simple_cli
Simple command line processor for easy and fast use.

## Simple Example
```cpp
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
```

## Program Parameters
Create program parameters:
```cpp
smplcli::ProgramParameters program("program_name", "program_version");
```

To add a new parameter, call ```.AddParameter<data_type>(<parameter_type>, parameter_names...)```:
```cpp
using ParamType = smplcli::Parameter::ParameterType;
program.AddParameter<float>(ParamType::Option, "--float", "-f");

// supported data types: std::string, int, float, bool
```

You can get a reference to the parameter:
```cpp
auto& float_param = program["--float"]; // by name
auto& pos0 = program[0]; // by position (for pos args only)

```

To start parsing:
```cpp
try {
    program.Parse(argc, argv);
} catch (const std::invalid_argument& e) {
    std::cerr << "Invalid argument: " << e.what() << std::endl;
    exit(1);
}
```



### Subprograms
The Subprograms are a representation of a similar syntax:
```
./program_name -subprog1 sp1_param1 sp1_param2 -subprog2 sp2_param1
```

To add a new subprogram, call ```.AddSubprog(subprog_names...)```:

```cpp
auto& subprog = program.AddSubprog("-subprog");
```

You can also add parameters:

```cpp
using ParamType = smplcli::Parameter::ParameterType;
subprog.AddParameter<int>(ParamType::Option, "--int", "-i");
```

You can get a reference to the subprogram:
```cpp
auto& subprogram = program.GetSubprogram("-subprog");
```

You can get iterators for the subprograms that was found after parsing:

```cpp
for (auto it = program.OrderedSubprogsBegin(); it != program.OrderedSubprogsEnd(); ++it) {
    std::cout << "Current subprogramm name: " << (*it)->GetName() << std::endl;
}

```

## Parameters

You can get value of ```<TValue>``` type from parameter:
```cpp
auto& float_param = program["--float"];
if (float_param.HasValue()) {
    float_param.Get<float>(); 
}
```

You can set default value and Help text:
```cpp
float_param.DefaultValue(1.5f)
.Help("This is float option");
```


## Help Generation
Use ```.ConstructHelp()```:

```cpp
std::cout << program.ConstructHelp();
```