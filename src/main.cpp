#include "dbus-abstraction.h"
#include "setting.h"
#include "settingInitializerHardcoded.h"
#include "settingReaderFile.h"
#include "settingTypes.h"
#include <sdbus-c++/sdbus-c++.h>
#include "log.h"
#include "version.h"
#include <memory>
#include <thread>
#include <chrono>
#include <csignal>
#include <iomanip>

constexpr auto DBUS_SERVICE = "owl.gesh";
constexpr auto DBUS_PATH = "/owl/gesh";

void signalHandler( int signum ) {
   ERROR << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here
   // terminate program

   exit(signum);
}

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

    std::vector<sdbus::Variant> Get(const std::vector<std::string>& names) override {

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

    std::map<std::string, sdbus::Variant> GetAll() override {
        auto out = std::map<std::string, sdbus::Variant>();
        for (auto setting: m_handler->GetAll()) {
            out[setting.first] = ToSdBusVariant(setting.second.Get());
        }
        return out;
    }

    void Set(const std::map<std::string, sdbus::Variant>& settings) {
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

private:
    sdbus::Variant ToSdBusVariant(const setting_t &val) {
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
    std::string ToString(const sdbus::Variant &val) {
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
    SettingHandler *m_handler;
};

class TimeKeeper {
public:
    TimeKeeper() {
        m_start = std::chrono::steady_clock::now();
        m_last = m_start;
    };
    void setLast(std::chrono::time_point<std::chrono::steady_clock> time) { m_last = time; };
    friend std::ostream& operator<<(std::ostream& os, TimeKeeper& t) {
        auto t_now = std::chrono::steady_clock::now();
        const std::chrono::duration diff_start = t_now - t.m_start;
        const std::chrono::duration diff_last = t_now - t.m_last;

        double diff_start_double = 1000 * double(diff_start.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
        double diff_last_double = 1000* double(diff_last.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
;

        os << std::fixed << std::setprecision(2) << std::left;
        std::cout << "Time since start: " << diff_start_double << "ms, Delta time: " << diff_last_double << "ms.";
        t.setLast(t_now);
        return os;
    };

protected:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
    std::chrono::time_point<std::chrono::steady_clock> m_last;
};

int main()
{
    auto t_keeper = TimeKeeper();

    signal(SIGINT, signalHandler);

    INFO << "Gesh - Good enough setting handler" << std::endl;
    INFO << "Git SHA: " << BUILD_VERSION << std::endl;
    INFO << "Build: " << BUILD_DATE << std::endl;

    auto init = SettingInitializerHardcoded();
    std::vector<ISettingReader*> readers = {new SettingReaderCsv("/home/mandus/git/gesh/test.csv")};

    DEBUG << "Initializing settings... " << t_keeper << std::endl;
    auto handler = SettingHandler(init, readers);
    DEBUG << "Setting initialization DONE. " << t_keeper << std::endl;
    for ( auto const& [key, val] : handler.GetAll() ) {
        DEBUG << val << std::endl;
    }

    auto connection = sdbus::createSessionBusConnection();
    connection->requestName(DBUS_SERVICE);
    connection->enterEventLoopAsync();
    DEBUG << "D-Bus service name aquired. " << t_keeper << std::endl;

    auto manager = std::make_unique<ManagerAdaptor>(*connection, DBUS_PATH);
    auto settingManager = std::make_unique<DbusAdaptor>(*connection, DBUS_PATH, &handler);
    DEBUG << "D-Bus objects registered. " << t_keeper << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    connection->releaseName(DBUS_SERVICE);
    connection->leaveEventLoop();
    return 0;
}
