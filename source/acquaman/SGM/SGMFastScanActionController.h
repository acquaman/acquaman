#ifndef SGMFASTSCANACTIONCONTROLLER_H
#define SGMFASTSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class AMScanActionControllerScanAssembler;

class SGMFastScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	SGMFastScanActionController(SGMFastScanConfiguration2013 *configuration, QObject *parent = 0);

protected slots:
	void onHackedActionsSucceeded();

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

	bool event(QEvent *e);

protected:
	SGMFastScanConfiguration2013 *configuration_;

	int encoderStartValue_;
	double spacingParam_;
	double c1Param_;
	double c2Param_;
	double sParam_;
	double thetaParam_;

	QMap< QString, QVector<double> > allDataMap_;
	AMnDIndex insertionIndex_;
};

#endif // SGMFASTSCANACTIONCONTROLLER_H
