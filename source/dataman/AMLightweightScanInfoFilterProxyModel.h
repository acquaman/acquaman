#ifndef AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H
#define AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
class AMDbThumbnail;
class AMLightweightScanInfoFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
private:
	int runId_;
	int experimentId_;
public:
	explicit AMLightweightScanInfoFilterProxyModel(QObject *parent = 0);
	void setRunId(int id);
	int runId() const;
	void setExperimentId(int id);
	int experimentId() const;
protected:
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
	bool filterAcceptsThumbnail(int source_row, const QModelIndex &parent) const;
	bool filterAcceptsScan(int source_row, const QModelIndex &parent) const;
signals:

public slots:
	
};

#endif // AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H
