#ifndef TEST_SUPPORT_H
#define TEST_SUPPORT_H

#include <iostream>
#include <sstream>
#include <stdexcept>

inline void checkTrue(bool condition, const char* expr, const char* file, int line) {
  if (condition) {
    return;
  }
  std::ostringstream oss;
  oss << file << ":" << line << ": CHECK failed: " << expr;
  throw std::runtime_error(oss.str());
}

template <typename A, typename B>
inline void checkEqual(const A& actual,
                       const B& expected,
                       const char* actualExpr,
                       const char* expectedExpr,
                       const char* file,
                       int line) {
  if (actual == expected) {
    return;
  }
  std::ostringstream oss;
  oss << file << ":" << line << ": CHECK_EQ failed: "
      << actualExpr << " != " << expectedExpr
      << " (actual=" << actual << ", expected=" << expected << ")";
  throw std::runtime_error(oss.str());
}

inline int runTest(void (*testFn)(), const char* name) {
  try {
    testFn();
    std::cout << "PASS " << name << "\n";
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << "FAIL " << name << ": " << ex.what() << "\n";
    return 1;
  } catch (...) {
    std::cerr << "FAIL " << name << ": unknown exception\n";
    return 1;
  }
}

#define CHECK(expr) checkTrue((expr), #expr, __FILE__, __LINE__)
#define CHECK_EQ(actual, expected) checkEqual((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define RUN_TEST(fn) do { failures += runTest(fn, #fn); } while (0)

#endif