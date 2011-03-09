#ifndef PGTDETECTORVIEW_H
#define PGTDETECTORVIEW_H

#include "AMDetectorView.h"

class PGTBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit PGTBriefDetectorView(PGTDetector *detector = 0, QWidget *parent = 0);

	AMDetector* detector();

protected:
	QLabel *controlLabel_;
	QHBoxLayout *hl_;
	PGTDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector);
};

class PGTDetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit PGTDetailedDetectorView(PGTDetector *detector = 0, QWidget *parent = 0);

	AMDetector* detector();

protected:
	PGTDetector *detector_;
	QFormLayout *fl_;
	QLabel *hvSetpointLabel_;
	QLabel *integrationModeLabel_;
	QLabel *integrationTimeLabel_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector);
};

#endif // PGTDETECTORVIEW_H
