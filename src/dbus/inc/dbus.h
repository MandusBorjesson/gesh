#ifndef DBUS_H_
#define DBUS_H_

#include "dbus-management.h"
#include "dbus-setting.h"
#include "setting.h"
#include "log.h"
#include "settingInterface.h"
#include <sdbus-c++/sdbus-c++.h>

namespace {
std::string DBUS_PATH = "/owl/gesh/";
std::string SETTING_SUBPATH = "settings/";
}

std::unique_ptr<sdbus::IConnection> getDbusConnection();

using dbusGet_t = std::tuple<std::map<std::string, sdbus::Variant>, std::map<std::string, std::string>>;
using dbusGetAll_t = std::map<std::string, sdbus::Struct<std::string, int, sdbus::Variant>>;

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

class DBusGeshSetting final : public sdbus::AdaptorInterfaces< owl::gesh::setting_adaptor,
                                                sdbus::ManagedObject_adaptor,
                                                sdbus::Properties_adaptor >,
                          public ISettingApiManager
{
public:
    DBusGeshSetting(sdbus::IConnection& connection, SettingHandler *handler, SettingLayerHandler *layerHandler, SettingInterface *iface, Log &logger)
    : AdaptorInterfaces(connection, DBUS_PATH + SETTING_SUBPATH + iface->Name()), m_handler(handler), m_layerHandler(layerHandler), m_iface(iface), log(logger.getChild(SETTING_SUBPATH + iface->Name()))
    {
        log.info() << "Handler registered created at: " << getObjectPath();
        registerAdaptor();
        emitInterfacesAddedSignal({owl::gesh::setting_adaptor::INTERFACE_NAME});
    }

    ~DBusGeshSetting()
    {
        emitInterfacesRemovedSignal({owl::gesh::setting_adaptor::INTERFACE_NAME});
        unregisterAdaptor();
    }

    dbusGet_t Get(const std::vector<std::string>& keys) override;
    dbusGetAll_t GetAll() override;
    void Set(const std::string &layer, const std::map<std::string, sdbus::Variant>& update, const std::vector<std::string>& invalidate);

    void handleSettingsUpdated(const std::map<std::string, setting_t>& settings);

private:
    sdbus::Variant ToSdBusVariant(const setting_t &val) const;
    setting_t ToSetting(const sdbus::Variant &val) const;
    SettingHandler *m_handler;
    SettingLayerHandler *m_layerHandler;
    SettingInterface *m_iface;
    Log log;
};

class DBusGeshManagement final : public sdbus::AdaptorInterfaces< owl::gesh::management_adaptor,
                                                sdbus::ManagedObject_adaptor,
                                                sdbus::Properties_adaptor >
{
public:
    DBusGeshManagement(sdbus::IConnection& connection, SettingHandler *handler, SettingLayerHandler *layerHandler, std::string &path, Log &logger)
    : AdaptorInterfaces(connection, DBUS_PATH + path), m_handler(handler), m_layerHandler(layerHandler), log(logger.getChild(path))
    {
        log.info() << "Handler registered created at: " << getObjectPath();
        registerAdaptor();
        emitInterfacesAddedSignal({owl::gesh::management_adaptor::INTERFACE_NAME});
    }

    ~DBusGeshManagement()
    {
        emitInterfacesRemovedSignal({owl::gesh::management_adaptor::INTERFACE_NAME});
        unregisterAdaptor();
    }

    void Import(const std::string& layer, const std::string& file) override;

private:
    SettingHandler *m_handler;
    SettingLayerHandler *m_layerHandler;
    Log log;
};

#endif /* DBUS_H_ */
