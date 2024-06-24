#include <smplcli/parameter.h>
#include <variant>

namespace smplcli {
std::vector<std::string>::const_iterator Parameter::NamesBegin() const {
    return names_.begin();
}

std::vector<std::string>::const_iterator Parameter::NamesEnd() const {
    return names_.end();
}

Parameter& Parameter::Help(std::string help) {
    parameter_info_.help_ = std::move(help);
    return *this;
}

std::string Parameter::ContructHelp(bool del_help) const {
    std::string result;

    if (parameter_info_.parameter_type_ == Option) {
        result += "[";
    } else {
        result += "<";
    }

    for (size_t i = 0; i < names_.size() - 1; ++i) {
        result += names_[i];
        result += ", ";
    }

    result += names_[names_.size() - 1];

    if (parameter_info_.parameter_type_ == Option) {
        result += "]";
    } else {
        result += ">";
    }

    if (!IsFlag() && parameter_info_.parameter_type_ == Option) {
        result += " <argument>";
    }

    if (!del_help) {
        result += "\t";
        result += parameter_info_.help_;
    }

    return result;
}

bool Parameter::IsFlag() const {
    return flags_.is_flag;
}

bool Parameter::HasValue() const {
    return flags_.has_default_value;
}

void Parameter::AssignValueFromString::operator()(std::string& obj) {
    param_.value_ = value_;
}

void Parameter::AssignValueFromString::operator()(int obj) {
    for (const auto& ch : value_) {
        if (!std::isdigit(ch)) {
            throw std::invalid_argument("The provided string is not a integer number");
        }
    }

    try {
        param_.value_ = std::stoi(value_);
    } catch (const std::invalid_argument& err) {
        throw;
    } catch (const std::out_of_range& err) {
        throw;
    }
}

void Parameter::AssignValueFromString::operator()(float obj) {
    bool has_dot = false;
    for (const auto& ch : value_) {
        if (!std::isdigit(ch)) {
            if (ch == '.' && !has_dot) {
                has_dot = true;
                continue;
            }

            throw std::invalid_argument("The provided string is not a float number");
        }
    }

    try {
        param_.value_ = std::stof(value_);
    } catch (const std::invalid_argument& err) {
        throw;
    } catch (const std::out_of_range& err) {
        throw;
    }
}

void Parameter::AssignValueFromString::operator()(bool obj) {
    param_.value_ = bool(obj);
}

Parameter& Parameter::DefaultValueFromString(std::string value) {
    std::visit(Parameter::AssignValueFromString{value, *this}, value_);
    flags_.has_default_value = true;
    return *this;
}

}  // namespace smplcli
