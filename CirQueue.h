#pragma once
#include<stdlib.h>
#include<stdio.h>
#include<iostream>

using namespace std;

template<typename T>
class CirQueue
{
public:
	CirQueue();
	CirQueue(unsigned long size);
	~CirQueue();
public:
	void setSize(unsigned long size);
	bool isEmpty();
	bool isFull();

	void push(T element);
	T pop();
	T head();
	T last();

	void resize(unsigned long size);
	unsigned long size();
	unsigned long length();
private:
	T *m_data = NULL;
	unsigned long m_front;
	unsigned long m_rear;
	unsigned long M_SIZE;
public:

};

#include "CirQueue.h"

template<typename T>
CirQueue<T>::CirQueue()
{
	m_data = NULL;
	m_front = 0;
	m_rear = 0;
	M_SIZE = 0;
}
template<typename T>
CirQueue<T>::CirQueue(unsigned long size)
{
	m_front = 0;
	m_rear = 0;
	M_SIZE = size;
	m_data = new T[M_SIZE]();
}

template<typename T>
CirQueue<T>::~CirQueue()
{
	delete[] m_data;
	m_data = NULL;
}

template<typename T>
void CirQueue<T>::setSize(unsigned long size)
{
	m_front = 0;
	m_rear = 0;
	M_SIZE = size;
	if (m_data != NULL)
	{
		delete[] m_data;
	}
	m_data = new T[M_SIZE]();
}

template<typename T>
bool CirQueue<T>::isFull()
{
	return m_front == ((m_rear + 1) % M_SIZE);
}

template<typename T>
bool CirQueue<T>::isEmpty()
{
	return m_front == m_rear;
}

template<typename T>
void CirQueue<T>::push(T element)
{
	if (isFull())
	{
        printf("The Circle Queue Has  Be FULL yuanshiduilie !!\n");
		return;
	}
	m_data[m_rear] = element;
	m_rear = (m_rear + 1) % M_SIZE;
}

template<typename T>
T CirQueue<T>::pop()
{
	if (isEmpty())
	{
		printf("The Circle Queue is Empty!!\n");
		return m_data[m_front];
	}
	m_front = (m_front + 1) % M_SIZE;
	return m_data[m_front - 1];
}

template<typename T>
T CirQueue<T>::head()
{
	return m_data[m_front];
}

template<typename T>
T CirQueue<T>::last()
{
	return m_data[m_rear - 1];
}

template<typename T>
unsigned long CirQueue<T>::size()
{
	return M_SIZE;
}

template<typename T>
unsigned long CirQueue<T>::length()
{
	return (m_rear - m_front + M_SIZE) % M_SIZE;
}

template<typename T>
void CirQueue<T>::resize(unsigned long size)
{
	T* temp = new T[size]();
	memset(temp, 0, sizeof(T) * size);
	unsigned long count = 0;
	for (size_t i = m_front; i < m_rear; i = (i + 1) % M_SIZE)
	{
		temp[count++] = m_data[i];
	}
	m_front = 0;
	m_rear = count;
	M_SIZE = size;
	delete[] m_data;
	m_data = temp;
}
