#ifndef AMSCANVIEWPLOTSELECTEDTOOLSVIEW_H
#define AMSCANVIEWPLOTSELECTEDTOOLSVIEW_H

#include <QWidget>
#include <QLayout>

#include "dataman/AMScanViewPlotTools.h"

class AMScanViewPlotSelectedToolsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewPlotSelectedToolsView(AMScanViewPlotTools *tools, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewPlotSelectedToolsView();
	/// Returns the plot tools.
	AMScanViewPlotTools* tools() const { return tools_; }

signals:
	/// Notifier that the plot tools have changed.
	void toolsChanged(AMScanViewPlotTools *newTools);

public slots:
	/// Sets the tools.
	void setTools(AMScanViewPlotTools *newTools);

	/// Clears the view.
	virtual void clear();
	/// Updates the view to match the present tool information.
	virtual void update();
	/// Clears and then updates the view.
	virtual void refresh();

protected:
	/// Creates a tool view suitable to represent the given tool. Returns 0 if no view for the given tool is available.
	QWidget* createToolView(MPlotAbstractTool *tool);

protected:
	/// The available plot tools.
	AMScanViewPlotTools *tools_;

	/// The tool-specific views.
	QList<QWidget*> toolViews_;
	/// The tool-specific views layout.
	QHBoxLayout *layout_;
};

#endif // AMSCANVIEWPLOTSELECTEDTOOLSVIEW_H
