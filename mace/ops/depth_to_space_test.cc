//
// Copyright (c) 2017 XiaoMi All rights reserved.
//

#include "mace/core/operator.h"
#include "mace/ops/ops_test_util.h"

namespace mace {
namespace ops {
namespace test {

class DepthToSpaceOpTest : public OpsTestBase {};

TEST_F(DepthToSpaceOpTest, C8G4_CPU) {
  // Construct graph
  OpsTestNet net;
  OpDefBuilder("DepthToSpace", "DepthToSpaceTest")
      .Input("Input")
      .Output("Output")
      .AddIntArg("block_size", 2)
      .Finalize(net.NewOperatorDef());

  // Add input data
  net.AddInputFromArray<DeviceType::CPU, float>(
      "Input", {1, 2, 2, 4},
      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});

  // Run
  net.RunOp();

  // Check
  auto expected = CreateTensor<float>(
      {1, 4, 4, 1}, {1, 2, 5, 6, 3, 4, 7, 8, 9, 10, 13, 14, 11, 12, 15, 16});

  ExpectTensorNear<float>(*expected, *net.GetOutput("Output"), 0.001);
}

TEST_F(DepthToSpaceOpTest, C16G4_OPENCL) {
  // Construct graph
  OpsTestNet net;

  // Add input data
  net.AddInputFromArray<DeviceType::OPENCL, float>(
      "Input", {1, 1, 2, 16},
      {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
       16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31});
  BufferToImage<DeviceType::OPENCL, float>(&net, "Input", "InputImage",
                                           kernels::BufferType::IN_OUT_CHANNEL);

  OpDefBuilder("DepthToSpace", "DepthToSpaceTest")
      .Input("InputImage")
      .Output("OutputImage")
      .AddIntArg("block_size", 1)
      .Finalize(net.NewOperatorDef());

  // Run
  net.RunOp(DeviceType::OPENCL);

  // Transfer output
  ImageToBuffer<DeviceType::OPENCL, float>(&net, "OutputImage", "Output",
                                           kernels::BufferType::IN_OUT_CHANNEL);

  // Check
  auto expected = CreateTensor<float>(
      {1, 1, 2, 16},
      {0,  4,  8,  12, 1,  5,  9,  13, 2,  6,  10, 14, 3,  7,  11, 15,
       16, 20, 24, 28, 17, 21, 25, 29, 18, 22, 26, 30, 19, 23, 27, 31});

  ExpectTensorNear<float>(*expected, *net.GetOutput("Output"), 0.001);
}

}  // namespace test
}  // namespace ops
}  // namespace mace
