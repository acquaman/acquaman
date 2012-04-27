#ifndef SGMBEAMLINEDETECTORCONNECTIONVIEW_H
#define SGMBEAMLINEDETECTORCONNECTIONVIEW_H

#include <QWidget>

class QFormLayout;
class AMDetector;

class SGMBeamlineDetectorConnectionView : public QWidget
{
Q_OBJECT

public:
	SGMBeamlineDetectorConnectionView(QWidget *parent = 0);

protected slots:
	void onDetectorAvailabilityChanged(AMDetector *detector, bool isAvailable);

protected:
	QFormLayout *fl_;
};

#endif // SGMBEAMLINEDETECTORCONNECTIONVIEW_H
