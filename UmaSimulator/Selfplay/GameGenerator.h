#pragma once
#include <vector>
#include "SelfplayParam.h"
#include "../Game/Game.h"
#include "../NeuralNet/Evaluator.h"
class GameGenerator
{
  //���������һЩ���֣�Ȼ������������һЩ�غ�����������gameBuf���ڷ�����֮ǰ�ټ������
  SelfplayParam param;
  Evaluator evaluator;
  
  std::mt19937_64 rand;
  std::vector<Game> gameBuf;
  int nextGamePointer;

  std::vector<int> cardRank[5];//���������ǿ�������

  void loadCardRankFile();

  Game randomOpening();
  Game randomizeBeforeOutput(const Game& game0);
  void newGameBatch();
  bool isVaildGame(const Game& game);


  std::vector<int> getRandomCardset(); //��ȡһ��������飬���ʴ����ˣ�Խ�õĿ��ĸ���Խ��
  void randomizeUmaCardParam(Game& game); //������
public:
  GameGenerator(SelfplayParam param, Model* model);
  Game get();
};