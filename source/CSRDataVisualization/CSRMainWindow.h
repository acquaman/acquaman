#ifndef CSRMAINWINDOW_H
#define CSRMAINWINDOW_H

#include <QWidget>

#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDir>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QThread>

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
	/// Notifier that the computation thread should start.
	void startComputation();

public slots:

protected slots:
	/// Handles grabbing the directory where the files will reside.
	void onGetDirectoryButtonClicked();
	/// Gets the ball rolling on the computation.
	void onComputeClicked();
	/// Handles adding a file configuration.
	void onFileConfigurationAddRequested();
	/// Handles removing and deleting file configurations.
	void onFileConfigurationDeleteRequested(CSRFileConfigurationView *view);
	/// Handles deleting the computation thread.
	void onCompuatationThreadFinished();
	/// Handles updating the progress bar and string.
	void onComputationModelUpdate(int fileFinished);

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
	/// Push button for getting the directory.
	QPushButton *getDirectoryButton_;
	/// The add file configuration button.
	QPushButton *addButton_;
	/// The compute button.
	QPushButton *computeButton_;
	/// List of file configuration views.
	QList<CSRFileConfigurationView *> fileConfigurationViews_;
	/// The layout for the file configuration views.
	QVBoxLayout *configurationLayout_;
	/// The progress bar.
	QProgressBar *progressBar_;
	/// The label with the progress string.
	QLabel *progressLabel_;
	/// Computation thread.
	QThread *computationThread_;
};

#endif // CSRMAINWINDOW_H
