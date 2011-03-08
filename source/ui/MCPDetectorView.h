#ifndef MCPDETECTORVIEW_H
#define MCPDETECTORVIEW_H

#include "AMDetectorView.h"

class MCPDetectorView : public AMDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit MCPDetectorView(MCPDetector *detector = 0, QWidget *parent = 0);

	AMDetector* detector();

protected:
	AMControlEditor *fbk_;
	QHBoxLayout *hl_;
	MCPDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector);
};

#endif // MCPDETECTORVIEW_H
