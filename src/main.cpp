#include "dbus.h"
#include "setting.h"
#include "settingInitializerHardcoded.h"
#include "settingReaderFactory.h"
#include <sdbus-c++/sdbus-c++.h>
#include "log.h"
#include "version.h"
#include <thread>
#include <csignal>

constexpr auto DBUS_SERVICE = "owl.gesh";

void signalHandler( int signum ) {
   auto log = Log("interrupt");
   log.error() << "Interrupt signal (" << signum << ") received.";

   // cleanup and close up stuff here
   // terminate program

   exit(signum);
}

void print_help() {
    auto log = Log();
    log.none() << "Usage:";
    log.none() << " gesh [options]";
    log.none();
    log.none() << "Options:";
    log.none() << " -d, --directory <dir>    search directory for fragments";
    log.none();
    log.none() << " -h, --help               display this help";
    log.none() << " -v, --version            display version";
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);

    auto log = Log();
    log.info() << "Gesh - Good enough setting handler";
    log.info() << "Git SHA: " << BUILD_VERSION;
    log.info() << "Build: " << BUILD_DATE;

    log = Log("main");

    std::vector<std::string> searchPaths;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            return 0;
        } else if (arg == "-d" || arg == "--directory") {
            // Try to grab the next argument, a directory
            if (i+1 < argc) {
                searchPaths.push_back(std::string(argv[i+1]));
                i++;
            } else {
                log.error() << "Missing argument after " << arg;
                print_help();
                return 1;
            }
        } else {
            log.error() << "Unknown argument: " << arg;
            print_help();
            return 1;
        }
    }

    auto setting_logger = Log("setting");
    auto init = SettingInitializerDefault();
    auto dflt = std::make_shared<SettingReaderDefault>();
    auto srf = SettingReaderFactory(searchPaths, setting_logger);
    auto readers = srf.getReaders();
    readers.insert(readers.begin(), dflt);

    auto handler = SettingHandler(init, readers, setting_logger);

    auto connection = sdbus::createSessionBusConnection();
    connection->requestName(DBUS_SERVICE);
    connection->enterEventLoopAsync();
    log.notice() << "D-Bus service name aquired. ";

    auto manager = std::make_unique<ManagerAdaptor>(*connection, "/");
    std::vector<std::shared_ptr<DBusGeshSetting>> dbus_settings;

    auto dbus_logger = Log("dbus");
    auto manager_setting_if = SettingInterface("management");
    auto dbus_manager = std::make_shared<DBusGeshManagement>(*connection, &handler, &manager_setting_if, dbus_logger);

    for ( auto & iface : init.Interfaces() ) {
        auto manager = std::make_shared<DBusGeshSetting>(*connection, &handler, iface, dbus_logger);
        iface->RegisterManager(manager);
        dbus_settings.push_back(manager);
    }
    log.notice() << "D-Bus objects registered. ";

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    connection->releaseName(DBUS_SERVICE);
    connection->leaveEventLoop();
    return 0;
}
