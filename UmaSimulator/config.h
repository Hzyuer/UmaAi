#pragma once

#define UMAAI_MAINAI   //ʹ��ai
//#define UMAAI_TESTSCORE   //����ai����
//#define UMAAI_TESTCARDSSINGLE   //�⿨���������ſ�����ֻ�ı�һ��
//#define UMAAI_SIMULATOR   //����ģ����
//#define UMAAI_SELFPLAY   //�����ݣ�����������ѵ����
//#define UMAAI_TESTLIBTORCH   //����c++��torch

#if defined UMAAI_TESTSCORE || defined UMAAI_SIMULATOR 
#define PRINT_GAME_EVENT
#endif

const int MAX_SCORE = 200000;//�������ķ�����70000��larc�籾�϶��ܹ����ˣ������ų�selfplay��������Ŀ��ִ���һЩ�������