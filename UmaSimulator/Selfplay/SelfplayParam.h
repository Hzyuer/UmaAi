#pragma once
#include <vector>
#include <string>
struct SelfplayParam
{
  int threadNum = 24;
  int threadNumInner = 1;
  int maxSampleNum = 1000 * 1024 * 16;
  int batchsize = 4;
  //std::string modelPath = "model_traced.pt";
  std::string modelPath = "";
  std::string exportDataDir = "./selfplay/0/";
  int sampleNumEachFile = 1024;

  //log(searchN)~��̬�ֲ�(searchN_logmean,searchN_logstdev)
  double searchN_logmean = 6.93;//1024
  double searchN_logstdev = 0.0;
  int searchN_min = 128;
  int searchN_max = 65536;

  //radicalFactor=radicalFactor_scale * (1/pow(rand(),radicalFactor_pow) - 1)
  double radicalFactor_scale = 2.0;
  double radicalFactor_pow = 1.0;
  double radicalFactor_max = 50;

  //maxDepth_fullProb��������������ֽ�����������log(maxDepth)~��̬�ֲ�(logmean,logstdev)
  double maxDepth_logmean = 2.3;
  double maxDepth_logstdev = 0.7;
  int maxDepth_min = 5;
  double maxDepth_fullProb = 1.0;

  //�俨�������ʽ
  //0 �̶�����̶���̶�����
  //1 ������̶���ssr���ˣ�����������ѡ����Խǿ�Ŀ�����Խ��
  //2 ��1�Ļ����ϣ������ӳɣ����Ĳ���Ҳ�Ŷ������������
  int cardRandType = 2;
  
};