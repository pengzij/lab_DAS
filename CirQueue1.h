#ifndef CIRQUEUE1_H
#define CIRQUEUE1_H
#include<stdlib.h>
#include<stdio.h>
#include<iostream>
#define SmoothLength 7
using namespace std;

template<typename T>
class CirQueue1
{
public:
    CirQueue1();
    CirQueue1(unsigned long size);
    ~CirQueue1();
public:
    void setSize(unsigned long size);
    void push(T element, int QueueNum);
    void pop(int QueueNum);
    T regionMean(int QueueNum);

public:
    T *m_data[SmoothLength];
    unsigned short *m_front = NULL;
    unsigned short *m_rear = NULL;
    float *regionSum = NULL;
    unsigned short M_SIZE;
};



template<typename T>
CirQueue1<T>::CirQueue1()
{
    for (int i = 0;i <SmoothLength;i++)
    {
        m_data[i] = NULL;
    }
    m_front = NULL;
    m_rear = NULL;
    regionSum = NULL;
    M_SIZE = 0;
}

template<typename T>
CirQueue1<T>::CirQueue1(unsigned long size)
{
    m_front = new unsigned short[size*3]();
    m_rear = new unsigned short[size*3]();
    M_SIZE = SmoothLength;
    for (int i = 0;i <SmoothLength;i++)
    {
        m_data[i] = new T[size*3]();
    }
}

template<typename T>
CirQueue1<T>::~CirQueue1()
{
    delete[] m_data;
    delete[] m_front;
    delete[] m_rear;
    delete[] regionSum;
    m_front = NULL;
    m_rear = NULL;
    regionSum = NULL;
    for (int i = 0;i <SmoothLength;i++)
    {
        m_data[i] = NULL;
    }

}

template<typename T>
void CirQueue1<T>::setSize(unsigned long size)
{
    m_front = new unsigned short[size*3]();
    m_rear = new unsigned short[size*3]();
    regionSum = new float[size*3]();
    M_SIZE = SmoothLength;
    if (m_data[0] != NULL)
    {
        delete[] m_data;
    }
    for (int i = 0;i <SmoothLength;i++)
    {
        m_data[i] = new T[size*3]();
    }
}



template<typename T>
void CirQueue1<T>::push(T element, int QueueNum)
{
    m_data[m_rear[QueueNum]][QueueNum] = element;
    regionSum[QueueNum] +=element;
    m_rear[QueueNum] = (m_rear[QueueNum] + 1) % M_SIZE;
}

template<typename T>
void CirQueue1<T>::pop(int QueueNum)
{
    regionSum[QueueNum] -= m_data[m_front[QueueNum]][QueueNum];
    m_front[QueueNum] = (m_front[QueueNum] + 1) % M_SIZE;
}

template<typename T>
T CirQueue1<T>::regionMean(int QueueNum)
{
    return regionSum[QueueNum]/SmoothLength;
}
#endif // CIRQUEUE1_H
