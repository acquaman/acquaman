#ifndef AM2DSCANCONFIGURATIONGENERALVIEW_H
#define AM2DSCANCONFIGURATIONGENERALVIEW_H

#include "AMDbObjectGeneralView.h"

class AM2DScanConfigurationGeneralView : public AMDbObjectGeneralView
{
Q_OBJECT
public:
	Q_INVOKABLE AM2DScanConfigurationGeneralView(QWidget *parent = 0);

	virtual void createView(const QString &databaseName, const QString &tableName, int dbId);
	virtual bool canView(const QString &databaseName, const QString &tableName, int dbId);
};

#endif // AM2DSCANCONFIGURATIONGENERALVIEW_H
