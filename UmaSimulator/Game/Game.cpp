#include <iostream>
#include <cassert>
#include "Game.h"
using namespace std;
static bool randBool(mt19937_64& rand, double p)
{
  return rand() % 65536 < p * 65536;
}

void Game::newGame(mt19937_64& rand, bool enablePlayerPrint, int newUmaId, int umaStars, int newCards[6], int newZhongMaBlueCount[5], int newZhongMaExtraBonus[6])
{
  playerPrint = enablePlayerPrint;

  umaId = newUmaId;
  for (int i = 0; i < 5; i++)
    fiveStatusBonus[i] = GameDatabase::AllUmas[umaId].fiveStatusBonus[i];
  eventStrength = GameConstants::EventStrengthDefault;
  turn = 0;
  vital = 100;
  maxVital = 100;
  isQieZhe = false;
  isAiJiao = false; 
  failureRateBias = 0;
  skillPt = 120;
  skillScore = umaStars >= 3 ? 170 * (umaStars - 2) : 120 * (umaStars);//���м���
  motivation = 3;
  isPositiveThinking = false;
  for (int i = 0; i < 5; i++)
    trainLevelCount[i] = 0;
  isRacing = false;

  larc_zuoyueType = 0;
  larc_zuoyueVitalBonus = 0;
  larc_zuoyueStatusBonus = 0;
  for (int i = 0; i < 18; i++)
  {
    persons[i] = Person();
  }
  persons[15].personType = 4;
  persons[16].personType = 5;
  persons[17].personType = 6;
  for (int i = 0; i < 18; i++)
  {
    if (persons[i].personType == 0)
      persons[i].personType = 3;
  }


  normalCardCount = 0;//���������ǿ�������
  saihou = 0;
  for (int i = 0; i < 6; i++)
  {
    int cardId = newCards[i];
    cardParam[i] = GameDatabase::AllCards[cardId];
    SupportCard& cardP = cardParam[i];
    saihou += cardP.saiHou;
    int cardType = cardP.cardType;
    if (cardType == 5 || cardType == 6)
    {

      int realCardId = cardId / 10;
      if (realCardId == 30160 || realCardId == 10094)//������
      {
        if (realCardId == 30160)
          larc_zuoyueType = 1;
        else 
          larc_zuoyueType = 2;

        int zuoyueLevel = cardId % 10;
        if (larc_zuoyueType==1)
        {
          larc_zuoyueVitalBonus = GameConstants::ZuoyueVitalBonusSSR[zuoyueLevel];
          larc_zuoyueStatusBonus = GameConstants::ZuoyueStatusBonusSSR[zuoyueLevel];
        }
        else
        {
          larc_zuoyueVitalBonus = GameConstants::ZuoyueVitalBonusR[zuoyueLevel];
          larc_zuoyueStatusBonus = GameConstants::ZuoyueStatusBonusR[zuoyueLevel];
        }
        larc_zuoyueVitalBonus += 1e-10;
        larc_zuoyueStatusBonus += 1e-10;//�Ӹ�С����������Ϊ�����������

        persons[17].personType = 1;
        persons[17].cardIdInGame = i;
        persons[17].friendship = cardP.initialJiBan;
      }
      else
      {
        throw string("��֧�ִ�������������˻��Ŷӿ�");
      }
    }
    else//���������ǿ�
    {
      Person& p = persons[normalCardCount];
      normalCardCount += 1;
      p.personType = 2;
      p.cardIdInGame = i;
      p.friendship = cardP.initialJiBan;
      p.larc_isLinkCard = cardP.larc_isLink;

      std::vector<int> probs = { 100,100,100,100,100,50 }; //�������ʣ����������Ǹ�
      probs[cardP.cardType] += cardP.deYiLv;  
      p.distribution = std::discrete_distribution<>(probs.begin(), probs.end());
    }
  }


  motivationDropCount = 0;

  larc_isAbroad = false;
  larc_supportPtAll = 0;
  larc_shixingPt = 0;
  for (int i = 0; i < 10; i++)larc_levels[i] = 0;
  larc_isSSS = false;
  larc_ssWin = 0;
  larc_ssWinSinceLastSSS = 0;
  for (int i = 0; i < 9; i++)
    larc_allowedDebuffsFirstLarc[i] = false;

  //larc_zuoyueType
  //larc_zuoyueCardLevel
  larc_zuoyueFirstClick = false;
  larc_zuoyueOutgoingUnlocked = false; 
  larc_zuoyueOutgoingRefused = false;
  larc_zuoyueOutgoingUsed = 0;

  for (int i = 0; i < 5; i++)
    zhongMaBlueCount[i] = newZhongMaBlueCount[i];
  for (int i = 0; i < 6; i++)
    zhongMaExtraBonus[i] = newZhongMaExtraBonus[i];


  for (int i = 0; i < 5; i++)
    fiveStatusLimit[i] = GameConstants::BasicFiveStatusLimit[i]; //ԭʼ��������
  for (int i = 0; i < 5; i++)
    fiveStatusLimit[i] += int(zhongMaBlueCount[i] * 5.34 * 2); //��������--�������ֵ

  //�����μ̳е����飬��ʱ����˵
  //for (int i = 0; i < 5; i++)
   // fiveStatusLimit[i] += rand() % 20; //��������--�����μ̳��������


  for (int i = 0; i < 5; i++)
    fiveStatus[i] = GameDatabase::AllUmas[umaId].fiveStatusInitial[i] - 10 * (5 - umaStars); //�������ʼֵ
  for (int i = 0; i < 6; i++)//֧Ԯ����ʼ�ӳ�
  {
    for (int j = 0; j < 5; j++)
      addStatus(j, cardParam[i].initialBonus[j]);
    skillPt += cardParam[i].initialBonus[5];
  }
  for (int i = 0; i < 5; i++)
    addStatus(i, zhongMaBlueCount[i] * 7); //����



  //initRandomGenerators();

  stageInTurn = 0;
  larc_ssPersonsCountLastTurn = 0;
  randomDistributeCards(rand); 
}

void Game::initNPCsTurn3(std::mt19937_64& rand)
{
  //int allSpecialBuffsNum[13] = {0,0,0,1,1,2,2,4,1,1,0,0,3};   // ��Ӧ�޽�ǰ�汾����Ҫ���������
  int allSpecialBuffsNum[13] = { 0,0,0,0,2,2,2,4,1,1,0,0,3 };   // 2.24 �޽��������� ��ͨ�����ɻ��������
  int specialBuffEveryPerson[15];
  for (int i = 0; i < 15; i++)specialBuffEveryPerson[i] = 0;

  //�������ſ��̶�������buff
  for (int i = 0; i < normalCardCount; i++)
  {
    assert(persons[i].personType == 2);
    int s = cardParam[persons[i].cardIdInGame].larc_linkSpecialEffect;
    if (s != 0)
    {
      specialBuffEveryPerson[i] = s;
      allSpecialBuffsNum[s] -= 1;
      assert(allSpecialBuffsNum[s] >= 0);
    }
    
  }

  //û�̶�����buff�ľ������������buff
  vector<int> specialBuffNotAssigned;
  for (int i = 0; i < 13; i++)
  {
    int n = allSpecialBuffsNum[i];
    if (n >= 0)
    {
      for (int j = 0; j < n; j++)
        specialBuffNotAssigned.push_back(i);
    }
  }
  std::shuffle(specialBuffNotAssigned.begin(), specialBuffNotAssigned.end(), rand);
  int c = 0;
  for (int i = 0; i < 15; i++)
  {
    if (specialBuffEveryPerson[i] == 0)
    {
      specialBuffEveryPerson[i] = specialBuffNotAssigned[c];
      c += 1;
    }
  }

  //��ͷ����
  vector<int> s = { 0,0,0,1,1,1,2,2,2,3,3,3,4,4,4 };
  assert(s.size() == 15);
  std::shuffle(s.begin(), s.end(), rand);

  //��ʼ��
  for (int i = 0; i < 15; i++)
  {
    persons[i].initAtTurn3(rand, specialBuffEveryPerson[i], s[i]);
  }


}

