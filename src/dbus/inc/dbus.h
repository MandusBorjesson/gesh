#include "dbus-abstraction.h"
#include "setting.h"
#include "log.h"
#include "settingInterface.h"
#include <sdbus-c++/sdbus-c++.h>

constexpr auto DBUS_PATH = "/";

using dbusGet_t = std::tuple<std::map<std::string, sdbus::Variant>, std::map<std::string, std::string>>;

class ManagerAdaptor : public sdbus::AdaptorInterfaces< sdbus::ObjectManager_adaptor >
{
public:
    ManagerAdaptor(sdbus::IConnection& connection, std::string path)
    : AdaptorInterfaces(connection, std::move(path))
    {
        registerAdaptor();
    }

    ~ManagerAdaptor()
    {
        unregisterAdaptor();
    }
};

class DbusAdaptor final : public sdbus::AdaptorInterfaces< owl::gesh::setting_adaptor,
                                                sdbus::ManagedObject_adaptor,
                                                sdbus::Properties_adaptor >,
                          public ISettingApiManager
{
public:
    DbusAdaptor(sdbus::IConnection& connection, SettingHandler *handler, SettingInterface *iface, Log &logger)
    : AdaptorInterfaces(connection, DBUS_PATH + iface->Name()), m_handler(handler), m_iface(iface), log(logger.getChild(iface->Name()))
    {
        registerAdaptor();
        emitInterfacesAddedSignal({owl::gesh::setting_adaptor::INTERFACE_NAME});
    }

    ~DbusAdaptor()
    {
        emitInterfacesRemovedSignal({owl::gesh::setting_adaptor::INTERFACE_NAME});
        unregisterAdaptor();
    }

    dbusGet_t Get(const std::vector<std::string>& keys) override;
    std::tuple<std::map<std::string, sdbus::Variant>, std::vector<std::string>> GetAll() override;
    void Set(const std::map<std::string, sdbus::Variant>& update, const std::vector<std::string>& invalidate);

    void handleSettingsUpdated(const std::map<std::string, setting_t>& settings);

private:
    sdbus::Variant ToSdBusVariant(const setting_t &val) const;
    setting_t ToSetting(const sdbus::Variant &val) const;
    SettingHandler *m_handler;
    SettingInterface *m_iface;
    Log log;
};
