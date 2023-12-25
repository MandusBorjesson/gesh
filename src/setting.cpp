#include "log.h"
#include "setting.h"

Setting::Setting(std::string name, std::string value, ISettingSource *source, ISettingRule *rule) : m_name(name), m_source(source), m_rule(rule) {
    if (m_rule) {
        try {
            m_value = m_rule->ToSetting(value);
            m_good = true;
        } catch ( SettingException const& ex ) {
            ERROR << "Failed to initialize: " << m_name << ": " << ex.what() << std::endl;
        }
    } else {
        m_value = value;
        m_good = true;
    }
}

void Setting::Set(std::string value, ISettingSource *source) {

    if (!m_rule) {
        m_value = value;
        m_source = source;
        return;
    }

    try {
        m_value = m_rule->ToSetting(value);
        m_source = source;
    } catch ( SettingException const& ex ) {
        WARNING << "Failed to set: " << m_name << ": " << value << ". Error: " << ex.what() << std::endl;
    }
}

std::string Setting::Source() const {
    return m_source->Alias();
}

std::ostream& operator<<(std::ostream& os, const Setting& s)
{
    os << "[" << s.Source() << "] " << s.Name() << " ";
    auto val = s.Get();

    if (std::holds_alternative<std::string>(val)) {
        os << std::get<std::string>(val);
    } else if (std::holds_alternative<int>(val)) {
        os << std::get<int>(val);
    } else if (std::holds_alternative<bool>(val)) {
        os << std::get<bool>(val) ? "True" : "False";
    } else {
        os << "N/A";
    }

    return os;
}

SettingHandler::SettingHandler(ISettingInitializer &initializer,
                               std::vector<ISettingReader*> &readers) {

    auto settings = initializer.InitializeSettings();
    bool settings_ok = true;

    for (auto setting : settings) {
        if (setting.Ok()) {
            m_settings[setting.Name()] = setting;
        } else {
            settings_ok = false;
        }
    }
    if (!settings_ok) {
        CRITICAL << "One or more setting(s) failed initialization. This is bad, and probably programmer error. The settings will not be available!" << std::endl;
    }

    for (auto reader : readers) {
        INFO << "[SettingHandler] Fetching settings from " << reader->Alias() << std::endl;
        for (auto setting : reader->GetSettings()) {
            if (m_settings.find(setting.first) == m_settings.end()) {
                WARNING << "Unknown setting" << setting.first << std::endl;
                continue;
            }
            m_settings[setting.first].Set(setting.second, reader);
        }
    }
}

void SettingHandler::UpdateSettings(const std::map<std::string, int> newSettings) {
    return;
}
