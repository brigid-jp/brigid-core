// Copyright (c) 2021 <dev@brigid.jp>
// This software is released under the MIT License.
// https://opensource.org/licenses/mit-license.php

#include <brigid/error.hpp>
#include "ubench.hpp"

#define NOMINMAX
#include <windows.h>

#include <iostream>

namespace brigid {
  namespace ubench {
    void check_platform(std::ostream& out) {
      LARGE_INTEGER frequency;
      if (!QueryPerformanceFrequency(&frequency)) {
        throw BRIGID_RUNTIME_ERROR("cannot QueryPerformanceFrequency");
      }

      int64_t count;
      int64_t duration;
      LARGE_INTEGER t;
      LARGE_INTEGER u;

      if (!QueryPerformanceCounter(&t)) {
        throw BRIGID_RUNTIME_ERROR("cannot QueryPerformanceCounter");
      }
      for (count = 1; ; ++count) {
        if (!QueryPerformanceCounter(&u)) {
          throw BRIGID_RUNTIME_ERROR("cannot QueryPerformanceCounter");
        }
        duration = u.QuadPart - t.QuadPart;
        if (duration > 0) {
          break;
        }
      }
      t = u;
      for (count = 1; ; ++count) {
        if (!QueryPerformanceCounter(&u)) {
          throw BRIGID_RUNTIME_ERROR("cannot QueryPerformanceCounter");
        }
        duration = u.QuadPart - t.QuadPart;
        if (duration > 0) {
          break;
        }
      }

      out
        << "QueryPerformanceCounter\n"
        << "  frequency: " << frequency.QuadPart << "\n";
        << "  count: " << count << "\n"
        << "  duration: " << duration * 1000000000 / frequency.QuadPart << "\n"
        << "  t: " << t * 1000000000 / frequency.QuadPart << "\n"
        << "  u: " << u * 1000000000 / frequency.QuadPart << "\n";
    }
  }
}
