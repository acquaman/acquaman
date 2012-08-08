#ifndef SGMPERIODICTABLEVIEW_H
#define SGMPERIODICTABLEVIEW_H

#include <QWidget>
#include <QMenu>

#include "util/SGM/SGMPeriodicTable.h"
#include "ui/util/AMPeriodicTableView.h"

class SGMPeriodicTableView : public QWidget
{
Q_OBJECT

public:
	SGMPeriodicTableView(SGMPeriodicTable *sgmPeriodicTable, QWidget *parent = 0);

protected slots:
	void onClicked(int atomicNumber);

	void onHoveredAvailableScansMenu();
	void onTriggerAvailableScansMenu();

protected:
	SGMPeriodicTable *sgmPeriodicTable_;
	AMPeriodicTableView *periodicTableView_;

	QMenu *availableScansMenu_;
	int activeMenuData_;
};

#endif // SGMPERIODICTABLEVIEW_H
