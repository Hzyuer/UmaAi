#pragma once
#include <cstdint>
#include <random>
#include <string>

enum TrainActionTypeEnum :int16_t
{
  TRA_speed = 0,
  TRA_stamina,
  TRA_power,
  TRA_guts,
  TRA_wiz,
  TRA_rest,
  TRA_outgoing,
  TRA_race,
  TRA_redistributeCardsForTest = -1 //ʹ��������ʱ��˵��ҪrandomDistributeCards�����ڲ���ai��������Search::searchSingleActionThread��ʹ��
};
enum XiangtanTypeEnum :int16_t
{
  //b=blue,r=red,y=yellow, XT_ab means a to b, XT_a means all to a
  XT_none = 0,
  XT_br,
  XT_by,
  XT_rb,
  XT_ry,
  XT_yb,
  XT_yr,
  XT_b,
  XT_r,
  XT_y
};
struct Action //һ���غϵĲ���
{
  static const int MAX_ACTION_TYPE = 10 * 5 + 3;//10����̸*5��ѵ��+�����Ϣ����
  static const int XiangtanFromColor[10];
  static const int XiangtanToColor[10];
  static const int XiangtanNumCost[10];
  static const std::string trainingName[8];
  static const std::string xiangtanName[10];
  static const Action Action_RedistributeCardsForTest;

  int16_t train;//01234���������ǣ�5�����6��Ϣ��7���� 
  //ע��������������������û������ͨ��������ṩѡ��
  
  int16_t xiangtanType;//��̸��10�ַ�ʽ�������ǲ�̸��6�ֵ�����̸��3��������̸

  int toInt() const;
  std::string toString() const;
  static Action intToAction(int i);
};