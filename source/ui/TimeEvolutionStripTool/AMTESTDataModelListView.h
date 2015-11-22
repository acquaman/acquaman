#ifndef AMTESTDATAMODELLISTVIEW_H
#define AMTESTDATAMODELLISTVIEW_H

#include <QWidget>

#include <QVBoxLayout>
#include <QStringList>
#include <QCheckBox>
#include <QMap>

class AMTESTServerConnection;

/// A simple list view for displaying the available data models.
class AMTESTDataModelListView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMTESTDataModelListView(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMTESTDataModelListView();

signals:

public slots:
	/// Slots that adds new sources after a connection has been created.
	void addNewDataModels(AMTESTServerConnection *connection);
	/// Slots that remove the sources after a connection has been closed.
	void removeDataModels(const QStringList &dataModelNames);

protected:
	/// The layout that holds the checkboxes for data models.
	QVBoxLayout *dataModelLayout_;
	/// The map of check boxes for easy removal.
	QMap<QString, QCheckBox *> dataModelCheckBoxes_;
};

#endif // AMTESTDATAMODELLISTVIEW_H
