#include "AMPIC887HexapodPosition.h"

AMPIC887HexapodPosition::AMPIC887HexapodPosition()
{
	xAxisPosition_ = 0.0;
	yAxisPosition_ = 0.0;
	zAxisPosition_ = 0.0;
	uAxisPosition_ = 0.0;
	vAxisPosition_ = 0.0;
	wAxisPosition_ = 0.0;
	time_ = 0.0;
	isValid_ = false;
}

AMPIC887HexapodPosition::AMPIC887HexapodPosition(double xAxisPosition,
												 double yAxisPosition,
												 double zAxisPosition,
												 double uAxisPosition,
												 double vAxisPosition,
												 double wAxisPosition,
												 double time)
{
	xAxisPosition_ = xAxisPosition;
	yAxisPosition_ = yAxisPosition;
	zAxisPosition_ = zAxisPosition;
	uAxisPosition_ = uAxisPosition;
	vAxisPosition_ = vAxisPosition;
	wAxisPosition_ = wAxisPosition;
	time_ = time;
	isValid_ = true;
}

double AMPIC887HexapodPosition::xAxisPosition() const
{
	return xAxisPosition_;
}

double AMPIC887HexapodPosition::yAxisPosition() const
{
	return yAxisPosition_;
}

double AMPIC887HexapodPosition::zAxisPosition() const
{
	return zAxisPosition_;
}

double AMPIC887HexapodPosition::uAxisPosition() const
{
	return uAxisPosition_;
}

double AMPIC887HexapodPosition::vAxisPosition() const
{
	return vAxisPosition_;
}

double AMPIC887HexapodPosition::wAxisPosition() const
{
	return wAxisPosition_;
}

double AMPIC887HexapodPosition::time() const
{
	return time_;
}

QString AMPIC887HexapodPosition::toString() const
{
	if(isValid_) {
		return QString("[%1 ms] X:%2mm Y:%3mm Z:%4mm U:%5mm V:%6mm W:%7mm")
				.arg(time_)
				.arg(xAxisPosition_)
				.arg(yAxisPosition_)
				.arg(zAxisPosition_)
				.arg(uAxisPosition_)
				.arg(vAxisPosition_)
				.arg(wAxisPosition_);
	} else {

		return "Invalid";
	}
}

bool AMPIC887HexapodPosition::isValid() const
{
	return isValid_;
}
