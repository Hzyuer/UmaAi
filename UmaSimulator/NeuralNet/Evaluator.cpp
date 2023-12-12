#include <cassert>
#include <iostream>
#include "Evaluator.h"
#include "../Search/Search.h"

//void Evaluator::evaluate(const Game* games, const float* targetScores, int mode, int gameNum)
//{
//  assert(false);
//}

void Evaluator::evaluateSelf(int mode, const SearchParam& param)
{
  if (model == NULL)//û�����磬��д�߼�
  {
    if (mode == 0)//value�������վֲſɼ���
    {
      for (int i = 0; i < maxBatchsize; i++)
      {
        const Game& game = gameInput[i];
        assert(game.isEnd() && "��������ʱ��ֻ����Ϸ������ſɼ���value");
        int score = game.finalScore();

        auto& v = valueResults[i];
        v.scoreMean = score;
        v.scoreStdev = 0; //�������վֵ������������Ϊ0
        v.value = score;
      }
    }
    else if (mode == 1)//policy����д�߼������ŵ�ѡ����1����������0
    {
      for (int i = 0; i < maxBatchsize; i++)
      {
        actionResults[i] = handWrittenStrategy(gameInput[i]);
      }
    }
  }
  else
  {
    for (int i = 0; i < maxBatchsize; i++)
    {
      const Game& game = gameInput[i];
      if(!game.isEnd())
        game.getNNInputV1(inputBuf.data() + i * NNINPUT_CHANNELS_V1, param);
    }
    model->evaluate(this, inputBuf.data(), outputBuf.data(), maxBatchsize);
    if (mode == 0)//value
    {
      for (int i = 0; i < maxBatchsize; i++)
      {
        const Game& game = gameInput[i];
        if (game.isEnd())
        {
          int score = game.finalScore();

          auto& v = valueResults[i];
          v.scoreMean = score;
          v.scoreStdev = 0; //�������վֵ������������Ϊ0
          v.value = score;
        }
        else
        {
          valueResults[i] = extractValueFromNNOutputBuf(outputBuf.data() + NNOUTPUT_CHANNELS_V1 * i);
        }
      }
    }
    else if (mode == 1)//policy
    {
      for (int i = 0; i < maxBatchsize; i++)
      {
        const Game& game = gameInput[i];
        if (game.isEnd())
        {
        }
        else
        {
          actionResults[i] = extractActionFromNNOutputBuf(outputBuf.data() + NNOUTPUT_CHANNELS_V1 * i, game);
        }
      }
    }
    else assert(false);
  }
}

ModelOutputValueV1 Evaluator::extractValueFromNNOutputBuf(float* buf)
{
  ModelOutputValueV1 v;
  v.scoreMean = 30000 + 200 * buf[NNOUTPUT_CHANNELS_POLICY_V1 + 0];
  v.scoreStdev = 100 * buf[NNOUTPUT_CHANNELS_POLICY_V1 + 1]; 
  v.value = 30000 + 200 * buf[NNOUTPUT_CHANNELS_POLICY_V1 + 2];
  return v;
}

Action Evaluator::extractActionFromNNOutputBuf(float* buf, const Game& game)
{
  Action action;
  action.train = -1;
  action.buy50p = false;
  action.buyFriend20 = false;
  action.buyPt10 = false;
  action.buyVital20 = false;
  float bestPolicy = -1e20;
  for (int i = 0; i < 10; i++)
  {
    if (buf[i] > bestPolicy)
    {
      Action action1 = action;
      action1.train = i;
      if (game.isLegal(action1))
      {
        action.train = i;
        bestPolicy = buf[i];
      }
    }
  }
  if (action.train < 5)
  {
    float bestBuyPolicy = -1e20;
    for (int i = 0; i < Search::buyBuffChoiceNum(game.turn); i++)
    {
      Action action1 = Search::buyBuffAction(i, game.turn);
      action1.train = action.train;
      if (!game.isLegal(action1))
        continue;
      float p = 0;
      if (action1.buy50p)
        p += buf[10 + action1.train];
      if (action1.buyPt10)
        p += buf[15];
      if (action1.buyVital20)
        p += buf[16];
      if (action1.buy50p && (action1.buyPt10 || action1.buyVital20))
        p += buf[17];
      if (p > bestBuyPolicy)
      {
        bestBuyPolicy = p;
        action = action1;
      }
    }
  }
  return action;
}

