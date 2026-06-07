#pragma once
// Tiny header-only test runner shared between the small CPU test suites.
// We don't want to introduce a third-party dependency (gtest/Catch2) for the
// baseline build, so a few macros are sufficient for these focused checks.
#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace tests {

struct TestCase {
    std::string         name;
    std::function<void()> fn;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> r;
    return r;
}

inline int& fail_counter() { static int f = 0; return f; }

inline void register_test(const std::string& name, std::function<void()> fn) {
    registry().push_back({name, std::move(fn)});
}

inline int run_all() {
    int passed = 0, failed = 0;
    for (auto& t : registry()) {
        int before = fail_counter();
        std::printf("[ RUN  ] %s\n", t.name.c_str());
        try {
            t.fn();
        } catch (const std::exception& e) {
            ++fail_counter();
            std::fprintf(stderr, "  exception: %s\n", e.what());
        } catch (...) {
            ++fail_counter();
            std::fprintf(stderr, "  unknown exception thrown\n");
        }
        if (fail_counter() == before) {
            ++passed;
            std::printf("[  OK  ] %s\n", t.name.c_str());
        } else {
            ++failed;
            std::printf("[ FAIL ] %s\n", t.name.c_str());
        }
    }
    std::printf("%d passed, %d failed (%zu total)\n",
                passed, failed, registry().size());
    return failed == 0 ? 0 : 1;
}

}  // namespace tests

#define TEST(name)                                                           \
    static void name##_impl();                                               \
    static const int name##_registered = (                                   \
        ::tests::register_test(#name, &name##_impl), 0);                     \
    static void name##_impl()

#define EXPECT_TRUE(cond)                                                    \
    do {                                                                     \
        if (!(cond)) {                                                       \
            ++::tests::fail_counter();                                       \
            std::fprintf(stderr, "  %s:%d EXPECT_TRUE(%s) failed\n",         \
                         __FILE__, __LINE__, #cond);                         \
        }                                                                    \
    } while (0)

#define EXPECT_NEAR(a, b, eps)                                               \
    do {                                                                     \
        double _a = double(a), _b = double(b), _e = double(eps);             \
        if (std::fabs(_a - _b) > _e) {                                       \
            ++::tests::fail_counter();                                       \
            std::fprintf(stderr,                                             \
                         "  %s:%d EXPECT_NEAR(%s, %s, %s) "                  \
                         "got %g vs %g (delta %g, eps %g)\n",                \
                         __FILE__, __LINE__, #a, #b, #eps,                   \
                         _a, _b, _a - _b, _e);                               \
        }                                                                    \
    } while (0)
