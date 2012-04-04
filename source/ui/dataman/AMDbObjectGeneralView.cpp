#include "AMDbObjectGeneralView.h"

#include <QFormLayout>
#include <QLabel>
#include <QDebug>
#include <QStringBuilder>

#include "dataman/database/AMDbObjectSupport.h"
#include "util/AMErrorMonitor.h"

AMDbObjectGeneralView::AMDbObjectGeneralView(QWidget *parent) :
	QWidget(parent)
{
}

void AMDbObjectGeneralView::createView(const AMDbObject *dbObject)
{
	if(dbObject)
		createView(dbObject->database()->connectionName(), dbObject->dbTableName(), dbObject->id());
}

void AMDbObjectGeneralView::createView(const QString &databaseName, const QString &tableName, int dbId)
{
	AMDatabase *database = AMDatabase::database(databaseName);
	if(database){
		QSqlQuery q = database->query();
		q.prepare("PRAGMA table_info("%tableName%")");
		if(!AMDatabase::execQuery(q)) {
			q.finish();
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -275001, QString("General Object View: There was an error while trying to read meta data on table %1.").arg(tableName)));
		}
		else{
			QStringList columnNames;
			while(q.next()){
				columnNames << q.value(1).toString();
			}
			columnNames.removeFirst();
			q.finish();

			QVariantList columnValues = database->retrieve(dbId, tableName, columnNames);

			QFormLayout *fl = new QFormLayout();
			QLabel *tempLabel;
			for(int x = 0; x < columnNames.count(); x++){
				QString labelValue = columnValues.at(x).toString();
				if(labelValue.isEmpty())
					labelValue = "<EMPTY>";
				tempLabel = new QLabel(labelValue);
				fl->addRow(columnNames.at(x), tempLabel);
			}

			fl->setLabelAlignment(Qt::AlignLeft);
			setLayout(fl);
		}
	}
}

bool AMDbObjectGeneralView::canView(const QString &databaseName, const QString &tableName, int dbId){
	Q_UNUSED(databaseName)
	Q_UNUSED(tableName)
	Q_UNUSED(dbId)
	return true;
}
