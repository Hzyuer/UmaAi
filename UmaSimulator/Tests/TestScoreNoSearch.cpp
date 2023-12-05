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
#include "../Tests/TestConfig.h"

using namespace std;

void main_testScoreNoSearch()
{
  const string modelpath = "../training/example/model_traced.pt";
  //const string modelpath = "";
  const int threadNum = 8;
  const double radicalFactor = 5;//������
  TestConfig test;

  // ��鹤��Ŀ¼
  GameDatabase::loadTranslation("../db/text_data.json");
  GameDatabase::loadUmas("../db/umaDB.json");
  GameDatabase::loadDBCards("../db/cardDB.json");

  test = TestConfig::loadFile("../ConfigTemplate/testConfig.json");  
  cout << test.explain() << endl;

  SearchParam searchParam = { test.totalGames,TOTAL_TURN,radicalFactor };

  cout << "�������ļ���" << modelpath << "   ������" << test.totalGames << endl;

  cout << "���ڲ��ԡ���\033[?25l" << endl;
  random_device rd;
  auto rand = mt19937_64(rd());

  int batchsize = 1024;
  Model* modelptr = NULL;
  Model model(modelpath, batchsize);
  if (modelpath != "")
  {
    modelptr = &model;
  }

  Search search(modelptr, batchsize, threadNum, searchParam);

  Game game;
  game.newGame(rand, false, test.umaId, test.umaStars, &test.cards[0], &test.zhongmaBlue[0], &test.zhongmaBonus[0]);
  for (int i = 0; i < 9; i++)
    game.larc_allowedDebuffsFirstLarc[i] = test.allowedDebuffs[i];
  Action action = { 8,false,false,false,false };//��������������������ӵ�һ�غϵ���ͷ�ֲ���
  auto value = search.evaluateSingleAction(game, rand, action);

  cout << "ƽ������=\033[1;32m" << int(value.scoreMean) << " \033[0m" << "���ַ���=\033[1;32m" << int(value.value) << "\033[0m " << "��׼��=\033[1;32m" << int(value.scoreStdev) << "\033[0m  " << endl;

  system("pause");
}