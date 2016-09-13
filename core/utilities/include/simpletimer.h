/********************************************************************************
 *  SALSA Utilities Library                                                     *
 *  Copyright (C) 2007-2011 Gianluca Massera <emmegian@yahoo.it>                *
 *                                                                              *
 *  This program is free software; you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation; either version 2 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program; if not, write to the Free Software                 *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
 ********************************************************************************/

#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include "utilitiesconfig.h"

#ifdef SALSA_WIN
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <unistd.h>
#endif

namespace salsa {

/**
 * \brief A simple function to sleep for the given amount of milliseconds
 *
 * \param msec the amout of milliseconds to sleep
 */
inline void SALSA_UTIL_TEMPLATE msleep(unsigned int msec)
{
#ifdef SALSA_WIN
	Sleep(msec);
#else
	usleep(msec * 1000);
#endif
}

/*! \brief SimpleTimer object
 *
 *  \par Motivation
 *  It is a simple timer for performance checks
 *
 *  \par Description
 *  SimpleTimer counts microseconds elapsed since the last tic() calls
 *
 *  \par Warnings
 *
 * \ingroup utilities_timer
 */
class SALSA_UTIL_TEMPLATE SimpleTimer {
public:
	/*! Construct the timer */
	SimpleTimer() {
#ifdef SALSA_WIN
		QueryPerformanceFrequency( &frequency );
		QueryPerformanceCounter( &baseCount );
#else
		struct timeval tv;
		gettimeofday( &tv, nullptr );
		lastTime = tv.tv_sec*1000000 + tv.tv_usec;
#endif
	};
	/*! return microsecond elapsed from last tic() call */
	int tac() {
#ifdef SALSA_WIN
		unsigned ticks;
		QueryPerformanceCounter( &count );
		count.QuadPart -= baseCount.QuadPart;
		ticks = unsigned( count.QuadPart * LONGLONG (1000000) / frequency.QuadPart );
		return ticks;
#else
		struct timeval tv;
		gettimeofday( &tv, nullptr );
		return (tv.tv_sec*1000000 + tv.tv_usec) - lastTime;
#endif
	};
	/*! return microsecond elapsed from last tic() call */
	int tic() {
#ifdef SALSA_WIN
		unsigned ticks;
		QueryPerformanceCounter( &count );
		count.QuadPart -= baseCount.QuadPart;
		ticks = unsigned( count.QuadPart * LONGLONG (1000000) / frequency.QuadPart );
		baseCount = count;
		return ticks;
#else
		struct timeval tv;
		gettimeofday( &tv, nullptr );
		int ret = (tv.tv_sec*1000000 + tv.tv_usec) - lastTime;
		lastTime = (tv.tv_sec*1000000 + tv.tv_usec);
		return ret;
#endif
	};
private:
#ifdef SALSA_WIN
	LARGE_INTEGER count;
	LARGE_INTEGER frequency;
	LARGE_INTEGER baseCount;
#else
	long int lastTime;
#endif
};

} // end namespace salsa

#endif
