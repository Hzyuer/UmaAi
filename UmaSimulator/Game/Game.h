#pragma once
#include <random>
#include <array>
#include "../GameDatabase/GameDatabase.h"
#include "Person.h"
#include "Action.h"

struct SearchParam;

struct Game
{
  //��ʾ���
  bool playerPrint;//�������ʱ����ʾ������Ϣ

  //����״̬����������ǰ�غϵ�ѵ����Ϣ
  int32_t umaId;//�����ţ���KnownUmas.cpp
  int16_t fiveStatusBonus[5];//�������ά���Եĳɳ���
  int16_t eventStrength;//ÿ�غ��У����⣩���ʼ���ô�����ԣ�ģ��֧Ԯ���¼�

  int16_t turn;//�غ�������0��ʼ����77����
  int16_t vital;//������������vital������Ϊ��Ϸ��������е�
  int16_t maxVital;//��������
  int16_t motivation;//�ɾ�����1��5�ֱ��Ǿ����������õ�

  int16_t fiveStatus[5];//��ά���ԣ�1200���ϲ�����
  int16_t fiveStatusLimit[5];//��ά�������ޣ�1200���ϲ�����
  int16_t skillPt;//���ܵ�
  int16_t skillScore;//�����ܵķ���

  float ptScoreRate;//ÿpt���ٷ�
  int16_t failureRateBias;//ʧ���ʸı�������ϰ����=-2����ϰ����=2
  //bool isQieZhe;//����  �ϲ���ptScoreRate��
  bool isAiJiao;//����
  bool isPositiveThinking;//�ݥ��ƥ���˼�������˵����γ���ѡ�ϵ�buff�����Է�һ�ε�����


  int16_t zhongMaBlueCount[5];//����������Ӹ���������ֻ��3��
  int16_t zhongMaExtraBonus[6];//����ľ籾�����Լ����ܰ����ӣ���Ч��pt����ÿ�μ̳мӶ��١�ȫ��ʦ�����ӵ���ֵ��Լ��30��30��200pt
  
  int16_t saihou;//����ӳ�
  bool isRacing;//����غ��Ƿ��ڱ���

  Person persons[9];//���9��ͷ��������6�ſ������³�6������7��û����������8�������������Ǿ���ǰ6��λ�ã�8��λ�þͿ����ˣ���
  int16_t personDistribution[5][5];//ÿ��ѵ������Щ��ͷid��personDistribution[�ĸ�ѵ��][�ڼ�����ͷ]����λ��Ϊ-1
  int lockedTrainingId;//�Ƿ���ѵ�����Լ��������ĸ�ѵ���������Ȳ��ӣ���ai��������ʱ���ټӡ�


  //�籾���
  int16_t uaf_trainingColor[5];//����ѵ������ɫ
  int16_t uaf_trainingLevel[3][5];//������ɫ����ѵ���ĵȼ�
  bool uaf_winHistory[5][3][5];//�˶�����ʷս��
  bool uaf_lastTurnNotTrain;//�ϻغ��Ƿ�û��ѵ�������û�У���غϵĵȼ�������+3
  int16_t uaf_xiangtanRemain;//��ʣ������̸

  int16_t uaf_buffActivated[3];//����Ƶ�buff�Ѿ������������ˣ���¼�����Ҫ������ʶ��ʲôʱ��Ӧ���������غ�buff���������ѵ����ȼ����370����ʱ���buffActivated=6������2�غ�buff���ĳ�7��˵���ռ���350����buff�������buffActivated=7������buff��˵��350����buff�Ѿ��������
  int16_t uaf_buffNum[3];//����Ƶ�buff��ʣ������

  //������������������Ϊ�ӽ��ش������������Ŷӿ����Ժ��ٿ���
  int16_t lianghua_type;//0û����������1 ssr����2 r��
  int16_t lianghua_personId;//��������persons��ı��
  //int16_t lianghua_stage;//0��δ�����1���ѵ����δ�������У�2���ѽ�������    ��Σ����������������˵����ȫ����Person������
  int16_t lianghua_outgoingUsed;//�����ĳ����Ѿ����˼�����   ��ʱ���������������Ŷӿ��ĳ���
  double lianghua_vitalBonus;//�������Ļظ�������������1.60��
  double lianghua_statusBonus;//���������¼�Ч������������1.25��


  //����ͨ���������Ϣ�����õķǶ�������Ϣ��ÿ�غϻ��߸������غϣ�ÿ�����ᣩ����һ��

  //�������
  int16_t uaf_trainLevelColorTotal[3];//������ɫ�ĵȼ��ܺ�
  int16_t uaf_colorWinCount[3];//������ɫ�ֱ��ۼ�win�����ٴ�
  int16_t uaf_trainingBonus;//�籾ѵ���ӳɣ�ȡ����������ɫ��win������ÿ�������һ��

  //ÿ�غϸ���
  int16_t trainValue[5][7];//��һ�����ǵڼ���ѵ�����ڶ���������������������pt����
  int16_t failRate[5];//ѵ��ʧ����
  int16_t uaf_trainLevelGain[5];//���ѵ���ֱ�Ӷ���ѵ���ȼ�������������ֻ����ǰѵ��ͷ�������֣�
  int16_t trainShiningNum[5];//ÿ��ѵ���м�����Ȧ







