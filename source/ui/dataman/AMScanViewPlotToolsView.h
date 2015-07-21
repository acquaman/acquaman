#ifndef AMSCANVIEWPLOTTOOLSVIEW_H
#define AMSCANVIEWPLOTTOOLSVIEW_H

#include <QWidget>
#include <QLayout>

#include "dataman/AMScanViewPlotTools.h"

#include "ui/dataman/AMScanViewPlotToolsButtonView.h"
#include "ui/dataman/AMScanViewPlotSelectedToolsView.h"

class AMScanViewPlotToolsView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMScanViewPlotToolsView(AMScanViewPlotTools *tools, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanViewPlotToolsView();
	/// Returns the plot tools.
	AMScanViewPlotTools* tools() const { return tools_; }

signals:
	/// Notifier that the plot tools have changed.
	void toolsChanged(AMScanViewPlotTools *newTools);

public:
	/// Sets the plot tools.
	void setTools(AMScanViewPlotTools *newTools);

	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Clears and then updates the view.
	void refresh();

protected:
	/// The plot tools.
	AMScanViewPlotTools *tools_;

	/// The button view.
	AMScanViewPlotToolsButtonView *buttonView_;
	/// The selected tools view.
	AMScanViewPlotSelectedToolsView *selectedToolsView_;
};

#endif // AMSCANVIEWPLOTTOOLSVIEW_H
