#include "log.h"
#include "setting.h"

Setting::Setting(const std::string &name,
                 const std::string &value,
                 ISettingSource *source,
                 ISettingRule *rule,
                 std::vector<SettingInterface*> readers,
                 std::vector<SettingInterface*> writers) : m_name(name), m_source(source), m_rule(rule), m_readers(readers), m_writers(writers) {
    if (!m_rule) {
        throw SettingRuleMissingException(name);
    }

    try {
        m_value = m_rule->ToSetting(value);
        m_good = true;
    } catch ( SettingException const& ex ) {
    }
}

bool Setting::Set(const setting_t &value, ISettingSource *source) {
    setting_t new_value;
    try {
        new_value = m_rule->ToSetting(value);
    } catch ( SettingException const& ex ) {
        return false;
    }

    m_source = source;

    if (m_value == new_value) {
        return false;
    }
    m_value = value;
    return true;
}

setting_t Setting::Get() const {
    return m_value;
}

std::string Setting::Source() const {
    if (m_source) {
        return m_source->Alias();
    } else {
        return "None";
    }
}

bool Setting::canRead(SettingInterface *iface) {
    for (auto interface : m_readers) {
        if (iface == interface) {
            return true;
        }
    }
    return false;
}

bool Setting::canWrite(SettingInterface *iface) {
    for (auto interface : m_writers) {
        if (iface == interface) {
            return true;
        }
    }
    return false;
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

class SettingHandlerException : public SettingException {
public:
    SettingHandlerException(const std::string &msg) : SettingException(msg, ".handler") {}
};

class SettingKeyException : public SettingException {
public:
    SettingKeyException(const std::string &msg) : SettingException(msg, ".key") {}
};

class SettingAccessException : public SettingException {
public:
    SettingAccessException(const std::string &msg) : SettingException(msg, ".access") {}
};

SettingHandler::SettingHandler(ISettingInitializer &initializer,
                               std::vector<ISettingReader*> &readers): m_interfaces(initializer.Interfaces()) {

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
            DEBUG << "Setting: " << setting.first << std::endl;
            if (m_settings.find(setting.first) == m_settings.end()) {
                WARNING << "Unknown setting: " << setting.first << std::endl;
                continue;
            }
            try {
                auto val = m_settings[setting.first].Rule()->ToSetting(setting.second);
                m_settings[setting.first].Set(val, reader);
            } catch ( SettingRuleException const& ex ) {
                continue;
            }
        }
    }
}

std::vector<setting_t> SettingHandler::Get(const std::vector<std::string> &keys, SettingInterface *iface) {
    std::vector<setting_t> out;
    for (auto key: keys) {
        if (m_settings.find(key) == m_settings.end()) {
            throw SettingKeyException(key);
        }
        if (!m_settings[key].canRead(iface)) {
            throw SettingAccessException(key);
        }
        out.push_back(m_settings[key].Get());
    }
    return out;
}

void SettingHandler::Set(const std::map<std::string, setting_t> &settings, SettingInterface *iface) {
    for ( auto const& [key, val] : settings ) {
        if (m_settings.find(key) == m_settings.end()) {
            throw SettingKeyException(key);
        }
        if (!m_settings[key].canWrite(iface)) {
            throw SettingAccessException(key);
        }

        // Will throw if validation fails
        m_settings[key].Rule()->ToSetting(val);
    }
    std::map<std::string, setting_t> updated;
    for ( auto const& [key, val] : settings ) {
        if(m_settings[key].Set(val, nullptr)) {
            updated[key] = val;
        }
    }

    for (const auto interface : m_interfaces) {
        std::map<std::string, setting_t> temp;
        for (const auto& [key, val] : updated) {
            if(m_settings[key].canRead(interface)) {
                temp[key] = val;
            }
        }
        interface->Manager()->handleSettingsUpdated(temp);
    }
}
