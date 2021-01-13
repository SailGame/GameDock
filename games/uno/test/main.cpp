#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main(int argc, char **argv) {
    spdlog::set_default_logger(spdlog::basic_logger_mt("UnoTest", "test.log"));
    spdlog::set_level(spdlog::level::info);
    spdlog::default_logger()->flush_on(spdlog::level::info);
    spdlog::info("hello, uno client!");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}