#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>

#include "dataman/AMDatabase.h"

class AMComboBox: public QComboBox
{
	Q_OBJECT

public:
	AMComboBox (QWidget *parent = 0); //Constructor


	QList<QString> searchDbRuns(const QString& tableName, const QString& colName) const;
	// void addRuns();

	//virtual ~AMComboBox();            //Destructor
	~AMComboBox();

protected:
	void autoAddRuns();

	AMDatabase* database_;

	AMDatabase* database() const { return database_;}
};

#endif // COMBOBOX_H
