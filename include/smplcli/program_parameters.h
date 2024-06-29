#pragma once

#include <smplcli/parameter.h>

#include <vector>
#include <unordered_map>
#include <list>
#include <string>
#include <functional>
#include <iostream>
#include <smplcli/tokenizer.h>

namespace smplcli {
class ProgramParameters {
public:
    explicit ProgramParameters(std::string program_name = "", std::string version = "1.0");
    ~ProgramParameters();

    ProgramParameters(const ProgramParameters &other) = delete;
    ProgramParameters(const ProgramParameters &&other) = delete;
    ProgramParameters &operator=(const ProgramParameters &other) = delete;
    ProgramParameters &operator=(const ProgramParameters &&other) = delete;

    void Parse(int argc, char **argv);

    Parameter &operator[](const std::string &name);
    Parameter &operator[](size_t pos_of_arg);

    ProgramParameters &GetSubprogram(const std::string &name);

    const std::string &GetName() const;

    template <typename... TNames>
    ProgramParameters &AddSubprog(TNames... names) {
        std::vector<std::string> unpacked_names({std::string(std::move(names))...});
        subprogs_.emplace_back(unpacked_names[0]);
        auto subprogs_it = subprogs_.end();
        --subprogs_it;
        for (auto it = unpacked_names.begin(); it != unpacked_names.end(); ++it) {
            subprogs_mapping_.insert({*it, subprogs_it});
        }

        return *subprogs_it;
    }

    template <typename TValue, typename... TNames>
    Parameter &AddParameter(Parameter::ParameterType parameter_type, TNames... names) {
        std::vector<std::string> unpacked_names({std::string(std::move(names))...});

        switch (parameter_type) {
            case Parameter::ParameterType::Option: {
                options_.push_back(Parameter::Create<TValue>(std::move(unpacked_names),
                                                             Parameter::ParameterType::Option));
                auto option_it = options_.end();
                --option_it;
                IndexParameter(option_it);
                return *option_it;
            }

            case Parameter::ParameterType::Argument:
            default: {
                args_.push_back(Parameter::Create<TValue>(std::move(unpacked_names),
                                                          Parameter::ParameterType::Argument));
                auto arg_it = args_.end();
                --arg_it;
                args_mapping_.push_back(arg_it);
                IndexParameter(arg_it);
                return *arg_it;
            }
        }
    }

    std::string ConstructHelp();
    
    ProgramParameters& Description(std::string&& desc);

    std::list<std::list<ProgramParameters>::iterator>::iterator OrderedSubprogsBegin();
    std::list<std::list<ProgramParameters>::iterator>::iterator OrderedSubprogsEnd();

private:
    using ParameterIt = std::list<Parameter>::iterator;
    using SubprogIt = std::list<ProgramParameters>::iterator;

private:
    std::string ConstructHelpAsSubprogramm();
    void ParseAsSubprogramm(details::Tokenizer &tokenizer);

    void IndexParameter(ParameterIt parameter) {
        for (auto it = parameter->NamesBegin(); it != parameter->NamesEnd(); ++it) {
            params_mapping_.insert({*it, parameter});
        }
    }

    struct ProcessParameter {
        void operator()(details::PosArgToken &pos_arg);
        void operator()(details::ShortOptionToken &short_opt);
        void operator()(details::LongOptionToken &long_opt);

        ProgramParameters &program_params;
        details::Tokenizer &tokenizer_;
    };

    bool CheckSubprogramm(details::Tokenizer &tokenizer);

private:
    std::list<Parameter> args_;
    std::list<Parameter> options_;
    std::list<ProgramParameters> subprogs_;

    std::unordered_map<std::string, SubprogIt> subprogs_mapping_;
    std::unordered_map<std::string, ParameterIt> params_mapping_;
    std::vector<ParameterIt> args_mapping_;

    std::list<SubprogIt> subprogs_input_order_;
    ParameterIt current_expected_arg;

private:
    struct ProgramInfo {
        std::string name_;
        std::string version_;
        std::string description_ = "";
    };

    ProgramInfo program_info_;
};
}  // namespace smplcli