#ifndef CSRFILECONFIGURATIONVIEW_H
#define CSRFILECONFIGURATIONVIEW_H

#include <QWidget>

#include "CSRDataVisualization/CSRFileConfiguration.h"

#include <QSpinBox>

/// Simple view that allows you to edit a file configuration.
class CSRFileConfigurationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CSRFileConfigurationView(CSRFileConfiguration *info, QWidget *parent = 0);

	/// Returns the file configuration this view visualizes.
	CSRFileConfiguration *fileConfiguration() const { return fileConfiguration_; }

signals:
	/// Notifier that the delete button was clicked.  Passes the pointer of the widget.
	void deleteRequested(CSRFileConfigurationView *);

public slots:

protected slots:
	/// Handles changes to the file size.
	void onFileSizeChanged();
	/// Handles number of points changed.
	void onNumberOfPointsChanged();
	/// Handles the number of revolutions changed.
	void onNumberOfRevolutionsChanged();
	/// Handles emitting the delete signal.
	void onDeleteRequested();

protected:
	/// The file configuration.
	CSRFileConfiguration *fileConfiguration_;
	/// Holds the edit for the file size.
	QSpinBox *fileSize_;
	/// Holds the number of points.
	QSpinBox *numberOfPoints_;
	/// Holds the number of revolutions.
	QSpinBox *numberOfRevolutions_;
};

#endif // CSRFILECONFIGURATIONVIEW_H