void Game::randomDistributeCards(std::mt19937_64& rand)
{
  //assert(stageInTurn == 0 || turn == 0);
  stageInTurn = 1;

  //�����غϵ���ͷ����ͱ���/Զ���غϵ�ss������Ҫ���㣬��Ϊ������������
  if (isRacing)
    return;//�������÷��俨�飬��Ҫ��stageInTurn
  
  //�Ƚ�6�ſ����䵽ѵ����
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
      personDistribution[i][j] = -1;

  int headCountEveryTrain[5] = { 0,0,0,0,0 };//���ѵ���ֱ��ж����ˣ�����5��Ҳ������

  //��һ����ͷ����ĳ��ѵ��������������5���и�������ߵ�һ��
  auto setHead = [&](int head, int whichTrain)
  {
    if (whichTrain >= 5)return;
    int p = headCountEveryTrain[whichTrain];
    if (p < 5)//��û5��ͷ
    {
      personDistribution[whichTrain][p] = head;
    }
    else
    {
      // ��Ϲ�µĹ�ʽ�����Ա�֤������ͷ��λƽ�ȣ�5/(p+1)�ĸ����滻������һ��ͷ��(p-4)/(p+1)�ĸ��ʸ�
      int r = rand() % (p + 1);
      if (r < 5)//�������һ����
        personDistribution[whichTrain][r] = head;
    }
    headCountEveryTrain[whichTrain] += 1;//�����Ƿ��������ѵ�����������һ
  };


  for (int i = 0; i < 18; i++)
  {
    if (turn < 2 && persons[i].personType != 2)//ǰ���غ�û��������npc�����³����ߵ�
      continue;
    if (larc_isAbroad && (i == 15 || i == 16 || (i == 17 && persons[i].personType != 1)))//Զ��ʱ���³����߲��ڣ�������������Ҳ����
      continue;
    if (turn < 10 && i == 16)//���ߵ�10���غ���
      continue;

    if (i == 17 && larc_zuoyueType == 1 && persons[i].friendship >= 60)//ssr�������60���������˵�������
    {
      int whichTrain1 = rand() % 6;
      setHead(i, whichTrain1);
      if (whichTrain1 == 5)
      {
        int whichTrain2 = rand() % 6;
        setHead(i, whichTrain2);
      }
      else
      {
        //����ڶ���λ�ú͵�һ������ײ����������ʵ����ʱȽϽӽ�
        int whichTrain2 = rand() % 5;
        if (whichTrain2 >= whichTrain1)whichTrain2++;
        setHead(i, whichTrain2);
      }
    }
    else
    {
      int whichTrain = persons[i].distribution(rand);
      setHead(i, whichTrain);
    }

    //�Ƿ���hint
    if (persons[i].personType == 2)
    {
      double hintProb = 0.06 * (1 + 0.01 * cardParam[persons[i].cardIdInGame].hintProbIncrease);
      bernoulli_distribution d(hintProb);
      persons[i].isHint = d(rand);
    }
  }
  
  //����ss��ͷ
  if (!larc_isAbroad)
  {
    int fullNum = 0;
    for (int i = 0; i < 5; i++)
      larc_ssPersons[i] = -1;
    for (int i = 0; i < 15; i++)
    {
      if (persons[i].larc_charge == 3)
      {
        if (fullNum < 5)
          larc_ssPersons[fullNum] = i;
        else
        {
          int r = rand() % (fullNum + 1);
          if (r < 5)//�������һ����
            larc_ssPersons[r] = i;
        }
        fullNum += 1;
      }
    }
    larc_ssPersonsCount = fullNum > 5 ? 5 : fullNum;

    bool isNewFullSS = larc_ssPersonsCount >= 5 && larc_ssPersonsCountLastTurn < 5;//Ϊ�˱�����10����������ssʱ�������ʹ��ss��ʱ����������
    larc_ssPersonsCountLastTurn = larc_ssPersonsCount;
    if (isNewFullSS)
    {
      larc_isSSS = randBool(rand, sssProb(larc_ssWinSinceLastSSS));
    }
  }
  calculateTrainingValue();
}

