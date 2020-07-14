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

// RUN: tfrt_translate -mlir-to-bef %s | bef_executor -devices=cpu | FileCheck %s --dump-input=fail

// CHECK: --- Running 'log_f32'
func @log_f32() -> !tfrt.chain {
  %ch0 = tfrt.new.chain
  %cpu = corert.get_op_handler %ch0 "cpu"

  %operand = corert.executeop(%cpu) "tf.Const"()
    {value = dense<[1.0, 2.5, 3.0, 4.5, 5.0]> : tensor<5xf32>, dtype = f32} : 1

  %cpu_handle_result = corert.executeop(%cpu) "tf.Log"(%operand) : 1

  // CHECK: DenseHostTensor dtype = F32, shape = [5]
  // CHECK-SAME: values = [0.000000e+00, 9.162908e-01, 1.098612e+00, 1.504077e+00, 1.609438e+00]
  %ch_print_cpu = corert.executeop.seq(%cpu, %ch0) "tfrt_test.print"(%cpu_handle_result) : 0
  tfrt.return %ch_print_cpu : !tfrt.chain
}

// CHECK: --- Running 'log1p_f32'
func @log1p_f32() -> !tfrt.chain {
  %ch0 = tfrt.new.chain
  %cpu = corert.get_op_handler %ch0 "cpu"

  %operand = corert.executeop(%cpu) "tf.Const"()
    {value = dense<[1.0, 2.5, 3.0, 4.5, 5.0]> : tensor<5xf32>, dtype = f32} : 1

  %cpu_handle_result = corert.executeop(%cpu) "tf.Log1p"(%operand) : 1

  // CHECK: DenseHostTensor dtype = F32, shape = [5]
  // CHECK-SAME: values = [6.931472e-01, 1.252763e+00, 1.386294e+00, 1.704748e+00, 1.791759e+00]
  %ch_print_cpu = corert.executeop.seq(%cpu, %ch0) "tfrt_test.print"(%cpu_handle_result) : 0
  tfrt.return %ch_print_cpu : !tfrt.chain
}
