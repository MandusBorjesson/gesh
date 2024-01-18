#include "dbus.h"
#include "setting.h"
#include "settingInitializerHardcoded.h"
#include "settingReaderFactory.h"
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
    auto t_keeper = TimeKeeper();

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
