#ifndef AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H
#define AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
class AMDbThumbnail;
class AMLightweightScanInfoFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
private:
	int runId_;
public:
	explicit AMLightweightScanInfoFilterProxyModel(QObject *parent = 0);
	void setRunId(int id);
	int runId() const;
protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
signals:

public slots:
	
};

#endif // AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H
