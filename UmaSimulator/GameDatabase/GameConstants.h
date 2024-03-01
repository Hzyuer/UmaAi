#pragma once
#include <string>
#include <unordered_map>
#include "../config.h"

const int TOTAL_TURN = 78;

class GameConstants
{
public:
  static const int TrainingBasicValue[3][5][5][7]; //TrainingBasicValue[��ɫ][�ڼ���ѵ��][LV��][����������pt����]
  static const int BasicFiveStatusLimit[5];//��ʼ���ޣ�1200���Ϸ���

  //������Ϸ����
  static const int NormalRaceFiveStatusBonus;//����������Լӳ�=3�������������⴦���Ҷ�˹�ȣ�
  static const int NormalRacePtBonus;//�������pt�ӳ�
  static const double EventProb;//ÿ�غ���EventProb�������һ�������Լ�pt +EventStrengthDefault��ģ��֧Ԯ���¼�
  static const int EventStrengthDefault;

  //�籾�����
  static const double FriendUnlockOutgoingProbEveryTurnLowFriendship;//ÿ�غϽ�������ĸ��ʣ��С��60
  static const double FriendUnlockOutgoingProbEveryTurnHighFriendship;//ÿ�غϽ�������ĸ��ʣ����ڵ���60
  //static const double FriendEventProb;//�����¼�����//����0.4д���ڶ�Ӧ��������
  static const double FriendVitalBonusSSR[5];//����SSR���Ļظ�������������1.6��
  static const double FriendVitalBonusR[5];//����R���Ļظ�������
  static const double FriendStatusBonusSSR[5];//����SSR�����¼�Ч������������1.25��
  static const double FriendStatusBonusR[5];//����R�����¼�Ч������
  

  //�籾���
  static const int UAF_LinkCharas[6];// Link��ɫ
  static const int UAF_WinNumTrainingBonus[26];//ÿ����ɫwin���ٴ��ж���ѵ���ӳ�
  static const int UAF_RedBuffBonus[6];//��buff��ͬɫѵ��Ϊ012345ʱ��ѵ���ӳɣ�0������ֻ��������ÿ�ε��ö�Ҫ-1����ͬ
  //static const int UAF_BlueBuffBonus
  static const int UAF_LinkNumBonus[6];//��buffʱlinkѵ����ͬɫѵ��Ϊ012345ʱ��ѵ���ӳ�
  static const int UAF_LinkNumBonusXiahesu[6];//��buffʱ�ĺ����ڼ�linkѵ����ͬɫѵ��Ϊ012345ʱ��ѵ���ӳ�
  static const int UAF_LinkVitalCostGain[6];//linkѵ����ͬɫѵ��Ϊ012345ʱ��������������


  //����
  static const int FiveStatusFinalScore[1200+800*2+1];//��ͬ���Զ�Ӧ������
  static const double ScorePtRate;//Ϊ�˷��㣬ֱ����Ϊÿ1pt��Ӧ���ٷ֡�
  static const double ScorePtRateQieZhe;//Ϊ�˷��㣬ֱ����Ϊÿ1pt��Ӧ���ٷ֡�����


};