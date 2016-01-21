#ifndef BIOXASZEBRALOGICBLOCKINPUTINVERTEDVIEW_H
#define BIOXASZEBRALOGICBLOCKINPUTINVERTEDVIEW_H

#include <QWidget>
#include <QCheckBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraLogicBlockInput.h"

class BioXASZebraLogicBlockInputInvertedView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraLogicBlockInputInvertedView(BioXASZebraLogicBlockInput *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockInputInvertedView();

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
	/// Updates the inverted checkbox.
	void updateInvertedBox();
	/// Updates the control.
	void updateControl();

protected:
	/// The logic block input control being viewed.
	BioXASZebraLogicBlockInput *control_;
	/// The inverted checkbox.
	QCheckBox *invertedBox_;
};

#endif // BIOXASZEBRALOGICBLOCKINPUTINVERTEDVIEW_H
