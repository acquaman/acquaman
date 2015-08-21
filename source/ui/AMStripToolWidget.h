#ifndef AMSTRIPTOOLWIDGET_H
#define AMSTRIPTOOLWIDGET_H

#include "ui/AMStripToolView.h"

class AMStripToolWidget : public AMStripToolView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripToolWidget(AMStripTool *stripTool, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMStripToolWidget();

	/// Returns the striptool being viewed.
	AMStripTool* stripTool() const { return stripTool_; }

signals:
	/// Notifier that the striptool being viewed has changed.
	void stripToolChanged(AMStripTool *newTool);

public:
	/// Sets the striptool being viewed.
	void setStripTool(AMStripTool *newTool);

protected slots:
	/// Displays a dialog for adding a control by name.
	void showAddPVDialog();

	/// Handles creating a custom context menu.
	virtual void onCustomContextMenuRequested(QPoint point);

protected:
	/// The striptool being viewed.
	AMStripTool *stripTool_;

	/// The plot widget.
	MPlotWidget *plotWidget_;
};

#endif // AMSTRIPTOOLWIDGET_H
