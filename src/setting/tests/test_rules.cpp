#include <gtest/gtest.h>
#include "test_stubs.h"

class SettingRuleTest : public SettingTest {};

TEST_F(SettingRuleTest, Int) {
    EXPECT_NO_THROW(m_handler->Set({{"i1", 25}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"i1", "123"}}, &m_interface2, m_layer1));
    EXPECT_THROW(m_handler->Set({{"i1", "hej"}}, &m_interface2, m_layer1), SettingRuleException);
}

TEST_F(SettingRuleTest, RangedInt) {
    EXPECT_NO_THROW(m_handler->Set({{"r1", 5}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"r1", "5"}}, &m_interface2, m_layer1));
    EXPECT_THROW(m_handler->Set({{"r1", -1}}, &m_interface2, m_layer1), SettingRuleException);
    EXPECT_THROW(m_handler->Set({{"r1", 25}}, &m_interface2, m_layer1), SettingRuleException);
}

TEST_F(SettingRuleTest, String) {
    EXPECT_NO_THROW(m_handler->Set({{"s1", "25"}}, &m_interface2, m_layer1));
    EXPECT_THROW(m_handler->Set({{"s1", 25}}, &m_interface2, m_layer1), SettingRuleException);
}

TEST_F(SettingRuleTest, StringEnum) {
    EXPECT_NO_THROW(m_handler->Set({{"e1", "YES"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"e1", "NO"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"e1", "MAYBE"}}, &m_interface2, m_layer1));
    EXPECT_THROW(m_handler->Set({{"e1", "YeS"}}, &m_interface2, m_layer1), SettingRuleException);
    EXPECT_THROW(m_handler->Set({{"e1", "PERHAPS"}}, &m_interface2, m_layer1), SettingRuleException);
}

TEST_F(SettingRuleTest, Bool) {
    EXPECT_NO_THROW(m_handler->Set({{"b1", "0"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"b1", "1"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"b1", "true"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"b1", "false"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"b1", "True"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"b1", "FaLsE"}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"b1", true}}, &m_interface2, m_layer1));
    EXPECT_NO_THROW(m_handler->Set({{"b1", false}}, &m_interface2, m_layer1));
    EXPECT_THROW(m_handler->Set({{"b1", 25}}, &m_interface2, m_layer1), SettingRuleException);
}
