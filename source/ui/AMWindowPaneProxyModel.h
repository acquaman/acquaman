#ifndef AMWINDOWPANEPROXYMODEL_H
#define AMWINDOWPANEPROXYMODEL_H

#include <QSortFilterProxyModel>

class AMWindowPaneProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /// Constructor.
    explicit AMWindowPaneProxyModel(QObject *parent = 0);
    /// Destructor.
    virtual ~AMWindowPaneProxyModel();

public slots:
    /// Sets the proxy model's source. Reimplemented to invalidate the current sort/filter when the source's data changes.
    virtual void setSourceModel(QAbstractItemModel *sourceModel);

protected:
    /// Returns true if the item with the given parent and row should be included in the model.
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

};

#endif // AMWINDOWPANEPROXYMODEL_H
