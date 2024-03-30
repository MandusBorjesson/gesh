#pragma once

#include "log.h"
#include "settingSource.h"
#include "settingRule.h"
#include "settingTypes.h"
#include <string>
#include <map>
#include <vector>
#include <ostream>
#include <optional>

class ISettingRule;
class ISettingSource;
class ISettingInitializer;
class ISettingReader;

class Setting {
public:
    Setting() = default;
    Setting(const std::string &name,
            const std::optional<std::string> &value,
            ISettingSource *source,
            ISettingRule *rule,
            std::vector<SettingInterface*> readers,
            std::vector<SettingInterface*> writers
            );
    bool Set(const std::optional<setting_t> &value, ISettingSource *source);
    setting_t Get() const;
    std::string Name() const { return m_name; };
    std::string Source() const;
    ISettingRule* Rule() const {return m_rule;};
    bool canRead(SettingInterface *iface);
    bool canWrite(SettingInterface *iface);
    bool Ok() const { return m_good; };

    friend std::ostream& operator<<(std::ostream& os, const Setting& s);
private:
    bool m_good{false};
    std::string m_name;
    setting_t m_value;
    ISettingSource *m_source;
    ISettingRule *m_rule;
    std::vector<SettingInterface*> m_readers;
    std::vector<SettingInterface*> m_writers;
};

class SettingHandler {
public:
    SettingHandler(ISettingInitializer &initializer,
                   std::vector<ISettingReader*> &readers,
                   Log &logger);

    setting_t Get(const std::string &key, SettingInterface *iface);
    void Set(const std::map<std::string, setting_t> &settings, SettingInterface *iface);
    std::map<std::string, Setting> GetAll(SettingInterface *iface) const;

private:
    std::map<std::string, Setting> m_settings;
    std::vector<SettingInterface*> m_interfaces;
    Log log;
};
