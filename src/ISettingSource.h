#pragma once

#include <variant>
#include <vector>
#include <string>
#include "ISettingRule.h"

class ISettingSource {
public:
    struct Setting {
        std::string name;
        std::variant<bool, int, std::string> value;
        ISettingSource *source;
        ISettingRule *rule;
    };

    ISettingSource(std::string name) : m_alias(name) {};
    virtual std::vector<ISettingSource::Setting> GetSettings() = 0;
    const std::string Alias() { return m_alias; };

protected:
    std::string m_alias;
};
