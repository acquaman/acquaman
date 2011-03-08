#ifndef MCPDETECTOR_H
#define MCPDETECTOR_H

#include "AMDetector.h"
#include "dataman/MCPDetectorInfo.h"
#include "AMControlSet.h"

class MCPDetector : public MCPDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	MCPDetector(const QString &name, AMControlSet *controls, QObject *parent = 0);
	MCPDetector(const QString& name, AMControl *reading, AMControl *hvSetpoint, AMControl *hvFbk, QObject *parent = 0);
	~MCPDetector();

	const QMetaObject* getMetaObject();
	AMDetectorInfo toInfo();

	AMControl* readingCtrl() const;
	AMControl* hvSetpointCtrl() const;
	AMControl* hvFbkCtrl() const;

	bool setFromInfo(const AMDetectorInfo &info);

	bool settingsMatchFbk(MCPDetectorInfo* settings);

public slots:
	virtual bool setControls(MCPDetectorInfo *mcpSettings);

protected slots:
	void onControlsConnected(bool connected);
	void onControlValuesChanged();

protected:
	void initializeFromControlSet(AMControlSet *controls);

protected:
	AMControlSet *controls_;
	bool ownsControlSet_;
	/*
	AMControl *reading_;
	AMControl *hvSetpoint_;
	AMControl *hvFbk_;
	*/
};


#endif // MCPDETECTOR_H
