#ifndef CSRMAINWINDOW_H
#define CSRMAINWINDOW_H

#include <QWidget>

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDir>

#include "CSRDataVisualization/CSRDataModel.h"
#include "CSRDataVisualization/CSRFileConfigurationView.h"

class CSRMainWindow : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit CSRMainWindow(CSRDataModel *model, QWidget *parent = 0);
	/// Destructor.
	~CSRMainWindow();

signals:

public slots:

protected slots:
	/// Handles grabbing the directory where the files will reside.
	void onGetDirectoryButtonClicked();
	/// Gets the ball rolling on the computation.
	void onComputeClicked();

protected:
	/// The data model.
	CSRDataModel *model_;
	/// Holds the list of file names.
	QStringList fileNames_;
	/// The directory.
	QDir destinationDirectory_;
	/// Label that holds the directory that was chosen.
	QLabel *directoryLabel_;
	/// Label to display how many files of various types there are.
	QLabel *fileInformationLabel_;
	/// The compute button.
	QPushButton *computeButton_;
	/// List of file configuration views.
	QList<CSRFileConfigurationView *> fileConfigurationViews_;
};

#endif // CSRMAINWINDOW_H