  //��Ϸ�������------------------------------------------------------------------------------------------

public:

  void newGame(std::mt19937_64& rand,
    bool enablePlayerPrint,
    int newUmaId,
    int umaStars,
    int newCards[6],
    int newZhongMaBlueCount[5],
    int newZhongMaExtraBonus[6]);//������Ϸ�����֡�umaId��������


  //��������Ƿ������Һ���
  //������İ�����������Ҫ��̸��������ʣ����̸��������ǰʮ�����Լ�ura�ڼ�������غϱ�����
  //������İ�������̸�˲�ѵ�����Ѳ����ڵ���ɫ��̸��������ɫ��ѡ���ѵ��������̸��ԭɫ��Ŀ��ɫ
  bool isLegal(Action action) const;

  //����Action��һֱ������У�ֱ����һ����Ҫ��Ҿ��ߣ����������غϣ�������غ���>=78��ʲô������ֱ��return������Ҫ����������ˣ�
  void applyTrainingAndNextTurn(
    std::mt19937_64& rand,
    Action action);

  int finalScore() const;//�����ܷ�
  bool isEnd() const;//�Ƿ��Ѿ��վ�



  //ԭ�����⼸��private���У����private��ĳЩ�ط��ǳ��������Ǿ͸ĳ�public
private:
  void randomDistributeCards(std::mt19937_64& rand);//���������ͷ
  void calculateTrainingValue();//��������ѵ���ֱ�Ӷ��٣�������ʧ���ʡ�ѵ���ȼ�������
  bool applyTraining(std::mt19937_64& rand, Action action);//���� ѵ��/����/���� �����������˵���¼����������̶��¼��;籾�¼���������Ϸ����򷵻�false���ұ�֤�����κ��޸�
  void checkEventAfterTrain(std::mt19937_64& rand);//���̶��¼�������¼�����������һ���غ�

  void checkFixedEvents(std::mt19937_64& rand);//ÿ�غϵĹ̶��¼��������籾�¼��͹̶������Ͳ��������¼���
  void checkRandomEvents(std::mt19937_64& rand);//ģ��֧Ԯ���¼�����������¼������������������飬������ȣ�

  //���ýӿ�-----------------------------------------------------------------------------------------------

  bool loadGameFromJson(std::string jsonStr);

  //���������룬���治��Ҫʵ��
  void getNNInputV1(float* buf, const SearchParam& param) const;

  void print() const;//�ò�ɫ������ʾ��Ϸ����
  void printFinalStats() const;//��ʾ���ս��




  //���ָ���������ӿڣ����Ը�����Ҫ���ӻ���ɾ��-------------------------------------------------------------------------------
public:
  bool isXiangtanLegal(int xiangtanType);//����̸�Ƿ�Ϸ�
  void xiangtanAndRecalculate(int xiangtanType);//��̸�������¼�������ֵ
  void runRace(int basicFiveStatusBonus, int basicPtBonus);//�ѱ��������ӵ����Ժ�pt�ϣ������ǲ�������ӳɵĻ���ֵ


  void addStatus(int idx, int value);//��������ֵ�����������
  void addAllStatus(int value);//ͬʱ�����������ֵ
  void addVital(int value);//���ӻ�������������������
  void addMotivation(int value);//���ӻ�������飬ͬʱ���ǡ�isPositiveThinking��
  void addJiBan(int idx,int value);//����������ǰ���

  void addStatusFriend(int idx, int value);//���˿��¼�����������ֵ����pt��idx=5���������¼��ӳ�
  void addVitalFriend(int value);//���˿��¼����������������ǻظ����ӳ�



  float getSkillScore() const;//���ܷ֣�����������֮ǰҲ������ǰ��ȥ
  int getTrainingLevel(int trainIdx) const;//����ѵ���ȼ���1~19,20~21,...50~100
  bool cardIsShining(int personIdx) const;    // �ж�ָ�����Ƿ����ʡ���ͨ�����������ѵ�����Ŷӿ���friendOrGroupCardStage
  //bool trainShiningCount(int trainIdx) const;    // ָ��ѵ����Ȧ�� //uaf��һ������
  int calculateFailureRate(int trainType, double failRateMultiply) const;//����ѵ��ʧ���ʣ�failRateMultiply��ѵ��ʧ���ʳ���=(1-֧Ԯ��1��ʧ�����½�)*(1-֧Ԯ��2��ʧ�����½�)*...
  //void calculateTrainingValueSingle(int trainType);//����ÿ��ѵ���Ӷ���   uaf�籾�������ѵ��һ����ȽϷ���


  //���˿�����¼�
  void handleFriendUnlock(std::mt19937_64& rand);//�����������
  void handleFriendOutgoing();//�������
  void handleFriendClickEvent(std::mt19937_64& rand, int atTrain);//�����¼�����ƣ�옔��
  void handleFriendFixedEvent();//���˹̶��¼�������+����

  //��ʾ�¼�
  void printEvents(std::string s) const;//����ɫ������ʾ�¼�
  void printCardEffect();

};

