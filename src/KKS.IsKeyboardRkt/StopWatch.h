#pragma once
#ifndef __StopWatch_H__
#define __StopWatch_H__
#include <Windows.h>

namespace StopWatch 
{
	class Clock {
	private:
		LARGE_INTEGER m_frequency;
		bool		  m_isStarted;
		LARGE_INTEGER m_startingTime;
		LARGE_INTEGER m_endingTime;
	public:
		Clock()
			: m_isStarted(false)
		{
			QueryPerformanceFrequency(&this->m_frequency);
		}

		void Start() {
			if (this ->m_isStarted) {
				return;
			}
			
			QueryPerformanceCounter(&this->m_startingTime);
			
			this->m_isStarted = true;
		}

		void Stop() {
			if (!this->m_isStarted) {
				return;
			}

			QueryPerformanceCounter(&this->m_endingTime);

			this->m_isStarted = false;
		}

		ULONGLONG GetSnapshotTimeInMs() {
			if (!this->m_isStarted) {
				return -1;
			}

			LARGE_INTEGER endingTime = { 0 };
			QueryPerformanceCounter(&endingTime);

			LARGE_INTEGER elapsedTime = { 0 };
			elapsedTime.QuadPart = endingTime.QuadPart - this->m_startingTime.QuadPart;

			elapsedTime.QuadPart *= 1000000;
			elapsedTime.QuadPart /= this->m_frequency.QuadPart;

			elapsedTime.QuadPart /= (1000);//*1000);

			return elapsedTime.LowPart;
		}

		ULONGLONG GetElapsedTimeInMs() {
			if (this->m_isStarted) {
				return -1;
			}

			LARGE_INTEGER elapsedTime = { 0 };
			elapsedTime.QuadPart = this->m_endingTime.QuadPart - this->m_startingTime.QuadPart;
			
			elapsedTime.QuadPart *= 1000000;
			elapsedTime.QuadPart /= this->m_frequency.QuadPart;

			elapsedTime.QuadPart /= (1000);// *1000);
			
			return elapsedTime.LowPart;
		}
	};
}

#endif