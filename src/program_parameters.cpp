#include <smplcli/program_parameters.h>
#include <smplcli/errors.h>

#include <iostream>

namespace smplcli {

namespace details {

std::string parse_name(std::string_view value) {
    size_t equal_sign = value.find("=");

    if (equal_sign != std::string::npos) {
        return std::string(value.begin(), value.begin() + equal_sign);
    }

    return std::string(value);
}

std::string parse_arg(std::string_view value) {
    size_t equal_sign = value.find("=");

    if (equal_sign != std::string::npos) {
        return std::string(value.begin() + equal_sign + 1, value.end());
    }

    return std::string();
}

std::string parse_arg_from_tokenizer(Tokenizer& tokenizer) {
    tokenizer.Next();

    if (tokenizer.IsEnd()) {
        return std::string();
    }

    details::Token arg_token = tokenizer.GetToken();

    if (!std::get_if<details::PosArgToken>(&arg_token)) {
        return std::string();
    }

    return std::get_if<details::PosArgToken>(&arg_token)->name;
}

}  // namespace details

Parameter& ProgramParameters::operator[](const std::string& name) {
    return *(params_mapping_[name]);
}
Parameter& ProgramParameters::operator[](size_t pos_of_arg) {
    return *(args_mapping_[pos_of_arg]);
}
ProgramParameters& ProgramParameters::GetSubprogram(const std::string& name) {
    return *(subprogs_mapping_[name]);
}

void ProgramParameters::Parse(int argc, char** argv) {
    current_expected_arg = args_.begin();
    details::Tokenizer tokenizer(argc, argv);
    ProcessParameter process_param{*this, tokenizer};

    tokenizer.Next();

    while (!tokenizer.IsEnd()) {
        details::Token token = tokenizer.GetToken();

        if (CheckSubprogramm(tokenizer)) {
            if (tokenizer.IsEnd()) {
                return;
            }

            token = tokenizer.GetToken();
            continue;
        }

        std::visit(process_param, token);

        tokenizer.Next();
    }
}

ProgramParameters::ProgramParameters(std::string program_name, std::string version)
    : program_info_{std::move(program_name), std::move(version)} {};

ProgramParameters::~ProgramParameters() = default;

void ProgramParameters::ProcessParameter::operator()(details::PosArgToken& pos_arg) {
    if (program_params.current_expected_arg == program_params.args_.end()) {
        errors::LotOfPosArgs();
    }

    program_params.current_expected_arg->DefaultValueFromString(pos_arg.name);
    program_params.current_expected_arg++;
}

void ProgramParameters::ProcessParameter::operator()(details::ShortOptionToken& short_opt) {
    std::string name = "-" + details::parse_name(short_opt.name);

    if (program_params.params_mapping_.find(name) == program_params.params_mapping_.end()) {
        errors::InvalidShortOption(name);
    };

    Parameter& param = *(program_params.params_mapping_[name]);

    if (param.IsFlag()) {
        param.DefaultValue(true);
        return;
    }

    std::string arg = details::parse_arg(short_opt.name);

    if (arg.empty()) {
        arg = details::parse_arg_from_tokenizer(tokenizer_);
        if (arg.empty()) {
            errors::ArgumentRequired(name);
        }
    }

    param.DefaultValueFromString(arg);
}

void ProgramParameters::ProcessParameter::operator()(details::LongOptionToken& long_opt) {
    std::string name = "--" + details::parse_name(long_opt.name);

    if (program_params.params_mapping_.find(name) == program_params.params_mapping_.end()) {
        errors::InvalidShortOption(name);
    };

    Parameter& param = *(program_params.params_mapping_[name]);

    if (param.IsFlag()) {
        param.DefaultValue(true);
        return;
    }

    std::string arg = details::parse_arg(long_opt.name);

    if (arg.empty()) {
        arg = details::parse_arg_from_tokenizer(tokenizer_);
        if (arg.empty()) {
            errors::ArgumentRequired(name);
        }
    }

    param.DefaultValueFromString(arg);
}

bool ProgramParameters::CheckSubprogramm(details::Tokenizer& tokenizer) {
    auto token = tokenizer.GetToken();

    if (!std::get_if<details::ShortOptionToken>(&token)) {
        return false;
    }

    auto name = "-" + std::get_if<details::ShortOptionToken>(&token)->name;
    if (subprogs_mapping_.find(name) == subprogs_mapping_.end()) {
        return 0;
    }

    ProgramParameters& subprog = GetSubprogram(name);

    try {
        subprog.ParseAsSubprogramm(tokenizer);
    } catch (const std::invalid_argument& err) {
    };

    subprogs_input_order_.push_back(subprogs_mapping_[name]);

    return true;
}

void ProgramParameters::ParseAsSubprogramm(details::Tokenizer& tokenizer) {
    current_expected_arg = args_.begin();
    ProcessParameter process_param{*this, tokenizer};

    tokenizer.Next();

    while (!tokenizer.IsEnd()) {
        details::Token token = tokenizer.GetToken();

        std::visit(process_param, token);

        tokenizer.Next();
    }
}

const std::string& ProgramParameters::GetName() const {
    return program_info_.name_;
}

std::string ProgramParameters::ConstructHelp() {
    std::string result;
    result += "help:\t" + program_info_.name_ + " " + "v" + program_info_.version_ + "\n\n";
    result += "\t" + program_info_.description_ + "\n\n";

    result += "Options:\n";
    
    for (const auto& option : options_) {
        result += "\t  " + option.ContructHelp() + "\n";
    }

    result += "\nArguments:\n";

    for (const auto& arg : args_) {
        result += "\t" + arg.ContructHelp() + "\n";
    }

    result += "\nCommands:\n";

    for (auto& subprog : subprogs_) {
        result += subprog.ConstructHelpAsSubprogramm() + "\n";
    }

    return result;
}

std::string ProgramParameters::ConstructHelpAsSubprogramm() {
    std::string result;
    result += "\t" + program_info_.name_ + " ";

    for (const auto& option : options_) {
        result += option.ContructHelp(true) + " ";
    }
    
    for (const auto& arg : args_) {
        result += arg.ContructHelp(true) + " ";
    }

    result += "\n\t" + program_info_.description_ + "\n";

    return result;
}

ProgramParameters& ProgramParameters::Description(std::string&& desc) {
    program_info_.description_ = std::move(desc);
    return *this;
}

std::list<std::list<ProgramParameters>::iterator>::iterator
ProgramParameters::OrderedSubprogsBegin() {
    return subprogs_input_order_.begin();
}

std::list<std::list<ProgramParameters>::iterator>::iterator
ProgramParameters::OrderedSubprogsEnd() {
    return subprogs_input_order_.end();
}
}  // namespace smplcli