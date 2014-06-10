#ifndef CLSAMPTEKSDD123DETECTORNEWVIEW_H
#define CLSAMPTEKSDD123DETECTORNEWVIEW_H

#include "ui/beamline/AMDetectorView.h"

class CLSAmptekSDD123DetectorNew;

class CLSAmptekSDD123DetectorNewView : public AMDetectorGeneralDetailedView
{
Q_OBJECT
public:
 	virtual ~CLSAmptekSDD123DetectorNewView();
	CLSAmptekSDD123DetectorNewView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent = 0);

protected:
	QDoubleSpinBox *detectorTemperatureDSBox_;
};

#endif // CLSAMPTEKSDD123DETECTORNEWVIEW_H
