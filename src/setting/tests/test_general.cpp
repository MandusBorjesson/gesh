#include <gtest/gtest.h>
#include "test_stubs.h"

TEST_F(SettingTest, SetValue) {
    EXPECT_THROW(m_handler->Get("s1", &m_interface1), SettingNoValueException);
    EXPECT_THROW(m_handler->Get("b2", &m_interface1), SettingNoValueException);

    m_handler->Set({{"s1", "test"}, {"b2", true}}, &m_interface1, m_layer1);

    ASSERT_EQ(std::get<std::string>(m_handler->Get("s1", &m_interface1)), "test");
    ASSERT_EQ(std::get<bool>(m_handler->Get("b2", &m_interface1)), true);
}

TEST_F(SettingTest, ChangesPropagateToStorage) {
    std::map<std::string, std::optional<std::string>> empty_storage {};
    std::map<std::string, std::optional<std::string>> expected_storage1 = {{"s1", "test"}, {"b2", "True"}};
    std::map<std::string, std::optional<std::string>> expected_storage2 = {{"r2", "5"}};

    ASSERT_EQ(m_storage1.getCache(), empty_storage);
    ASSERT_EQ(m_storage2.getCache(), empty_storage);
    ASSERT_FALSE(m_storage1.hasBeenFlushed());
    ASSERT_FALSE(m_storage2.hasBeenFlushed());

    m_handler->Set({{"s1", "test"}, {"r2", 5}, {"b2", true}}, &m_interface1, m_layer1);

    ASSERT_EQ(m_storage1.getCache(), expected_storage1);
    ASSERT_EQ(m_storage2.getCache(), expected_storage2);
    ASSERT_TRUE(m_storage1.hasBeenFlushed());
    ASSERT_TRUE(m_storage2.hasBeenFlushed());
}
