#ifndef AMTESTDATAMODELLISTVIEW_H
#define AMTESTDATAMODELLISTVIEW_H

#include <QWidget>

#include <QVBoxLayout>
#include <QStringList>
#include <QCheckBox>
#include <QMap>
#include <QButtonGroup>

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
	/// Notifier that a data model should be added to the plot.  Passes the name of the data model.
	void dataModelToBeAdded(const QString &);
	/// Notifier that a data model shold be removed from the plot. Passes the name of the data model.
	void dataModelToBeRemoved(const QString &);

public slots:
	/// Slots that adds new sources after a connection has been created.
	void addNewDataModels(AMTESTServerConnection *connection);
	/// Slots that remove the sources after a connection has been closed.
	void removeDataModels(const QStringList &dataModelNames);

protected slots:
	/// Handles emitting the necessary signals so that the central widget adds the appropriate MPlot items to the plot.
	void onDataModelCheckBoxChecked(QAbstractButton *checkBox);

protected:
	/// The layout that holds the checkboxes for data models.
	QVBoxLayout *dataModelLayout_;
	/// The map of check boxes for easy removal.
	QMap<QString, QCheckBox *> dataModelCheckBoxes_;
	/// The check box button group.
	QButtonGroup *seriesDataModelButtonGroup_;
	QButtonGroup *imageDataModelButtonGroup_;
};

#endif // AMTESTDATAMODELLISTVIEW_H
