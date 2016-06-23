#include "AMPIC887ErrorMessageClearer.h"
#include "PI_GCS2_DLL.h"
AMPIC887ErrorMessageClearer::AMPIC887ErrorMessageClearer(int controllerId) :
	QObject(0)
{
	controllerId_ = controllerId;
}

void AMPIC887ErrorMessageClearer::start(int mSecsDelay)
{
	timerId_ = startTimer(mSecsDelay);
}

void AMPIC887ErrorMessageClearer::timerEvent(QTimerEvent *)
{
	killTimer(timerId_);
	PI_GetError(controllerId_);
	deleteLater();
}
