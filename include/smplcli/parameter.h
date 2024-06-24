#pragma once

#include <any>
#include <vector>
#include <string_view>
#include <string>
#include <variant>
#include <type_traits>
#include <smplcli/errors.h>

#include <iostream>

namespace smplcli {
class Parameter {
public:
    enum ParameterType { Argument, Option };

public:
    template <typename TValue>
    static Parameter Create(std::vector<std::string>&& names,
                            ParameterType parameter_type = ParameterType::Option) {
        return Parameter(std::move(names), parameter_type, TValue());
    }

    template <typename TValue>
    constexpr const TValue& Get() const {
        if (!std::get_if<TValue>(&value_)) {
            throw std::logic_error("bad get");
        }

        return *(std::get_if<TValue>(&value_));
    }

    template <typename TValue>
    Parameter& DefaultValue(TValue value) {
        if (!std::holds_alternative<TValue>(value_)) {
            return *this;
        }

        value_ = value;
        flags_.has_default_value = true;

        return *this;
    }

    Parameter& DefaultValueFromString(std::string value);

    Parameter& Help(std::string help);

    std::string ContructHelp(bool del_help = false) const;

    bool IsFlag() const;
    bool HasValue() const;

    std::vector<std::string>::const_iterator NamesBegin() const;
    std::vector<std::string>::const_iterator NamesEnd() const;

private:
    template <typename TValue>
    Parameter(std::vector<std::string>&& names, ParameterType parameter_type, TValue value)
        : names_(std::move(names)),
          value_(value),
          parameter_info_{.parameter_type_ = parameter_type} {
        if constexpr (std::is_same_v<TValue, bool>) {
            flags_.is_flag = true;
        }
    };

    struct AssignValueFromString {
        void operator()(std::string& obj);
        void operator()(int obj);
        void operator()(float obj);
        void operator()(bool obj);

        std::string& value_;
        Parameter& param_;
    };

private:
    using TParam = std::variant<std::string, int, float, bool>;

    std::vector<std::string> names_;
    TParam value_;

    struct ParameterInfo {
        ParameterType parameter_type_;
        std::string help_;
    };

    ParameterInfo parameter_info_;

    struct Flags {
        bool is_flag = false;
        bool has_default_value = false;
    };

    Flags flags_;
};
}  // namespace smplcli
