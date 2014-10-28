#ifndef SXRMBPERSISTENTVIEW_H
#define SXRMBPERSISTENTVIEW_H

#include <QWidget>

class QVBoxLayout;
class QGroupBox;

class AMExtendedControlEditor;

class SXRMBPersistentView : public QWidget
{

Q_OBJECT

public:
	/// Constructor, takes no arguments other than parent
	SXRMBPersistentView(QWidget *parent = 0);

	/// Destructor
	virtual ~SXRMBPersistentView();

protected:
	/// Main layout holding overall group box
	QVBoxLayout *mainGroupBoxVL_;
	/// Main group box for the widget
	QGroupBox *mainGroupBox_;

	/// Main vertical layout inside of group box
	QVBoxLayout *mainVL_;

	/// Control editor for the energy
	AMExtendedControlEditor *energyControlEditor_;
	/// Control editor for the status (read only)
	AMExtendedControlEditor *statusControlEditor_;
};

#endif // SXRMBPERSISTENTVIEW_H
