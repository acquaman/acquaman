#ifndef BIOXASZEBRALOGICBLOCKVIEW_H
#define BIOXASZEBRALOGICBLOCKVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASZebraLogicBlock.h"
#include "ui/BioXAS/BioXASZebraLogicBlockInputView.h"

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
	/// Notifier that the logic block control being viewed has changed.
	void controlChanged(BioXASZebraLogicBlock *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the logic block control being viewed.
	void setControl(BioXASZebraLogicBlock *newControl);

protected slots:
	/// Updates the logic block input views.
	void updateInputViews();
	/// Updates the output label.
	void updateOutputStatusLabel();

protected:
	/// The logic block control being viewed.
	BioXASZebraLogicBlock *control_;

	/// The list of logic block input views.
	QList<BioXASZebraLogicBlockInputView*> inputViews_;
	/// The input views layout.
	QVBoxLayout *inputViewsLayout_;
	/// The output status label.
	QLabel *outputStatusLabel_;
};

#endif // BIOXASZEBRALOGICBLOCKVIEW_H
