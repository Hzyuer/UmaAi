#include <iostream>
#include <sstream>
#include <cassert>
#include "vector"
#include "../External/json.hpp"
#include "Protocol.h"
#include "Game.h"
using namespace std;
using json = nlohmann::json;

// �Ƿ�ѵ��ƿ��������ƴ����ᵼ��һ����Ԥ��ƫ�
// ΪTrueʱ�������ID�����λ��Ϊ���ƣ�����5xxx����4xxx��
static bool maskUmaId = true;

int mask_umaId(int umaId)
{
    return umaId % 1000000;
}

bool Game::loadGameFromJson(std::string jsonStr)
{
  if (jsonStr == "[test]" || jsonStr == "{\"Result\":1,\"Reason\":null}")
  {
    std::cout << "�ѳɹ���URA�������ӣ�����δ���յ��غ���Ϣ���ȴ���Ϸ��ʼ" << std::endl;
    return false;
  }
  try
  {
    json j = json::parse(jsonStr, nullptr, true, true);

    umaId = j["umaId"];
    if (maskUmaId)
      umaId = mask_umaId(umaId);
    if (!GameDatabase::AllUmas.count(umaId))
      throw string("δ֪�����Ҫ����ai");
    for (int i = 0; i < 5; i++)
      fiveStatusBonus[i] = GameDatabase::AllUmas[umaId].fiveStatusBonus[i];

    turn = j["turn"];
    if (turn >= TOTAL_TURN && turn < 0)
      throw string("�غ�������ȷ");
    isRacing = GameConstants::LArcIsRace[turn];
    larc_isAbroad = (turn >= 36 && turn <= 42) || (turn >= 60 && turn <= 67);

    vital = j["vital"];
    maxVital = j["maxVital"];
    isQieZhe = j["isQieZhe"];
    isAiJiao = j["isAiJiao"];
    failureRateBias = j["failureRateBias"];
    for (int i = 0; i < 5; i++)
      fiveStatus[i] = j["fiveStatus"][i];
    eventStrength = GameConstants::EventStrengthDefault;
    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] = j["fiveStatusLimit"][i];

    skillPt = j["skillPt"];
    skillScore = 0;
    motivation = j["motivation"];
    isPositiveThinking = j["isPositiveThinking"];

    for (int i = 0; i < 5; i++)
      trainLevelCount[i] = j["trainLevelCount"][i];

    for (int i = 0; i < 5; i++)
      zhongMaBlueCount[i] = j["zhongMaBlueCount"][i];

    for (int i = 0; i < 6; i++)
      zhongMaExtraBonus[i] = j["zhongMaExtraBonus"][i];

    normalCardCount = j["normalCardCount"];


    saihou = 0;
    for (int i = 0; i < 6; i++)
    {
      int cardId = j["cardId"][i];
      int realCardId = cardId / 10;
      int cardLevel = cardId % 10;

      //���û�ҵ�������һ��������ĸ����ڲ������ݿ���
      if (!GameDatabase::AllCards.count(cardId) || GameDatabase::AllCards[cardId].filled == false)
      {
        while (cardLevel < 5)
        {
          cardLevel += 1;
          int cardId1 = realCardId * 10 + cardLevel;
          if (GameDatabase::AllCards.count(cardId1) && GameDatabase::AllCards[cardId1].filled == true)
            break;
          if (cardLevel == 4)
            throw string("δ֪֧Ԯ������Ҫ����ai");
        }
      }


      cardParam[i] = GameDatabase::AllCards[realCardId * 10 + cardLevel];
      cardParam[i].cardID = cardId;
      SupportCard& cardP = cardParam[i];
      saihou += cardP.saiHou;
      int cardType = cardP.cardType;
      if (cardType == 5 || cardType == 6)
      {
        if (realCardId == 30160 || realCardId == 10094)//������
        {
          if (realCardId == 30160)
            larc_zuoyueType = 1;
          else
            larc_zuoyueType = 2;

          int zuoyueLevel = cardId % 10;
          if (larc_zuoyueType == 1)
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

        }
        else
        {
          throw string("��֧�ִ�������������˻��Ŷӿ�");
        }
      }
    }

    for (int i = 0; i < 18; i++) //��ͷ
    {
      auto p = j["persons"][i];
      persons[i].personType = p["personType"];
      if (persons[i].personType == 0)persons[i].personType = 3;
      persons[i].charaId = p["charaId"];
      persons[i].cardIdInGame = p["cardIdInGame"];
      persons[i].friendship = p["friendship"];
      persons[i].isHint = p["isHint"];
      persons[i].cardRecord = p["cardRecord"];
      persons[i].larc_charge = p["larc_charge"];
      persons[i].larc_statusType = p["larc_statusType"];
      persons[i].larc_specialBuff = p["larc_specialBuff"];
      persons[i].larc_level = p["larc_level"];
      persons[i].larc_buffLevel = p["larc_buffLevel"];
      for (int k = 0; k < 3; k++)
        persons[i].larc_nextThreeBuffs[k] = p["larc_nextThreeBuffs"][k];
      if (persons[i].personType == 2)//���������ǿ�
      {
        SupportCard& cardP = cardParam[persons[i].cardIdInGame];
        persons[i].larc_isLinkCard = cardP.larc_isLink;

        std::vector<int> probs = { 100,100,100,100,100,50 }; //�������ʣ����������Ǹ�
        // �����ʹ�����Ҫ����������
        double deYiLv = cardP.deYiLv;
        probs[cardP.cardType] += deYiLv;  
        persons[i].distribution = std::discrete_distribution<>(probs.begin(), probs.end());
      }
      else
      {
        persons[i].larc_isLinkCard = false;
        std::vector<int> probs = { 1,1,1,1,1,1 }; //�������ʣ����������Ǹ�
        persons[i].distribution = std::discrete_distribution<>(probs.begin(), probs.end());
      }
    }

    //motivationDropCount = j["motivationDropCount"];
    motivationDropCount = 0;

    larc_supportPtAll = j["larc_supportPtAll"];
    larc_shixingPt = j["larc_shixingPt"];
    for (int i = 0; i < 10; i++)
      larc_levels[i] = j["larc_levels"][i];


    larc_isSSS = j["larc_isSSS"];
    larc_ssWin = j["larc_ssWin"];
    larc_ssWinSinceLastSSS = j["larc_ssWinSinceLastSSS"];
    for (int i = 0; i < 9; i++)
      larc_allowedDebuffsFirstLarc[i] = false;
    larc_allowedDebuffsFirstLarc[4] = true; //todo �û��Զ���

    larc_zuoyueFirstClick = j["larc_zuoyueFirstClick"];
    larc_zuoyueOutgoingUnlocked = j["larc_zuoyueOutgoingUnlocked"];
    larc_zuoyueOutgoingRefused = j["larc_zuoyueOutgoingRefused"];
    larc_zuoyueOutgoingUsed = j["larc_zuoyueOutgoingUsed"];

    stageInTurn = 1;


    for (int i = 0; i < 5; i++)
      for (int k = 0; k < 5; k++)
      {
        personDistribution[i][k] = j["personDistribution"][i][k];
      }

    larc_ssPersonsCount = j["larc_ssPersonsCount"];
    for (int i = 0; i < 5; i++)
      larc_ssPersons[i] = j["larc_ssPersons"][i];
    larc_ssPersonsCountLastTurn = larc_ssPersonsCount;

    calculateTrainingValue(); //��ȫһЩû�����Ϣ

    for (int i = 0; i < 5; i++)
      for (int k = 0; k < 7; k++)
      {
        trainValue[i][k] = j["trainValue"][i][k];
      }

    for (int i = 0; i < 5; i++)
      failRate[i] = j["failRate"][i];

    calculateTrainingValue();
  }
  catch (string e)
  {
    cout << "��ȡ��Ϸ��Ϣjson����" << e << endl;
    //cout << "-- json --" << endl << jsonStr << endl;
    return false;
  }
  catch (std::exception& e)
  {
    cout << "��ȡ��Ϸ��Ϣjson����δ֪����" << endl << e.what() << endl;
    //cout << "-- json --" << endl << jsonStr << endl;
    return false;
  }
  catch (...)
  {
    cout << "��ȡ��Ϸ��Ϣjson����δ֪����"  << endl;
    return false;
  }

  return true;
}

