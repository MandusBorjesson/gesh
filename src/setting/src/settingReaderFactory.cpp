#include "settingReaderFactory.h"
#include "settingReaderFile.h"
#include "settingTypes.h"
#include "log.h"

namespace Factory {
std::shared_ptr<ISettingReader> Reader(const std::string &path) {
    auto suffix = path.substr(path.find_last_of(".") + 1);
    auto log = Log("setting.reader");
    if ( suffix == "csv" ) {
        return std::make_shared<SettingReaderCsv>(path, log);
    }
    return nullptr;
}
} // namespace Factory
