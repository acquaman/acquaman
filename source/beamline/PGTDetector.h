#ifndef PGTDETECTOR_H
#define PGTDETECTOR_H

#include "AMDetector.h"
#include "dataman/PGTDetectorInfo.h"
#include "AMControlSet.h"

class PGTDetector : public PGTDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	PGTDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, QObject *parent = 0);
	PGTDetector(const QString& name, AMControl *dataWaveform, AMControl *hv, AMControl *integrationTime, AMControl *integrationMode, QObject *parent = 0);
	~PGTDetector();

	const QMetaObject* getMetaObject();

	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMDetectorInfo* toInfo() const;
	PGTDetectorInfo toPGTInfo() const;

	/* NTBA March 14, 2011 David Chevrier
	bool setFromInfo(const AMDetectorInfo &info);
	*/
	bool setFromInfo(const AMDetectorInfo *info);
	bool setFromInfo(const PGTDetectorInfo &info);

	bool isPoweredOn();

	AMControl* dataWaveformCtrl() const;
	AMControl* hvCtrl() const;
	AMControl* integrationTimeCtrl() const;
	AMControl* integrationModeCtrl() const;

	bool settingsMatchFbk(PGTDetectorInfo* settings);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(PGTDetectorInfo *pgtSettings);

signals:
	void poweredOnChanged(bool poweredOn);

protected slots:
	void onControlsConnected(bool connected);
	void onReadingsControlValuesChanged();
	void onSettingsControlValuesChanged();

protected:
	bool initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls);

protected:
	AMControlSet *readingsControls_;
	AMControlSet *settingsControls_;
	bool ownsControlSets_;

private:
	bool poweredOn_;
};


#endif // PGTDETECTOR_H
