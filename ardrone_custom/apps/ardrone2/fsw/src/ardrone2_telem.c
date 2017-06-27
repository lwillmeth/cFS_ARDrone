/******************************************************************************
**
**        Copyright (c) 2016, Odyssey Space Research, LLC.
**
**        Software developed under contract NNJ14HA64B, subcontract
**        NNJ14HA64B-ODY1.
**
**        All rights reserved.  Odyssey Space Research grants
**        to the Government, and other acting on its behalf, a paid-up,
**        nonexclusive, irrevocable, worldwide license in such copyrighted
**        computer software to reproduce, prepare derivative works, and perform
**        publicly and display publicly (but not to distribute copies to the
**        public) by or on behalf of the Government.
**
**        Created by Mathew Benson, mbenson@odysseysr.com
**
******************************************************************************/

/*=======================================================================================
** File Name:  ardrone2_telem.c
**
** Title:  Function Definitions ARDRONE2 App API telemetry
**
**
** Purpose:  Return ARDRONE telemetry
**
** Functions Defined:
**    ARDrone2_ReadTelem()
**    ARDrone2_TlmCleanupCallback()
**
** Limitations, Assumptions, External Events, and Notes:
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2013-09-15 | Mathew Benson | Code Started
**
**=====================================================================================*/

/* Includes */
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "cfe.h"
#include "network_includes.h"
#include "ardrone2_app.h"
#include "ardrone2_telem.h"

/* Extern variables */
extern AppData_t  g_ARDrone2_AppData;

/* Local sturcture */
typedef enum
{
	INITIAL,
	WAITING_FOR_COMMAND_MASK,
	TELEMETRY_ACTIVATED
} TelemState_t;

/* Local variables */
int8        tempBuffer[ARDRONE2_MAX_NAV_BUFFER_SIZE];
NavData_t   *pNavData;
uint32		navSequence;
struct sockaddr_in 		addrNav;
TelemState_t state;
int NavSocketID;

/* Local functions */
int32 ARDrone2_ReadTelem(void);
void ARDrone2_TlmCleanupCallback(void);

/* Main function */
void ARDrone2_TlmMain(void)
{
    CFE_ES_RegisterChildTask();

	ARDrone2_TlmUpdates = 0;

    /* Increment the Main task Execution Counter */
    CFE_ES_IncrementTaskCounter();

	OS_printf("ARDrone2: Telemetry task started.\n");

    pNavData = (NavData_t *) &tempBuffer[0];
    navSequence = ARDRONE2_NAVDATA_SEQUENCE_DEFAULT-1;
    state = INITIAL;

    /* Initiate NavData as stale until one is received. */
    OS_MutSemTake(g_ARDrone2_AppData.NavDataMutexId);
    g_ARDrone2_AppData.NavDataStale = TRUE;
    OS_MutSemGive(g_ARDrone2_AppData.NavDataMutexId);

    /* Create socket */
    NavSocketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(NavSocketID < 0)
    {
    	CFE_EVS_SendEvent(ARDRONE2_INIT_ERR_EID, CFE_EVS_ERROR,"ARDrone2: Create Nav socket failed = %d", errno);
    	goto end_of_function;
    }

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler((void*)&ARDrone2_TlmCleanupCallback);

	memset((char *) &addrNav, 0, sizeof(addrNav));
	addrNav.sin_family      = AF_INET;
    addrNav.sin_port        = htons(ARDRONE2_NAV_DATA_PORT+100);
    addrNav.sin_addr.s_addr = htonl(INADDR_ANY);

	OS_printf("ARDrone2: Telem Task Binding NAV socket.\n");
	if ( bind(NavSocketID, (struct sockaddr *) &addrNav,
			sizeof(addrNav)) < 0)
	{
		CFE_EVS_SendEvent(ARDRONE2_INIT_ERR_EID, CFE_EVS_ERROR,
                          "ARDrone2: Failed to bind NAV command socket.  errno: %d", errno);
		goto end_of_function;
	}

	OS_printf("ARDrone2: Telem Task Resetting connection.\n");
	ARDrone2_ResetTlmConnection();

	OS_printf("ARDrone2: Telem Task Telem entering main loop.\n");
    while (g_ARDrone2_AppData.TelemetryTaskShouldRun)
    {
        CFE_ES_PerfLogEntry(ARDRONE2_TELEM_TASK_PERF_ID);

        ARDrone2_ReadTelem();

    	ARDrone2_TlmUpdates++;

        CFE_ES_PerfLogExit(ARDRONE2_TELEM_TASK_PERF_ID);
	}

end_of_function:
	OS_printf("ARDrone2: Telem exiting main loop.\n");
	ARDrone2_TlmCleanupCallback();

	CFE_ES_ExitChildTask();
}


void ARDrone2_TlmCleanupCallback(void)
{
	if(NavSocketID > 0)
	{
		OS_printf("ARDrone2: Telem cleanup socket.\n");
		close(NavSocketID);
		NavSocketID = 0;
	}
}


