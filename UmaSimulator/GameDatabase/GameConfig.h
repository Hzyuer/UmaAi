#pragma once

#include "iostream"
#include "fstream"
#include "string"
#include "sstream"
#include "filesystem"
#include "../External/json.hpp"

struct GameConfig 
{
    static double radicalFactor; // �����ȣ�Ĭ��Ϊ5����߻ᵼ�¼������
    static int eventStrength;  // ģ������ÿ�غ���40%���ʼ���ô�����ԣ�ģ��֧Ԯ���¼���config��������������Ŀ����ĳ�̶ֳ��ϴ��漤���ȣ������˻�������ڿ�����
    static bool removeDebuff5; // �ڶ��ο�����ǰ�Ƿ���Ҫ��������debuff
    static bool removeDebuff7; // �ڶ��ο�����ǰ�Ƿ���Ҫ����ǿ����debuff

    static std::string modelPath;    // �������ļ�����Ŀ¼
    static int threadNum;   // �߳�����Ĭ��Ϊ12���ɸ�������CPU�������������ֱ�����4
    static int batchSize;   // �Կ����batchSize�������������������𲻴��������ֱ�����256
    static int searchN;  // ���ؿ���������Ĭ��Ϊ6144����С��ֵ���Լӿ��ٶȣ����ή��׼ȷ��
    static int searchDepth;  // ���ؿ�����ȣ�������Ĭ����10������������ֱ���ѵ���Ϸ������TOTAL_TURN=67��

    static bool useWebsocket;    // �Ƿ�ʹ��websocket��С�ڰ�ͨ�ţ�����ʹ���ļ�ͨ��
    static std::string role;    // ̨�ʷ��

    static bool debugPrint; // ��ʾ������Ϣ�����������'.'��Ĭ��ΪFalse
    static bool noColor;    // ΪTrueʱ����ʾ��ɫ

    static void load(const std::string &path);
};
