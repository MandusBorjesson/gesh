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
    Setting(std::string name, std::string value, ISettingSource *source, ISettingRule *rule);
    setting_t Try(const std::string &value);
    void Set(std::string value, ISettingSource *source);
    setting_t Get() const { return m_value; };
    std::string Name() const { return m_name; };
    std::string Source() const;
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

    void UpdateSettings(const std::map<std::string, int> newSettings);
    std::vector<setting_t> Get(const std::vector<std::string> keys);
    void Set(const std::map<std::string, std::string> settings);
    std::map<std::string, Setting> GetAll() { return m_settings; };

private:
    std::map<std::string, Setting> m_settings;
};
