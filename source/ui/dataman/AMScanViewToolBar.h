#ifndef AMSCANVIEWTOOLBAR_H
#define AMSCANVIEWTOOLBAR_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QLayout>

class MPlot;

class AMScanViewToolBar : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewToolBar(MPlot *plot, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewToolBar();

	/// Returns the plot being edited.
	MPlot* plot() const { return plot_; }

signals:
	/// Notifier that the plot being edited has changed.
	void plotChanged(MPlot *newPlot);

public slots:
	/// Sets the plot to be edited.
	void setPlot(MPlot *newPlot);

protected:
	/// Handles updating the plot with the selected tool.
	void onToolBoxItemActivated(const QString &text);

protected:
	/// The plot being edited.
	MPlot *plot_;

	/// The combo box displaying the currently active tool, and containing all available tool options.
	QComboBox *toolBox_;
};

#endif // AMSCANVIEWTOOLBAR_H
