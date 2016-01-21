#ifndef BIOXASZEBRALOGICBLOCKVIEW_H
#define BIOXASZEBRALOGICBLOCKVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraLogicBlock.h"

class BioXASZebraLogicBlockView : public QGroupBox
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASZebraLogicBlockView(BioXASZebraLogicBlock *control, const QString &title = QString(), QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraLogicBlockView();

	/// Returns the logic block control being viewed.
	BioXASZebraLogicBlock* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(BioXASZebraLogicBlock *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the control being viewed.
	void setControl(BioXASZebraLogicBlock *newControl);

protected slots:
	/// Updates the input views.
	void updateInputViews();
	/// Updates the output status label.
	void updateOutputStatusLabel();

protected:
	/// Clears the input views.
	void clearInputViews();

protected:
	/// The logic block control being viewed.
	BioXASZebraLogicBlock *control_;

	/// The output status label.
	QLabel *outputStatusLabel_;
	/// The input views layout.
	QGridLayout *inputViewsLayout_;
};

#endif // BIOXASZEBRALOGICBLOCKVIEW_H
