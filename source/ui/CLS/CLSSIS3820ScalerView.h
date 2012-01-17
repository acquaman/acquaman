#ifndef CLSSIS3820SCALERVIEW_H
#define CLSSIS3820SCALERVIEW_H

#include <QWidget>
#include "ui/beamline/AMControlEditor.h"

class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;

class CLSSIS3820Scaler;

/*!
	This class builds a view for the SIS3820 scalar used throughout the CLS.  It takes a scalar object and builds a standard view
	around it that currently is modeled after the current Scalar app view.
  */
class CLSSIS3820ScalerView : public QWidget
{

Q_OBJECT

public:
	/// Constructor.  Takes a scalar object.
	CLSSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent = 0);

protected:
	/// Pointer to the scalar being viewed.
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
