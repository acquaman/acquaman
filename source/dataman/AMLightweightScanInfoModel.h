#ifndef AMLIGHTWEIGHTSCANINFOMODEL_H
#define AMLIGHTWEIGHTSCANINFOMODEL_H

#include <QAbstractItemModel>
#include "AMLightweightScanInfoCollection.h"
class AMLightweightScanInfoModel : public QAbstractItemModel
{
	Q_OBJECT
	AMLightweightScanInfoCollection* scanInfo_;
private:
public:
	explicit AMLightweightScanInfoModel(AMLightweightScanInfoCollection* scanInfo, QObject *parent = 0);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	QUrl rowToUrl(int rowIndex);
signals:
	
public slots:
	
};

#endif // AMLIGHTWEIGHTSCANINFOMODEL_H
