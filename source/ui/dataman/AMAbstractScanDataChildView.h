#ifndef AMABSTRACTSCANDATACHILDVIEW_H
#define AMABSTRACTSCANDATACHILDVIEW_H

#include <QAbstractItemView>
#include <QSortFilterProxyModel>
#include <QIcon>
#include <QUrl>
/// Abstract class which provides a base for all views which can be added to an AMScanDataView
class AMAbstractScanDataChildView : public QAbstractItemView
{
	Q_OBJECT
private:
	QString title_;

public:
	explicit AMAbstractScanDataChildView(const QString& title, QWidget *parent = 0);
	/// Sets the model which controls the scan data displayed by this view
	void setModel(QSortFilterProxyModel* model);
	/// A title associated with the child view. Displays on the TabWidget in the controlling AMScanDataView
	QString title() const;
	/// An icon associated with the child view. Displays on the ToolbarButton in the controlling AMScanDataView
	QIcon icon() const;
protected:
	/// Function to be implemented by views which inherit from this class. Performs any actions for which the
	/// model must be known (eg setting the model of views which are contained within this class)
	virtual void onModelChangedImplementation() = 0;
	QSortFilterProxyModel* model_;
	QIcon icon_;
signals:
	
public slots:
	
};

#endif // AMABSTRACTSCANDATACHILDVIEW_H
