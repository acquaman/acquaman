#ifndef SGMPERSISTENTVIEW_H
#define SGMPERSISTENTVIEW_H

#include <QWidget>
#include "ui/beamline/AMExtendedControlEditor.h"

class SGMBeamOnControl;
class SGMEnergyView;
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

	SGMEnergyView* energyView_;
	SGMBeamOnControl* beamControl_;
	AMExtendedControlEditor* exitSlitGapControlEditor_;
	AMExtendedControlEditor* hexapodVelocityControlEditor_;
	AMExtendedControlEditor* endStationTranslationSetpointControlEditor_;
	AMExtendedControlEditor* endStationTranslationFeedbackControlEditor_;
	AMExtendedControlEditor* endStationLinearStageControlEditor_;

};

#endif // SGMPERSISTENTVIEW_H
