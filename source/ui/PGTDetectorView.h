#ifndef PGTDETECTORVIEW_H
#define PGTDETECTORVIEW_H

#include "AMDetectorView.h"

class PGTBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit PGTBriefDetectorView(PGTDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	AMDetector* detector();

protected slots:
	void onPoweredOnChanged(bool poweredOn);

protected:
	//QLabel *controlLabel_;
	AMControlEditor *readingCE_;
	QToolButton *powerState_;
	QIcon powerOnState_, powerOffState_;
	QHBoxLayout *hl_;
	PGTDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class PGTDetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit PGTDetailedDetectorView(PGTDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	AMDetector* detector();

	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
	AMDetectorInfo* configurationSettings() const;

protected slots:
	void onControlSetpointRequested();

protected:
	void setConfigurationSettingsFromDetector();

protected:
	PGTDetector *detector_;
	PGTDetectorInfo *configurationSettings_;
	QGridLayout *gl_;
	AMControlEditor *readingCE_;
	AMControlEditor *hvCE_;
	AMControlEditor *integrationModeCE_;
	AMControlEditor *integrationTimeCE_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

#endif // PGTDETECTORVIEW_H
