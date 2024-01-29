#pragma once

#include "settingSource.h"
#include "settingRule.h"
#include "settingTypes.h"
#include <string>
#include <map>
#include <vector>
#include <ostream>

class ISettingRule;
class ISettingSource;
class ISettingInitializer;
class ISettingReader;

class Setting {
public:
    Setting() = default;
    Setting(const std::string &name, const std::string &value, ISettingSource *source, ISettingRule *rule);
    void Try(const setting_t &value);
    bool Set(const setting_t &value, ISettingSource *source);
    setting_t Get() const { return m_value; };
    std::string Name() const { return m_name; };
    std::string Source() const;
    ISettingRule* Rule() const {return m_rule;};
    bool Ok() const { return m_good; };

    friend std::ostream& operator<<(std::ostream& os, const Setting& s);
private:
    bool m_good{false};
    std::string m_name;
    setting_t m_value;
    ISettingSource *m_source;
    ISettingRule *m_rule;
};

class SettingHandler {
public:
    SettingHandler(ISettingInitializer &initializer,
                   std::vector<ISettingReader*> &readers);

    std::vector<setting_t> Get(const std::vector<std::string> &keys);
    std::map<std::string, setting_t> Set(const std::map<std::string, setting_t> &settings);
    std::map<std::string, Setting> GetAll() const { return m_settings; };

private:
    std::map<std::string, Setting> m_settings;
};
