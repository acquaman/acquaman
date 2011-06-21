#ifndef MCPDETECTOR_H
#define MCPDETECTOR_H

#include "AMDetector.h"
#include "dataman/MCPDetectorInfo.h"
#include "AMControlSet.h"
#include "AMBeamlineActionItem.h"

class MCPDetector : public MCPDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	MCPDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	MCPDetector(const QString& name, AMControl *reading, AMControl *hv, AMBeamlineActionItem *toggleOnAction, AMBeamlineActionItem *toggleOffAction, AMDetector::ReadMethod readMethod = AMDetector::ImmediateRead, QObject *parent = 0);
	~MCPDetector();

	const QMetaObject* getMetaObject();

	virtual double reading() const;

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMDetectorInfo* toInfo() const;
	MCPDetectorInfo toMCPInfo() const;

	bool isPoweredOn();

	AMControl* readingCtrl() const;
	AMControl* hvCtrl() const;

	/* NTBA March 14, 2011 David Chevrier
	bool setFromInfo(const AMDetectorInfo &info);
	   */
	bool setFromInfo(const AMDetectorInfo *info);
	bool setFromInfo(const MCPDetectorInfo &info);

	bool activate();
	AMBeamlineActionItem* turnOnAction();

	bool settingsMatchFbk(MCPDetectorInfo* settings);

	QString description() const;

public slots:
	void setDescription(const QString &description);
	virtual bool setControls(MCPDetectorInfo *mcpSettings);

signals:
	void poweredOnChanged(bool poweredOn);

protected slots:
	void onControlsConnected(bool connected);
	void onReadingsControlValuesChanged();
	void onSettingsControlValuesChanged();

protected:
	void initializeFromControlSet(AMControlSet *readingsControls, AMControlSet *settingsControls);

protected:
	AMControlSet *readingsControls_;
	AMControlSet *settingsControls_;
	bool ownsControlSets_;

	AMBeamlineActionItem *toggleOnAction_;
	AMBeamlineActionItem *toggleOffAction_;

private:
	bool poweredOn_;
};


#endif // MCPDETECTOR_H
