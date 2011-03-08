#ifndef AMSINGLECONTROLDETECTOR_H
#define AMSINGLECONTROLDETECTOR_H

#include "AMDetector.h"

class AMSingleControlDetector : public AMDetectorInfo, public AMDetector
{
Q_OBJECT
public:
	AMSingleControlDetector(const QString& name, AMControl *control, QObject *parent = 0);
	~AMSingleControlDetector();

	const QMetaObject* getMetaObject();
	AMDetectorInfo toInfo();

	AMControl* control();

	bool setFromInfo(const AMDetectorInfo &info);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(AMDetectorInfo *detectorSettings);

protected slots:
	void onControlConnected(bool connected);

protected:
	AMControl *control_;
};

#endif // AMSINGLECONTROLDETECTOR_H
