#ifndef TEST_STUBS_H
#define TEST_STUBS_H

#include <gtest/gtest.h>
#include "setting.h"
#include "log.h"

class StubbedStorage : public ISettingStorage {
    public:
        StubbedStorage() : ISettingStorage("stubbed") {}; 

        std::map<std::string, std::pair<SettingLayer*, std::optional<std::string>>> GetSettings() { return {}; };
        const std::string LayerAlias(SettingLayer* layer) { return "stubbed"; };
        void Flush(SettingLayer* layer) { m_flushed = true; };

        bool hasBeenFlushed() { return m_flushed; };
        std::map<std::string, std::optional<std::string>> getCache() { return m_cache; };
    private:
        bool m_flushed{false};
};

namespace {
auto ruleStr = SettingRuleString();
auto ruleEnum = SettingRuleStringEnum({"YES","NO","MAYBE"});
auto ruleBool = SettingRuleBool();
auto ruleInt = SettingRuleInt();
auto ruleRange = SettingRuleRangedInt(0, 10);
}

class SettingInitializerTest : public ISettingInitializer {
    public:
        SettingInitializerTest (
                SettingInterface* if1,
                SettingInterface* if2,
                ISettingStorage* s1,
                ISettingStorage* s2) :
            ISettingInitializer({if1, if2}, {s1, s2}),
            m_if1(if1),
            m_if2(if2),
            m_s1(s1),
            m_s2(s2)
            {
            }

        std::vector<Setting> InitializeSettings() {
            return std::vector<Setting> {
                Setting("s1", &ruleStr,   m_s1, {m_if1, m_if2}, {m_if1, m_if2}),
                Setting("s2", &ruleStr,   m_s1, {m_if1},        {m_if1}),
                Setting("e1", &ruleEnum,  m_s1, {m_if1, m_if2}, {m_if2}),
                Setting("e2", &ruleEnum,  m_s1, {m_if1},        {m_if1, m_if2}),
                Setting("b1", &ruleBool,  m_s1, {m_if1},        {m_if2}),
                Setting("b2", &ruleBool,  m_s1, {m_if1, m_if2}, {m_if1, m_if2}),
                Setting("i1", &ruleInt,   m_s2, {m_if1, m_if2}, {m_if1, m_if2}),
                Setting("i2", &ruleInt,   m_s2, {m_if1, m_if2}, {m_if1, m_if2}),
                Setting("r1", &ruleRange, m_s2, {m_if1, m_if2}, {m_if1, m_if2}, "b1"),
                Setting("r2", &ruleRange, m_s2, {m_if1, m_if2}, {m_if1, m_if2}, "b2"),
            };
        }

    private:

        SettingInterface* m_if1;
        SettingInterface* m_if2;
        ISettingStorage* m_s1;
        ISettingStorage* m_s2;
};

class SettingTest : public testing::Test {
    protected:
        SettingTest() {
            auto init = SettingInitializerTest(&m_interface1, &m_interface2, &m_storage1, &m_storage2);
            auto setting_logger = Log();
            setting_logger.mute();
            m_handler = new SettingHandler(init, setting_logger);
        }

        ~SettingTest() {
            delete m_handler;
            delete m_layer1;
            delete m_layer2;
        }
        SettingInterface m_interface1{SettingInterface("Interface 1")};
        SettingInterface m_interface2{SettingInterface("Interface 2")};
        StubbedStorage m_storage1;
        StubbedStorage m_storage2;
        SettingLayer* m_layer1{new SettingLayer("Layer 1", 1)};
        SettingLayer* m_layer2{new SettingLayer("Layer 2", 2)};
        SettingHandler *m_handler;
};

#endif /* TEST_STUBS_H */
