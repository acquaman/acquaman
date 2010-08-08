#ifndef AMDETECTORVIEW_H
#define AMDETECTORVIEW_H

#include "AMControlSetView.h"
#include "beamline/AMDetector.h"
#include "QMessageBox"

class AMDetectorView : public QGroupBox
{
Q_OBJECT
public:
	explicit AMDetectorView(QWidget *parent = 0);

signals:

public slots:
};


class PGTDetectorView : public PGTDetectorInfoView
{
	Q_OBJECT
public:
	PGTDetectorView(PGTDetector *detector, AMDetectorInfo *configDetector = 0, bool editMode = false, QWidget *parent = 0);

protected slots:
	void onIntegrationModeUpdate(double value);
	void onIntegrationModeChange(int index);
	void setEditMode(bool editMode);
	void setEditable();

protected:
	PGTDetector *detector_;
	bool editMode_;
	QDoubleSpinBox *integrationTimeFbk_;
	QComboBox *integrationModeFbk_;
	QDoubleSpinBox *hvFbk_;
};

class MCPDetectorView : public MCPDetectorInfoView
{
	Q_OBJECT
public:
	MCPDetectorView(MCPDetector *detector, AMDetectorInfo *configDetector = 0, bool editMode = false, QWidget *parent = 0);

protected slots:
	void setEditMode(bool editMode);
	void setEditable();

protected:
	MCPDetector *detector_;
	bool editMode_;
	QDoubleSpinBox *hvFbk_;
};

class AMDetectorSetView : public AMDetectorInfoSetView
{
	Q_OBJECT
public:
	AMDetectorSetView(AMDetectorInfoSet *viewSet, AMDetectorInfoSet *configSet = 0, bool setup = true, QWidget *parent = 0);

public slots:
	void setEditMode(bool editMode);
	void setEditable();

protected:
	bool editMode_;

	virtual void runSetup();
	virtual QWidget* detailViewByType(AMDetectorInfo *detector, AMDetectorInfo *configDetector);
};


#endif // AMDETECTORVIEW_H