Evaluator::Evaluator()
{
}

Evaluator::Evaluator(Model* model, int maxBatchsize):model(model), maxBatchsize(maxBatchsize)
{
  gameInput.resize(maxBatchsize);
  inputBuf.resize(NNINPUT_CHANNELS_V1 * maxBatchsize);
  outputBuf.resize(NNOUTPUT_CHANNELS_V1 * maxBatchsize);
  valueResults.resize(maxBatchsize);
  //policyResults.resize(maxBatchsize);
  actionResults.resize(maxBatchsize);

#if USE_BACKEND == BACKEND_CUDA
  inputBufSparseIdx.reserve(maxBatchsize * NNINPUT_CHANNELS_V1);
  inputBufSparseValue.reserve(maxBatchsize * NNINPUT_CHANNELS_V1);
#endif
}


static double vitalEvaluation(int vital,int maxVital)
{
  if (vital <= 50)
    return 2.0 * vital;
  else if (vital <= 70)
    return 1.5 * (vital - 50) + vitalEvaluation(50, maxVital);
  else if (vital <= maxVital - 10)
    return 1.0 * (vital - 70) + vitalEvaluation(70, maxVital);
  else
    return 0.5 * (vital - (maxVital - 10)) + vitalEvaluation(maxVital - 10, maxVital);
}


static const int finalBonus = 70;//�籾����ʱ�Ĺ̶���������

const double jibanValue = 2.5;
const double friendValue_nonAbroad = 10;//�Ǻ�������˵ķ�����������������
const double friendValue_abroad = 10;//��������˵ķ�������������������pt
const double shixingPtValueSecondYear = 0.3;//�ڶ���Զ������pt��ֵ����������Ϊ0
const double ssPriorityValue = 130;//�ܹ�5��ʱ����ѵ��value�������ֵ������ss�����򱬵������һ�غϵ�ss��������
const double vitalFactor = 1;
const double smallFailValue = -30;
const double bigFailValue = -90;
const double wizFailValue = 5;
const double statusWeights[5] = { 1.0,1.0,1.0,1.0,1.0 };
const double ptWeight = 0.5;
const double restValueFactor = 1.5;//��Ϣ��ֵȨ��
const float remainStatusFactorEachTurnAbroad = 40;//������ʱ��ÿ�غ�Ԥ�����٣�Զ����
const float remainStatusFactorEachTurnBeforeAbroad = 20;//������ʱ��ÿ�غ�Ԥ�����٣�Զ��ǰ��
const double outgoingBonusIfNotFullMotivation = 30;//������ʱ����������


