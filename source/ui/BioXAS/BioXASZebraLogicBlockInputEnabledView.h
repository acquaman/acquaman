#ifndef BIOXASZEBRALOGICBLOCKINPUTENABLEDVIEW_H
#define BIOXASZEBRALOGICBLOCKINPUTENABLEDVIEW_H

#include <QWidget>
#include <QCheckBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraLogicBlockInput.h"

class BioXASZebraLogicBlockInputEnabledView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraLogicBlockInputEnabledView(BioXASZebraLogicBlockInput *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockInputEnabledView();

	/// Returns the logic block input control being viewed.
	BioXASZebraLogicBlockInput* control() const { return control_; }

signals:
	/// Notifier that the logic block input control being viewed has changed.
	void controlChanged(BioXASZebraLogicBlockInput *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the logic block input control being viewed.
	void setControl(BioXASZebraLogicBlockInput *newControl);

protected slots:
	/// Updates the enabled checkbox.
	void updateEnabledBox();
	/// Updates the control.
	void updateControl();

protected:
	/// The logic block input control being viewed.
	BioXASZebraLogicBlockInput *control_;
	/// The enabled checkbox.
	QCheckBox *enabledBox_;
};

#endif // BIOXASZEBRALOGICBLOCKINPUTENABLEDVIEW_H
