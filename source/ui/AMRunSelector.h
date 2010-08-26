#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include <dataman/AMRun.h>
#include "dataman/AMDatabase.h"
#include "ui/AMNewRunDialog.h"


#include <QDebug>
class AMRunSelector: public QComboBox
{
	Q_OBJECT


public:
	AMRunSelector (QWidget *parent=0); //Constructor
	virtual ~AMRunSelector();            //Destructor

	int currentRunId() const ;

	/// Sometimes this combo box likes to get huge and ugly-wide. This lets you constrain the size hint, instead of enforcing a harsh hard maximum size:
	void setMaximumSizeHint(const QSize& maxSizeHint) {
		maxSizeHint_ = maxSizeHint;
	}

	QSize sizeHint() const {
		QSize rv = QComboBox::sizeHint().boundedTo(maxSizeHint_);
		qDebug() << "AMRunSelect: size hint" << rv;
		qDebug() << "AMRunSelect: min size" << minimumSize();
		qDebug() << "AMRunSelect: max size" << maximumSize();
		return rv;
	}

public slots:
	void setCurrentRunId(int runId);

protected slots:
	void autoAddRuns();
	void onComboBoxActivated(int index);
	void newRunBox();
	void addNewRun();
	void onDatabaseUpdate(const QString&, int);

signals:
	void currentRunIdChanged(int runId);

protected:

	AMDatabase* database_;
	AMNewRunDialog *box;
	bool runUpdateScheduled_;

	QSize maxSizeHint_;

};

#endif // COMBOBOX_H
