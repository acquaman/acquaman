#ifndef AMSAMPLEPOSITIONVIEWACTIONSWIDGET_H
#define AMSAMPLEPOSITIONVIEWACTIONSWIDGET_H

#include <QFrame>

namespace Ui {
	class AMSamplePositionViewActionsWidget;
}

/// This widget adds a "Mark", "Move To", and "Remove" button when editing sample position items; it's used by AMSamplePlateView.
class AMSamplePositionViewActionsWidget : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.  \c row is the row index that we are acting as the editor for.
	explicit AMSamplePositionViewActionsWidget(int row, QWidget *parent = 0);
	~AMSamplePositionViewActionsWidget();

signals:
	/// Emitted with the row number when the 'Mark' button is pressed
	void rowMarkPressed(int row);
	/// Emitted with the row number when the 'Move To' button is pressed
	void rowMoveToPressed(int row);
	/// Emitted with the row number when the "Remove" button is pressed
	void rowRemovePressed(int row);

protected:
	/// The row that this editor is being used on. (Since we only handle list models, this is sufficient)
	int row_;

protected slots:
	void onMarkButtonPressed() { emit rowMarkPressed(row_); }
	void onMoveToButtonPressed() { emit rowMoveToPressed(row_); }
	void onRemoveButtonPressed() { emit rowRemovePressed(row_); }

private:
	Ui::AMSamplePositionViewActionsWidget *ui;
};

#endif // AMSAMPLEPOSITIONVIEWACTIONSWIDGET_H
