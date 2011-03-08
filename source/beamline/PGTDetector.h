#ifndef PGTDETECTOR_H
#define PGTDETECTOR_H

#include "AMDetector.h"
#include "dataman/PGTDetectorInfo.h"

class PGTDetector : public PGTDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	PGTDetector(const QString& name, AMControl *dataWaveform, AMControl *hvSetpoint, AMControl *hvFbk, AMControl *integrationTime, AMControl *integrationMode, QObject *parent = 0);
	~PGTDetector();

	const QMetaObject* getMetaObject();
	AMDetectorInfo toInfo();

	bool setFromInfo(const AMDetectorInfo &info);

	AMControl* dataWaveformCtrl() const;
	AMControl* hvSetpointCtrl() const;
	AMControl* hvFbkCtrl() const;
	AMControl* integrationTimeCtrl() const;
	AMControl* integrationModeCtrl() const;

	bool settingsMatchFbk(PGTDetectorInfo* settings);

public slots:
	virtual bool setControls(PGTDetectorInfo *pgtSettings);

protected:
	AMControl *dataWaveform_;
	AMControl *hvSetpoint_;
	AMControl *hvFbk_;
	AMControl *integrationTime_;
	AMControl *integrationMode_;
};


#endif // PGTDETECTOR_H
