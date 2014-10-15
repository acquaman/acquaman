#ifndef AMSLEEPSUPPORT_H
#define AMSLEEPSUPPORT_H

#include <time.h>

namespace AMSleepSupport
{
	static void msleep(long miliSecondDelayTime)
	{
		struct timespec req={0, 0};
		time_t sec = (int)(miliSecondDelayTime/1000);
		miliSecondDelayTime = miliSecondDelayTime-(sec*1000);
		req.tv_sec = sec;
		req.tv_nsec = miliSecondDelayTime*1000000L;
		while(nanosleep(&req, &req) == -1)
			continue;
	}
}

#endif // AMSLEEPSUPPORT_H
