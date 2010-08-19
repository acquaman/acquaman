#ifndef AMNEWRUNDIALOG_H
#define AMNEWRUNDIALOG_H

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSqlQuery>
#include <QGridLayout>
#include "dataman/AMDatabase.h"
#include "ui/AMPrefixSuffixLineEdit.h"
#include "dataman/AMRun.h"

class AMNewRunDialog: public QWidget
{
	Q_OBJECT

public:
	AMNewRunDialog(QWidget *parent = 0); //constructor
	~AMNewRunDialog();

protected slots:
	void facilitySelectCbChanged(int index);
	void okButtonPressed();
	void cancelButtonPressed();

signals:
	void dialogBoxClosed();

protected:
	void addFacility();
	QComboBox *facilitySelectCb;
	AMPrefixSuffixLineEdit *runNameLineEdit;
	QGridLayout *addRunsAndFacilitiesLayout;
	AMDatabase* database_;
};
#endif // AMNEWRUNDIALOG_H
