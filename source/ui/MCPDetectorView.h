#ifndef MCPDETECTORVIEW_H
#define MCPDETECTORVIEW_H

#include "AMDetectorView.h"

class MCPBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit MCPBriefDetectorView(MCPDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	AMDetector* detector();

protected slots:
	void onPoweredOnChanged(bool poweredOn);

protected:
	AMControlEditor *readingCE_;
	QToolButton *powerState_;
	QIcon powerOnState_, powerOffState_;
	QHBoxLayout *hl_;
	MCPDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

class MCPDetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit MCPDetailedDetectorView(MCPDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/* NTBA March 14, 2011 David Chevrier
	   Needs a destructor for configurationSettings_
	   */

	AMDetector* detector();

	/// The view is managing this created object, hook up to destroyed() if you need long-term notification
	AMDetectorInfo* configurationSettings() const;

protected slots:
	void onControlSetpointRequested();

protected:
	AMControlEditor *readingCE_;
	AMControlEditor *hvCE_;
	QGridLayout *gl_;
	MCPDetector *detector_;
	MCPDetectorInfo *configurationSettings_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

#endif // MCPDETECTORVIEW_H
