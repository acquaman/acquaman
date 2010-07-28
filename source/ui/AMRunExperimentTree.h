#ifndef AMRUNEXPERIMENTTREE_H
#define AMRUNEXPERIMENTTREE_H

#include <QStandardItemModel>

class AMRunExperimentModel : public QStandardItemModel
{
Q_OBJECT
public:
    explicit AMRunExperimentModel(QObject *parent = 0);

signals:

public slots:


protected:
	void refresh();

	/// Top-level items (run header and experiment header)
	QStandardItem *runItem, *experimentItem;
};

#include <QTreeView>

class AMRunExperimentTree : public QTreeView {
	Q_OBJECT
public:
	explicit AMRunExperimentTree(QWidget* parent = 0);

signals:

public slots:

protected:
	AMRunExperimentModel* model_;

};

/* QString treeview_ss = "QTreeView { color: white; background: black; }" +
		QString("QTreeView { show-decoration-selected: 0; }") +
		QString("QTreeView::item:selected { border: 1px solid grey; }") +
		QString("QTreeView::item:selected:!active { color: white; border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:selected:active { border: 1px solid grey; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #000000, stop: 1 grey); }") +
		QString("QTreeView::item:hover { border: 1px solid grey; }") + */

#endif // AMRUNEXPERIMENTTREE_H
