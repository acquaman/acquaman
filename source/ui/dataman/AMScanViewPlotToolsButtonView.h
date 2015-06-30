#ifndef AMSCANVIEWPLOTTOOLSBUTTONVIEW_H
#define AMSCANVIEWPLOTTOOLSBUTTONVIEW_H

#include <QWidget>
#include <QButtonGroup>
#include <QLayout>
#include <QAbstractButton>
#include <QToolButton>

#include "dataman/AMScanViewPlotTools.h"

class AMScanViewPlotToolsButtonView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewPlotToolsButtonView(AMScanViewPlotTools *tools = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewPlotToolsButtonView();

	/// Returns the plot tools.
	AMScanViewPlotTools* tools() const { return tools_; }

signals:
	/// Notifier that the plot tools have changed.
	void toolsChanged(AMScanViewPlotTools *newTools);

public slots:
	/// Sets the tools.
	void setTools(AMScanViewPlotTools *newTools);

	/// Clears the view.
	void clear();
	/// Updates the view to match the present tool information.
	void update();
	/// Clears and then updates the view.
	void refresh();

protected:
	/// Creates a tool button suitable to represent the given tool.
	QToolButton* createToolButton(MPlotAbstractTool *tool);
	/// Returns an icon representation of the given tool.
	QIcon toolToIcon(MPlotAbstractTool *tool);

protected:
	/// The available plot tools.
	AMScanViewPlotTools *tools_;
	/// The tool buttons group.
	QButtonGroup *buttons_;

	/// The main layout.
	QHBoxLayout *layout_;
};

#endif // AMSCANVIEWPLOTTOOLSBUTTONVIEW_H
