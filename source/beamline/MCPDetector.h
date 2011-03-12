#ifndef MCPDETECTOR_H
#define MCPDETECTOR_H

#include "AMDetector.h"
#include "dataman/MCPDetectorInfo.h"
#include "AMControlSet.h"

class MCPDetector : public MCPDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	MCPDetector(const QString &name, AMControlSet *readingsControls, AMControlSet *settingsControls, QObject *parent = 0);
	MCPDetector(const QString& name, AMControl *reading, AMControl *hv, QObject *parent = 0);
	~MCPDetector();

	const QMetaObject* getMetaObject();

	/// NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	AMDetectorInfo* toInfo() const;
	MCPDetectorInfo toMCPInfo() const;

	bool isPoweredOn();

	AMControl* readingCtrl() const;
	AMControl* hvCtrl() const;

	//bool setFromInfo(const AMDetectorInfo &info);
	bool setFromInfo(const AMDetectorInfo *info);
	bool setFromInfo(const MCPDetectorInfo &info);

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

private:
	bool poweredOn_;
};


#endif // MCPDETECTOR_H
