#include <vector>
#include "settingSource.h"
#include "log.h"

class SettingReaderFactory {
    std::vector<std::string> supportedFormats = {"csv"};
public:
    SettingReaderFactory(std::vector<std::string> searchPaths, Log &logger) : m_paths(searchPaths), log(logger.getChild("factory")) {};
    std::vector<ISettingReader*> getReaders();

protected:
    std::vector<std::string> m_paths;
    Log log;
};
