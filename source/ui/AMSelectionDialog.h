#ifndef AMSELECTIONDIALOG_H
#define AMSELECTIONDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QStringList>

/// This class provides a means of showing a selection of items and then providing a means of retrieving them afterwards.
class AMSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds the dialog from the provided string list.
 	virtual ~AMSelectionDialog();
	AMSelectionDialog(const QString &title, const QStringList &items, QWidget *parent = 0);

	/// Returns the items that are selected as a string list.
	QStringList selectedItems() const;

protected:
	/// The button group that holds all of the check boxes.
	QButtonGroup *items_;
};

#endif // AMSELECTIONDIALOG_H
