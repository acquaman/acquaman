#include "BioXASFrontEndShutters.h"

BioXASFrontEndShutters:: BioXASFrontEndShutters(QObject *parent) :
	BioXASFrontEndShuttersControl("BioXASFrontEndShutters", parent)
{
	setUpstreamPhotonShutter(new AMReadOnlyPVControl("IPSH1407-I00-01", "IPSH1407-I00-01:state", this));
	setDownstreamPhotonShutter(photonShutterDownstream_ = new CLSBiStateControl("IPSH1407-I00-02", "IPSH1407-I00-02", "IPSH1407-I00-02:state", "IPSH1407-I00-02:opr:open", "IPSH1407-I00-02:opr:close", new AMControlStatusCheckerDefault(2), this));
	setSafetyShutter(new CLSBiStateControl("SSH1407-I00-01", "SSH1407-I00-01", "SSH1407-I00-01:state", "SSH1407-I00-01:opr:open", "SSH1407-I00-01:opr:close", new AMControlStatusCheckerDefault(2), this));
}

BioXASFrontEndShutters::~BioXASFrontEndShutters()
{

}
