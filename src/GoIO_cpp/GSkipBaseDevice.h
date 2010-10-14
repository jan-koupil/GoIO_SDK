// GSkipBaseDevice.h
//

#ifndef _GSKIPBASEDEVICE_H_
#define _GSKIPBASEDEVICE_H_

#include "GDeviceIO.h"

#include "GSkipCommExt.h"
#include "GCyclopsCommExt.h"
#include "GSkipComm.h"
#include "GMBLSensor.h"
#include "GVernierUSB.h"
#include "GCircularBuffer.h"

#define SKIP_HOST_IO_STATUS_TIMED_OUT	1

class GSkipBaseDevice : public GDeviceIO
{
public:
						GSkipBaseDevice(GPortRef *pPortRef);

	virtual				~GSkipBaseDevice(); 

	virtual int			GetVendorID(void) = 0;
	virtual int			GetProductID(void) = 0;

	int 				Open(GPortRef *pPortRef);// override from GDeviceIO

	// Platform specific routines:

	// overrides from GDeviceIO:
	bool				OSInitialize(void);
	int 				OSOpen(GPortRef *pPortRef);
	int					OSClose(void);
	void				OSDestroy(void);

	// Note that these are packet oriented, rather than byte oriented.
	int					OSReadMeasurementPackets(void * pBuffer, int * pIONumPackets, int nBufferSizeInPackets);
	int					OSReadCmdRespPackets(void * pBuffer, int * pIONumPackets, int nBufferSizeInPackets);
	int 				OSWriteCmdPackets(void * pBuffer, int nNumPackets);

	int					OSMeasurementPacketsAvailable(unsigned char *pNumMeasurementsInLastPacket = NULL);
	int					OSCmdRespPacketsAvailable(void);

	int 				OSClearIO(void);
	int					OSClearMeasurementPacketQueue();
	int					OSClearCmdRespPacketQueue();

	int 				OSBytesAvailable(void); //not used - will assert!
	int 				OSRead(void * /*pBuffer*/, int * /*pIONumBytes*/, int /*nBufferSize*/); //not used - will assert!
	int 				OSWrite(void * /*pBuffer*/, int * /*pIONumBytes*/); //not used - will assert!

	// End of platform specific routines.

	int					SendCmd(unsigned char cmd, void *pParams, int nParamBytes);
	int					GetNextResponse(void *pRespBuf, int *pnRespBytes, unsigned char *pCmd, bool *pErrRespFlag, 
							int nTimeoutMs = 1000, bool *pExitFlag = NULL);
	virtual int			SendCmdAndGetResponse(unsigned char cmd, void *pParams, int nParamBytes, void *pRespBuf, int *pnRespBytes, 
							int nTimeoutMs = 1000, bool *pExitFlag = NULL);

	void				GetLastCmdResponseStatus(unsigned char *pLastCmd, unsigned char *pLastCmdStatus,
							unsigned char *pLastCmdWithErrorRespSentOvertheWire, unsigned char *pLastErrorSentOvertheWire);

	int					ReadNonVolatileMemory(bool bLocal, void *pBuf, unsigned int addr, unsigned int nBytesToRead,
							int nTimeoutMs = 1000, bool *pExitFlag = NULL);
	int					WriteNonVolatileMemory(bool bLocal, void *pBuf, unsigned int addr, unsigned int nBytesToRead,
							int nTimeoutMs = 1000, bool *pExitFlag = NULL);

	virtual unsigned int	GetMaxLocalNonVolatileMemAddr(void) = 0;
	virtual unsigned int	GetMaxRemoteNonVolatileMemAddr(void) = 0;

	virtual	int			ReadSensorDDSMemory(unsigned char * /*pBuf*/, unsigned int /*ddsAddr*/, unsigned int /*nBytesToRead*/, 
							int nTimeoutMs = 1000, bool *pExitFlag = NULL) = 0;
	virtual	int			WriteSensorDDSMemory(unsigned char * /*pBuf*/, unsigned int /*ddsAddr*/, unsigned int /*nBytesToWrites*/,
							int nTimeoutMs = 1000, bool *pExitFlag = NULL) = 0;

	virtual real		GetMeasurementTickInSeconds(void) = 0; 
	real				CalculateNearestLegalMeasurementPeriod(real fPeriodInSeconds);
	int					SetMeasurementPeriod(real fPeriodInSeconds, int nTimeoutMs = 1000);
	real				GetMeasurementPeriod(int nTimeoutMs = 1000);

	virtual real		GetMinimumMeasurementPeriodInSeconds(void) = 0;
	virtual real		GetMaximumMeasurementPeriodInSeconds(void) = 0;

	int					MeasurementsAvailable(void);
	virtual intVector	ReadRawMeasurements(int count = -1);
    bool                AreMeasurementsEnabled() { return m_bIsMeasuring; }

	int					GetLatestRawMeasurement(void);

	unsigned int		GetHostIOStatus() { return m_hostIOStatus;}

	virtual real		ConvertToVoltage(int raw, EProbeType eProbeType, bool bCalibrateADCReading = true) = 0;

	void				SetDiagnosticsFlag(bool bFlag) { m_bDiagnosticsEnabled = bFlag; }
	bool				GetDiagnosticsFlag() { return m_bDiagnosticsEnabled; }
	GCircularBuffer		*GetDiagnosticInputBufferPtr() { return m_diagnosticInputBufferPtr; }
	GCircularBuffer		*GetDiagnosticOutputBufferPtr() { return m_diagnosticOutputBufferPtr; }

	static StringVector OSGetAvailableDevicesOfType(int nVendorID, int nProductID);

protected:
	virtual int			GetInitCmdResponse(void *pRespBuf, int *pnRespBytes, int nTimeoutMs = 1000, bool *pExitFlag = NULL);

	static real			kVoltsPerBit_ProbeTypeAnalog5V;
	static real			kVoltsOffset_ProbeTypeAnalog5V;
	static real			kVoltsPerBit_ProbeTypeAnalog10V;
	static real			kVoltsOffset_ProbeTypeAnalog10V;

	int					m_nLatestRawMeasurement;
    bool                m_bIsMeasuring;
	unsigned int		m_hostIOStatus;
	unsigned char		m_lastCmd;
	unsigned char		m_lastCmdRespStatus;
	unsigned char		m_lastCmdWithErrorRespSentOvertheWire;
	unsigned char		m_lastErrorSentOvertheWire;
	bool				m_bDiagnosticsEnabled;
	GCircularBuffer		*m_diagnosticInputBufferPtr;
	GCircularBuffer		*m_diagnosticOutputBufferPtr;
	GPriorityMutex		*m_pTraceQueueAccessMutex;
		
private:
	typedef GDeviceIO TBaseClass;
	
};

#endif // _GSKIPBASEDEVICE_H_