void Game::calculateTrainingValue()
{
  //�ȼ������Եȼ��ӳ�
  for (int i = 0; i < 6; i++)
    larc_staticBonus[i] = 0;
  for (int i = 0; i < 5; i++)
    if (larc_levels[GameConstants::UpgradeId50pEachTrain[i]] >= 1)
      larc_staticBonus[i] += 3;

  if (larc_levels[5] >= 1)
    larc_staticBonus[5] += 10;
  if (larc_levels[5] >= 3)
    larc_staticBonus[5] += 10;
  if (larc_levels[6] >= 1)
    larc_staticBonus[1] += 3;
  if (larc_levels[7] >= 1)
    larc_staticBonus[3] += 3;

  int larc_trainBonusLevel = (larc_supportPtAll + 85) / 8500;
  if (larc_trainBonusLevel > 40)larc_trainBonusLevel = 40;
  larc_trainBonus = GameConstants::LArcTrainBonusEvery5Percent[larc_trainBonusLevel];

  for (int i = 0; i < 6; i++)
  {
    persons[i].isShining = false;
  }
  
  // TODO: ����ͬ��֮ǰ���·����޷�ִ��
  for (int trainType = 0; trainType < 5; trainType++)
  {
    calculateTrainingValueSingle(trainType);
  }

  if(!larc_isAbroad)
    calculateSS();
      
}
void Game::addStatus(int idx, int value)
{
  fiveStatus[idx] += value;
  if (fiveStatus[idx] > fiveStatusLimit[idx])
    fiveStatus[idx] = fiveStatusLimit[idx];
  if (fiveStatus[idx] < 1)
    fiveStatus[idx] = 1;
}
void Game::addVital(int value)
{
  vital += value;
  if (vital > maxVital)
    vital = maxVital;
  if (vital < 0)
    vital = 0;
}
void Game::runSS(std::mt19937_64& rand)
{
  //ssʧ�ܵ�û������buff������buffҲ����������Լ���֧Ԯ��pt��һ�룬���ֵ�û���١��������⣨�����ԣ������pt�ȣ���ȫ��ͬ
  //sss��ʤ

  int ssWinNum = 0;

  //�����Ͱ������ӣ���Ϊ�ȼ����������ټ��������ȼ���ٰ���
  int vitalGain = 0;
  bool newAiJiao = false;

  for (int i = 0; i < larc_ssPersonsCount; i++)
  {
    int id = larc_ssPersons[i];
    Person& p = persons[id];
    p.larc_charge = 0;
    p.larc_level += 1;
    bool suc = larc_ssFailRate > 0 ? randBool(rand, 0.01 * (100 - larc_ssFailRate)) : true;
    if (suc)//����buff��Ч
    {
      ssWinNum += 1;
      int buff = p.larc_nextThreeBuffs[0];
      if (isAiJiao && buff == 8) //�ظ�������������
        buff = 11;
      if (buff == 1)//����
      {
        int equalPt = 6;//��Чpt��·����ͷ�������1�����ܴ����û�ã����Էָ��͵�
        if (p.personType == 2)//֧Ԯ��
        {
          equalPt = cardParam[p.cardIdInGame].hintBonus[5];
          if (equalPt == 0)equalPt = 6;//������֮���û����
        }
        skillPt += equalPt;
      }
      else if (buff == 3)//����
      {
        vitalGain += 20;
      }
      else if (buff == 4)//����������
      {
        maxVital += 4;
        vitalGain += 20;
      }
      else if (buff == 5)//������ɾ�
      {
        addMotivation(1);
        vitalGain += 20;
      }
      else if (buff == 6)//���
      {
        p.larc_charge = 3;
      }
      else if (buff == 7)//����pt
      {
        larc_shixingPt += 50;
      }
      else if (buff == 8)//����
      {
        assert(!isAiJiao && "�����ظ�������");
        newAiJiao = true;
      }
      else if (buff == 9)//��ϰ����
      {
        failureRateBias = -2;
      }
      else if (buff == 11)//����
      {
        if(p.larc_isLinkCard)
          addAllStatus(3);//���һ������+15��Ϊ�˷���ֱ��ƽ̯
        else
          addAllStatus(2);//���һ������+10��Ϊ�˷���ֱ��ƽ̯
      }
      else if (buff == 12)//���ܵ�
      {
        skillPt += 20;
      }
      else
      {
        assert(false && "δ֪��ss buff");
      }


      p.larc_nextBuff(rand);
    }

    if (p.personType == 2)//֧Ԯ�������
    {
      addJiBan(id, 7);
    }
  
  }
  if (newAiJiao)isAiJiao = true;
  addVital(vitalGain);


  for (int i = 0; i < 5; i++)
    addStatus(i, larc_ssValue[i]);
  skillPt += 5 * larc_ssPersonsCount;
  if (larc_isSSS)skillPt += 25;
  larc_shixingPt += 10 * larc_ssPersonsCount;

  int supportPtFactor = larc_ssPersonsCount + ssWinNum;//���˵�Լ���ڰ����ͷ
  int supportPtEveryHalfHead = larc_isSSS ? 1000 + rand() % 64 : 580 + rand() % 64;//�����ͷ��supportPt�������������⼸����ͷ���������е������й�ϵ���ǳ����ӣ�Ϊ��ʡ�¾�ȡ�˸�ƽ���ټ�һЩ���

  int oldSupportPt = larc_supportPtAll;
  larc_supportPtAll += supportPtFactor * supportPtEveryHalfHead;
  checkSupportPtEvents(oldSupportPt, larc_supportPtAll);//�ڴ��������¼�

  larc_ssWin += ssWinNum;

  if (larc_isSSS)
    larc_ssWinSinceLastSSS = 0;
  else
    larc_ssWinSinceLastSSS += ssWinNum;


  //����
  larc_ssPersonsCount = 0;
  larc_ssPersonsCountLastTurn = 0;

  for (int i = 0; i < 5; i++)larc_ssPersons[i] = -1;
  larc_isSSS = false;

}
void Game::addMotivation(int value)
{
  if (value < 0)
  {
    if (isPositiveThinking)
      isPositiveThinking = false;
    else
    {
      motivation += value;
      if (motivation < 1)
        motivation = 1;
    }
  }
  else
  {
    motivation += value;
    if (motivation > 5)
      motivation = 5;
  }
}
void Game::addJiBan(int idx, int value)
{
  auto& p = persons[idx];
  if (p.personType == 1 || p.personType == 2)
  {
    if (isAiJiao)value += 2;
  }
  else if (p.personType == 4 || p.personType == 5 || p.personType == 6)
  {
    //����
  }
  else //npc
    value = 0;
  p.friendship += value;
  if (p.friendship > 100)p.friendship = 100;
}
void Game::addTrainingLevelCount(int item, int value)
{
  assert(item >= 0 && item < 5 && "addTrainingLevelCount���Ϸ�ѵ��");
  trainLevelCount[item] += value;
  if (trainLevelCount[item] > 4 * 4)trainLevelCount[item] = 4 * 4;
}
void Game::charge(int idx, int value)
{
  if (value == 0)return;
  persons[idx].larc_charge += value;
  if (persons[idx].larc_charge > 3)persons[idx].larc_charge = 3;
}
void Game::unlockUpgrade(int idx)
{
  if (larc_levels[idx] == 0)
    larc_levels[idx] = 1;
  //ֻ���ڻغϽ���ʱ������������Բ���Ҫ���¼���ѵ��ֵ
}
int Game::buyUpgradeCost(int idx, int level) const
{
  int cost = 0;

  if (larc_levels[idx] == 0)
    return -1;//û����
  else if (larc_levels[idx] == 1)
  {
    if (level == 2)
      cost = GameConstants::LArcUpgradesCostLv2[idx];
    else if (level == 3)
      cost = GameConstants::LArcUpgradesCostLv3[idx] + GameConstants::LArcUpgradesCostLv2[idx];
    else
      return -1;
  }
  else if (larc_levels[idx] == 2)
  {
    if (level == 2)
      cost = 0;
    else if (level == 3)
      cost = GameConstants::LArcUpgradesCostLv3[idx];
    else
      return -1;
  }
  else if (larc_levels[idx] == 3)
  {
    return -1;
  }

  return cost;
}
bool Game::tryBuyUpgrade(int idx, int level)
{
  int cost = buyUpgradeCost(idx, level);

  if (cost > larc_shixingPt)return false;
  if (cost < 0)return false;//δ����
  if (cost == 0)return true;//�ѹ���
  larc_shixingPt -= cost;
  larc_levels[idx] = level;
  
  if (level == 3)//��Ҫ���¼���ѵ��ֵ
    calculateTrainingValue();

  return true;
}
int Game::removeDebuffsFirstNCost(int n) const
{
  int totalCost = 0;

  for (int i = 0; i < n; i++)
  {
    if (larc_allowedDebuffsFirstLarc[i])
      continue;
    if (larc_levels[i] == 0)
      return 10000;
    else if (larc_levels[i] == 1)
      totalCost += GameConstants::LArcUpgradesCostLv2[i];
  }

  return totalCost;
}

bool Game::tryRemoveDebuffsFirstN(int n)
{

  if (larc_shixingPt < removeDebuffsFirstNCost(n))
    return false;

  for (int i = 0; i < n; i++)
  {
    if (larc_allowedDebuffsFirstLarc[i])
      continue;
    else if (larc_levels[i] == 1)
    {
      bool suc = tryBuyUpgrade(i, 2);
      assert(suc);
    }
  }
  return true;

}
void Game::addAllStatus(int value)
{
  for (int i = 0; i < 5; i++)addStatus(i, value);
}
int Game::calculateFailureRate(int trainType, double failRateMultiply) const
{
  //������ϵ�ѵ��ʧ���ʣ����κ��� A*x^2 + B*x + C + 0.5 * trainLevel
  //���Ӧ����2%����
  static const double A = 0.0245;
  static const double B[5] = { -3.77,-3.74,-3.76,-3.81333,-3.286 };
  static const double C[5] = { 130,127,129,133.5,80.2 };

  double f = A * vital * vital + B[trainType] * vital + C[trainType] + 0.5 * getTrainingLevel(trainType);
  //int fr = round(f);
  if (vital > 60)f = 0;//�����Ƕ��κ�������������103ʱ�������fr����0��������Ҫ�ֶ�����
  if (f < 0)f = 0;
  if (f > 99)f = 99;//����ϰ���֣�ʧ�������99%
  f *= failRateMultiply;//֧Ԯ����ѵ��ʧ�����½�����
  int fr = round(f);
  fr += failureRateBias;
  if (fr < 0)fr = 0;
  if (fr > 100)fr = 100;
  return fr;
}
void Game::runRace(int basicFiveStatusBonus, int basicPtBonus)
{
  double raceMultiply = 1 + 0.01 * saihou;
  int fiveStatusBonus = floor(raceMultiply * basicFiveStatusBonus);
  int ptBonus = floor(raceMultiply * basicPtBonus);
  addAllStatus(fiveStatusBonus);
  skillPt += basicPtBonus;
}

void Game::addStatusZuoyue(int idx, int value)
{
  value = int(value * larc_zuoyueStatusBonus);
  if (idx == 5)skillPt += value;
  else addStatus(idx, value);
}

void Game::addVitalZuoyue(int value)
{
  value = int(value * larc_zuoyueVitalBonus);
  addVital(value);
}


