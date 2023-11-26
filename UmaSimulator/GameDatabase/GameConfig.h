#pragma once

#include "iostream"
#include "fstream"
#include "string"
#include "sstream"
#include "filesystem"
#include "../External/json.hpp"

struct GameConfig 
{
    static bool noColor;    // ΪTrueʱ����ʾ��ɫ
    static double radicalFactor; // �����ȣ�Ĭ��Ϊ5����߻ᵼ�¼������
    static int eventStrength;  // ģ������ÿ�غ���40%���ʼ���ô�����ԣ�ģ��֧Ԯ���¼���config��������������Ŀ����ĳ�̶ֳ��ϴ��漤���ȣ������˻�������ڿ�����
    static int threadNum;   // �߳�����Ĭ��Ϊ12���ɸ�������CPU����
    static int searchN;  // ����������Ĭ��Ϊ6144����С��ֵ���Լӿ��ٶȣ����ή��׼ȷ��
    static bool debugPrint; // ��ʾ������Ϣ�����������'.'��Ĭ��ΪFalse
    static bool useWebsocket;    // �Ƿ�ʹ��websocket��С�ڰ�ͨ�ţ�����ʹ���ļ�ͨ��
    static std::string role;    // ̨�ʷ��

    static void load(const std::string &path);
};
