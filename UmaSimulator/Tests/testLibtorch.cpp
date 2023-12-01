#include "tests.h"
#ifdef UMAAI_TESTLIBTORCH
//if you switch debug/release. you should also change PATH and restart visual studio
#include <torch/script.h>
#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "../External/cnpy/cnpy.h"
const bool useCuda = true;
void main_testLibtorch() {

  // ����ģ��
  torch::jit::script::Module model;
  try {
    model = torch::jit::load("../training/example/model_traced.pt");
  }
  catch (const c10::Error& e) {
    std::cerr << "ģ�ͼ���ʧ�ܡ�" << e.what() << std::endl;
    return;
  }
  if (useCuda) {
    // ��ģ���ƶ���GPU
    model.to(torch::kCUDA);
  }
  else
  {
    model.to(torch::kCPU);
  }

  // ׼����������
  std::string filename = "../training/example/256.npz";
  cnpy::npz_t my_npz = cnpy::npz_load(filename);
  cnpy::NpyArray arr = my_npz["x"];

  // ������ת��Ϊ�ʵ��ĸ�ʽ����ȡǰN��
  float* loaded_data = arr.data<float>();
  std::vector<float> data;
  int num_rows = arr.shape[0];
  int num_cols = arr.shape[1];

  for (size_t i = 0; i < 8; ++i) { // ֻȡǰN��
    for (size_t j = 0; j < num_cols; ++j) {
      data.push_back(loaded_data[i * num_cols + j]);
    }
  }
  torch::Tensor input = torch::from_blob(data.data(), { 8, num_cols });
  if (useCuda) {
    input = input.to(at::kCUDA);
  }
  // ����ģ��
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(input);
  at::Tensor output = model.forward(inputs).toTensor().to(at::kCPU);

  // ת�����Ϊ����������
  float* output_data = output.data_ptr<float>();

  // ���ǰ��������Խ�����֤
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 5; ++j) {
      std::cout << output_data[i * 21 + j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "Benchmark:" << std::endl;
  std::vector<int> toTestBatchsize = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768 };
  for (int bsi = 0; bsi < toTestBatchsize.size(); bsi++)
  {
    int batchSize = toTestBatchsize[bsi];
    int epoches = 100;
    auto start = std::chrono::high_resolution_clock::now();
    for (int epoch = 0; epoch < epoches; epoch++)
    {

      torch::Tensor input = torch::randn({ batchSize, 2341 });
      if (useCuda) {
        input = input.to(at::kCUDA);
      }

      // ����ģ��
      std::vector<torch::jit::IValue> inputs;
      inputs.push_back(input);
      at::Tensor output = model.forward(inputs).toTensor().to(at::kCPU);

      // ת�����Ϊ����������
      float* output_data = output.data_ptr<float>();

    }
    auto stop = std::chrono::high_resolution_clock::now();

    // �������ʱ��
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    double duration_s = 0.000001 * duration.count();
    // �������ʱ��
    std::cout << "Time: " << duration_s << " s, batchsize=" << batchSize << " batchnum=" << epoches << " speed=" << batchSize * epoches / duration_s << std::endl;
  }
  return;
}
#else
void main_testLibtorch() {
}
#endif