Action Evaluator::handWrittenStrategy(const Game& game)
{
  Action action;
  action.train = -1;
  action.buy50p = false;
  action.buyFriend20 = false;
  action.buyPt10 = false;
  action.buyVital20 = false;
  if(game.isEnd())
    return action;



  if (game.isRacing)//�����غ�
  {
    assert(false);
    return action;
  }
  double basicChoiceValues[9];//9��������Ŀ��ѵ������Ϣ�������
  for (int i = 0; i < 9; i++)
    basicChoiceValues[i] = -100000;



  int vitalAfterRest = std::min(int(game.maxVital), int(50 + game.vital));
  if (game.turn == 65)vitalAfterRest = game.vital;//���һ�غ�
  else if (game.turn == 63)vitalAfterRest = std::min(50, vitalAfterRest);//�����ڶ��غ�
  else if (game.turn == 62)vitalAfterRest = std::min(65, vitalAfterRest);//���������غ�
  else if (game.turn == 61)vitalAfterRest = std::min(80, vitalAfterRest);//�������Ļغ�
  else if (game.turn == 60)vitalAfterRest = std::min(95, vitalAfterRest);//��������غ�
  double restValue = restValueFactor * (vitalEvaluation(vitalAfterRest, game.maxVital) - vitalEvaluation(game.vital, game.maxVital));
  
  bool friendOutgoingAvailable = game.larc_zuoyueOutgoingUnlocked && game.larc_zuoyueOutgoingUsed < 5 && !game.larc_isAbroad;
  
  if (friendOutgoingAvailable)
  {
    if (game.motivation < 5)restValue += outgoingBonusIfNotFullMotivation;
    basicChoiceValues[7] = restValue;
  }
  else
  {
    if (game.motivation < 5 && game.larc_isAbroad && game.turn != 65)restValue += outgoingBonusIfNotFullMotivation;
    if (game.turn == 65)restValue = 0;
    basicChoiceValues[6] = restValue;
  }

  //����ss�ļ�ֵ
  if (game.turn == 58)//���һ�غϣ�ֱ�ӿ�ss����ļ�ֵ
  {
    if (game.larc_isSSS)
      basicChoiceValues[5] = 200;
    else
      basicChoiceValues[5] = 25 * game.larc_ssPersonsCount;
  }
  else if (!game.larc_isAbroad && game.larc_ssPersonsCount != 0)
  {
    if (game.larc_ssPersonsCount >= 5)
      basicChoiceValues[5] = ssPriorityValue;
    else
      basicChoiceValues[5] = 0;
  }

  for (int item = 0; item < 5; item++)
  {
    double value = 0;



    //�����
    if (game.turn >= 2 && !game.larc_isAbroad)
    {
      double expectChargeNum = 0;
      double chargeValue = game.turn < 20 ? 12.0 :
        game.turn < 40 ? 10.0 :
        game.turn < 58 ? 5.0 :
        0.0;


      bool haveZuoyue = false;
      int chargeN = game.trainShiningNum[item] + 1;
      int totalCharge = 0;
      int totalChargeFull = 0;
      for (int j = 0; j < 5; j++)
      {
        int p = game.personDistribution[item][j];
        if (p < 0)break;//û��
        int personType = game.persons[p].personType;

        if (personType == 1)//������
        {
          haveZuoyue = true;
        }
        else if (personType == 2 || personType == 3)//��ͨ��,npc
        {
          totalCharge += std::min(chargeN, 3 - game.persons[p].larc_charge);
          if (game.persons[p].larc_charge < 3 && game.persons[p].larc_charge + chargeN >= 3)
            totalChargeFull += 1;
        }
      }
      expectChargeNum = totalCharge;
      if (haveZuoyue)
      {
        value += friendValue_nonAbroad;
        expectChargeNum += 2;
        if (game.larc_zuoyueType == 1 && game.persons[17].friendship < 60)
          expectChargeNum += 2;
          
      }
      value += chargeValue * expectChargeNum;

    }
    else if (game.larc_isAbroad && game.turn < 50)//�ڶ���Զ��
    {
      value += shixingPtValueSecondYear * game.larc_shixingPtGainAbroad[item];

      for (int j = 0; j < 5; j++)
      {
        int p = game.personDistribution[item][j];
        if (p < 0)break;//û��
        int personType = game.persons[p].personType;

        if (personType == 1)//������
        {
          value += shixingPtValueSecondYear * 20;
          value += friendValue_abroad;
          break;
        }
      }
    }






    //����hint���
    int cardHintNum = 0;//����hint���ȡһ�������Դ�ֵ�ʱ��ȡƽ��
    for (int j = 0; j < 5; j++)
    {
      int p = game.personDistribution[item][j];
      if (p < 0)break;//û��
      if (game.persons[p].isHint)
        cardHintNum += 1;
    }

    for (int j = 0; j < 5; j++)
    {
      int pi = game.personDistribution[item][j];
      if (pi < 0)break;//û��
      const Person& p = game.persons[pi];
      if (p.personType != 2)continue;//���������Ѿ���ǰ�濼����

      if (p.friendship < 80)
      {
        float jibanAdd = 7;
        if (game.isAiJiao)jibanAdd += 2;
        if (p.isHint)
        {
          jibanAdd += 5 / cardHintNum;
          if (game.isAiJiao)jibanAdd += 2 / cardHintNum;
        }
        jibanAdd = std::min(float(80 - p.friendship), jibanAdd);

        value += jibanAdd * jibanValue;
      }
      
      if (p.isHint)
      {
        for (int i = 0; i < 5; i++)
          value += game.cardParam[p.cardIdInGame].hintBonus[i] * statusWeights[i] / cardHintNum;
        value += game.cardParam[p.cardIdInGame].hintBonus[5] * ptWeight / cardHintNum;
      }

    }
      
    for (int i = 0; i < 5; i++)
    {
      //����Ҫ�������ԼӶ��٣���Ҫ�����Ƿ����
      float gain = game.trainValue[item][i];
      float remain = game.fiveStatusLimit[i] - game.fiveStatus[i] - finalBonus;
      if (remain < 0)remain = 0;
      if (gain > remain)gain = remain;
      float turnReserve = game.turn >= 60 ?
        remainStatusFactorEachTurnAbroad * (TOTAL_TURN - game.turn - 2) :
        remainStatusFactorEachTurnBeforeAbroad * (TOTAL_TURN - game.turn - 2);//�������޲���turnReserveʱ����Ȩ��
        


      float remainAfterTrain = remain - gain;

      if (remainAfterTrain < turnReserve)//��remain-turnReserve��remain�𽥽���Ȩ��
      {
        if (remain < turnReserve)//ѵ��ǰ�ͽ�����turnReserve����
        {
          gain = 0.5 * (remain * remain - remainAfterTrain * remainAfterTrain) / turnReserve;
        }
        else
        {
          gain = (remain - turnReserve) + 0.5 * turnReserve - 0.5 * remainAfterTrain * remainAfterTrain / turnReserve;
        }
      }

          
      value += gain * statusWeights[i];
    }
    double ptWeightThisGame = ptWeight;
    if (game.isQieZhe) 
      ptWeightThisGame *= GameConstants::ScorePtRateQieZhe / GameConstants::ScorePtRate;
    value += game.trainValue[item][5] * ptWeightThisGame;
    //value += vitalValue * game.trainValue[item][6];

    int vitalAfterTrain = std::min(int(game.maxVital), game.trainValue[item][6] + game.vital);
    if (game.turn == 65)vitalAfterTrain = game.vital;//���һ�غ�
    else if (game.turn == 63)vitalAfterTrain = std::min(50, vitalAfterTrain);//�����ڶ��غ�
    else if (game.turn == 62)vitalAfterTrain = std::min(65, vitalAfterTrain);//���������غ�
    else if (game.turn == 61)vitalAfterTrain = std::min(80, vitalAfterTrain);//�������Ļغ�
    else if (game.turn == 60)vitalAfterTrain = std::min(95, vitalAfterTrain);//��������غ�
    value += vitalFactor * (vitalEvaluation(vitalAfterTrain, game.maxVital) - vitalEvaluation(game.vital, game.maxVital));
        

    double failRate = game.failRate[item];
    if (failRate > 0)
    {
      double failValueAvg = wizFailValue;
      if (item != 5)
      {
        double bigFailProb = failRate;
        if (failRate < 20)bigFailProb = 0;
        failValueAvg = 0.01 * bigFailProb * bigFailValue + (1 - 0.01 * bigFailProb) * smallFailValue;
      }
      value = 0.01 * failRate * failValueAvg + (1 - 0.01 * failRate) * value;
    }
    
    basicChoiceValues[item] = value;

  }

  //�ҵ���õ��Ǹ�ѵ��
  double bestValue = -1e4;
  for (int i = 0; i < 9; i++)
  {
    if (basicChoiceValues[i] > bestValue)
    {
      action.train = i;
      bestValue = basicChoiceValues[i];
    }
  }
  
  if (game.larc_isAbroad && action.train < 5)//Ҫ���ǹ�����������
  {
    //ֻ���ǹ���+50%���������ļ��ٲ���10pt��20%������˿������Զ���
    int cost50p = 100000;
    if (game.larc_levels[GameConstants::UpgradeId50pEachTrain[action.train]] == 2)
      cost50p = 200;
    else if (game.larc_levels[GameConstants::UpgradeId50pEachTrain[action.train]] == 1)
      cost50p = 300;

    if (game.turn < 50)// �ڶ���
    {
      int buy50pCount = 0; //��pt+10Ҳ���ϣ����Ѷ���200
      for (int i = 0; i < 6; i++)
      {
        if (game.larc_levels[i] == 3)
          buy50pCount += 1;
      }
      //����������һ��+50%���ǲ���
      if (buy50pCount < 2 &&
        action.train < 4 &&
        bestValue>100 &&
        game.larc_shixingPt >= cost50p)
      {
        action.buy50p = true;
      }
    }
    else //������
    {
      //������
      if (
        action.train < 5 &&
        game.larc_shixingPt >= cost50p)
      {
        action.buy50p = true;
      }

    }
  }
  return action;
}

