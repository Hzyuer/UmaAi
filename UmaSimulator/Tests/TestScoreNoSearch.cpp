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
#if USE_BACKEND == BACKEND_LIBTORCH
  const string modelpath = "../training/example/model_traced.pt";
#elif USE_BACKEND == BACKEND_NONE
  const string modelpath = "";
#else
  const string modelpath = "../training/example/model.txt";
#endif

  const int threadNum = 8;
  int batchsize = 2048;
  const double radicalFactor = 5;//������


  TestConfig test;




  // ��鹤��Ŀ¼
  GameDatabase::loadTranslation("../db/text_data.json");
  GameDatabase::loadUmas("../db/umaDB.json");
  GameDatabase::loadDBCards("../db/cardDB.json");

  test = TestConfig::loadFile("../ConfigTemplate/testConfig.json");  
  cout << test.explain() << endl;

  test.totalGames = ((test.totalGames - 1) / (threadNum * batchsize) + 1) * threadNum * batchsize;
  SearchParam searchParam = { test.totalGames,TOTAL_TURN,radicalFactor };

  cout << "�߳�����" << threadNum << "   batchsize��" << batchsize << endl;
  cout << "�������ļ���" << modelpath << "   ������" << test.totalGames << "���Ѷ�batchsize*�߳���ȡ����" << endl;

  cout << "���ڲ��ԡ���\033[?25l" << endl;



  random_device rd;
  auto rand = mt19937_64(rd());

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

  auto start = std::chrono::high_resolution_clock::now();
  auto value = search.evaluateSingleAction(game, rand, action);
  auto stop = std::chrono::high_resolution_clock::now();

  cout << "ƽ������=\033[1;32m" << int(value.scoreMean) << " \033[0m" << "���ַ���=\033[1;32m" << int(value.value) << "\033[0m " << "��׼��=\033[1;32m" << int(value.scoreStdev) << "\033[0m  " << endl;

  // �������ʱ��
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  double duration_s = 0.000001 * duration.count();
  // �������ʱ��
  std::cout << "��ʱ: " << duration_s << " s, ÿ�� " << test.totalGames / duration_s << " ��" << std::endl;

  system("pause");

}