#ifndef AMNEWRUNDIALOG_H
#define AMNEWRUNDIALOG_H

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSqlQuery>
#include <QGridLayout>
#include <QDialog>

#include "dataman/AMDatabase.h"
// #include "ui/AMPrefixSuffixLineEdit.h"
#include "dataman/AMRun.h"

/// author: Rachel Si (2010)
class AMNewRunDialog: public QDialog
{
	Q_OBJECT

public:
	AMNewRunDialog(AMDatabase* db, QWidget *parent = 0); //constructor
	~AMNewRunDialog();

protected slots:
	void facilitySelectCbChanged(int index);
	void okButtonPressed();
	void cancelButtonPressed();

signals:
	/// Emitted when the dialog box is cancelled or accepted. The newRunId is the id of the newly created run, or -1 if cancelled.
	void dialogBoxClosed(int newRunId);

protected:
	void addFacility();
	QComboBox *facilitySelectCb;

	// AMPrefixSuffixLineEdit* runNameLineEdit;
	QLineEdit *runNameLineEdit;

	QGridLayout *addRunsAndFacilitiesLayout;
	AMDatabase* database_;
};
#endif // AMNEWRUNDIALOG_H
