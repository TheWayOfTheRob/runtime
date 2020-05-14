// Copyright 2020 The TensorFlow Runtime Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//===- tracing_benchmark.cc -------------------------------------*- C++ -*-===//
//
// Benchmark measuring tracing overhead.
//
//===----------------------------------------------------------------------===//

#include <chrono>
#include <future>

#include "benchmark/benchmark.h"
#include "gtest/gtest.h"
#include "tfrt/cpp_tests/error_util.h"
#include "tfrt/support/logging.h"
#include "tfrt/support/string_util.h"
#include "tfrt/tracing/tracing.h"

namespace tfrt {
namespace tracing {
namespace {

class BenchmarkTracingSink : TracingSink {
 public:
  BenchmarkTracingSink() {
    RegisterTracingSink(this);
#ifndef TFRT_BM_DISABLE_TRACING_REQUEST
    EXPECT_TRUE(IsSuccess(RequestTracing(true)));
#endif
  }
  ~BenchmarkTracingSink() override {
    EXPECT_TRUE(IsSuccess(RequestTracing(false)));
    EXPECT_EQ(num_completed_, num_scopes_ + num_ranges_);
    TFRT_LOG(INFO) << "Recorded " << num_events_ << " events, " << num_scopes_
                   << " scopes, " << num_ranges_ << " ranges.";
  }
  Error RequestTracing(bool enable) override {
    TFRT_LOG(INFO) << "Tracing enable: " << enable;
    return Error::success();
  }
  void RecordTracingEvent(const char*, string_view) override { ++num_events_; }
  void RecordTracingEvent(const char*, const char*) override { ++num_events_; }
  void RecordTracingEvent(const char*, std::string&&) override {
    ++num_events_;
  }
  void PushTracingScope(const char*, string_view) override { ++num_scopes_; }
  void PushTracingScope(const char*, const char*) override { ++num_scopes_; }
  void PushTracingScope(const char*, std::string&&) override { ++num_scopes_; }
  void PopTracingScope() override { ++num_completed_; }

 private:
  uint64_t num_events_ = 0;
  uint64_t num_scopes_ = 0;
  uint64_t num_ranges_ = 0;
  uint64_t num_completed_ = 0;
};
BenchmarkTracingSink kTracingSink;

void BM_EmptyLoop(benchmark::State& state) {
  for (auto _ : state) {
  }
}
BENCHMARK(BM_EmptyLoop);

void BM_TracingEvents(benchmark::State& state) {
  for (auto _ : state) {
    RecordTracingEvent("event");
  }
}
BENCHMARK(BM_TracingEvents);

void BM_StrCatTracingEvents(benchmark::State& state) {
  for (auto _ : state) {
    RecordTracingEvent([&] { return StrCat("event", ""); });
  }
}
BENCHMARK(BM_StrCatTracingEvents);

void BM_TracingScopes(benchmark::State& state) {
  for (auto _ : state) {
    TracingScope("scope");
  }
}
BENCHMARK(BM_TracingScopes);

void BM_StrCatTracingScopes(benchmark::State& state) {
  for (auto _ : state) {
    TracingScope([&] { return StrCat("scope", ""); });
  }
}
BENCHMARK(BM_StrCatTracingScopes);

}  // namespace
}  // namespace tracing
}  // namespace tfrt
