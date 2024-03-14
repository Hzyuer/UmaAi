#pragma once
struct SearchParam
{
  int searchSingleMax; //��һѡ��ﵽ���ٴ�������ֹͣ���ؿ���
  int searchTotalMax; //����ѡ���ܹ��ﵽ���ٴ�������ֹͣ���ؿ��壬0Ϊ����
  int searchGroupSize; //����ʱÿ�η�����ټ�����������128����ҪС��16*�߳�����̫С������ܴ���⿪����ÿsearchGroupSize������Ҫ����O(200000)�Σ�
  double searchCpuct; //cpuct������ԽС����Խ����
  int maxDepth; //���ؿ������
  double maxRadicalFactor; //��󼤽��ȡ������ʱ�𽥽���

  SearchParam();
  SearchParam(int searchSingleMax, double maxRadicalFactor);
  SearchParam(
    int searchSingleMax,
    int searchTotalMax,
    int searchGroupSize,
    double searchCpuct,
    int maxDepth,
    double maxRadicalFactor
  );
};