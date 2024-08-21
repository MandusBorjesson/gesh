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

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);

    auto log = Log();
    log.info() << "Gesh - Good enough setting handler";
    log.info() << "Git SHA: " << BUILD_VERSION;
    log.info() << "Build: " << BUILD_DATE;

    // The pair in 'args' should be read as 'argument', 'consumed'. The latter
    // indicates wether a class has consumed the argument.
    std::vector<std::pair<std::string, bool>> args;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        args.push_back(std::make_pair(argv[i], false));
    }
    for (auto & [arg, consumed] : args) {
        if (arg == "-h" || arg == "--help") {
            consumed = true;

            log.none() << "Usage:";
            log.none() << " gesh [options]";
            log.none();
            log.none() << "Options:";
            log.none() << " -h, --help                   display this help";
            log.none();
        }
    }
    SettingLayerHandler layer_handler;
    if(!layer_handler.handle_args(args, log)) {
        log.none() << "Required arguments missing for layer handler!";
        return -1;
    }

    auto init = SettingInitializerDefault();
    for ( auto storage : init.Storages() ) {
        if (!storage->handle_args(args, log)) {
            log.none() << "Required arguments missing for handler: " << storage->Alias();
            return -1;
        }
    }

    // Check if help flag was provided
    for (auto & [arg, consumed] : args) {
        if (arg == "-h" || arg == "--help") {
            return 0;
        }
    }

    // Check for non-consumed args
    for (auto & [arg, consumed] : args) {
        if (consumed == false) {
            log.none() << "Unhandled argument: " << arg;
            log.none() << "Use --help for usage.";
            return -1;
        }
    }

    log = Log("main");

    auto setting_logger = Log("setting");

    std::vector<SettingLayer*> layers = layer_handler.layers();
    for ( auto storage : init.Storages() ) {
        storage->registerLayers(layers);
    }

    auto handler = SettingHandler(init, setting_logger);

    auto dflt = std::make_shared<SettingReaderDefault>();
    handler.importFromReader(dflt, layer_handler.defaultLayer());

    log.notice() << "Setting initialization DONE. ";
    handler.printSettings();

    auto connection = getDbusConnection();
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
