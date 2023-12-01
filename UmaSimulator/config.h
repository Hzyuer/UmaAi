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

#define USE_BACKEND_LIBTORCH //ʹ��libtorch����������


const int MAX_SCORE = 200000;//�������ķ�����70000��larc�籾�϶��ܹ����ˣ������ų�selfplay��������Ŀ��ִ���һЩ�������

#if defined USE_BACKEND_LIBTORCH || defined UMAAI_TESTLIBTORCH

const int LIBTORCH_USE_GPU = true;//�Ƿ�ʹ��GPU

#ifdef _DEBUG 
#pragma comment(lib, "C:/local/libtorch_debug/lib/torch.lib")
#pragma comment(lib, "C:/local/libtorch_debug/lib/c10.lib")
#pragma comment(lib, "C:/local/libtorch_debug/lib/torch_cuda.lib")
#pragma comment(lib, "C:/local/libtorch_debug/lib/torch_cpu.lib")
#pragma comment(lib, "C:/local/libtorch_debug/lib/c10_cuda.lib")
#else
#pragma comment(lib, "C:/local/libtorch/lib/torch.lib")
#pragma comment(lib, "C:/local/libtorch/lib/c10.lib")
#pragma comment(lib, "C:/local/libtorch/lib/torch_cuda.lib")
#pragma comment(lib, "C:/local/libtorch/lib/torch_cpu.lib")
#pragma comment(lib, "C:/local/libtorch/lib/c10_cuda.lib")
#endif
#endif