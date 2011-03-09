#ifndef MCPDETECTORVIEW_H
#define MCPDETECTORVIEW_H

#include "AMDetectorView.h"

class MCPBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit MCPBriefDetectorView(MCPDetector *detector = 0, QWidget *parent = 0);

	AMDetector* detector();

protected:
	AMControlEditor *fbk_;
	QHBoxLayout *hl_;
	MCPDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector);
};

class MCPDetailedDetectorView : public AMDetailedDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit MCPDetailedDetectorView(MCPDetector *detector = 0, QWidget *parent = 0);

	AMDetector* detector();

protected:
	QLabel *hvSetpointLabel_;
	QFormLayout *fl_;
	MCPDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector);
};

#endif // MCPDETECTORVIEW_H
