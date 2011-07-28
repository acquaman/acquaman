#ifndef VORTEXDETECTORSTATUSHELPER_H
#define VORTEXDETECTORSTATUSHELPER_H

#include <QObject>

#include "beamline/AMProcessVariable.h"

class VortexDetectorStatusHelper : public QObject
{
	Q_OBJECT
public:
	/// Constructor that sets up some PVs and connects them to others.  This is required because of naming convention problems between similar detectors used on VESPERS.
	explicit VortexDetectorStatusHelper(QObject *parent = 0);

signals:

public slots:

protected slots:
	/// Determines if the new single element MCA status value needs to be propogated.
	void setSingleMCA(int val) { if (singleMCAto_->getInt() != singleMCAfrom_->getInt()) singleMCAfrom_->setValue(val); }
	/// Determines if the new single element DXP status value needs to be propogated.
	void setSingleDXP(int val) { if (singleDXPto_->getInt() != singleDXPfrom_->getInt()) singleDXPfrom_->setValue(val); }
	/// Determines if the new four element MCA status value needs to be propogated.
	void setFourMCA(int val) { if (fourMCAto_->getInt() != fourMCAfrom_->getInt()) fourMCAfrom_->setValue(val); }
	/// Determines if the new four element DXp status value needs to be propogated.
	void setFourDXP(int val) { if (fourDXPto_->getInt() != fourDXPfrom_->getInt()) fourDXPfrom_->setValue(val); }

protected:
	/// PVs for the different things that need connection.
	AMProcessVariable *singleMCAto_;
	AMProcessVariable *singleMCAfrom_;

	AMProcessVariable *singleDXPto_;
	AMProcessVariable *singleDXPfrom_;

	AMProcessVariable *fourMCAto_;
	AMProcessVariable *fourMCAfrom_;

	AMProcessVariable *fourDXPto_;
	AMProcessVariable *fourDXPfrom_;
};

#endif // VORTEXDETECTORSTATUSHELPER_H
