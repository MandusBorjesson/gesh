#include "dbus.h"
#include "log.h"

std::vector<sdbus::Variant> DbusAdaptor::Get(const std::vector<std::string>& names) {
    std::vector<setting_t> settings;
    try {
        settings = m_handler->Get(names);
    } catch ( SettingException const & ex ) {
        std::string err = ex.what();
        ERROR << "Failed to get setting(s): " + err << std::endl;;
        throw sdbus::Error("own.gesh.Error", "Failed to get setting(s): " + err);
    }
    auto out = std::vector<sdbus::Variant>();
    for (auto setting: settings) {
        out.push_back(sdbus::Variant(ToSdBusVariant(setting)));
    }
    return out;
}

std::map<std::string, sdbus::Variant> DbusAdaptor::GetAll() {
    auto out = std::map<std::string, sdbus::Variant>();
    for (auto setting: m_handler->GetAll()) {
        out[setting.first] = ToSdBusVariant(setting.second.Get());
    }
    return out;
}

void DbusAdaptor::Set(const std::map<std::string, sdbus::Variant>& settings) {
    DEBUG << "Set called" << std::endl;
    std::map<std::string, std::string> in;
    for (auto const& [key, val] : settings) {
        in[key] = std::string(ToString(val));
    }
    try {
        m_handler->Set(in);
    } catch ( SettingException const & ex ) {
        std::string err = ex.what();
        ERROR << "Failed to set setting(s): " + err << std::endl;;
        throw sdbus::Error("own.gesh.Error", "Failed to set setting(s): " + err);
    }
    emitSettingsUpdated(settings);
}

sdbus::Variant DbusAdaptor::ToSdBusVariant(const setting_t &val) {
    if (std::holds_alternative<std::string>(val)) {
        return sdbus::Variant(std::get<std::string>(val));
    } else if (std::holds_alternative<int>(val)) {
        return sdbus::Variant(std::get<int>(val));
    } else if (std::holds_alternative<bool>(val)) {
        return sdbus::Variant(std::get<bool>(val));
    } else {
        return sdbus::Variant(NULL);
    }
}
std::string DbusAdaptor::ToString(const sdbus::Variant &val) {
    auto type = val.peekValueType();
    if (type == "s") {
        return std::string(val);
    } else if (type == "i") {
        return std::to_string(int(val));
    } else if (type == "b") {
        return (bool(val) == true) ? "1" : "0";
    } else {
        auto err = "Unknown variant type '" + type + "'";
        ERROR << err << std::endl;
        throw sdbus::Error("own.gesh.Error", err);
    }
}
