#include <gtest/gtest.h>

#include "../include/utils.hpp"

TEST(ParseIntTest, ParsesValidNumbers) {
    int value = 0;
    EXPECT_TRUE(parseInt("0", value));
    EXPECT_EQ(value, 0);

    EXPECT_TRUE(parseInt("123", value));
    EXPECT_EQ(value, 123);
}

TEST(ParseIntTest, RejectsInvalidStrings) {
    int value = 0;
    EXPECT_FALSE(parseInt("", value));
    EXPECT_FALSE(parseInt("abc", value));
    EXPECT_FALSE(parseInt("12a", value));
    EXPECT_FALSE(parseInt("-40", value));
}

TEST(CstrEqTest, ComparesStringsCorrectly) {
    EXPECT_TRUE(cstrEq("test", "test"));
    EXPECT_TRUE(cstrEq("", ""));
    EXPECT_FALSE(cstrEq("test", "Test"));
    EXPECT_FALSE(cstrEq("foo", "foobar"));
}

TEST(HashStrTest, EqualStringsHaveSameHash) {
    EXPECT_EQ(hashStr("cmd"), hashStr("cmd"));
    EXPECT_EQ(hashStr("TYPE"), hashStr("TYPE"));
}

TEST(ParseCommandLineTest, ParsesValidArguments) {
    const char* argv[] = {
        "MyApp",
        "-a", "10.0.0.1",
        "-p", "8080",
        "-r", "Server",
        "-i", "7",
        "-L", "mylib"
    };
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    AppConfig cfg{};
    ASSERT_NO_THROW(parseCommandLine(argc, const_cast<char**>(argv), cfg));

    EXPECT_STREQ(cfg.address, "10.0.0.1");
    EXPECT_EQ(cfg.port, 8080);
    EXPECT_STREQ(cfg.role, "Server");
    EXPECT_EQ(cfg.id, 7);
    EXPECT_STREQ(cfg.library, "mylib");
}

TEST(ParseCommandLineTest, ThrowsOnInvalidPort) {
    const char* argv[] = {
        "MyApp",
        "-p", "not_number"
    };
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    AppConfig cfg{};
    EXPECT_THROW(
        parseCommandLine(argc, const_cast<char**>(argv), cfg),
        std::runtime_error
    );
}

TEST(ParseCommandLineTest, ThrowsOnInvalidId) {
    const char* argv[] = {
        "MyApp",
        "-i", "bad"
    };
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    AppConfig cfg{};
    EXPECT_THROW(
        parseCommandLine(argc, const_cast<char**>(argv), cfg),
        std::runtime_error
    );
}

TEST(ParseCommandLineTest, ThrowsOnUnknownFlag) {
    const char* argv[] = {
        "MyApp",
        "-x", "123"
    };
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    AppConfig cfg{};
    EXPECT_THROW(
        parseCommandLine(argc, const_cast<char**>(argv), cfg),
        std::runtime_error
    );
}

