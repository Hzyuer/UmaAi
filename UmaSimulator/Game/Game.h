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
  int16_t eventStrength;//ÿ�غ���40%���ʼ���ô�����ԣ�ģ��֧Ԯ���¼�
  int16_t turn;//�غ�������0��ʼ����77����
  int16_t vital;//������������vital������Ϊ��Ϸ��������е�
  int16_t maxVital;//��������
  bool isQieZhe;//����
  bool isAiJiao;//����
  int16_t failureRateBias;//ʧ���ʸı�������ϰ����=-2����ϰ����=2
  int16_t fiveStatus[5];//��ά���ԣ�1200���ϲ�����
  int16_t fiveStatusLimit[5];//��ά�������ޣ�1200���ϲ�����
  int16_t skillPt;//���ܵ�
  int16_t skillScore;//�����ܵķ���
  int16_t motivation;//�ɾ�����1��5�ֱ��Ǿ����������õ�
  bool isPositiveThinking;//�ݥ��ƥ���˼�������˵����γ���ѡ�ϵ�buff�����Է�һ�ε�����
  int16_t trainLevelCount[5];//���ѵ���ĵȼ��ļ�����ʵ��ѵ���ȼ�=min(5,t/4+1)
  int16_t zhongMaBlueCount[5];//����������Ӹ���������ֻ��3��
  int16_t zhongMaExtraBonus[6];//����ľ籾�����Լ����ܰ����ӣ���Ч��pt����ÿ�μ̳мӶ��١�ȫ��ʦ�����ӵ���ֵ��Լ��30��30��200pt
  int normalCardCount;//���������ǿ�������
  SupportCard cardParam[6];//���ſ��Ĳ�����������Game���һ�����ڲ��䣬˳�����⡣��������Ŀ����ѵ��aiʱ����Ҫ����ı俨�Ĳ������³���ԣ�����ÿ��game�Ŀ��Ĳ������ܲ�һ��
  int16_t saihou;//����ӳ�
  Person persons[18];//����������������Ŷӿ������18��ͷ��������15���ɳ����ͷ������֧Ԯ����˳�����⣩��0~4��5������npc��5��6~14�������³�15������16������17����û��������17��
  bool isRacing;//����غ��Ƿ��ڱ���

  int motivationDropCount;//�������������ˣ��������籾�¼�����֪ͬһ�������鲻����ֶ�Σ�һ��5���������¼�������֮ǰ����Խ�֮࣬����ĸ��ʲ�֪���᲻��Խ��Խ�ͣ������ᣩ��

  //���������
  bool larc_isAbroad;//����غ��Ƿ��ں���
  //int32_t larc_supportPt;//�Լ���֧Ԯpt
  int32_t larc_supportPtAll;//�����ˣ��Լ�+�����ˣ���֧Ԯpt֮�ͣ�ÿ1700֧Ԯpt��Ӧ1%�ڴ���
  int16_t larc_shixingPt;//����pt
  int16_t larc_levels[10];//10���������Եĵȼ���0Ϊδ������˳������Ϸ������ϵ����µ�˳��˳������С�ڰ崫������ʱ���Ѿ��������
  bool larc_isSSS;//�Ƿ�Ϊsss
  int16_t larc_ssWin;//һ��������ͷ��ss
  int16_t larc_ssWinSinceLastSSS;//���ϴ�sss������win������ss����������һ����sss�ĸ��ʣ�
  bool larc_allowedDebuffsFirstLarc[9];//��һ�ο����ſ��Բ�����Щdebuff����ҿ������ã���������Ϊ����Ӯ������

  int16_t larc_zuoyueType;//û��������=0������SSR��=1������R��=2
  double larc_zuoyueVitalBonus;//�������Ļظ�������������1.8��
  double larc_zuoyueStatusBonus;//���������¼�Ч������������1.2��
  bool larc_zuoyueFirstClick;//�����Ƿ�����һ��
  bool larc_zuoyueOutgoingUnlocked;//�����������
  bool larc_zuoyueOutgoingRefused;//�Ƿ�ܾ����������
  int16_t larc_zuoyueOutgoingUsed;//����������˼�����

  //��ǰ�غϵ�ѵ����Ϣ
  //0֧Ԯ����δ���䣬1֧Ԯ��������ϻ������ʼǰ��2ѵ������������������0�����̶ֹ��¼�������¼���������һ���غ�
  //stageInTurn=0ʱ�������������������ֵ��stageInTurn=1ʱ�����������������policy
  int16_t stageInTurn;
  int16_t personDistribution[5][5];//ÿ��ѵ������Щ��ͷid��personDistribution[�ĸ�ѵ��][�ڼ�����ͷ]������ͷΪ-1

  int16_t larc_ssPersonsCount;//ss�м�����
  int16_t larc_ssPersons[5];//ss���ļ�����
  int16_t larc_ssPersonsCountLastTurn;//�ϸ��Ǳ�����Զ���غ��м���ss��ͷ��ֻ�����ж�����غ��ǲ����µ�ss����������sss��Ϊ�˱�����10����������ssʱ�������ʹ��ss��ʱ����������

  //ͨ�������õ���Ϣ
  int16_t trainValue[5][7];//��һ�����ǵڼ���ѵ�����ڶ���������������������pt����
  int16_t failRate[5];//ѵ��ʧ����
  int16_t trainShiningNum[5];//���ѵ���м�����Ȧ
  int16_t larc_staticBonus[6];//�������������棬����ǰ5��1���͵�6����1��3��pt+10
  int16_t larc_shixingPtGainAbroad[5];//����ѵ������pt����
  int16_t larc_trainBonus;//�ڴ���ѵ���ӳ�
  int16_t larc_ssValue[5];//ss�����������ǣ��������ϲ�����ԣ�
  int16_t larc_ssFailRate;//ss��ʧ����

  //��Ϸ����:���Ĺ��ˣ������ǹ�ʱ�ģ�
  //newGame();
  //for (int t = 0; t < TOTAL_TURN; t++)
  //{
  //  if (!isRacing)//����ѵ���غ�
  //  {
  //    randomDistributeCards();
  //    PLAYER_CHOICE;
  //    applyTraining();
  //    checkEventAfterTrain();
  //  }
  //  else//�����غ�
  //  {
  //    randomDistributeCards();//ֻ��stageInTurn�ĳ�1
  //    if(venusAvailableWisdom!=0)//�Ƿ�ʹ��Ů����ǣ�����ʹ�õ�ʱ��ֱ���������߲�
  //    {
  //      PLAYER_CHOICE;
  //    }
  //    applyTraining();//�������ֻ��Ů�񣬲��ɱ��
  //    checkEventAfterTrain();//�����Ӷ�������������ﴦ��
  //  }
  //}
  //finalScore();
  //

  void newGame(std::mt19937_64& rand,
    bool enablePlayerPrint,
    int newUmaId, 
    int umaStars,
    int newCards[6],
    int newZhongMaBlueCount[5],
    int newZhongMaExtraBonus[6]);//������Ϸ�����֡�umaId��������

  void initNPCsTurn3(std::mt19937_64& rand);//�����غϳ�ʼ��npc��ͷ

  bool loadGameFromJson(std::string jsonStr);

  void randomDistributeCards(std::mt19937_64& rand);//������俨���npc
  void calculateTrainingValue();//��������ѵ���ֱ�Ӷ��٣�������ʧ����

  // ����ѵ����ı仯��������Ϸ����򷵻�false���ұ�֤�����κ��޸�
  // ���У�chosenTrain����ѡ���ѵ����01234�ֱ������������ǣ�5����Ϣ��6�������7�Ǳ�����
  // useVenus�Ǽ���Ů���������Ƿ���Ů��
  // chosenSpiritColor�Ǽ������Ů����ѡһ�¼���ѡ�����Ƭ��ɫ�������Ʒֱ�012
  // chosenOutgoing����������ѡ��������Ŀ�������������ֱ���01234����ͨ�����5��
  //ע����ͨ�غ���14�ֿ��ܣ�5��ѵ��������һ��ѵ�����ܻ����Ů����ѡһ��������������Ϣ��������5�ֳ��У��������غ�ֻ�п�����Ů������ѡ��
  // forceThreeChoicesEvent��ǿ���ٻ���ѡһ�¼���1Ϊǿ���ٻ���-1Ϊǿ�Ʋ��ٻ���0Ϊ�������������ٻ����������ý�����ai����
  bool applyTraining(
    std::mt19937_64& rand, 
    Action action);
  void checkEventAfterTrain(std::mt19937_64& rand);//���̶��¼�������¼�����������һ���غ�

  void checkFixedEvents(std::mt19937_64& rand);//ÿ�غϵĹ̶��¼��������籾�¼��͹̶�������
  void checkSupportPtEvents(int oldSupportPt, int newSupportPt);//�ڴ��������¼�
  void checkRandomEvents(std::mt19937_64& rand);//ģ��֧Ԯ���¼��������¼������������������飬������ȣ�

  void applyTrainingAndNextTurn(
    std::mt19937_64& rand,
    Action action);//һֱ������У�ֱ����һ����Ҫ��Ҿ���

  bool isLegal(Action action) const;//��������Ƿ�����

  int finalScore() const;//�����ܷ�
  bool isEnd() const;//

  //��������
  double sssProb(int ssWinSinceLastSSS) const;//��sss�ĸ���
  int getTrainingLevel(int item) const;//����ѵ���ȼ�����0��ʼ����Ϸ���k����������k-1����Զ����5��
  bool cardIsShining(int which) const;    // �ж�ָ�����Ƿ�����
  bool trainShiningCount(int train) const;    // �ж�ָ��ѵ����Ȧ��
  //void runTestGame();

  void getNNInputV1(float* buf, const SearchParam& param) const;//����������
  void print() const;//�ò�ɫ������ʾ��Ϸ����
  float getSkillScore() const;//���ܷ֣�����������֮ǰҲ������ǰ��ȥ
  void printFinalStats() const;//��ʾ���ս��

  void addStatus(int idx, int value);//��������ֵ�����������
  void addAllStatus(int value);//ͬʱ�����������ֵ
  void addVital(int value);//���ӻ�������������������
  void addMotivation(int value);//���ӻ�������飬ͬʱ���ǡ�isPositiveThinking��
  void addJiBan(int idx,int value);//����������ǰ���
  void addTrainingLevelCount(int item, int value);//����ѵ���ȼ�������ÿ4Ϊ1����ѵ��+1���ڴ��ȴﵽĳ�����ȼ�+4��
  void charge(int idx, int value);//���
  void unlockUpgrade(int idx);//����ĳ������
  int buyUpgradeCost(int idx, int level) const;//����ĳ�������Ļ��ѣ����û�����򷵻�-1
  bool tryBuyUpgrade(int idx, int level);//����ĳ����������������򷵻�false
  int removeDebuffsFirstNCost(int n) const;//��������ǰn��debuff�Ļ���
  bool tryRemoveDebuffsFirstN(int n);//�����Ƿ��������ǰn��debuff�������������������ҷ���true������ʲô�������ҷ���false

  int calculateFailureRate(int trainType, double failRateMultiply) const;//����ѵ��ʧ���ʣ�failRateMultiply��ѵ��ʧ���ʳ���=(1-֧Ԯ��1��ʧ�����½�)*(1-֧Ԯ��2��ʧ�����½�)*...
  void calculateTrainingValueSingle(int trainType);//����ÿ��ѵ���Ӷ���
  void calculateSS();//����ss����ֵ
  void runRace(int basicFiveStatusBonus, int basicPtBonus);//�ѱ��������ӵ����Ժ�pt�ϣ������ǲ�������ӳɵĻ���ֵ
  void runSS(std::mt19937_64& rand);//����ss��ս

  //���˿�����¼�
  void addStatusZuoyue(int idx, int value);//�������¼�����������ֵ����pt��idx=5���������¼��ӳ�
  void addVitalZuoyue(int value);//�������¼����������������ǻظ����ӳ�
  void handleFriendUnlock(std::mt19937_64& rand);//�����������
  void handleFriendOutgoing();//�������
  void handleFriendClickEvent(std::mt19937_64& rand, int atTrain);//�����¼�����ƣ�옔��
  void handleFriendFixedEvent();//���˹̶��¼�������+����

  //��ʾ�¼�
  void printEvents(std::string s) const;//����ɫ������ʾ�¼�
  void printCardEffect();

};

