#pragma once
#include <cstdint>
#include <random>
#include <string>
struct SupportCard;
struct Game;
struct Person //�κ�һ�����ܳ�����ѵ�������ͷ
{
  int8_t personType;//0����δ֪��1��������֧Ԯ����R��SSR���У���2������֧ͨԮ����3����npc��ͷ��4���³���5���ߣ�6���������������ݲ�֧����������/�Ŷӿ�
  //int16_t cardId;//֧Ԯ��id������֧Ԯ����0
  int16_t charaId;//npc��ͷ������id������npc��0������дҲ����һ��0��ֻ���ڻ��npc�����֣�

  int8_t cardIdInGame;// Game.cardParam���֧Ԯ����ţ���֧Ԯ��Ϊ-1
  int8_t friendship;//�
  //bool atTrain[5];//�Ƿ������ѵ���������ͨ�Ŀ�ֻ��one-hot����ȫ�գ�����ssr��������������true
  bool isShining;//�Ƿ����ʡ��޷����ʵĿ�����npc��Ϊfalse
  bool isHint;//�Ƿ���hint�����˿�����npc��Ϊfalse
  int8_t cardRecord;//��¼һЩ��������ʱ����ı�Ĳ�������������ֵĹ���

  bool larc_isLinkCard;//�Ƿ�Ϊlink֧Ԯ��
  int8_t larc_charge;//���ڳ��˼���
  int8_t larc_statusType;//����������01234
  int8_t larc_specialBuff;//ÿ3����������У����ͬ��Ϸ��
  int8_t larc_level;//����
  int8_t larc_buffLevel;//�ڼ���buff
  int8_t larc_nextThreeBuffs[3];//��ǰ�Լ�����������buff
  
  //int8_t larc_assignedStatusTypeWhenFull;//�����Ӧ��buff�ǡ����Լӳɡ������ĸ�����������ʱ���Ѿ�ȷ��
  //����Ū�ˣ�ֱ��ȫ����+2���ˡ������С�ڰ崫��������������С�ڰ�Ϊ׼
  
  std::discrete_distribution<> distribution;//distribution(rand)���Ը��ݵ���������0~5���������������ſ����������������Ǹ롣ssr������������

  Person();//δ���ص���ͷ
  void initAtTurn3(std::mt19937_64& rand, int specialBuff, int statusType);//�����غϳ�ʼ��

  

  void larc_nextBuff(std::mt19937_64& rand);//ɾ����ǰ���buff�������µ�buff
  //int larc_getNextBuff(std::mt19937_64& rand);//����larc_level+3����buff

  void getNNInputV1(float* buf, const Game& game, int index) const;//����������������������֧Ԯ��������Game����֧Ԯ���������ڶ�Ӧλ��
  std::string getPersonName(const Game& game) const;//�����������
  std::string getPersonStrColored(const Game& game) const;//����������������������ϳɴ���ɫ���ַ�������С�ڰ�������ʾ
};