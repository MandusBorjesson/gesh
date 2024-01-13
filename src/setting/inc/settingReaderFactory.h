#include <vector>
#include "settingSource.h"

class SettingReaderFactory {
public:
    SettingReaderFactory(std::vector<std::string> searchPaths) : m_paths(searchPaths) {};
    std::vector<ISettingReader*> getReaders();

protected:
    std::vector<std::string> m_paths;
};
