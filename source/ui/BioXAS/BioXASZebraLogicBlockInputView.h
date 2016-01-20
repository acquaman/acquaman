#ifndef BIOXASZEBRALOGICBLOCKINPUTVIEW_H
#define BIOXASZEBRALOGICBLOCKINPUTVIEW_H

#include <QWidget>
#include <QCheckBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraLogicBlockInput.h"
#include "ui/BioXAS/BioXASZebraInputView.h"

class BioXASZebraLogicBlockInputView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraLogicBlockInputView(BioXASZebraLogicBlockInput *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockInputView();

	/// Returns the input control being viewed.
	BioXASZebraLogicBlockInput* control() const { return control_; }

signals:
	/// Notifier that the input control being viewed has changed.
	void controlChanged(BioXASZebraLogicBlockInput *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the input control being viewed.
	void setControl(BioXASZebraLogicBlockInput *newControl);

protected slots:
	/// Updates the enabled box.
	void updateEnabledBox();
	/// Updates the input view.
	void updateInputView();
	/// Updates the inverted box.
	void updateInvertedBox();

	/// Updates the control.
	void updateControl();

protected:
	/// The logic input control being viewed.
	BioXASZebraLogicBlockInput *control_;

	/// The enabled box.
	QCheckBox *enabledBox_;
	/// The input view.
	BioXASZebraInputView *inputView_;
	/// The inverted box.
	QCheckBox *invertedBox_;
};

#endif // BIOXASZEBRALOGICBLOCKINPUTVIEW_H
