#pragma once

#ifdef WHUT_DEMODULATION_API
#define USB_API_WHUT __declspec(dllexport)
#else
#define USB_API_WHUT __declspec(dllimport)
#endif

//typedef struct CircleQueue
//{
//	float* data = new float[5]();
//	unsigned long front = 0;
//	unsigned long rear = 0;
//	unsigned long SIZE = 5;
//	float sum = 0;
//}SmoothQueue;


class USB_API_WHUT Demodulate
{
	typedef struct CircleQueue
	{
		float* data;
		unsigned long front;
		unsigned long rear;
		unsigned long SIZE ;
		float sum;
	}SmoothQueue;
private:
	SmoothQueue *Que1;
	SmoothQueue *Que2;
	SmoothQueue *Que3;
	unsigned windSize;
	float* PriorPhase;
	float* RealPh;
	float* phase;
    int* K;
	int m_peakNum;
	float a1, a2;
	bool isUnwrap;
public:
	void InitParam(int peakNum);
	void setDemoduCoeff(float a1, float a2);
	void setIsUnwrap(bool unwrap);
	void setWindSize(int winSize);

	void smooth(float* CH1, float* CH2, float *CH3);
	float* getDemoduData(float *CH1, float *Ch2, float *CH3);
public:
	Demodulate();
	Demodulate(int peakNum, bool unwrap, int winSize);
	~Demodulate();
};

