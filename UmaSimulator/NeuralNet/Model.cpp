#include "Model.h"

Model::Model(std::string path, int batchsize)
{
#ifdef USE_BACKEND_LIBTORCH
  model = torch::jit::load(path);
  
  if (LIBTORCH_USE_GPU) {
    // ��ģ���ƶ���GPU
    model.to(torch::kCUDA);
  }
  else
  {
    model.to(torch::kCPU);
  }
#else
  assert(false);
#endif
}

void Model::evaluate(float* inputBuf, float* outputBuf, int gameNum)
{
#ifdef USE_BACKEND_LIBTORCH
  torch::Tensor input = torch::from_blob(inputBuf, { gameNum, NNINPUT_CHANNELS_V1 });
  if (LIBTORCH_USE_GPU) {
    input = input.to(at::kCUDA);
  }
  // ����ģ��
  at::Tensor output = model.forward({ input }).toTensor().to(at::kCPU);

  // ת�����Ϊ����������
  float* output_data = output.data_ptr<float>();
  memcpy(outputBuf, output_data, gameNum * NNOUTPUT_CHANNELS_V1 * sizeof(float));
#else
  assert(false);
#endif
}
