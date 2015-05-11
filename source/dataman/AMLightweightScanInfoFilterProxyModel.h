#ifndef AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H
#define AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
class AMDbThumbnail;
/**
 * A class representing an extension of QSortFilterProxyModel which adds:
 * - The ability to add a single 'top level' filter which is applied above whatever standard filter
 *   is applied to the model. This is currently set with setRunId(int) and setExperimentId(int), which
 *   will first filter by run/experiment, and then by whatever other filter has been applied. Note that
 *   only one of these can be set at once.
 * - The ability to search for 'data source' which is a field of the second level of the hierarchical
 *   source model. This is achieved by hardcoding this field as filterKeyColumn() number 8.
 */
class AMLightweightScanInfoFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
	explicit AMLightweightScanInfoFilterProxyModel(QObject *parent = 0);
	/// Sets the provided runId to be the top level filter associated with the model. Setting this
	/// to -1 indicates clearing the runId filter. If there is currently an experimentId, this will
	/// be reset to -1, effectively clearing the experiment filter
	void setRunId(int id);
	/// Returns the runId which is currently filtering the model, if none is applied then -1 is returned
	int runId() const;
	/// Sets the provided experimentId to be the top level filter associated with the model. Setting this
	/// to -1 indicates clearing the experimentId filter. if there is currently a runId, this will be reset
	/// A map of all the runs known. Maps Runids to Run Names.
	const QHash<int, QString> runMap() const;
	/// to -1, effectively clearing the run filter
	void setExperimentId(int id);
	/// Returns the experimentId which is currently filtering the model, if none is applied then -1
	/// is returned
	int experimentId() const;
	/// Sets the source model which this proxy will sort and filter
	void setSourceModel(QAbstractItemModel *sourceModel);
protected:
	/// Virtual function overriding QSortFilterProxyModel::filterAcceptsRow
	/// returns true if the index at the given source row, with the provided source_parent
	/// meets the criteria that is currently applied to the filter
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	/// Helper function which determines whether a source_row meets the filter criteria when
	/// we already know it refers to a thumbnail
	bool filterAcceptsThumbnail(int sourceRow, const QModelIndex &parent) const;
	/// Helper function which determines whether a source_row meets the filter criteria when
	/// we already know it refers to a scan
	bool filterAcceptsScan(int sourceRow, const QModelIndex &parent) const;
signals:
	/// Signals that the underlying model has indicated the run map has been altered
	void runMapUpdated();
private:
	int runId_;
	int experimentId_;
	
};

#endif // AMLIGHTWEIGHTSCANINFOFILTERPROXYMODEL_H
