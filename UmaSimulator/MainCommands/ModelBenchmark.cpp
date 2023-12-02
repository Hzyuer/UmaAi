#include "../config.h"
#ifdef USE_BACKEND_LIBTORCH
#include <torch/script.h>
#include <torch/torch.h>
#endif
#include <iostream>
#include <vector>
#include <chrono>
#include "MainCommands.h"
using namespace std;
void main_modelBenchmark() {


  std::string modelpath = "../training/example/model_traced1.pt";
  const int threadNum = 4;
  const int batchSize = 1024;
  int64_t N = 1000000;

  int batchNumEveryThread = 1 + (N - 1) / (batchSize * threadNum);
  N = (batchSize * threadNum) * batchNumEveryThread;


  cout << "N=" << N << " thread=" << threadNum << " batchsize=" << batchSize << " batchNumEveryThread=" << batchNumEveryThread << endl;


#ifdef USE_BACKEND_LIBTORCH
  // ����ģ��
  torch::jit::script::Module model;
  try {
    model = torch::jit::load(modelpath);
  }
  catch (const c10::Error& e) {
    std::cerr << "ģ�ͼ���ʧ�ܡ�" << e.what() << std::endl;
    return;
  }
  if (LIBTORCH_USE_GPU) {
    // ��ģ���ƶ���GPU
    model.to(torch::kCUDA);
  }
  else
  {
    model.to(torch::kCPU);
  }

  std::cout << "Benchmark:" << std::endl;
  auto start = std::chrono::high_resolution_clock::now();

  auto modelthread = [&model](int batchNumEveryThread)
    {
      int nc = 357 + 20 * (72 + 74);
      vector<float> data(batchSize * nc);
      for (int b = 0; b < batchNumEveryThread; b++)
      {

        torch::Tensor input = torch::from_blob(data.data(), { batchSize, nc });
        if (LIBTORCH_USE_GPU) {
          input = input.to(at::kCUDA);
        }

        // ����ģ��
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(input);
        at::Tensor output = model.forward(inputs).toTensor().to(at::kCPU);

        // ת�����Ϊ����������
        float* output_data = output.data_ptr<float>();

      }
    };

  std::vector<std::thread> threads;
  for (int i = 0; i < threadNum; i++)
    threads.emplace_back(modelthread, batchNumEveryThread);

  // �ȴ������߳����
  for (auto& th : threads) {
    th.join();
  }

  auto stop = std::chrono::high_resolution_clock::now();

  // �������ʱ��
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  double duration_s = 0.000001 * duration.count();
  // �������ʱ��
  std::cout << "Time: " << duration_s << " s, speed=" << N / duration_s << std::endl;
#endif
  
  return;
}