void Game::handleFriendOutgoing()
{
  assert(larc_zuoyueOutgoingUnlocked && larc_zuoyueOutgoingUsed < 5);
  if (larc_zuoyueOutgoingUsed == 0)
  {
    addVitalZuoyue(30);
    addMotivation(1);
    addStatusZuoyue(3, 5);
    addJiBan(17, 5);
  }
  else if (larc_zuoyueOutgoingUsed == 1)
  {
    addVitalZuoyue(25);
    addMotivation(1);
    addStatusZuoyue(2, 5);
    addStatusZuoyue(3, 5);
    addJiBan(17, 5);
  }
  else if (larc_zuoyueOutgoingUsed == 2)
  {
    addVitalZuoyue(35);
    addMotivation(1);
    addStatusZuoyue(3, 15);
    isPositiveThinking = true;
    addJiBan(17, 5);
  }
  else if (larc_zuoyueOutgoingUsed == 3)
  {
    addVitalZuoyue(25);
    addStatusZuoyue(3, 20);
    addJiBan(17, 5);
  }
  else if (larc_zuoyueOutgoingUsed == 4)//��Ϊ��ɹ��ͳɹ���ȡ��ƽ��
  {
    addVitalZuoyue(37);
    addStatusZuoyue(3, 7);
    addMotivation(1);
    addJiBan(17, 5);
  }
  else assert(false && "δ֪�ĳ���");
  larc_zuoyueOutgoingUsed += 1;
}
void Game::handleFriendUnlock(std::mt19937_64& rand)
{
  //����: 2ѡ��2/3���ʳɹ����ɹ�ʱѡ���棬ʧ��ʱѡ����
  larc_zuoyueOutgoingUnlocked = true;
  larc_zuoyueOutgoingRefused = false;
  if (rand() % 3 > 0)
  {
    printEvents("�������������2ѡ��ɹ�");
    addVitalZuoyue(35);
    addMotivation(1);
    addJiBan(17, 10);
  }
  else
  {
    printEvents("�������������2ѡ��ʧ�ܣ���ѡ��1ѡ��");
    addVitalZuoyue(15);
    addMotivation(1);
    addStatusZuoyue(3, 5);
    addJiBan(17, 5);
  }
}
void Game::handleFriendClickEvent(std::mt19937_64& rand, int atTrain)
{
  if (!larc_zuoyueFirstClick)
  {
    printEvents("��һ�ε�����");
    larc_zuoyueFirstClick = true;
    addJiBan(17, 10);
    addStatusZuoyue(3, 15);
    addStatusZuoyue(5, 5);
    addMotivation(1);
  }
  else
  {
    if (rand() % 5 < 3)return;//40%���ʳ��¼���60%���ʲ���
    addJiBan(17, 7);
    addStatus(3, 3);
    skillPt += 3;
    bool isMotivationFull = motivation == 5;
    if (rand() % 10 == 0)
    {
      if (!isMotivationFull)
        printEvents("�����¼�����+1");
      addMotivation(1);//10%���ʼ�����
    }

    if (!larc_isAbroad)
    {
      printEvents("�������˳��");
      //���ڵ�ǰѵ���������ˣ�Ȼ��ӵ���û������������������
      bool isNotFullCharge[15];

      for (int i = 0; i < 15; i++)
      {
        if (persons[i].larc_charge < 3)
        {
          isNotFullCharge[i] = true;
        }
        else
          isNotFullCharge[i] = false;
      }


      int toChargePersons[5] = { -1,-1,-1,-1,-1 };
      int count = 0;



      for (int i = 0; i < 5; i++)
      {
        int p = personDistribution[atTrain][i];
        if (p < 0)break;//û��
        int personType = persons[p].personType;

        if (personType == 2 || personType == 3)//�ɳ����ͷ
        {
          if (persons[p].larc_charge < 3)
          {
            toChargePersons[count] = p;
            count++;
            isNotFullCharge[p] = false;
          }
        }
      }
      int fixedcount = count;//�ڵ�ǰѵ�����س�

      vector<int> randomChargePersons;//���ڵ�ǰѵ���ķ�������ͷ
      for (int i = 0; i < 15; i++)
      {
        if (isNotFullCharge[i])
        {
          randomChargePersons.push_back(i);
        }
      }
      if (randomChargePersons.size() >= 2)
      {
        std::shuffle(randomChargePersons.begin(), randomChargePersons.end(), rand);
      }
      int requiredRandomPersons = 5 - fixedcount;
      if (requiredRandomPersons > randomChargePersons.size())
        requiredRandomPersons = randomChargePersons.size();

      for (int i = 0; i < requiredRandomPersons; i++)
      {
        toChargePersons[count] = randomChargePersons[i];
        assert(count < 5);
        count += 1;
      }


      for (int i = 0; i < 5; i++)
      {
        if (toChargePersons[i] != -1)
          charge(toChargePersons[i], 1);
      }
    }
    else
    {
      printEvents("�������˼�����pt");
      larc_shixingPt += 50;
    }
  }

}
void Game::calculateTrainingValueSingle(int trainType)
{
  //�������ˣ��������������Լ�ֵ
  //failRate[trainType] = 

  //double failRateBasic = calculateFailureRate(trainType);//�������ʧ����

  for (int j = 0; j < 6; j++)
  {
    trainValue[trainType][j] = 0;
  }

  int personCount = 0;//��+npc����ͷ�������������³��ͼ���
  vector<CardTrainingEffect> effects;

  double vitalCostDrop = 1;

  //[��]�湭�쳵(id:30149)�Ĺ��������ʵ�ѵ��60�ɾ��ӳɣ������ڰ������ͷ���һ��֮ǰ����֪����û���ʣ���˼���������ͷ֮����Ҫ��������ſ��Ĳ������д���
  int card30149place = -1;

  for (int i = 0; i < 5; i++)
  {
    int p = personDistribution[trainType][i];
    if (p < 0)break;//û��
    int personType = persons[p].personType;
    if (personType == 1 || personType == 2)//��
    {
      personCount += 1; 
      CardTrainingEffect eff = cardParam[persons[p].cardIdInGame].getCardEffect(*this, trainType, persons[p].friendship, persons[p].cardRecord);
      effects.push_back(eff);
      if (eff.youQing > 0)
      {
        persons[p].isShining = true;
      }
      if (cardParam[persons[p].cardIdInGame].cardID / 10 == 30149)
        card30149place = effects.size() - 1;
    }
    else if (personType == 3)//npc
    {
      personCount += 1;
    }
  }

  trainShiningNum[trainType] = 0;
  double failRateMultiply = 1;
  for (int i = 0; i < effects.size(); ++i) {
    failRateMultiply *= (1 - 0.01 * effects[i].failRateDrop);//ʧ�����½�
    vitalCostDrop *= (1 - 0.01 * effects[i].vitalCostDrop);//���������½�
    if (effects[i].youQing > 0)trainShiningNum[trainType] += 1;//ͳ�Ʋ�Ȧ��
  }

  //[��]�湭�쳵(id:30149)�Ĺ��������ʵ�ѵ��60�ɾ��ӳɣ������ڰ������ͷ���һ��֮ǰ����֪����û���ʣ���˼���������ͷ֮����Ҫ��������ſ��Ĳ������д���
  /*
  if (card30149place >= 0 && trainShiningNum[trainType] == 0)
  {
    effects[card30149place].ganJing -= 60;
  }*/

  failRate[trainType] = calculateFailureRate(trainType,failRateMultiply);

  if (larc_isAbroad)
    larc_shixingPtGainAbroad[trainType] = personCount * 20 + trainShiningNum[trainType] * 20 + (trainType == 4 ? 30 : 50);
  else
    larc_shixingPtGainAbroad[trainType] = 0;

  //�����²���ֵ
  int cardNum = effects.size();
  //1.��ͷ�����ʣ�npcҲ��
  double cardNumMultiplying = 1 + 0.05 * personCount;
  //2.��Ȧ(����ѵ��)���ʣ�ע���Ƿ������Ѿ���getCardEffect�￼�ǹ���
  double youQingMultiplying = 1;
  for (int i = 0; i < cardNum; i++)
    youQingMultiplying *= (1 + 0.01 * effects[i].youQing);
  //3.ѵ������
  double xunLianBonusTotal = 0;
  for (int i = 0; i < cardNum; i++)
    xunLianBonusTotal += effects[i].xunLian;
  double xunLianMultiplying = 1 + 0.01 * xunLianBonusTotal;
  //4.�ɾ�����
  double ganJingBasic = 0.1 * (motivation - 3);
  double ganJingBonusTotal = 0;
  for (int i = 0; i < cardNum; i++)
    ganJingBonusTotal += effects[i].ganJing;
  double ganJingMultiplying = 1 + ganJingBasic * (1 + 0.01 * ganJingBonusTotal);

  //�벻ͬ�����޹ص��ܱ���
  double totalMultiplying = cardNumMultiplying * youQingMultiplying * xunLianMultiplying * ganJingMultiplying;

  //5.����ֵ
  int trainLv = getTrainingLevel(trainType);
  int basicValue[6] = { 0,0,0,0,0,0 };
  for (int i = 0; i < cardNum; i++)
  {
    for (int j = 0; j < 6; j++)
      basicValue[j] += effects[i].bonus[j];
  }
  for (int j = 0; j < 6; j++)
  {
    int b = GameConstants::TrainingBasicValue[trainType][trainLv][j];
    if(b>0)//��������
      basicValue[j] += b;
    else
      basicValue[j] = 0;
  }

  //6.�ɳ���
  double growthRates[6] = { 1,1,1,1,1,1 };
  for (int j = 0; j < 5; j++)
    growthRates[j] = 1.0 + 0.01 * fiveStatusBonus[j];
    //growthRates[j] = 1.0 + 0.01 * GameDatabase::AllUmas[umaId].fiveStatusBonus[j];

  //�²�����ֵ
  int totalValueLower[6];
  for (int j = 0; j < 6; j++)
  {
    int v = int(totalMultiplying * basicValue[j] * growthRates[j]);//����ȡ����
    if (v > 100)v = 100;
    totalValueLower[j] = v;
  }
  
  //7.�ϲ�
  double upperRate = 1;
  upperRate += 0.01 * larc_trainBonus;//�ڴ��ȼӳ�
  if (larc_isAbroad && larc_levels[GameConstants::UpgradeId50pEachTrain[trainType]] >= 3)
    upperRate += 0.5;//����+50%
  if (larc_levels[8] >= 1)//�����ڶ���������ѵ��+5%
    upperRate += 0.05;
  if (larc_levels[7] >= 3 && trainShiningNum[trainType] > 0)//����+20%
    upperRate *= 1.2;


  for (int j = 0; j < 6; j++)
  {
    int lower = totalValueLower[j];
    if (lower == 0)
    {
      trainValue[trainType][j] = 0;
      continue;
    }
    int total = int(double(lower + larc_staticBonus[j]) * upperRate);
    int upper = total - lower;
    if (upper > 100)upper = 100;
    trainValue[trainType][j] = lower + upper;
  } 
    


  //����
  double vitalChange=GameConstants::TrainingBasicValue[trainType][trainLv][6];
  for (int i = 0; i < cardNum; i++)
    vitalChange += effects[i].vitalBonus;
  if (vitalChange < 0)//��������ʱ������Ƿ�������-20%
  {
    vitalChange *= vitalCostDrop;
    if (larc_isAbroad && larc_levels[6] >= 3)//����-20%
    vitalChange *= 0.8;
  }
  int vitalChangeInt = round(vitalChange);
  if (vitalChangeInt > maxVital - vital)vitalChangeInt = maxVital - vital;
  if (vitalChangeInt < - vital)vitalChangeInt = - vital;


  trainValue[trainType][6] = vitalChangeInt;
}
void Game::calculateSS()
{
  for (int i = 0; i < 5; i++)larc_ssValue[i] = 0;
  int linkn = 0;
  for (int i = 0; i < larc_ssPersonsCount; i++)
  {
    if (persons[larc_ssPersons[i]].larc_isLinkCard)
      linkn += 1;
  }
  int p = larc_ssPersonsCount;

  int totalValue =
    turn < 40 ?
    5 * p + (4 * p + 2 * linkn) * (0.8 + larc_supportPtAll * 6e-6) :
    5 * p + (5 * p + 2 * linkn) * (1.0 + larc_supportPtAll * 6e-6);//�ճ�����ϵĹ�ʽ�����ͦ��ĵ�Ӧ�ò�̫Ӱ�����
  if (larc_isSSS)totalValue += 75;

  totalValue -= (4 * p + 2 * linkn);//4 * p + 2 * linkn�ǰ�����ͷ���Է���
  //ʣ�µ�ƽ������
  int div5 = totalValue / 5;
  for (int i = 0; i < 5; i++)larc_ssValue[i] = div5;
  for (int i = 0; i < totalValue - div5 * 5; i++)larc_ssValue[i] += 1;

  //��ͷ����
  for (int i = 0; i < larc_ssPersonsCount; i++)
  {
    auto& p = persons[larc_ssPersons[i]];
    larc_ssValue[p.larc_statusType] += (p.larc_isLinkCard ? 6 : 4);
  }

  //ss��ʧ���ʣ�Ϲ�µġ�
  //Ŀǰ��֪20�弰���ϲ���ʧ�ܣ�˫Ȧ��,10~19�ǵ�Ȧ��1~9�����ǣ�0�ǲ�
  //���Ǿ�����ٸ���û������Ҿ���Ҳ����Ҫ����Ϊû������ʱ��϶�����Ϣ/�����ss���������ss�ɹ�����һ���غ�Ҳû��ѵ��
  if(larc_isSSS)larc_ssFailRate = 0;
  else if (vital < 1)larc_ssFailRate = 80;
  else if (vital < 10)larc_ssFailRate = 50;
  else if (vital < 20)larc_ssFailRate = 30;
  else larc_ssFailRate = 0;
}
bool Game::applyTraining(std::mt19937_64& rand, Action action)
{
  assert(stageInTurn == 1);
  stageInTurn = 2;
  if (isRacing)
  {
    assert(false && "���������о籾��������checkEventAfterTrain()�ﴦ������applyTraining");
    return false;//���������о籾��������checkEventAfterTrain()�ﴦ���൱�ڱ����غ�ֱ���������������������
  }
  if (action.train == 6)//��Ϣ
  {
    if (larc_isAbroad)
    {
      addVital(50);
      addMotivation(1);
      larc_shixingPt += 100;
    }
    else
    {
      int r = rand() % 100;
      if (r < 25)
        addVital(70);
      else if (r < 82)
        addVital(50);
      else
        addVital(30);
    }

  }
  else if (action.train == 9)//����
  {
    if (turn <= 12 || larc_isAbroad)
    {
      printEvents("Cannot race now.");    // ����������
      return false;
    }
    addAllStatus(1);//������
    runRace(2, 40);//���ԵĽ���

    //����̶�15
    addVital(-15);
    if (rand() % 10 == 0)
      addMotivation(1);

    //���������ͷ���
    for (int i = 0; i < 2; i++)
      charge(rand() % 15, 1);
  }
  else if (action.train == 8)//��ͨ���
  {
    if (larc_isAbroad)
    {
      printEvents("����ֻ����Ϣ���������");
      return false;
    }
    //���ò�����ˣ���50%��2���飬50%��1����10����
    if (rand() % 2)
      addMotivation(2);
    else
    {
      addMotivation(1);
      addVital(10);
    }

  }
  else if (action.train == 7)//�������
  {
    if (larc_isAbroad)
    {
      printEvents("����ֻ����Ϣ�������������");
      return false;
    }
    if (!larc_zuoyueOutgoingUnlocked || larc_zuoyueOutgoingUsed == 5)
    {
      printEvents("���˳���δ��������������");
      return false;
    }
    handleFriendOutgoing();
  }
  else if (action.train == 5)//ss match
  {
    if (larc_isAbroad)
    {
      printEvents("���ⲻ��SS");
      return false;
    }
    if (larc_ssPersonsCount == 0)
    {
      printEvents("û��������ͷ����SS");
      return false;
    }
    runSS(rand);
    if (larc_ssWin >= 5)
    {
      unlockUpgrade(0);
      unlockUpgrade(1);
    }
    if (larc_ssWin >= 10)
    {
      unlockUpgrade(2);
      unlockUpgrade(3);
    }
  }
  else if (action.train <= 4 && action.train >= 0)//����ѵ��
  {
    //��������pt��ӳ�
    if (larc_isAbroad)
    {
      bool anyUpgrade = false;
      if (action.buy50p)
      {
        int upgradeIdx = GameConstants::UpgradeId50pEachTrain[action.train];
        if (larc_levels[upgradeIdx] < 3)
        {
          bool suc = tryBuyUpgrade(upgradeIdx, 3);
          if (suc)
            anyUpgrade = true;
          else
          {
            printEvents("����+50%");
            return false;
          }
        }
        else
        {
          printEvents("��Ҫ�ظ�����+50%");
          return false;
        }
      }
      if (action.buyFriend20)
      {
        assert(false && "������20%�Ѿ��ĳ�ȫ�Զ�����");
        if (larc_levels[7] < 3)
        {
          bool suc = tryBuyUpgrade(7, 3);
          if (suc)
            anyUpgrade = true;
          else
          {
            printEvents("����+20%");
            return false;
          }
        }
        else
        {
          printEvents("��Ҫ�ظ���������+20%");
          return false;
        }
      }
      if (action.buyPt10)
      {
        if (larc_levels[5] < 3)
        {
          bool suc = tryBuyUpgrade(5, 3);
          if (suc)
            anyUpgrade = true;
          else
          {
            printEvents("����pt+10");
            return false;
          }
        }
        else
        {
          printEvents("��Ҫ�ظ�����pt+10");
          return false;
        }
      }
      if (action.buyVital20)
      {
        if (larc_levels[6] < 3)
        {
          bool suc = tryBuyUpgrade(6, 3);
          if (suc)
            anyUpgrade = true;
          else
          {
            printEvents("��������-20%");
            return false;
          }
        }
        else
        {
          printEvents("��Ҫ�ظ���������-20%");
          return false;
        }
      }
      if (anyUpgrade)
        calculateTrainingValue();
    }

    if (turn == 41)//�ڶ��ο�����ǰ����һ���غϣ��������+20%���飨��pt+10����ѵ���ɹ���������Ԥ����debuff����
    {
      int shixingPtAssumeSuccess = larc_shixingPt + larc_shixingPtGainAbroad[action.train];
      //������������������Ƚ��鷳��ֱ�Ӽ��費�ᷢ���������
      assert(!larc_allowedDebuffsFirstLarc[7] && "��������������������+20%�����Ǹ�debuff��");
      assert(!larc_allowedDebuffsFirstLarc[5] && "����������������pt+10�����Ǹ�debuff��");
      int removeDebuffCost = removeDebuffsFirstNCost(8);
      int remainPt = shixingPtAssumeSuccess - removeDebuffCost;
      int remainPtAssumeFail = larc_shixingPt - removeDebuffCost;

      bool buy20p = false;
      if (larc_levels[7] < 3)
      {
        if (remainPtAssumeFail >= 300 || (remainPt >= 300 && failRate[action.train] < 90))
          buy20p = true;
      }

      if (buy20p)
      {
        remainPt -= 300;
        remainPtAssumeFail -= 300;
      }

      bool buy10pt = remainPt >= 200 && failRate[action.train] < 5 && larc_levels[5] < 3;
      


      bool anyUpgrade = false;
      if (buy20p)
      {
        bool suc = tryBuyUpgrade(7, 3);
        if (suc)
        {
          printEvents("ai����������+20%��");
          anyUpgrade = true;
        }
      }
      if (buy10pt && larc_levels[7] == 3)
      {
        bool suc = tryBuyUpgrade(5, 3);
        if (suc)
        {
          printEvents("ai������pt+10��");
          anyUpgrade = true;
        }
      }
      if (anyUpgrade)
        calculateTrainingValue();
    }

    if (turn >= 43)//�ڶ��ο����ź���������+20%�����������Ȼ����������pt+10��������
    {
      bool anyUpgrade = false;
      if (larc_levels[7] < 3)
      {
        bool suc = tryBuyUpgrade(7, 3);
        if (suc)
        {
          printEvents("ai����������+20%��");
          anyUpgrade = true;
        }
      }
      if (larc_levels[5] < 3 && larc_levels[7] == 3)
      {
        bool suc = tryBuyUpgrade(5, 3);
        if (suc)
        {
          printEvents("ai������pt+10��");
          anyUpgrade = true;
        }
      }
      if (anyUpgrade)
        calculateTrainingValue();
    }

    if (rand() % 100 < failRate[action.train])//ѵ��ʧ��
    {
      if (failRate[action.train] >= 20 && (rand() % 100 < failRate[action.train]))//ѵ����ʧ�ܣ�������Ϲ�µ�
      {
        printEvents("ѵ����ʧ�ܣ�");
        addStatus(action.train, -10);
        if (fiveStatus[action.train] > 1200)
          addStatus(action.train, -10);//��Ϸ��1200���Ͽ����Բ��۰룬�ڴ�ģ�������Ӧ1200���Ϸ���
        //�����2��10�������ĳ�ȫ����-4���������
        for (int i = 0; i < 5; i++)
        {
          addStatus(i, -4);
          if (fiveStatus[i] > 1200)
            addStatus(i, -4);//��Ϸ��1200���Ͽ����Բ��۰룬�ڴ�ģ�������Ӧ1200���Ϸ���
        }
        addMotivation(-3);
        addVital(10);
      }
      else//Сʧ��
      {
        printEvents("ѵ��Сʧ�ܣ�");
        addStatus(action.train, -5);
        if (fiveStatus[action.train] > 1200)
          addStatus(action.train, -5);//��Ϸ��1200���Ͽ����Բ��۰룬�ڴ�ģ�������Ӧ1200���Ϸ���
        addMotivation(-1);
      }
    }
    else
    {
      //�ȼ���ѵ��ֵ
      for (int i = 0; i < 5; i++)
        addStatus(i, trainValue[action.train][i]);
      skillPt += trainValue[action.train][5];
      addVital(trainValue[action.train][6]);

      int chargeNum = trainShiningNum[action.train] + 1;//�伸���
      if (turn < 2 || larc_isAbroad)chargeNum = 0;//ǰ���غ���Զ���޷����
      vector<int> hintCards;//���ļ����������̾����
      bool clickZuoyue = false;//���ѵ����û������
      for (int i = 0; i < 5; i++)
      {
        int p = personDistribution[action.train][i];
        if (p < 0)break;//û��
        int personType = persons[p].personType;

        if (personType == 1)//������
        {
          addJiBan(p, 4);
          clickZuoyue = true;
        }
        else if (personType==2)//��ͨ��
        {
          addJiBan(p, 7);
          if (!larc_isAbroad)charge(p, chargeNum);
          if(persons[p].isHint)
            hintCards.push_back(p);
        }
        else if (personType == 3)//npc
        {
          if (!larc_isAbroad)charge(p, chargeNum);
        }
        else if (personType == 4)//���³�
        {
          skillPt += 2;
          addJiBan(p, 7);
        }
        else if (personType == 5)//����
        {
          addStatus(action.train, 2);
          addJiBan(p, 7);
        }
        else if (personType == 6)//�޿�����
        {
          skillPt += 2;
          addJiBan(p, 7);
        }
      }

      if (hintCards.size() > 0)
      {
        int hintCard = hintCards[rand() % hintCards.size()];//���һ�ſ���hint

        addJiBan(hintCard, 5);
        auto& hintBonus = cardParam[persons[hintCard].cardIdInGame].hintBonus;
        for (int i = 0; i < 5; i++)
          addStatus(i, hintBonus[i]);
        skillPt += hintBonus[5];
      }

      if (clickZuoyue)
        handleFriendClickEvent(rand, action.train);
      
      if (larc_isAbroad)
        larc_shixingPt += larc_shixingPtGainAbroad[action.train];
      else
        addTrainingLevelCount(action.train, 1);

    }
  }
  else
  {
    printEvents("δ֪��ѵ����Ŀ");
    return false;
  }
  return true;
}


