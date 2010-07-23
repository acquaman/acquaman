#include <QGui>

#include "ComboBox.h"
#include <QSqlQuery>

ComboBox:: ComboBox(QWidget *parent)
	: QComboBox(parent)
{
/// Getting required information from database
	/* will need from database:
	   * run name
	   * run date
	   * run id
	*/

	// acquiring run name (will be in String format)
	QSqlQuery q = database()->query();
	q.prepare ("SELECT runName FROM runs WHERE ... ");
	q.bindValue(0, runName());






}


