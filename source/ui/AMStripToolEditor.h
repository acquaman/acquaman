#ifndef AMSTRIPTOOLEDITOR_H
#define AMSTRIPTOOLEDITOR_H

#include "dataman/AMScan.h"

#include "ui/AMStripToolView.h"
#include "ui/dataman/AMScanView.h"
#include "ui/dataman/AMDataSourcesEditor.h"

class AMStripToolEditor : public AMStripToolView
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripToolEditor(AMStripTool *stripTool, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMStripToolEditor();

	/// Returns the striptool being viewed.
	virtual AMStripTool* stripTool() const { return stripTool_; }

signals:

public slots:
	/// Sets the striptool being viewed.
	void setStripTool(AMStripTool *newTool);

protected:
	/// Creates and returns a scan populated with datasources matching the given striptool's.
	AMScan* createScan(AMStripTool *stripTool);

protected:
	/// The striptool being viewed.
	AMStripTool *stripTool_;
	/// The striptool 'scan'.
	AMScan *scan_;
	/// The scan set model.
	AMScanSetModel *scanSetModel_;
	/// The scan view used to visualize the strip tool data.
	AMScanView *stripToolView_;
	/// The editor used to editor the strip tool sources.
	AMDataSourcesEditor *sourcesEditor_;
};

#endif // AMSTRIPTOOLEDITOR_H
