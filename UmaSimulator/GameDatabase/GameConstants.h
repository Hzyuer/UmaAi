#pragma once
#include <string>
#include <unordered_map>
#include "../config.h"

const int TOTAL_TURN = 67;
static const int SHENTUAN_ID = 1;//���ſ���id

class GameConstants
{
public:
  //��Ϸ��ʼ��
  static const int TrainingBasicValue[5][6][7]; //TrainingBasicValue[�ڼ���ѵ��][LV��][����������pt����]����Ů����LV6
  static const int BasicFiveStatusLimit[5];//��ʼ���ޣ�1200���Ϸ���

  //������Ϸ����
  static const int NormalRaceFiveStatusBonus;//����������Լӳ�=3
  static const int NormalRacePtBonus;//�������pt�ӳ�
  static const int EventStrengthDefault;//ÿ�غ���40%�������һ�������Լ�pt +EventStrengthDefault��ģ��֧Ԯ���¼�

  //�籾�����
  static const double FriendUnlockOutgoingProbEveryTurnLowFriendship;//ÿ�غϽ�������ĸ��ʣ��С��60
  static const double FriendUnlockOutgoingProbEveryTurnHighFriendship;//ÿ�غϽ�������ĸ��ʣ����ڵ���60
  //static const double FriendEventProb;//�����¼�����//����0.4д���ڶ�Ӧ��������
  static const double ZuoyueVitalBonusSSR[5];//����SSR���Ļظ�������������1.8��
  static const double ZuoyueVitalBonusR[5];//����R���Ļظ�������
  static const double ZuoyueStatusBonusSSR[5];//����SSR�����¼�Ч������������1.2��
  static const double ZuoyueStatusBonusR[5];//����R�����¼�Ч������
  static const int LArcLinkCharas[7];// Link��ɫ
  static const int LArcLinkEffect[7]; // ��Ӧ������Buff
  

  //�籾���
  static const int UpgradeId50pEachTrain[5];//+50%�ֱ��Ӧ�ڼ�������
  static const int LArcTrainBonusEvery5Percent[41];//�ڴ��ȶ�Ӧ��ѵ���ӳɣ�ÿ5%һ��

  static const bool LArcIsRace[TOTAL_TURN];//�Ƿ�Ϊռ�ûغϵ�����
  static const int LArcSupportPtGainEveryTurn[TOTAL_TURN];//ÿ�غϹ̶���supportPt����
  static const int LArcUpgradesCostLv2[10];//lv2Ҫ��������pt
  static const int LArcUpgradesCostLv3[8];//lv3Ҫ��������pt

  static const std::string LArcSSBuffNames[13];//ss��õ���Ŀ�����ƣ����ִ���ɫ


  //����
  static const int FiveStatusFinalScore[1200+800*2+1];//��ͬ���Զ�Ӧ������
  static const double ScorePtRate;//Ϊ�˷��㣬ֱ����Ϊÿ1pt��Ӧ���ٷ֡�
  static const double ScorePtRateQieZhe;//Ϊ�˷��㣬ֱ����Ϊÿ1pt��Ӧ���ٷ֡�����


};