bool Game::isLegal(Action action) const
{
  if (isRacing)
  {
    assert(false && "���������о籾��������checkEventAfterTrain()�ﴦ������applyTraining");
    return false;//���������о籾��������checkEventAfterTrain()�ﴦ���൱�ڱ����غ�ֱ���������������������
  }

  if (action.buy50p || action.buyFriend20 || action.buyPt10 || action.buyVital20)
  {
    if (!(action.train <= 4 && action.train >= 0 && larc_isAbroad))
      return false;
  }

  if (action.train == 6)//��Ϣ
  {
    return true;
  }
  else if (action.train == 9)//����
  {
    if (turn <= 12 || larc_isAbroad)
    {
      return false;
    }
    return true;
  }
  else if (action.train == 8)//��ͨ���
  {
    if (larc_isAbroad)
    {
      return false;
    }
    return true;
  }
  else if (action.train == 7)//�������
  {
    if (larc_isAbroad)
    {
      return false;
    }
    if (!larc_zuoyueOutgoingUnlocked || larc_zuoyueOutgoingUsed == 5)
    {
      return false;
    }
    return true;
  }
  else if (action.train == 5)//ss match
  {
    if (larc_isAbroad)
    {
      return false;
    }
    if (larc_ssPersonsCount == 0)
    {
      return false;
    }
    return true;
  }
  else if (action.train <= 4 && action.train >= 0)//����ѵ��
  {
    //��������pt��ӳ�
    if (larc_isAbroad)
    {
      int remainPt = larc_shixingPt;
      if (action.buy50p)
      {
        int upgradeIdx = GameConstants::UpgradeId50pEachTrain[action.train];
        int cost = buyUpgradeCost(upgradeIdx, 3);
        if (cost <= 0)return false;
        if (cost > remainPt)return false;
        remainPt -= cost;
      }
      if (action.buyFriend20)
      {
        assert(false && "������20%�Ѿ��ĳ�ȫ�Զ�����");
        return false;
      }
      if (action.buyPt10)
      {
        if (turn >= 41)
          return false;
        int cost = buyUpgradeCost(5, 3);
        if (cost <= 0)return false;
        if (cost > remainPt)return false;
        remainPt -= cost;
      }
      if (action.buyVital20)
      {
        if (turn <= 59)
          return false;
        int cost = buyUpgradeCost(6, 3);
        if (cost <= 0)return false;
        if (cost > remainPt)return false;
        remainPt -= cost;
      }
    }
    return true;
  }
  else
  {
    assert(false && "δ֪��ѵ����Ŀ");
    return false;
  }
  return false;
}



