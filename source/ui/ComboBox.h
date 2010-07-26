#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QString>
#include <QList>
class ComboBox: public QComboBox
{
	Q_OBJECT

public:
	ComboBox (QWidget *parent = 0); //Constructor
	virtual ~ComboBox();            //Destructor

	QList<QString> searchDbRuns(const QString& tableName, const QString& colName) const;
	// void addRuns();

	int runCount();  // counting number of runs



protected:
	void autoAddRuns(int runCount());

};

#endif // COMBOBOX_H
