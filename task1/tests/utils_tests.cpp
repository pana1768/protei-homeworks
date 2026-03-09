#include <gtest/gtest.h>
#include "../include/AppSettings.h"

TEST(AppSettingsTest, ParsesValidArguments) {
    const char* argv[] = {
        "MyApp",
        "-a", "10.0.0.1",
        "-p", "8080",
        "-r", "Server",
        "-i", "7",
        "-L", "mylib"
    };
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    AppSettings cfg(argc, const_cast<char**>(argv));

    EXPECT_EQ(cfg.address(), "10.0.0.1");
    EXPECT_EQ(cfg.port(), 8080);
    EXPECT_EQ(cfg.role(), "Server");
    EXPECT_EQ(cfg.id(), 7);
    EXPECT_EQ(cfg.library(), "mylib");
}

TEST(AppSettingsTest, ThrowsOnInvalidPort) {
    const char* argv[] = {"MyApp", "-p", "not_number"};
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    EXPECT_THROW(
        AppSettings(argc, const_cast<char**>(argv)),
        std::runtime_error
    );
}

TEST(AppSettingsTest, ThrowsOnInvalidId) {
    const char* argv[] = {"MyApp", "-i", "bad"};
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    EXPECT_THROW(
        AppSettings(argc, const_cast<char**>(argv)),
        std::runtime_error
    );
}

TEST(AppSettingsTest, ThrowsOnUnknownFlag) {
    const char* argv[] = {"MyApp", "-x", "123"};
    int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));

    EXPECT_THROW(
        AppSettings(argc, const_cast<char**>(argv)),
        std::runtime_error
    );
}

TEST(AppSettingsTest, SetUserName) {
    const char* argv[] = {"MyApp"};
    int argc = 1;

    AppSettings cfg(argc, const_cast<char**>(argv));
    EXPECT_EQ(cfg.userName(), "user");

    cfg.setUserName("NewName");
    EXPECT_EQ(cfg.userName(), "NewName");
}
