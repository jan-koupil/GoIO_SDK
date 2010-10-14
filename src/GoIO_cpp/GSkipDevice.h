// GSkipDevice.h
//

#ifndef _GSKIPDEVICE_H_
#define _GSKIPDEVICE_H_

#include "GSkipBaseDevice.h"

class GSkipDevice : public GSkipBaseDevice
{
public:
						GSkipDevice(GPortRef *pPortRef);
	virtual				~GSkipDevice();

	virtual int			GetVendorID(void) { return VERNIER_DEFAULT_VENDOR_ID; }
	virtual int			GetProductID(void) { return SKIP_DEFAULT_PRODUCT_ID; }

	virtual unsigned int	GetMaxLocalNonVolatileMemAddr(void);
	virtual unsigned int	GetMaxRemoteNonVolatileMemAddr(void);

	static StringVector GetAvailableDevices(void) { return TBaseClass::OSGetAvailableDevicesOfType(VERNIER_DEFAULT_VENDOR_ID, SKIP_DEFAULT_PRODUCT_ID); }
	static void					StoreSnapshotOfAvailableDevices(const StringVector &devices) { m_snapshotOfAvailableDevices = devices; }
	static const StringVector &	GetSnapshotOfAvailableDevices(void) { return m_snapshotOfAvailableDevices; }

	virtual int		SendCmdAndGetResponse(unsigned char cmd, void *pParams, int nParamBytes, void *pRespBuf, int *pnRespBytes, 
							int nTimeoutMs = 1000, bool *pExitFlag = NULL);
	virtual	int		ReadSensorDDSMemory(unsigned char * /*pBuf*/, unsigned int /*ddsAddr*/, unsigned int /*nBytesToRead*/, 
							int nTimeoutMs = 1000, bool *pExitFlag = NULL);
	virtual	int		WriteSensorDDSMemory(unsigned char * /*pBuf*/, unsigned int /*ddsAddr*/, unsigned int /*nBytesToWrites*/,
							int nTimeoutMs = 1000, bool *pExitFlag = NULL);

	static real k_fSkipMaxDeltaT; //Const Min and max delta T
	static real k_fSkipMinDeltaT;

	real				GetMeasurementTickInSeconds(void) { return 0.001; }
	real				GetMinimumMeasurementPeriodInSeconds(void) { return k_fSkipMinDeltaT; }
	real				GetMaximumMeasurementPeriodInSeconds(void) { return k_fSkipMaxDeltaT; } 

	virtual real		ConvertToVoltage(int raw, EProbeType eProbeType, bool bCalibrateADCReading = true);
	int					ConvertVoltageToRaw(real fVoltage, EProbeType eProbeType);

	void				SetSkipFlashRecord(const GSkipFlashMemoryRecord &rec) { m_flashRec = rec; }
	void				GetSkipFlashRecord(GSkipFlashMemoryRecord *pRec) { *pRec = m_flashRec; }

	int					WriteSkipFlashRecord(const GSkipFlashMemoryRecord &rec, int nTimeoutMs);
	int					ReadSkipFlashRecord(GSkipFlashMemoryRecord *pRec, int nTimeoutMs);

private:
	typedef GSkipBaseDevice TBaseClass;

	int					SendInitCmdAndGetResponse(void *pParams, int nParamBytes, void *pRespBuf, int *pnRespBytes, 
							int nTimeoutMs, bool *pExitFlag = NULL);

	static StringVector	m_snapshotOfAvailableDevices;
	GSkipFlashMemoryRecord m_flashRec;
};

#endif // _GSKIPDEVICE_H_
