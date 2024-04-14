#include "dbus.h"
#include "setting.h"
#include "settingInitializerHardcoded.h"
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
    log.none() << " -l, --layers <l1>,<l2>...    Space-separated list of layers to use";
    log.none();
    log.none() << " -h, --help                   display this help";
    log.none() << " -v, --version                display version";
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);

    auto log = Log();
    log.info() << "Gesh - Good enough setting handler";
    log.info() << "Git SHA: " << BUILD_VERSION;
    log.info() << "Build: " << BUILD_DATE;

    log = Log("main");

    SettingLayerHandler layer_handler;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            return 0;
        } else if (arg == "-l" || arg == "--layers") {
            // Try to grab the next argument, a string
            if (i+1 < argc) {
                layer_handler.initialize(std::string(argv[i+1]));
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

    std::vector<SettingLayer*> layers = layer_handler.layers();
    for ( auto storage : init.Storages() ) {
        storage->registerLayers(layers);
    }

    auto handler = SettingHandler(init, setting_logger);

    auto dflt = std::make_shared<SettingReaderDefault>();
    handler.importFromReader(dflt, layer_handler.defaultLayer());

    log.notice() << "Setting initialization DONE. ";
    handler.printSettings();

    auto connection = sdbus::createSessionBusConnection();
    connection->requestName(DBUS_SERVICE);
    connection->enterEventLoopAsync();
    log.notice() << "D-Bus service name aquired. ";

    auto manager = std::make_unique<ManagerAdaptor>(*connection, "/");
    std::vector<std::shared_ptr<DBusGeshSetting>> dbus_settings;

    auto dbus_logger = Log("dbus");
    std::string management_path = "management";
    auto dbus_manager = std::make_shared<DBusGeshManagement>(*connection, &handler, &layer_handler, management_path, dbus_logger);

    for ( auto & iface : init.Interfaces() ) {
        auto manager = std::make_shared<DBusGeshSetting>(*connection, &handler, &layer_handler, iface, dbus_logger);
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
