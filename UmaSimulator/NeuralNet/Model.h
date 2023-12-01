#pragma once
#include "NNInput.h"
#include "../config.h"

#ifdef USE_BACKEND_LIBTORCH
#include <torch/torch.h>
#include <torch/script.h>
#endif


//��-1e7��ʾ���Ϸ���������������ѵ��ʱֱ�Ӻ��Ե���Щֵ
struct ModelOutputPolicyV1
{
  float trainingPolicy[10];
  float buy50pPolicy[5];//����ȷ��ѵ���������£����Ӧѵ����+50%�ĸ���
  float buyPt10;//����pt+10
  float buyVital20;//������������-20%

  //ͬʱ�����������ĸ��ʡ�
  //�������Ŀ���ǣ�������50%����pt+10�ķ�����࣬��ͬʱ��ᵼ���޷�����debuff����ȥ�����ţ���ʱ���������������ֵӦ����50%���������ǡ�ö�����50%������������Ϊͬʱ��������
  //��˼������������Ƿ�Ӧ��������
  float buyTwoUpgrades;
};
static_assert(sizeof(ModelOutputPolicyV1) == sizeof(float) * NNOUTPUT_CHANNELS_POLICY_V1, "NNOUTPUT_CHANNELS_POLICY_V1����");


struct ModelOutputValueV1
{
  float scoreMean;//score��ƽ��ֵ
  float scoreStdev;//score�ı�׼��
  //float winRate;//score>=target�ĸ���
  float value;//���Ǽ�����֮��Ĵ��
  //float extract(int i);
};
static_assert(sizeof(ModelOutputValueV1) == sizeof(float) * NNOUTPUT_CHANNELS_VALUE_V1, "NNOUTPUT_CHANNELS_VALUE_V1����");

struct ModelOutputV1
{
  ModelOutputValueV1 value;
  ModelOutputPolicyV1 policy;
};
static_assert(sizeof(ModelOutputV1) == sizeof(float) * NNOUTPUT_CHANNELS_V1,"NNOUTPUT_CHANNELS_V1����");

class Model
{
#ifdef USE_BACKEND_LIBTORCH
  torch::jit::script::Module model;
#endif

public:
  //static lock;//���е�evaluator����һ��lock
  Model(std::string path, int batchsize);
  void evaluate(float* inputBuf, float* outputBuf, int gameNum);//����gamesBuf��gameNum����Ϸ������������outputBuf
  static void detect(Model* md);
};