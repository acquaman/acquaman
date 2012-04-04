#ifndef AMDBOBJECTGENERALVIEW_H
#define AMDBOBJECTGENERALVIEW_H

#include <QWidget>
#include "dataman/database/AMDbObject.h"

class AMDbObjectGeneralView : public QWidget
{
Q_OBJECT
public:
	Q_INVOKABLE AMDbObjectGeneralView(QWidget *parent = 0);

	void createView(const AMDbObject *dbObject);
	virtual void createView(const QString &databaseName, const QString &tableName, int dbId);
	virtual bool canView(const QString &databaseName, const QString &tableName, int dbId);

protected:
	const AMDbObject *dbObject_;
};

#endif // AMDBOBJECTGENERALVIEW_H
