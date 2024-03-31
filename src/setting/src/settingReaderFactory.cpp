#include "settingReaderFactory.h"
#include "settingReaderFile.h"
#include <algorithm>
#include <filesystem>

static int priority(const std::string &s) {
    auto filename = s.substr(s.find_last_of("/") + 1);

    auto offset = filename.find_first_of("-");
    if (offset == filename.npos ) {
        return 0;
    }
    auto prefix = filename.substr(0, offset);

    int prio = 0;

    try {
        std::size_t pos{};
        prio = std::stoi(prefix, &pos);
    } catch (std::invalid_argument const& ex) {
        return 0;
    } catch (std::out_of_range const& ex) {
        return 0;
    }

    return prio;
}

struct fragment_priority
{
    inline bool operator() (const std::string& s1, const std::string& s2)
    {
        return (priority(s1) < priority(s2));
    }
};

std::optional<std::shared_ptr<ISettingReader>> SettingReaderFactory::getReader(const std::string &path) {
    auto suffix = path.substr(path.find_last_of(".") + 1);
    if ( suffix == "csv" ) {
        return std::make_shared<SettingReaderCsv> (path, log);
    }
    log.error() << "Cannot determine reader for file type '" << suffix << "' (" << path << ")";
    return std::nullopt;
}

std::vector<std::shared_ptr<ISettingReader>> SettingReaderFactory::getReaders() {
    log.info() << "Finding setting readers...";
    log.debug() << "Search paths:";
    for ( auto const &p : m_paths ) {
        log.debug() << "   " << p;
    }

    std::vector<std::string> fragments;
    for (auto const &path : m_paths) {
        for (const auto & file : std::filesystem::directory_iterator(path)) {
            if (!file.is_regular_file()) {
                continue;
            }
            std::string s = file.path();
            auto suffix = s.substr(s.find_last_of(".") + 1);
            if (std::find(supportedFormats.begin(), supportedFormats.end(), suffix) == supportedFormats.end()) {
                continue;
            }

            fragments.push_back(file.path());
        }
    }
    std::sort(fragments.begin(), fragments.end(), fragment_priority());

    std::vector<std::shared_ptr<ISettingReader>> out;
    for (auto const & fragment : fragments ) {
        auto reader = getReader(fragment);
        if (reader.has_value()) {
            out.push_back(reader.value());
        }
    }
    return out;
}
