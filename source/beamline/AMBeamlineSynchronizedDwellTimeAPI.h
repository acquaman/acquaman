#ifndef AMBEAMLINESYNCHRONIZEDDWELLTIMEAPI_H
#define AMBEAMLINESYNCHRONIZEDDWELLTIMEAPI_H

class AMSynchronizedDwellTime;

class AMBeamlineSynchronizedDwellTimeAPI {
public:
	AMBeamlineSynchronizedDwellTimeAPI();

	virtual ~AMBeamlineSynchronizedDwellTimeAPI();

	/// Returns the beamline's synchronized dwell time object if one is available. Returns 0 (NULL) otherwise.
	virtual AMSynchronizedDwellTime* synchronizedDwellTime() const { return 0; }
};

#endif // AMBEAMLINESYNCHRONIZEDDWELLTIMEAPI_H
