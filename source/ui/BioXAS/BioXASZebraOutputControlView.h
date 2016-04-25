#ifndef BIOXASZEBRAOUTPUTCONTROLVIEW_H
#define BIOXASZEBRAOUTPUTCONTROLVIEW_H

#include <QLabel>
#include <QSpinBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraOutputControl.h"

class BioXASZebraOutputControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraOutputControlView(BioXASZebraOutputControl *outputControl, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraOutputControlView();

	/// Returns the output control being viewed.
	BioXASZebraOutputControl* outputControl() const { return outputControl_; }

signals:
	/// Notifier that the output control being viewed has changed.
	void outputControlChanged(BioXASZebraOutputControl *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the output control being viewed.
	void setOutputControl(BioXASZebraOutputControl *newControl);

protected:
	/// The output control being viewed.
	BioXASZebraOutputControl *outputControl_;

	/// The output name label.
	QLabel *nameLabel_;
	/// The output control value spinbox.
	QSpinBox *outputValueBox_;
	/// The output control value label.
	QLabel *outputValueLabel_;
	/// The output status light.
	QLabel *outputStatusLabel_;
};

#endif // BIOXASZEBRAOUTPUTCONTROLVIEW_H
