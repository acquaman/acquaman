#ifndef BIOXASZEBRAINPUTVIEW_H
#define BIOXASZEBRAINPUTVIEW_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraInput.h"

class BioXASZebraInputView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraInputView(BioXASZebraInput *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraInputView();

	/// Returns the input control being viewed.
	BioXASZebraInput* control() const { return control_; }

signals:
	/// Notifier that the input control being viewed has changed.
	void controlChanged(BioXASZebraInput *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the input control being viewed.
	void setControl(BioXASZebraInput *newControl);

protected slots:
	/// Updates the input spinbox.
	void updateInputBox();
	/// Updates the input label.
	void updateInputLabel();
	/// Updates the input status label.
	void updateInputStatusLabel();

	/// Updates the control with view properties.
	void updateControl();

protected:
	/// The control being viewed.
	BioXASZebraInput *control_;

	/// The input control value spinbox.
	QSpinBox *inputBox_;
	/// The input control label.
	QLabel *inputLabel_;
	/// The input status light.
	QLabel *inputStatusLabel_;
};

#endif // BIOXASZEBRAINPUTVIEW_H
