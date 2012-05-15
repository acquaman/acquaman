#ifndef AMADDANALYSISBLOCKDIALOG_H
#define AMADDANALYSISBLOCKDIALOG_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QLabel;
class QPushButton;

/*!
  This is a simple dialog that provides a list of icons representing the available analysis blocks for the given dimensionality.

  \note Maybe at some point we should have analysis blocks go through the same registration system as actions?  Currently this will just keep need to be updated as new data sources are added.
  */
class AMAddAnalysisBlockDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds a dialog and populates it with the available analysis blocks for the given rank.
	AMAddAnalysisBlockDialog(int rank, QWidget *parent = 0);

	/// This method returns the name of a string based on the analysis block selected.  You must provide the rank so that the proper analysis blocks are populated.
	static QString getAnalysisBlock(int rank, QWidget *parent = 0);

	/// Returns teh current items name.
	QString name() const;

protected slots:
	/// Handles changing the dialog when the currently selected item changes.
	void onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

protected:
	/// Method that populates the dialog with 1D analysis blocks.
	void populateWith1D();
	/// Method that populates the dialog with 2D analysis blocks.
	void populateWith2D();

	/// Pointer to the list widget.
	QListWidget *listWidget_;
	/// The label holding the icon of the selected analysis block.
	QLabel *icon_;
	/// The label holding the name of the analysis block.
	QLabel *name_;
	/// The label holding the description of the analysis block.
	QLabel *description_;
	/// The add analysis block button.
	QPushButton *addAnalysisButton_;
};

#endif // AMADDANALYSISBLOCKDIALOG_H
