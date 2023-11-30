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
#include "../Tests/TestConfig.h"

using namespace std;

const bool handWrittenEvaluationTest = true;
const int threadNum = 8;
const int threadNumInner = 1;
const double radicalFactor = 5;//������
const int searchN = handWrittenEvaluationTest ? 1 : 2048;
SearchParam searchParam = { searchN,TOTAL_TURN,radicalFactor };
const bool recordGame = false;

int totalGames = handWrittenEvaluationTest ? 50000 : 10000000;
int gamesEveryThread = totalGames / threadNum;

TestConfig test;
/*
//int umaId = 108401;//��ˮ��30���ӳ�
int umaId = 106501;//̫����15��15���ӳ�
int umaStars = 5;
//int cards[6] = { 301604,301344,301614,300194,300114,301074 };//���ˣ��߷壬��ӥ��������������˾��
int cards[6] = { 301604,301724,301614,301304,300114,300374 };//���ˣ�������������ӥ������˹�������񣬸��ʵ�
  
int zhongmaBlue[5] = { 18,0,0,0,0 };
int zhongmaBonus[6] = { 10,10,30,0,10,70 };
bool allowedDebuffs[9] = { false, false, false, false, false, false, true, false, false };//�ڶ�����Բ����ڼ���debuff������������������߸���ǿ����
*/
std::atomic<double> totalScore = 0;
std::atomic<double> totalScoreSqr = 0;

std::atomic<int> bestScore = 0;
std::atomic<int> n = 0;
std::mutex printLock;
vector<atomic<int>> segmentStats= vector<atomic<int>>(700);//100��һ�Σ�700��
std::atomic<int> printThreshold = 2187;

void printProgress(int value, int maxValue, int width)
{
    stringstream buf;
    double rate = (double)value / maxValue;
    int n = rate * width;
    n = clamp(n, 0, maxValue);
    buf << "[" << string(n, '=') << ">" << string(width - n, ' ') << "] " << setprecision((int)(2+rate)) << rate * 100 << "% ";

    std::lock_guard<std::mutex> lock(printLock);    // ����ʱ�Զ��ͷ�cout��
    cout << buf.str() << "\033[0F" << endl;
    cout.flush();
}

void worker()
{
  random_device rd;
  auto rand = mt19937_64(rd());

  int batchsize = 256;
  //Model model("../training/example/model_traced.pt", batchsize);
  //Model* modelptr = &model;
  Model* modelptr = NULL;

  Search search(modelptr, batchsize, threadNumInner, searchParam);

  vector<Game> gameHistory;

  if (recordGame)
    gameHistory.resize(TOTAL_TURN);

  for (int gamenum = 0; gamenum < gamesEveryThread; gamenum++)
  {
    Game game;
    game.newGame(rand, false, test.umaId, test.umaStars, &test.cards[0], &test.zhongmaBlue[0], &test.zhongmaBonus[0]);
    for (int i = 0; i < 9; i++)
      game.larc_allowedDebuffsFirstLarc[i] = test.allowedDebuffs[i];

    while(!game.isEnd())
    {
      if (recordGame)
        gameHistory[game.turn] = game;
      Action action;
      if (handWrittenEvaluationTest) {
        action = Evaluator::handWrittenStrategy(game);
      }
      else {

        action = search.runSearch(game, rand);
      }
      game.applyTrainingAndNextTurn(rand, action);
    }
    //cout << termcolor::red << "���ɽ�����" << termcolor::reset << endl;
    int score = game.finalScore();
    if (score > 42000)
    {
      if (recordGame)
        for (int i = 0; i < TOTAL_TURN; i++)
          if (!GameConstants::LArcIsRace[i])
            gameHistory[i].print();
      game.printFinalStats();
    }
    n += 1;
    printProgress(n, totalGames, 70);
    totalScore += score;
    totalScoreSqr += score * score;
    for (int i = 0; i < 700; i++)
    {
      int refScore = i * 100;
      if (score >= refScore)
      {
        segmentStats[i] += 1;
      }
    }

    int bestScoreOld = bestScore;
    if (score > bestScore + printThreshold)
    {
        if (printThreshold < 100)
        {
            std::lock_guard<std::mutex> lock(printLock);
            game.printFinalStats();
            //cout << printThreshold << endl;
            cout.flush();
        }
        printThreshold = printThreshold / 3;
    }

    while (score > bestScoreOld && !bestScore.compare_exchange_weak(bestScoreOld, score)) {
      // ���val����old_max������max_val��ֵ����old_max����ô�ͽ�max_val��ֵ����Ϊval
      // ���max_val��ֵ�Ѿ��������̸߳��£���ô�Ͳ����κ����飬����old_max�ᱻ����Ϊmax_val����ֵ
      // Ȼ�������ٴν��бȽϺͽ���������ֱ���ɹ�Ϊֹ
    }


    //game.print();
    if (!handWrittenEvaluationTest || n == totalGames)
    {
      if(!handWrittenEvaluationTest)
        game.printFinalStats();
      cout << endl << n << "�֣�������=" << searchN << "��ƽ����" << totalScore / n << "����׼��" << sqrt(totalScoreSqr / n - totalScore * totalScore / n / n) << "����߷�" << bestScore << endl;
      //for (int i=0; i<400; ++i)
      //    cout << i*100 << ",";
      //cout << endl;
      //for (int i=0; i<400; ++i)
      //    cout << float(segmentStats[i]) / n << ",";
      //cout << endl;
      cout
        << "UE7����=" << float(segmentStats[327]) / n << ","
        << "UE8����=" << float(segmentStats[332]) / n << ","
        << "UE9����=" << float(segmentStats[338]) / n << ","
        << "UD0����=" << float(segmentStats[344]) / n << ","
        << "UD1����=" << float(segmentStats[350]) / n << ","
        << "UD2����=" << float(segmentStats[356]) / n << ","
        << "UD3����=" << float(segmentStats[362]) / n << ","
        << "UD4����=" << float(segmentStats[368]) / n << ","
        << "UD5����=" << float(segmentStats[375]) / n << ","
        << "UD6����=" << float(segmentStats[381]) / n << ","
        << "UD7����=" << float(segmentStats[387]) / n << ","
        << "UD8����=" << float(segmentStats[394]) / n << ","
        << "UD9����=" << float(segmentStats[400]) / n << ","
        << "UC0����=" << float(segmentStats[407]) / n << endl;

    }
  }

}

void main_testAiScore()
{
  // ��鹤��Ŀ¼
  GameDatabase::loadTranslation("../db/text_data.json");
  GameDatabase::loadUmas("../db/umaDB.json");
  GameDatabase::loadDBCards("../db/cardDB.json");

  test = TestConfig::loadFile("../ConfigTemplate/testConfig.json");  
  cout << test.explain() << endl;
  totalGames = test.totalGames;
  gamesEveryThread = totalGames / threadNum;

  for (int i = 0; i < 200; i++)segmentStats[i] = 0;

  cout << "���ڲ��ԡ���\033[?25l" << endl;

  std::vector<std::thread> threads;
  for (int i = 0; i < threadNum; ++i) {
    threads.push_back(std::thread(worker));
  }
  for (auto& thread : threads) {
    thread.join();
  }

  cout << n << "�֣�������=" << searchN << "��ƽ����" << totalScore / n << "����׼��" << sqrt(totalScoreSqr / n - totalScore * totalScore / n / n) << "����߷�" << bestScore << endl;
  system("pause");

}