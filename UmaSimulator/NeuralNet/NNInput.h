#pragma once
const int LATEST_NNINPUT_VERSION = 1;


//����������ṹ
//һ��һά����������Ϊ
//(ȫ����Ϣ)(֧Ԯ��1����)...(֧Ԯ��6����)(����������)(֧Ԯ����ͷ1)...(֧Ԯ����ͷ6)(npc1)...(npc10)(���³�)(����)(�п�����)(�޿�����)
//��������������� ֧Ԯ��6��֧Ԯ����ͷ6 Ϊ��
//���û���������� ������������npc10Ϊ��
const int NNINPUT_CHANNELS_SEARCHPARAM_V1 = 3; //��������ռ���ٸ�ͨ��
const int NNINPUT_CHANNELS_GAMEGLOBAL_V1 = 354;//Game����˿�����ͷ��ͨ����
const int NNINPUT_CHANNELS_CARD_V1 = 72; //ÿ��֧Ԯ������ռ����ͨ���������������ͷ���֮���
const int NNINPUT_CHANNELS_PERSON_V1 = 81; //ÿ����ͷ����ͨ����������֧Ԯ������
const int NNINPUT_CHANNELS_V1 = NNINPUT_CHANNELS_SEARCHPARAM_V1 +
                                NNINPUT_CHANNELS_GAMEGLOBAL_V1 +
                                NNINPUT_CHANNELS_CARD_V1 * 7 +
                                NNINPUT_CHANNELS_PERSON_V1 * 20;//��ͨ����








const int NNOUTPUT_CHANNELS_POLICY_V1 = 18;
const int NNOUTPUT_CHANNELS_VALUE_V1 = 3;
const int NNOUTPUT_CHANNELS_V1 = NNOUTPUT_CHANNELS_POLICY_V1 + NNOUTPUT_CHANNELS_VALUE_V1;