#ifndef CLSSIS3820SCALERVIEW_H
#define CLSSIS3820SCALERVIEW_H

#include <QWidget>
#include "ui/beamline/AMControlEditor.h"

class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;

class CLSSIS3820Scaler;

class CLSSIS3820ScalerView : public QWidget
{
Q_OBJECT
public:
	CLSSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent = 0);

protected:
	CLSSIS3820Scaler *scaler_;

	AMControlEditor *scanningCE_;
	AMControlEditor *continuousCE_;
	AMControlEditor *dwellTimeCE_;
	AMControlEditor *scansPerBufferCE_;
	AMControlEditor *totalScansCE_;

	QFormLayout *channelLayoutLeftTop_;
	QFormLayout *channelLayoutRightTop_;
	QFormLayout *channelLayoutLeftBottom_;
	QFormLayout *channelLayoutRightBottom_;
	QFormLayout *toggleLayout_;
	QFormLayout *dwellLayout_;
};

#endif // CLSSIS3820SCALERVIEW_H