int32 ARDrone2_ReadTelem(void)
{
    uint32 addr_len = 0;
	int32 len = 0;
	uint32 navdata_cks = 0;
	uint32 cks = 0;
	int32 size = 0;
	int32 status = 0;

	addr_len = sizeof(addrNav);

    /* Receive data */
    len = recvfrom(NavSocketID,
                   pNavData,
                   ARDRONE2_MAX_NAV_BUFFER_SIZE, 0, 
                   (struct sockaddr *) &addrNav, (socklen_t*)&addr_len);
    if (len < 0)
	{
    	/* IF the socket is non-zero this failure is unexpected */
    	if (NavSocketID > 0)
    	{
			CFE_EVS_SendEvent(TELEM_SOCKET_READ_ERR_EID, CFE_EVS_ERROR,
				"ARDrone2:  Telem socket read failed.  errno: %i", errno);
			status = -1;
    	}
    	/* IF the socket is zero, then the delete callback might have been called
    	 * so don't do anything else. */
		goto end_of_function;
	}
	else
	{
		if( pNavData->header == NAVDATA_HEADER )
		{
			if( ARDrone2_GetMaskFromState(pNavData->ardrone_state, ARDRONE2_NAVDATA_BOOTSTRAP) == TRUE)
			{
				OS_printf("ARDrone2:  Activating NavData Demo.\n");
				ARDrone2_configATCmd("general:navdata_demo", "TRUE");
			}

			if( ARDrone2_GetMaskFromState(pNavData->ardrone_state, ARDRONE2_COMMAND_MASK) == TRUE)
			{
				OS_printf("ARDRONE2_COMMAND_MASK == TRUE\n");
				ARDrone2_cmdSetControl(ACK_CONTROL_MODE);
			}

			if( pNavData->sequence > navSequence )
			{
                /* Lock the mutex */
                OS_MutSemTake(g_ARDrone2_AppData.NavDataMutexId);
                g_ARDrone2_AppData.NavDataStale = FALSE;
                
                memcpy(&g_ARDrone2_AppData.NavDataMsg.buffer[0],
                       &tempBuffer[0], ARDRONE2_MAX_NAV_BUFFER_SIZE);

				ARDrone2_NavDataUnpackAll(&g_ARDrone2_AppData.NavDataUnpacked, 
                                          pNavData, &navdata_cks);
                cks = ARDrone2_NavDataComputeCks(&g_ARDrone2_AppData.NavDataMsg.buffer[0], 
                                                 size - sizeof(NavData_Cks_t) );
                g_ARDrone2_AppData.lastNavDataOptions = 
                    g_ARDrone2_AppData.NavDataUnpacked.last_navdata_refresh;

                //FIXME: This is temporary.  Need a better solution.  Too many memcpy 
                /* Send out Select message */
                memcpy(&g_ARDrone2_AppData.NavDataSelectMsg.demo, 
                       &g_ARDrone2_AppData.NavDataDemoMsg.payload,
                       sizeof(g_ARDrone2_AppData.NavDataDemoMsg.payload));
                memcpy(&g_ARDrone2_AppData.NavDataSelectMsg.time,
                       &g_ARDrone2_AppData.NavDataUnpacked.time,
                       sizeof(g_ARDrone2_AppData.NavDataUnpacked.time));
                memcpy(&g_ARDrone2_AppData.NavDataSelectMsg.mag,
                       &g_ARDrone2_AppData.NavDataUnpacked.magneto,
                       sizeof(g_ARDrone2_AppData.NavDataUnpacked.magneto));

                /* Select message is sent out in the main application source SendOutData function */

                /* Unlock mutex */
                OS_MutSemGive(g_ARDrone2_AppData.NavDataMutexId);
                
                //TODO: Send out individual packets based on option mask 

				if( cks != navdata_cks )
				{
					CFE_EVS_SendEvent(TELEM_CHECKSUM_ERR_EID, CFE_EVS_ERROR,
							"ARDrone2:  [Navdata] Checksum failed : %d (distant) / %d (local)\n",
							(int)navdata_cks, (int)cks);
				}
			}
			else
			{
				CFE_EVS_SendEvent(TELEM_SEQUENCE_ERR_EID, CFE_EVS_ERROR,
						"ARDrone2:  [Navdata] Sequence pb : %d (distant) / %d (local)\n",
						(int)pNavData->sequence, (int)navSequence);
			}
			navSequence = pNavData->sequence;
		}
	}

end_of_function:
    
    return status;
}



void ARDrone2_ResetTlmConnection(void)
{
	int status = 0;
	struct sockaddr_in si_tmp;
	char cBuf[] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int len=14;

	OS_printf("ARDrone2: Telem Task Sending reset message.\n");

    memset((char *) &si_tmp, 0, sizeof(si_tmp));
    si_tmp.sin_family      = AF_INET;
    inet_aton(ARDRONE2_WIFI_IP, &si_tmp.sin_addr);
    si_tmp.sin_port        = htons(ARDRONE2_NAV_DATA_PORT);

	status = sendto(NavSocketID, cBuf, len, 0,
				(struct sockaddr *) &si_tmp, sizeof(si_tmp) );
	if ( status < 0 )
	{
		CFE_EVS_SendEvent(INIT_ERR_EID, CFE_EVS_ERROR, "ARDrone2: Failed to send reset.  errno: %d", errno);
	}
	else
	{
		OS_printf("ARDrone2: Telem Task Reset message sent.\n");
	}
}
