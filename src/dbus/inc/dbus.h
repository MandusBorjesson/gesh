#include "dbus-abstraction.h"
#include "setting.h"
#include <sdbus-c++/sdbus-c++.h>

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
                                                sdbus::Properties_adaptor >
{
public:
    DbusAdaptor(sdbus::IConnection& connection, std::string path, SettingHandler *handler)
    : AdaptorInterfaces(connection, std::move(path)), m_handler(handler)
    {
        registerAdaptor();
        emitInterfacesAddedSignal({owl::gesh::setting_adaptor::INTERFACE_NAME});
    }

    ~DbusAdaptor()
    {
        emitInterfacesRemovedSignal({owl::gesh::setting_adaptor::INTERFACE_NAME});
        unregisterAdaptor();
    }

    void UpdateRequiresReset(const std::string& name) override
    {
        return;
    }

    std::vector<sdbus::Variant> Get(const std::vector<std::string>& names) override;
    std::map<std::string, sdbus::Variant> GetAll() override;
    void Set(const std::map<std::string, sdbus::Variant>& settings);

private:
    sdbus::Variant ToSdBusVariant(const setting_t &val) const;
    setting_t ToSetting(const sdbus::Variant &val) const;
    SettingHandler *m_handler;
};
