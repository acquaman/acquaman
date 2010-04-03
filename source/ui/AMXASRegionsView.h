#ifndef AMXASREGIONSVIEW_H
#define AMXASREGIONSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include "acquaman/AMRegion.h"

class AMXASRegionsView : public QWidget
{
Q_OBJECT
public:
	explicit AMXASRegionsView(QList<AMXASRegion*> *regions, QWidget *parent = 0);

signals:

public slots:
protected:
	QTableView *tv_;
	QPushButton *addButton_;
	QPushButton *deleteButton_;
	QHBoxLayout *hl_;
	QFormLayout *fl_;
	AMXASRegionModel *rm_;
};

#endif // AMXASREGIONSVIEW_H
