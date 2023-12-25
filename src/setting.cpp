#include "log.h"
#include "setting.h"

Setting::Setting(std::string name, setting_t value, ISettingSource *source, ISettingRule *rule) : m_name(name), m_source(source), m_rule(rule) {
    Set(value);
}

bool Setting::Set(setting_t value) {
    if (!m_rule || m_rule->Verify(value)) {
        m_value = value;
        return true;
    }

    return false;
}

std::string Setting::Source() const {
    return m_source->Alias();
}

bool Setting::Ok() {
    if (!m_rule)
        return true;

    return m_rule->Verify(m_value);
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
                               std::vector<ISettingReader> &extraReaders) {

    auto settings = initializer.InitializeSettings();

    for (auto setting : settings) {
        if (setting.Ok()) {
            m_settings[setting.Name()] = setting;
        } else {
            WARNING << "Rule check failed for " << setting.Name() << std::endl;
        }
    }
}

void SettingHandler::UpdateSettings(const std::map<std::string, int> newSettings) {
    return;
}