float Game::getSkillScore() const
{
  float scorePtRate = isQieZhe ? GameConstants::ScorePtRateQieZhe : GameConstants::ScorePtRate;
  return scorePtRate * skillPt + skillScore;
}

int Game::finalScore() const
{
  int total = 0;
  for (int i = 0; i < 5; i++)
    total += GameConstants::FiveStatusFinalScore[min(fiveStatus[i],fiveStatusLimit[i])];
  
  total += getSkillScore();
  return total;
}

bool Game::isEnd() const
{
  return turn >= TOTAL_TURN;
}

int Game::getTrainingLevel(int item) const
{
  int level ;
  if(larc_isAbroad)
    level = 5;
  else
  {
    assert(trainLevelCount[item] <= 16, "ѵ���ȼ���������16");
    level = trainLevelCount[item] / 4;
    if (level > 4)level = 4;
  }
  return level;
}

double Game::sssProb(int ssWinSinceLastSSS) const
{
  return ssWinSinceLastSSS >= 8 ? 1.0 : 0.12 + 0.056 * ssWinSinceLastSSS;
}




void Game::checkEventAfterTrain(std::mt19937_64& rand)
{
  assert(stageInTurn == 2 || isRacing);
  stageInTurn = 0;



  checkFixedEvents(rand);

  checkRandomEvents(rand);


  //�غ���+1
  turn++;
  if (turn < TOTAL_TURN)
  {
    isRacing = GameConstants::LArcIsRace[turn];
    larc_isAbroad = (turn >= 36 && turn <= 42) || (turn >= 60 && turn <= 67);
    if (isRacing)
      checkEventAfterTrain(rand);//��������غ�
  }
  else
  {
    printEvents("���ɽ���!");
    printEvents("��ĵ÷��ǣ�" + to_string(finalScore()));
  }

}

