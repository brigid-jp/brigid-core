// Copyright (c) 2019 <dev@brigid.jp>
// This software is released under the MIT License.
// https://opensource.org/licenses/mit-license.php

#ifndef BRIGID_TEST_HPP
#define BRIGID_TEST_HPP

#include <sstream>
#include <stdexcept>
#include <string>

namespace brigid {
  template <class T>
  T check_impl(T result, const char* file, int line, const char* function) {
    if (!result) {
      std::ostringstream out;
      out << "check failed at " << file << ":" << line << " in " << function;
      throw std::runtime_error(out.str());
    }
    return result;
  }

  struct test_case {
  public:
    test_case(const std::string&, std::function<void()>);
  };

  int run_test_suite();
}

#define BRIGID_CHECK(expression) brigid::check_impl((expression), __FILE__, __LINE__, __func__)

#endif
