#ifndef SGMPERSISTENTVIEW_H
#define SGMPERSISTENTVIEW_H

#include <QWidget>
#include "ui/beamline/AMExtendedControlEditor.h"

class SGMHexapodTrajectoryView;
/*!
  * A user interface class for providing users access to the core controls on
  * the SGM beamline. Contains control editors for the:
  *   - Energy Control
  *   - Exit Slit
  *   - Grating
  */
class SGMPersistentView : public QWidget
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an SGMPersistentView.
	  */
    explicit SGMPersistentView(QWidget *parent = 0);

	/*!
	  * Virtual destructor for an SGMPersistentView.
	  */
	virtual ~SGMPersistentView(){}
signals:

public slots:
protected:

	/*!
	  * Helper function which instantiates the child controls and performs the
	  * layout.
	  */
	void setupUi();

	AMExtendedControlEditor* energyControlEditor_;
	AMExtendedControlEditor* exitSlitControlEditor_;
	AMExtendedControlEditor* gratingSelectionControlEditor_;
	AMExtendedControlEditor* hexapodVelocityControlEditor_;
	SGMHexapodTrajectoryView* hexapodTrajectoryView_;

};

#endif // SGMPERSISTENTVIEW_H
