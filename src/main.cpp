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
   ERROR << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here
   // terminate program

   exit(signum);
}

void print_help() {
    INFO << std::endl;
    INFO << "Usage:" << std::endl;
    INFO << " gesh [options]" << std::endl;
    INFO << std::endl;
    INFO << "Options:" << std::endl;
    INFO << " -d, --directory <dir>    search directory for fragments" << std::endl;
    INFO << std::endl;
    INFO << " -h, --help               display this help" << std::endl;
    INFO << " -v, --version            display version" << std::endl;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);

    INFO << "Gesh - Good enough setting handler" << std::endl;
    INFO << "Git SHA: " << BUILD_VERSION << std::endl;
    INFO << "Build: " << BUILD_DATE << std::endl;

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
                ERROR << "Missing argument after " << arg << std::endl;
                print_help();
                return 1;
            }
        } else {
            ERROR << "Unknown argument: " << arg << std::endl;
            print_help();
            return 1;
        }
    }

    auto init = SettingInitializerHardcoded();
    auto srf = SettingReaderFactory(searchPaths);
    std::vector<ISettingReader*> readers = srf.getReaders();

    DEBUG << "Initializing settings... " << std::endl;
    auto handler = SettingHandler(init, readers);
    DEBUG << "Setting initialization DONE. " << std::endl;
    for ( auto const& [key, val] : handler.GetAll(nullptr) ) {
        DEBUG << val << std::endl;
    }

    auto connection = sdbus::createSessionBusConnection();
    connection->requestName(DBUS_SERVICE);
    connection->enterEventLoopAsync();
    DEBUG << "D-Bus service name aquired. " << std::endl;

    auto manager = std::make_unique<ManagerAdaptor>(*connection, DBUS_PATH);
    std::vector<std::shared_ptr<DbusAdaptor>> dbusManagers;

    for ( auto & iface : init.Interfaces() ) {
        auto manager = std::make_shared<DbusAdaptor>(*connection, &handler, iface);
        iface->RegisterManager(manager);
        dbusManagers.push_back(manager);
    }
    DEBUG << "D-Bus objects registered. " << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    connection->releaseName(DBUS_SERVICE);
    connection->leaveEventLoop();
    return 0;
}
