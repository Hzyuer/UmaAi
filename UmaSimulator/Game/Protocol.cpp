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

    assert(false && "todo");
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