void Game::checkFixedEvents(std::mt19937_64& rand)
{
  //������̶ֹ��¼�
  int oldSupportPt = larc_supportPtAll;
  larc_supportPtAll += GameConstants::LArcSupportPtGainEveryTurn[turn];
  checkSupportPtEvents(oldSupportPt, larc_supportPtAll);//�ڴ��������¼�

  if (turn == 1)//����npc
  {
    initNPCsTurn3(rand);
  }
  else if (turn == 11)//����ս
  {
    runRace(3, 30);
  }
  else if (turn == 18)
  {
    larc_shixingPt += 100;
  }
  else if (turn == 23)//��һ�����
  {
    tryRemoveDebuffsFirstN(2);
    //tryRemoveDebuffsFirstN(4);
    //�������Ŀ��
    addAllStatus(3);
    skillPt += 20;

    //larc1
    runRace(3, 10);
    larc_shixingPt += 50;

    //���˿�����
    if (larc_zuoyueOutgoingUnlocked)
    {
      addMotivation(1);
      addStatusZuoyue(3, 15);
      skillPt += 10;//���ܵ�Ч
    }

    printEvents("larc1����");

  }
  else if (turn == 28)
  {
    addMotivation(1);
    addAllStatus(2);
  }
  else if (turn == 29)//�ڶ���̳�
  {

    for (int i = 0; i < 5; i++)
      addStatus(i, zhongMaBlueCount[i] * 6); //�����ӵ���ֵ

    double factor = double(rand() % 65536) / 65536 * 2;//�籾�������0~2��
    for (int i = 0; i < 5; i++)
      addStatus(i, int(factor*zhongMaExtraBonus[i])); //�籾����
    skillPt += int((0.5 + 0.5 * factor) * zhongMaExtraBonus[5]);//���߰��㼼�ܵĵ�Чpt

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += zhongMaBlueCount[i] * 2; //��������--�������ֵ��18�����μ̳й��Ӵ�Լ36���ޣ�ÿ��ÿ��������+1���ޣ�1200�۰��ٳ�2

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += rand() % 8; //��������--�����μ̳��������

    printEvents("�ڶ���̳�");
  }
  else if (turn == 33)//�ձ��±�
  {
    runRace(5, 45);

    for (int i = 0; i < 3; i++)
      charge(rand() % 15, 1);//����������˳�硣���ų���������Ѿ�����
    printEvents("�ձ��±Ƚ���");

  }
  else if (turn == 35)//larc2
  {
    runRace(5, 20);
    larc_shixingPt += 50;
    unlockUpgrade(4);
    unlockUpgrade(5);
    skillScore += 170;//���м��ܵȼ�+1

    printEvents("larc2������׼��Զ��");

  }
  else if (turn == 40)//�����
  {
    runRace(5, 40);
    larc_shixingPt += 50;
    unlockUpgrade(6);
    unlockUpgrade(7);

    printEvents("����ͽ���");

  }
  else if (turn == 42)//������1
  {
    bool willWin = tryRemoveDebuffsFirstN(8);//�ܳɹ����������趨��debuff��ģ�����ͼ�����Ի�ʤ��������Ϊ���ܻ�ʤ
    if (willWin)
    {
      runRace(7, 50);
      larc_shixingPt += 80;
      skillPt += 15;//���ܵ�Ч
      unlockUpgrade(9);
      printEvents("�ڶ��꿭���Ž������������������趨��debuff��ai������Ի�ʤ");
    }
    else
    {
      runRace(5, 50);
      larc_shixingPt += 50;

      printEvents("�ڶ��꿭���Ž�������û�����������趨��debuff��ai���費���Ի�ʤ");
    }

  }
  else if (turn == 43)//�̶�������
  {
    addMotivation(-2);
  }
  else if (turn == 44)//�̶�������
  {
    addMotivation(3);
    larc_shixingPt += 30;
  }
  else if (turn == 47)//�ڶ������
  {
    addVital(30);
  }
  else if (turn == 53)//������̳�&larc3
  {
    //�������Ŀ��
    addAllStatus(7);
    skillPt += 50;
    runRace(7, 30);
    larc_shixingPt += 80;
    skillScore += 170;//���м��ܵȼ�+1

    for (int i = 0; i < 5; i++)
      addStatus(i, zhongMaBlueCount[i] * 6); //�����ӵ���ֵ

    double factor = double(rand() % 65536) / 65536 * 2;//�籾�������0~2��
    for (int i = 0; i < 5; i++)
      addStatus(i, int(factor * zhongMaExtraBonus[i])); //�籾����
    skillPt += int((0.5 + 0.5 * factor) * zhongMaExtraBonus[5]);//���߰��㼼�ܵĵ�Чpt

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += zhongMaBlueCount[i] * 2; //��������--�������ֵ��18�����μ̳й��Ӵ�Լ36���ޣ�ÿ��ÿ��������+1���ޣ�1200�۰��ٳ�2

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += rand() % 8; //��������--�����μ̳��������

    printEvents("������̳�");
  }
  else if (turn == 59)//��ڣ����&larc4
  {
    runRace(5, 45);
    runRace(10, 40);
    larc_shixingPt += 100;

    addAllStatus(10);
    addMotivation(1);
    skillPt += 20;//���ܵ�Ч
    skillScore += 170;//���м��ܵȼ�+1

    if(larc_ssWin>=40)
      unlockUpgrade(8);

  }
  else if (turn == 64)//������
  {
    runRace(7, 40);
    larc_shixingPt += 100;

    printEvents("�����ͽ���");

  }
  else if (turn == 66)//������2����Ϸ����
  {

    tryBuyUpgrade(9, 2);//����������������������debuff��ֵǮ
    bool willWin = tryRemoveDebuffsFirstN(9);//�ܳɹ����������趨��debuff��ģ�����ͼ�����Ի�ʤ��������Ϊ���ܻ�ʤ
    if (larc_levels[9] >= 2)//�������һ��������
    {
      if (willWin)
        runRace(30, 140);
      else
        runRace(25, 120);
    }
    else
    {
      if (willWin)
        runRace(10, 60);
      else
        runRace(5, 40);
    }

    //���˿��¼�
    if (larc_zuoyueOutgoingUsed==5)//����������
    {
      addStatusZuoyue(2, 15);
      addStatusZuoyue(3, 25);
      addStatusZuoyue(5, 30);
    }
    else if(larc_zuoyueOutgoingUnlocked)
    {
      addStatusZuoyue(2, 15);
      addStatusZuoyue(3, 18);
      addStatusZuoyue(5, 20);
    }

    //����
    if (persons[16].friendship >= 100)
    {
      addAllStatus(5);
      skillPt += 20;
    }
    else if (persons[16].friendship >= 80)
    {
      addAllStatus(3);
      skillPt += 10;
    }
    else
    {
      skillPt += 5;
    }


    addAllStatus(20);
    skillPt += 60;
    skillPt += 10;//���ܵ�Ч
    if (larc_levels[9] >= 1)
    {
      addAllStatus(5);
      skillPt += 12;//���ܵ�Ч
    }
    if (willWin)
    {
      addAllStatus(5);
      skillPt += 18;//���ܵ�Ч
    }

    printEvents("��Ϸ����");
  }
}

