#ifndef PGTDETECTORVIEW_H
#define PGTDETECTORVIEW_H

#include "AMDetectorView.h"

class PGTDetectorView : public AMDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit PGTDetectorView(PGTDetector *detector = 0, QWidget *parent = 0);

	AMDetector* detector();

protected:
	QLabel *controlLabel_;
	QHBoxLayout *hl_;
	PGTDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector);
};

#endif // PGTDETECTORVIEW_H
