#ifndef AMSCANACTIONEDITOR_H
#define AMSCANACTIONEDITOR_H

#include <QFrame>
#include <actions3/actions/AMScanActionInfo.h>

class QLabel;

/// This class implements a simple editor widget for the AMScanActionInfo that can be shown inside action tree views.  It provides the ability to open up the configuration for the scan.
class AMScanActionEditor : public QFrame
{
	Q_OBJECT

public:
	/// Constructor: Must be Q_INVOKABLE, and accept an AMScanActionInfo as argument
	Q_INVOKABLE AMScanActionEditor(AMScanActionInfo *info, QWidget *parent = 0);
	/// Destructor.
	~AMScanActionEditor();

signals:

public slots:

protected slots:
	/// Helper slot that brings up the configuration for the scan.
	void onScanConfigurationViewRequested();
	/// Helper slot that sets the scan name if it is changed.
	void onScanNameChanged();

protected:
	/// Label that holds the name of the scan.
	QLabel *scanName_;
	/// Pointer to the info for this scan action.
	AMScanActionInfo *info_;
	/// Pointer to the configuration view.  This is to ensure that only one view is generated and you can't accidentally have 10 identical scan view's open simultaneously.
	QWidget *configView_;
};

#endif // AMSCANACTIONEDITOR_H