void Game::checkSupportPtEvents(int oldSupportPt, int newSupportPt)
{
  int bound;
  bound = 20 * 1700 - 85;//�ڴ��ȼ�����SupportPt/170�������룬����20.0%�ڴ��ȶ�Ӧ����20 * 1700 - 85
  if (oldSupportPt < bound && newSupportPt >= bound)
  {
    printEvents("�ڴ��ȴﵽ20%");
    addAllStatus(2);
    for (int i = 0; i < 5; i++)
      addTrainingLevelCount(i, 4);
  }
  bound = 40 * 1700 - 85;//�ڴ��ȼ�����SupportPt/170�������룬����20.0%�ڴ��ȶ�Ӧ����20 * 1700 - 85
  if (oldSupportPt < bound && newSupportPt >= bound)
  {
    printEvents("�ڴ��ȴﵽ40%");
    addAllStatus(3);
  }
  bound = 60 * 1700 - 85;//�ڴ��ȼ�����SupportPt/170�������룬����20.0%�ڴ��ȶ�Ӧ����20 * 1700 - 85
  if (oldSupportPt < bound && newSupportPt >= bound)
  {
    printEvents("�ڴ��ȴﵽ60%");
    addAllStatus(4);
    for (int i = 0; i < 5; i++)
      addTrainingLevelCount(i, 4);
  }
  bound = 80 * 1700 - 85;//�ڴ��ȼ�����SupportPt/170�������룬����20.0%�ڴ��ȶ�Ӧ����20 * 1700 - 85
  if (oldSupportPt < bound && newSupportPt >= bound)
  {
    printEvents("�ڴ��ȴﵽ80%");
    addAllStatus(5);
  }
  bound = 100 * 1700 - 85;//�ڴ��ȼ�����SupportPt/170�������룬����20.0%�ڴ��ȶ�Ӧ����20 * 1700 - 85
  if (oldSupportPt < bound && newSupportPt >= bound)
  {
    printEvents("�ڴ��ȴﵽ100%");
    addAllStatus(5);
    for (int i = 0; i < 5; i++)
      addTrainingLevelCount(i, 4);
  }

}

void Game::checkRandomEvents(std::mt19937_64& rand)
{
  if (larc_isAbroad)
    return;//Զ���ڼ䲻�ᷢ����������¼�

  //���˻᲻���������
  if (larc_zuoyueFirstClick && (!larc_zuoyueOutgoingRefused) && (!larc_zuoyueOutgoingUnlocked))
  {
    double unlockOutgoingProb = persons[17].friendship >= 60 ?
      GameConstants::FriendUnlockOutgoingProbEveryTurnHighFriendship :
      GameConstants::FriendUnlockOutgoingProbEveryTurnLowFriendship;
    if (randBool(rand, unlockOutgoingProb))//����
    {
      handleFriendUnlock(rand);
    }
  }

  //ģ���������¼�

  //֧Ԯ�������¼��������һ������5�
  int nonAbroadTurns = turn < 40 ? turn : turn - 7;
  double p = 0.4;
  if (randBool(rand, p))
  {
    int card = rand() % normalCardCount;
    addJiBan(card, 5);
    //addAllStatus(4);
    addStatus(rand() % 5, eventStrength);
    skillPt += eventStrength;
    printEvents("ģ��֧Ԯ������¼���" + cardParam[persons[card].cardIdInGame].cardName + " ���+5��pt���������+" + to_string(eventStrength));

    //֧Ԯ��һ����ǰ�����¼�������
    if (randBool(rand, 0.2 * (1.0 - turn * 1.0 / TOTAL_TURN)))
    {
      addMotivation(1);
      printEvents("ģ��֧Ԯ������¼�������+1");
    }
    if (randBool(rand, 0.3))
    {
      addVital(10);
      printEvents("ģ��֧Ԯ������¼�������+10");
    }
    else if (randBool(rand, 0.07))
    {
      addVital(-10);
      printEvents("ģ��֧Ԯ������¼�������-10");
    }
  }

  //ģ����������¼�
  if (randBool(rand, 0.1))
  {
    addAllStatus(3);
    printEvents("ģ����������¼���ȫ����+3");
  }

  //������
  if (randBool(rand, 0.10))
  {
    addVital(5);
    printEvents("ģ������¼�������+5");
  }

  //��30�������Է��¼���
  if (randBool(rand, 0.02))
  {
    addVital(30);
    printEvents("ģ������¼�������+30");
  }

  //������
  if (randBool(rand, 0.02))
  {
    addMotivation(1);
    printEvents("ģ������¼�������+1");
  }

  //������
  if (randBool(rand, 0.04))
  {
    addMotivation(-1);
    printEvents("ģ������¼���\033[0m\033[33m����-1\033[0m\033[32m");
  }

}

void Game::applyTrainingAndNextTurn(std::mt19937_64& rand, Action action)
{
  assert(stageInTurn == 1);
  assert(turn < TOTAL_TURN && "Game::applyTrainingAndNextTurn��Ϸ�ѽ���");
  assert(!isRacing && "�����غ϶���checkEventAfterTrain��������");
  bool suc = applyTraining(rand, action);
  assert(suc && "Game::applyTrainingAndNextTurnѡ���˲��Ϸ���ѵ��");

  checkEventAfterTrain(rand);
  if (isEnd()) return;

  assert(!isRacing && "�����غ϶���checkEventAfterTrain��������");

  // ���Ҫ֧������Ϸ�иı�����ʣ���Ҫ��������µ����ʵ�ֵ
  randomDistributeCards(rand);
}

// ����PersonDistribution cardType��Person.friendshipȷ��ĳ�����Ƿ�����
// ��Ϊ������ݷ�ɢ�ڸ��������Բ���getCardEffect������ʱ��ֻ����Game���Խϴ�Ŀ����ж�
bool Game::cardIsShining(int which) const
{
    // �Ŷӿ����Ҳ��ܡ���ͨ����������Щ
    if (which > 0 && which <= 5 && cardParam[which].cardType < 5 && persons[which].friendship >= 80)
    {
        // �����ж��Ƿ��ó�ѵ��
        for (int i=0; i<5; ++i)
            for (int j=0; j<5; ++j)
                if (personDistribution[i][j] == which)
                    return (i == cardParam[which].cardType);
    }
    return false;
}

// ����PersonDistribution cardType��Person.friendshipȷ��ѵ����Ȧ����
// ��Ϊ������ݷ�ɢ�ڸ��������Բ���getCardEffect������ʱ��ֻ����Game���Խϴ�Ŀ����ж�
// ����ʹ����getCardEffectά����trainShiningNum��Ա����ΪgetCardEffect�Լ���Ҫ���ø÷�����
bool Game::trainShiningCount(int train) const
{
    int count = 0;
    if (train > 0 && train <= 5)
    {
        for (int i = 0; i < 5; ++i)
        { 
            int which = personDistribution[train][i];
            if (which >= 0 && which <= 5 && cardParam[which].cardType == train && persons[which].friendship >= 80)
                ++count;
        }
    }
    return count;
}
