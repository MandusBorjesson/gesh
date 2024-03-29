#include "dbus.h"
#include "log.h"

dbusGet_t DbusAdaptor::Get(const std::vector<std::string>& keys) {
    INFO << "Get called" << std::endl;

    dbusGet_t ret;
    for (auto key: keys) {
        try {
            auto setting = m_handler->Get(key, m_iface);
            std::get<0>(ret)[key] = sdbus::Variant(ToSdBusVariant(setting));
        } catch ( SettingException const & ex ) {
            std::get<1>(ret)[key] = ex.who();
        }
    }
    INFO  << "Get returned " << std::get<0>(ret).size() << " settings, " << std::get<1>(ret).size()<< " errors" << std::endl;
    return ret;
}

std::map<std::string, sdbus::Variant> DbusAdaptor::GetAll() {
    auto out = std::map<std::string, sdbus::Variant>();
    for (auto setting: m_handler->GetAll(m_iface)) {
        out[setting.first] = ToSdBusVariant(setting.second.Get());
    }
    return out;
}

void DbusAdaptor::Set(const std::map<std::string, sdbus::Variant>& settings) {
    INFO << "Set called" << std::endl;

    std::map<std::string, setting_t> in;
    for (auto const& [key, val] : settings) {
        in[key] = ToSetting(val);
    }

    try {
        m_handler->Set(in, m_iface);
    } catch ( SettingException const & ex ) {
        throw sdbus::Error("own.gesh.Error", ex.what());
    }

}

sdbus::Variant DbusAdaptor::ToSdBusVariant(const setting_t &val) const {
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
setting_t DbusAdaptor::ToSetting(const sdbus::Variant &val) const {
    auto type = val.peekValueType();
    if (type == "s") {
        return std::string(val);
    } else if (type == "i") {
        return int(val);
    } else if (type == "b") {
        return bool(val);
    } else {
        auto err = "Unknown variant type '" + type + "'";
        ERROR << err << std::endl;
        throw sdbus::Error("own.gesh.Error", err);
    }
}

void DbusAdaptor::handleSettingsUpdated(const std::map<std::string, setting_t>& settings) {
    std::map<std::string, sdbus::Variant> out;
    for (auto const& [key, val] : settings) {
        out[key] = ToSdBusVariant(val);
    }

    if (!out.empty()) {
        emitSettingsUpdated(out);
    }
}
