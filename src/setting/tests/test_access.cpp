#include <gtest/gtest.h>
#include "test_stubs.h"

class SettingAccessTest : public SettingTest {};

TEST_F(SettingAccessTest, InvalidPermissions) {
    EXPECT_THROW(m_handler->Set({{"b124", true}}, &m_interface1, m_layer1), SettingAccessException);
}

TEST_F(SettingAccessTest, InvalidKey) {
    EXPECT_THROW(m_handler->Get("invalid-key", &m_interface1), SettingKeyException);
}

TEST_F(SettingAccessTest, NoValue) {
    EXPECT_THROW(m_handler->Get("s1", &m_interface1), SettingNoValueException);
}

TEST_F(SettingAccessTest, SettingDisabled) {
    EXPECT_THROW(m_handler->Get("b1", &m_interface1), SettingNoValueException);
    EXPECT_THROW(m_handler->Get("r1", &m_interface1), SettingDisabledException);

    m_handler->Set({{"b1", true}, {"r1", 4}}, &m_interface2, m_layer1);

    ASSERT_EQ(std::get<int>(m_handler->Get("r1", &m_interface1)), 4);

    m_handler->Set({{"b1", false}}, &m_interface2, m_layer1);

    EXPECT_THROW(m_handler->Get("r1", &m_interface1), SettingDisabledException);
}
