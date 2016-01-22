#ifndef BIOXASZEBRALOGICBLOCKINPUTCHECKBOX_H
#define BIOXASZEBRALOGICBLOCKINPUTCHECKBOX_H

#include <QWidget>
#include <QCheckBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraLogicBlockInput.h"

class BioXASZebraLogicBlockInputCheckbox : public QWidget
{
    Q_OBJECT

public:
	/// Enumeration of the different view modes.
	enum Mode { Enabled = 0, Inverted = 1 };

	/// Constructor.
	explicit BioXASZebraLogicBlockInputCheckbox(BioXASZebraLogicBlockInput *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockInputCheckbox();

	/// Returns the view mode.
	Mode mode() const { return mode_; }
	/// Returns the logic block input control being viewed.
	BioXASZebraLogicBlockInput* control() const { return control_; }

signals:
	/// Notifier that the mode has changed.
	void modeChanged(Mode newMode);
	/// Notifier that the logic block input control being viewed has changed.
	void controlChanged(BioXASZebraLogicBlockInput *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the mode.
	void setMode(Mode newMode);
	/// Sets the logic block input control being viewed.
	void setControl(BioXASZebraLogicBlockInput *newControl);

protected slots:
	/// Updates the checkbox.
	void updateBox();
	/// Updates the control.
	void updateControl();

protected:
	/// The logic block input control being viewed.
	BioXASZebraLogicBlockInput *control_;
	/// The checkbox.
	QCheckBox *checkbox_;
};

#endif // BIOXASZEBRALOGICBLOCKINPUTCHECKBOX_H
