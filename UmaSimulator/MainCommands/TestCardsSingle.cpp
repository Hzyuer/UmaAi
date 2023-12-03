//����ѵ������ֵ�㷨
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <cassert>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include "../Game/Game.h"
#include "../NeuralNet/Evaluator.h"
#include "../Search/Search.h"
#include "../External/termcolor.hpp"

#include "../GameDatabase/GameDatabase.h"
#include "../GameDatabase/GameConfig.h"

using namespace std;

struct testResult
{
  string cardName;
  int cardId;
  ModelOutputValueV1 result;
};

void main_testCardsSingle()
{
  //string modelPath = "db/model_traced.pt";
  string modelPath = "";

  GameDatabase::loadUmas("../db/umaDB.json");
  //GameDatabase::loadCards("../db/card");
  GameDatabase::loadDBCards("../db/cardDB.json");


  random_device rd;
  auto rand = mt19937_64(rd());

  int toTestCardType = 4;//����������

  //һ������һ�ǣ����ٿ�
  //int umaId = 101101;//���Ϸɣ�20��10���ӳ�
  //int cards[6] = { 301604,300374,300194,300114,301534,0};//���ˣ����ʵۣ������������������Ǻø�磬-   ���Ʊ��������ſ�����Ҫ��link
  
  //���ٶ���һ�ǣ����Ϳ�
  //int umaId = 102402;//���ڣ�10��10��10�Ǽӳ�
  //int cards[6] = { 301604,301524,301474,301614,301654,0 };//���ˣ��Ǻø�磬�ٱ�Ѩ������ӥ���͹�ˮ��-   ���Ʊ��������ſ�����Ҫ��link�����Ϳ�û��ӥ

  //���ٶ���һ�ǣ�������
  //int umaId = 102402;//���ڣ�10��10��10�Ǽӳ�
  //int cards[6] = { 301604,301524,301474,301074,301564,0 };//���ˣ��Ǻø�磬�ٱ�Ѩ����˾���������񿥣�-   ���Ʊ��������ſ�����Ҫ��link
   
  //һ������һ�ǣ������
  //int umaId = 101101;//���Ϸɣ�20��10���ӳ�
  //int cards[6] = { 301604,301524,301474,300194,300114,0};//���ˣ��Ǻø�磬�ٱ�Ѩ������������������-   ���Ʊ��������ſ�����Ҫ��link


  //һ������һ�ǣ����ǿ�
  int umaId = 101101;//���Ϸɣ�20��10���ӳ�
  int cards[6] = { 301604,300374,300194,300114,301474,0};//���ˣ����ʵۣ������������������ٱ�Ѩ��-   ���Ʊ��������ſ�����Ҫ��link
  

  int umaStars = 5;
  int zhongmaBlue[5] = { 18,0,0,0,0 };
  int zhongmaBonus[6] = { 10,10,30,0,10,70 };
  bool allowedDebuffs[9] = { false, false, false, false, true, false, false, false, false };//�ڶ�����Բ����ڼ���debuff������������������߸���ǿ����
  

  double radicalFactor = 10;//������
  int searchN = 100000;
  int threadNum = 12;
  //int initialStatusBonus = 40;//���ǵ���д�߼���ʵ��ai�ֵͣ��������ӳ�ʼ����
  //������д�߼���������ԣ����ǲ�����


  SearchParam searchParam = { searchN,TOTAL_TURN,radicalFactor };

  int batchsize = 1024;
  Model* modelptr = NULL;
  Model model(modelPath, batchsize);
  if (modelPath != "")
  {
    modelptr = &model;
  }

  Model::detect(modelptr);

  Search search(modelptr, batchsize, threadNum, searchParam);

  vector<testResult> allResult;
  for (int cardId = 39999; cardId >= 0; cardId--)
  {
    int cardIdLv50 = cardId * 10 + 4;
    if (!GameDatabase::AllCards.count(cardIdLv50))
      continue;
    auto& card = GameDatabase::AllCards[cardIdLv50];
    if (card.cardType != toTestCardType)
      continue;

    string cardName = card.cardName;

    if (cardId >= 30000)
    {
      cardName = "\033[1;37m" + cardName + "\033[0m";
    }
    else if (cardId >= 20000)
    {
      cardName = "\033[1;33m" + cardName + "SR\033[0m";
    }
    else if (cardId >= 10000)
    {
      cardName = "\033[1;36m" + cardName + "R\033[0m";
    }

    cardName = cardName + "(" + to_string(cardId) + ")";

    //û���ǹ��еĿ�
    if (card.uniqueEffectType == 5 ||
      card.uniqueEffectType == 12 ||
      card.uniqueEffectType == 15
      )
      cardName = "\033[41m\033[30m**\033[0m" + cardName;

    //������Ϊȫ���Ŀ�
    else if (card.uniqueEffectType == 6  
      || cardId == 30155
      || cardId == 30171
      )
      cardName = "\033[43m\033[30m#\033[0m" + cardName;
    else
      cardName = "\033[43m\033[30m\033[0m" + cardName;
    cout << setw(55) << cardName << "��";

    cards[5] = cardIdLv50;
    Game game;
    game.newGame(rand, false, umaId, umaStars, cards, zhongmaBlue, zhongmaBonus);
    //game.addAllStatus(initialStatusBonus);
    Action action = { 8,false,false,false,false };//��������������������ӵ�һ�غϵ���ͷ�ֲ���
    auto value = search.evaluateSingleAction(game, rand, action);

    cout << "���ַ���=\033[1;32m" << int(value.value) << "\033[0m  ƽ������=\033[1;32m" << int(value.scoreMean) << "\033[0m" << endl;
    
    testResult tr;
    tr.cardId = cardId;
    tr.cardName = cardName;
    tr.result = value;

    allResult.push_back(tr);
  }

  cout << endl;
  cout << "\033[41m\033[30m**\033[0m��û���ǹ��У�\033[43m\033[30m#\033[0m����Ϊ����ȫ��" << endl;
  cout << "�����ַ����Ӵ�С����" << endl;
  cout << "-------------------------------------------------------------------------" << endl;
  std::sort(allResult.begin(), allResult.end(), [](const testResult& a, const testResult& b) {
    return a.result.value > b.result.value;
    });

  for (int i = 0; i < allResult.size(); i++)
  {
    auto tr = allResult[i];
    cout << setw(55) << tr.cardName << "��"; 
    cout << "���ַ���=\033[1;32m" << int(tr.result.value) << "\033[0m  ƽ������=\033[1;32m" << int(tr.result.scoreMean) << "\033[0m" << endl;

  }
  //����������Ҫ������selfplay���ѡ��
  string resultname = "testcard_" + to_string(toTestCardType) + ".txt";
  ofstream fs(resultname);
  fs << allResult.size() << endl;
  for (int i = 0; i < allResult.size(); i++)
  {
    auto tr = allResult[i];
    fs << tr.cardId << " " << tr.result.value << " " << tr.result.scoreMean << endl;
  }
  fs.close();


  cout << endl;
  cout << "\033[41m\033[30m**\033[0m��û���ǹ��У�\033[43m\033[30m#\033[0m����Ϊ����ȫ��" << endl;
  cout << "��ƽ�������Ӵ�С����" << endl;
  cout << "-------------------------------------------------------------------------" << endl;
  std::sort(allResult.begin(), allResult.end(), [](const testResult& a, const testResult& b) {
    return a.result.scoreMean > b.result.scoreMean;
    });

  for (int i = 0; i < allResult.size(); i++)
  {
    auto tr = allResult[i];
    cout << setw(55) << tr.cardName << "��";
    cout << "���ַ���=\033[1;32m" << int(tr.result.value) << "\033[0m  ƽ������=\033[1;32m" << int(tr.result.scoreMean) << "\033[0m" << endl;

  }

}