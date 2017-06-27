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
** File Name:  ardrone2_app.c
**
** Title:  Function Definitions for the AR Drone 2.0 Driver
**
**
** Purpose:  This source file contains all necessary function definitions to run ARDrone2
**           application.  The ARDRONE2 application is the driver application
**           to communicate with the drone via sockets, according to the ARDrone2
**           SDK 2.0.
**
** Functions Defined:
** ARDrone2_InitApp()
** ARDrone2_InitEvent()
** ARDrone2_InitData()
** ARDrone2_InitPipe()
** ARDrone2_AppMain(void)
** ARDrone2_CleanupCallback()
** ARDrone2_RcvMsg()
** ARDrone2_ProcessNewCmds()
** ARDrone2_ProcessNewAppCmds()
** ARDrone2_ReportHousekeeping()
** ARDrone2_SendOutData()
** ARDrone2_VerifyCmdLength()
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to all functions in the file.
**    2. List the external source(s) and event(s) that can cause the functions in this
**       file to execute.
**    3. List known limitations that apply to the functions in this file.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2012-11-18 | Mathew Benson | Build #: Code Started
**
**=====================================================================================*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "cfe.h"
#include "osapi.h"

#include "ardrone2_app.h"

/*
** Local Defines
*/
//degrees to radians
#define DEG2RAD(x) ((x)*3.1415926/180)
//radians to degrees
#define RAD2DEG(x) ((x)/3.1415926*180)

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/
AppData_t  g_ARDrone2_AppData;

/*
** Local Variables
*/

/*
** Local Function Definitions
*/

/*=====================================================================================
** Name: ARDrone2_AppMain
**
** Purpose: To define ARDrone2 application's entry point and main process loop
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_RunLoop
**    CFE_ES_PerfLogEntry
**    CFE_ES_PerfLogExit
**    CFE_ES_ExitApp
**    ARDrone2_InitApp
**    ARDrone2_RcvMsg
**
** Called By:
**    TBD
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDrone2_AppMain(void)
{
	int32 iStatus = CFE_ES_RegisterApp();

    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("ARDrone2 - Failed to register the app (%ld)\n",
                             iStatus);
        /* Exit the application */
		CFE_ES_ExitApp(g_ARDrone2_AppData.uiRunStatus);
		return;
    }

    /* Start Performance Log entry */
    CFE_ES_PerfLogEntry(ARDRONE2_MAIN_TASK_PERF_ID);

    iStatus = ARDrone2_InitApp();

    /* Perform application initializations */
    if (iStatus != CFE_SUCCESS)
    {
        g_ARDrone2_AppData.uiRunStatus = CFE_ES_APP_ERROR;
    } else {
        // Don't want to do performance logging on the startup sync
        CFE_ES_PerfLogExit(ARDRONE2_MAIN_TASK_PERF_ID);
        CFE_ES_WaitForStartupSync(ARDRONE2_TIMEOUT_MSEC);
        CFE_ES_PerfLogEntry(ARDRONE2_MAIN_TASK_PERF_ID);
    }

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(ARDRONE2_MAIN_TASK_PERF_ID);

    /* Application main loop */
    while (CFE_ES_RunLoop(&g_ARDrone2_AppData.uiRunStatus) == TRUE)
    {
        /* run at 10hz unless a command is received */
        /* FIXME This is more stable than relying on SCH */
        //ARDrone2_RcvMsg(100);
        ARDrone2_RcvMsg(CFE_SB_PEND_FOREVER);
    }

    /* Stop Performance Log entry */
    CFE_ES_PerfLogExit(ARDRONE2_MAIN_TASK_PERF_ID);

    /* Exit the application */
    CFE_ES_ExitApp(g_ARDrone2_AppData.uiRunStatus);
} 

/*=====================================================================================
** Name: ARDrone2_InitApp
**
** Purpose: To initialize all data local to and used by ARDrone2 application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_ES_RegisterApp
**    CFE_ES_WriteToSysLog
**    CFE_EVS_SendEvent
**    OS_TaskInstallDeleteHandler
**    ARDrone2_InitEvent
**    ARDrone2_InitPipe
**    ARDrone2_InitData
**
** Called By:
**    ARDrone2_AppMain
**
** Global Inputs/Reads:
**
** Global Outputs/Writes:
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDrone2_InitApp()
{
    int32  iStatus=CFE_SUCCESS;
	int32 status = 0;
	int32 ReturnCode;

    g_ARDrone2_AppData.uiRunStatus = CFE_ES_APP_RUN;

    iStatus = ARDrone2_InitEvent();
    if( iStatus != CFE_SUCCESS)
    {
        iStatus = -1;
        goto ARDrone2_InitApp_Exit_Tag;
    }

    iStatus = ARDrone2_InitPipe();
    if( iStatus != CFE_SUCCESS)
    {
        iStatus = -1;
        goto ARDrone2_InitApp_Exit_Tag;
    }

    iStatus = ARDrone2_InitData();
    if( iStatus != CFE_SUCCESS)
    {
        iStatus = -1;
        goto ARDrone2_InitApp_Exit_Tag;
    }

    /* Init Mutex for ATCmd protection */
    ReturnCode = OS_MutSemCreate(&g_ARDrone2_AppData.AtCmdChildTaskMutexId,
    		"ARD2_ChldCmd_Mut", 0);

    if(ReturnCode != OS_SUCCESS)
    {
          CFE_ES_WriteToSysLog("ARD2 Init: Error: AtCmd Child Task mutex could not be created. RC=%ld\n", ReturnCode);
    }


    /* Init Mutex for AT Telem protection */
    ReturnCode = OS_MutSemCreate(&g_ARDrone2_AppData.AtTelemChildTaskMutexId,
    		"ARD2_ChlTelem_Mut", 0);

    if(ReturnCode != OS_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("ARD2 Init: Error: AtTelem Child Task mutex could not be created. RC=%ld\n", ReturnCode);
    }

	/* Create child task for AT commanding */
	OS_printf("ARDrone2: Creating AT Command task.\n");
	g_ARDrone2_AppData.ATCommandTaskShouldRun = TRUE;
	status = CFE_ES_CreateChildTask(
				&g_ARDrone2_AppData.ATCommandTaskId,
				"ARDrone2AtCmd",
				(CFE_ES_ChildTaskMainFuncPtr_t) ARDrone2_ATCmdMain,
				0, 16000, 50, 0);
	if(status != CFE_SUCCESS)
	{
		g_ARDrone2_AppData.ATCommandTaskId = 0;
		CFE_EVS_SendEvent(TELEM_TASK_ERR_EID, CFE_EVS_ERROR,
			"ARDrone2: Failed to create AT Command task.");
		g_ARDrone2_AppData.ATCommandTaskShouldRun = FALSE;
	}

	/* Create child task for telemetry listening */
	OS_printf("ARDrone2: Creating telemetry task.\n");
	g_ARDrone2_AppData.TelemetryTaskShouldRun = TRUE;
	status = CFE_ES_CreateChildTask(
				&g_ARDrone2_AppData.TelemetryTaskId,
				"ARDrone2Telem",
				(CFE_ES_ChildTaskMainFuncPtr_t) ARDrone2_TlmMain,
				0, 16000, 50, 0);
	if(status != CFE_SUCCESS)
	{
		g_ARDrone2_AppData.TelemetryTaskId = 0;
		CFE_EVS_SendEvent(TELEM_TASK_ERR_EID, CFE_EVS_ERROR,
			"ARDrone2: Failed to create telemetry listener task.");
		g_ARDrone2_AppData.TelemetryTaskShouldRun = FALSE;
	}

    /* Install the cleanup callback */
    OS_TaskInstallDeleteHandler((void*)&ARDrone2_CleanupCallback);

ARDrone2_InitApp_Exit_Tag:
    if (iStatus == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(INIT_INF_EID, CFE_EVS_INFORMATION,
                          "ARDrone2 - Application initialized");
        OS_printf("ARDrone2: Init complete.\n");
    }
    else
    {
        CFE_ES_WriteToSysLog("ARDrone2 - Application failed to initialize\n");
    }

    return (iStatus);
}

/*=====================================================================================
** Name: ARDrone2_InitEvent
**
** Purpose: To initialize and register event table for ARDrone2 application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_EVS_Register
**    CFE_ES_WriteToSysLog
**
** Called By:
**    ARDrone2_InitApp
**
** Global Inputs/Reads:
**    TBD
**
** Global Outputs/Writes:
**    g_ARDrone2_AppData.EventTbl
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDrone2_InitEvent()
{
    int32  iStatus=CFE_SUCCESS;

    /* Create the event table */
    memset((void*)g_ARDrone2_AppData.EventTbl, 0x00, sizeof(g_ARDrone2_AppData.EventTbl));

    g_ARDrone2_AppData.EventTbl[0].EventID = RESERVED_EID;
    g_ARDrone2_AppData.EventTbl[1].EventID = INF_EID;
    g_ARDrone2_AppData.EventTbl[2].EventID = INIT_INF_EID;
    g_ARDrone2_AppData.EventTbl[3].EventID = ILOAD_INF_EID;
    g_ARDrone2_AppData.EventTbl[4].EventID = CDS_INF_EID;
    g_ARDrone2_AppData.EventTbl[5].EventID = CMD_INF_EID;

    g_ARDrone2_AppData.EventTbl[ 6].EventID = ERR_EID;
    g_ARDrone2_AppData.EventTbl[ 7].EventID = INIT_ERR_EID;
    g_ARDrone2_AppData.EventTbl[ 8].EventID = ILOAD_ERR_EID;
    g_ARDrone2_AppData.EventTbl[ 9].EventID = CDS_ERR_EID;
    g_ARDrone2_AppData.EventTbl[10].EventID = CMD_ERR_EID;
    g_ARDrone2_AppData.EventTbl[11].EventID = PIPE_ERR_EID;
    g_ARDrone2_AppData.EventTbl[12].EventID = MSGID_ERR_EID;
    g_ARDrone2_AppData.EventTbl[13].EventID = MSGLEN_ERR_EID;
    g_ARDrone2_AppData.EventTbl[14].EventID = ACQ_PTR_ERR_EID;
    g_ARDrone2_AppData.EventTbl[15].EventID = CAL_TBL_REG_ERR_EID;
    g_ARDrone2_AppData.EventTbl[16].EventID = CAL_TBL_LOAD_ERR_EID;
    g_ARDrone2_AppData.EventTbl[17].EventID = CAL_TBL_MNG_ERR_EID;
    g_ARDrone2_AppData.EventTbl[18].EventID = CAL_TBL_GADR_ERR_EID;
    g_ARDrone2_AppData.EventTbl[19].EventID = TTYPA2_OPEN_ERR_EID;
    g_ARDrone2_AppData.EventTbl[20].EventID = FCNTL_ERR_EID;
    g_ARDrone2_AppData.EventTbl[21].EventID = TCGEETATTR_ERR_EID;
    g_ARDrone2_AppData.EventTbl[22].EventID = CTSETISPEED_ERR_EID;
    g_ARDrone2_AppData.EventTbl[23].EventID = CTSETOSPEED_ERR_EID;
    g_ARDrone2_AppData.EventTbl[24].EventID = TCSETATTR_ERR_EID;
    g_ARDrone2_AppData.EventTbl[25].EventID = GPIOSET_ERR_EID;
    g_ARDrone2_AppData.EventTbl[26].EventID = TTYPA2_WRITE_ERR_EID;
    g_ARDrone2_AppData.EventTbl[27].EventID = SOCKETCREATE_ERR_EID;

    /* Register the table with CFE */
    iStatus = CFE_EVS_Register(g_ARDrone2_AppData.EventTbl,
                               EVT_CNT, CFE_EVS_BINARY_FILTER);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("ARDrone2 - Failed to register with EVS (%ld)\n", iStatus);
    }

    return (iStatus);
}

/*=====================================================================================
** Name: ARDrone2_InitPipe
**
** Purpose: To initialize all message pipes and subscribe to messages for ARDrone2 application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_CreatePipe
**    CFE_SB_Subscribe
**    CFE_SB_SubscribeEx
**    CFE_ES_WriteToSysLog
**
** Called By:
**    ARDrone2_InitApp
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    g_ARDrone2_AppData.usSchPipeDepth
**    g_ARDrone2_AppData.cSchPipeName
**    g_ARDrone2_AppData.SchPipeId
**    g_ARDrone2_AppData.usCmdPipeDepth
**    g_ARDrone2_AppData.cCmdPipeName
**    g_ARDrone2_AppData.CmdPipeId
**    g_ARDrone2_AppData.usTlmPipeDepth
**    g_ARDrone2_AppData.cTlmPipeName
**    g_ARDrone2_AppData.TlmPipeId
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Psuedo-code or description of basic algorithm
**
** Author(s):  Mathew Benson 
**
**  History:
**  2012-11-18 | Mathew Benson | Code Started
**  2013-10-15 | Guy de Caurfel | Added AT Command Pipe
**=====================================================================================*/
int32 ARDrone2_InitPipe()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init schedule pipe */
    g_ARDrone2_AppData.usSchPipeDepth = ARDRONE2_SCH_PIPE_DEPTH;
    memset((void*)g_ARDrone2_AppData.cSchPipeName, '\0', 
           sizeof(g_ARDrone2_AppData.cSchPipeName));
    strncpy(g_ARDrone2_AppData.cSchPipeName, "ARDrone2_SCH_PIPE", 
            OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_ARDrone2_AppData.SchPipeId,
                                 g_ARDrone2_AppData.usSchPipeDepth,
                                 g_ARDrone2_AppData.cSchPipeName);
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("ARDrone2 - Failed to create SCH pipe (%ld)\n",
                             iStatus);
        goto ARDrone2_InitPipe_Exit_Tag;
    }
    
    iStatus = CFE_SB_Subscribe(ARDRONE2_WAKEUP_MID, 
                               g_ARDrone2_AppData.SchPipeId);
    if(iStatus != CFE_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("ARDrone2 - Failed to subscribe to "
                             "WAKEUP_MID. (%ld)\n", iStatus);
    	goto ARDrone2_InitPipe_Exit_Tag;
    }
    
    iStatus = CFE_SB_Subscribe(ARDRONE2_READ_INPUTS_MID, 
                               g_ARDrone2_AppData.SchPipeId);
    if(iStatus != CFE_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("ARDrone2 - Failed to subscribe to "
                             "READ_INPUTS_MID. (%ld)\n", iStatus);
    	goto ARDrone2_InitPipe_Exit_Tag;
    }

    iStatus = CFE_SB_Subscribe(ARDRONE2_WRITE_OUTPUTS_MID, 
                               g_ARDrone2_AppData.SchPipeId);
    if(iStatus != CFE_SUCCESS)
    {
    	CFE_ES_WriteToSysLog("ARDrone2 - SCH Pipe failed to subscribe to "
                             "WRITE_OUTPUTS_MID. (%ld)\n", iStatus);
        goto ARDrone2_InitPipe_Exit_Tag;
    }

    /* Init command pipe */
    g_ARDrone2_AppData.usCmdPipeDepth = ARDRONE2_CMD_PIPE_DEPTH ;
    memset((void*)g_ARDrone2_AppData.cCmdPipeName, '\0', 
           sizeof(g_ARDrone2_AppData.cCmdPipeName));
    strncpy(g_ARDrone2_AppData.cCmdPipeName, "ARDrone2_CMD_PIPE", 
            OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_ARDrone2_AppData.CmdPipeId,
                                 g_ARDrone2_AppData.usCmdPipeDepth,
                                 g_ARDrone2_AppData.cCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to command messages */
        iStatus = CFE_SB_Subscribe(ARDRONE2_CMD_MID, 
                                   g_ARDrone2_AppData.CmdPipeId);
        if(iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ARDrone2 - CMD Pipe failed to subscribe to "
                                 "ARDRONE2_CMD_MID. (%ld)\n",
                                 iStatus);
            goto ARDrone2_InitPipe_Exit_Tag;
        }
        
        iStatus = CFE_SB_Subscribe(ARDRONE2_SEND_HK_MID, 
                                   g_ARDrone2_AppData.CmdPipeId);
        if(iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ARDrone2 - CMD Pipe failed to subscribe to "
                                 "ARDRONE2_SEND_HK_MID. (%ld)\n",
                                 iStatus);
            goto ARDrone2_InitPipe_Exit_Tag;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("ARDrone2 - Failed to create CMD pipe (%ld)\n",
                             iStatus);
        goto ARDrone2_InitPipe_Exit_Tag;
    }
    
    /* Init AT command pipe */
    g_ARDrone2_AppData.usATCmdPipeDepth = ARDRONE2_AT_CMD_PIPE_DEPTH ;
    memset((void*)g_ARDrone2_AppData.cATCmdPipeName, '\0', 
           sizeof(g_ARDrone2_AppData.cATCmdPipeName));
    strncpy(g_ARDrone2_AppData.cATCmdPipeName, "ARDrone2_AT_CMD_PIPE", 
            OS_MAX_API_NAME-1);

    iStatus = CFE_SB_CreatePipe(&g_ARDrone2_AppData.ATCmdPipeId,
                                 g_ARDrone2_AppData.usATCmdPipeDepth,
                                 g_ARDrone2_AppData.cATCmdPipeName);
    if (iStatus == CFE_SUCCESS)
    {
        /* Subscribe to command messages */
        iStatus = CFE_SB_SubscribeEx(ARDRONE2_PLAT_CMD_MID, 
                                     g_ARDrone2_AppData.ATCmdPipeId,
                                     CFE_SB_Default_Qos,
                                     ARDRONE2_AT_CMD_PIPE_MAX_PLAT_CMDS);
        if(iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ARDrone2 - CMD Pipe failed to subscribe to "
                                 "ARDRONE2_SPECIFIC_CMD wakeup. (%ld)\n",
                                 iStatus);
            goto ARDrone2_InitPipe_Exit_Tag;
        }

        iStatus = CFE_SB_SubscribeEx(ROTWING_GENERIC_CMD_MID, 
                                     g_ARDrone2_AppData.ATCmdPipeId,
                                     CFE_SB_Default_Qos,
                                     ARDRONE2_AT_CMD_PIPE_MAX_ROTWING_CMDS);
        if(iStatus != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("ARDrone2 - CMD Pipe Failed to subscribe to "
                                 "ROTWING_GENERIC_CMD wakeup. (%ld)\n",
                                 iStatus);
            goto ARDrone2_InitPipe_Exit_Tag;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("ARDrone2 - Failed to create AT CMD pipe "
                             "(%ld)\n", iStatus);
        goto ARDrone2_InitPipe_Exit_Tag;
    }

ARDrone2_InitPipe_Exit_Tag:
    return (iStatus);
}

/*=====================================================================================
** Name: ARDrone2_InitData
**
** Purpose: To initialize global variables used by ARDrone2 application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization
**
** Routines Called:
**    CFE_SB_InitMsg
**
** Called By:
**    ARDrone2_InitApp
**
** Global Inputs/Reads:
**
** Global Outputs/Writes:
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**
** Author(s):  Mathew Benson 
**
** History:  
**  2012-11-18 | Mathew Benson | Code Started
**  2013-10-15 | Guy de Caurfel | Revised for mutex and NavData packets
**=====================================================================================*/
int32 ARDrone2_InitData()
{
    int32  iStatus=CFE_SUCCESS;

    /* Init housekeeping packet */
    memset((void*)&g_ARDrone2_AppData.HkTlm, 0x00, 
           sizeof(g_ARDrone2_AppData.HkTlm));
    CFE_SB_InitMsg(&g_ARDrone2_AppData.HkTlm,
    		ARDRONE2_HK_TLM_MID, sizeof(g_ARDrone2_AppData.HkTlm), TRUE);

    memset((void*)&g_ARDrone2_AppData.NavDataDemoMsg, 0x00, 
           sizeof(g_ARDrone2_AppData.NavDataDemoMsg));
	CFE_SB_InitMsg(&g_ARDrone2_AppData.NavDataDemoMsg,
			ARDRONE2_NAVDATA_DEMO_MID,
			sizeof(g_ARDrone2_AppData.NavDataDemoMsg),
			TRUE);

    memset((void*)&g_ARDrone2_AppData.NavDataSelectMsg, 0x00, 
           sizeof(g_ARDrone2_AppData.NavDataSelectMsg));
	CFE_SB_InitMsg(&g_ARDrone2_AppData.NavDataSelectMsg,
			ARDRONE2_NAVDATA_SELECT_MID,
			sizeof(g_ARDrone2_AppData.NavDataSelectMsg),
			TRUE);
    
    /* Init Mutex for NavData protection */
    iStatus = OS_MutSemCreate(&g_ARDrone2_AppData.NavDataMutexId, 
                              "ARD2_NavData_Mut", 0);

    /* Init Mutex for Cmd Sequence protection */
    iStatus = OS_MutSemCreate(&g_ARDrone2_AppData.CmdSeqMutexId, 
                              "ARD2_CmdSeq_Mut", 0);

    return (iStatus);
}

/*=====================================================================================
** Name: ARDrone2_CleanupCallback
**
** Purpose: To handle any necessary cleanup prior to application exit
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDrone2_CleanupCallback()
{
	OS_printf("ARDrone2 delete callback -- Notifying child tasks.\n");

	OS_MutSemTake(g_ARDrone2_AppData.AtCmdChildTaskMutexId);
	g_ARDrone2_AppData.ATCommandTaskShouldRun = FALSE;
	OS_MutSemGive(g_ARDrone2_AppData.AtCmdChildTaskMutexId);

	OS_MutSemTake(g_ARDrone2_AppData.AtTelemChildTaskMutexId);
	g_ARDrone2_AppData.TelemetryTaskShouldRun = FALSE;
	OS_MutSemGive(g_ARDrone2_AppData.AtTelemChildTaskMutexId);
}

/*=====================================================================================
** Name: ARDrone2_RcvMsg
**
** Purpose: To receive and process messages for ARDrone2 application
**
** Arguments:
**    None
**
** Returns:
**    int32 iStatus - Status of initialization 
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    ARDrone2_ProcessNewCmds
**    ARDrone2_SendOutData
**
** Called By:
**    ARDrone2_Main
**
** Global Inputs/Reads:
**    g_ARDrone2_AppData.SchPipeId
**
** Global Outputs/Writes:
**    g_ARDrone2_AppData.uiRunStatus
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32 ARDrone2_RcvMsg(int32 iBlocking)
{
    int32           iStatus=CFE_SUCCESS;
    CFE_SB_Msg_t*   MsgPtr=NULL;
    CFE_SB_MsgId_t  MsgId;

    /* Wait for WakeUp messages from scheduler */
    CFE_ES_PerfLogExit(ARDRONE2_MAIN_TASK_PERF_ID);

    iStatus = CFE_SB_RcvMsg(&MsgPtr, g_ARDrone2_AppData.SchPipeId, iBlocking);

    CFE_ES_PerfLogEntry(ARDRONE2_MAIN_TASK_PERF_ID);

    if (iStatus == CFE_SUCCESS)
    {
        MsgId = CFE_SB_GetMsgId(MsgPtr);
        switch (MsgId)
        {
            case ARDRONE2_WAKEUP_MID:
                ARDrone2_ProcessNewCmds();
            	ARDrone2_SendOutData();
                break;
            
            case ARDRONE2_READ_INPUTS_MID:
                ARDrone2_ProcessNewCmds();
                break;

            case ARDRONE2_WRITE_OUTPUTS_MID:
            	ARDrone2_SendOutData();
                break;

            default:
                CFE_EVS_SendEvent(MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "ARDrone2 - Recvd invalid ARDrone2 msgId "
                                  "(0x%08X)", MsgId);
                break;
        }
    }
    else if (iStatus == CFE_SB_NO_MESSAGE)
    {
    }
    else if (iStatus == CFE_SB_TIME_OUT)
    {
        /* This is here in case the task is run as a periodic task 
           (not SCH driven) */
        ARDrone2_ProcessNewCmds();
        ARDrone2_SendOutData();
    }
    else
    {
        CFE_EVS_SendEvent(PIPE_ERR_EID, CFE_EVS_ERROR,
			  "ARDrone2: SB pipe read error (0x%08X), app will exit", (unsigned int)iStatus);
        g_ARDrone2_AppData.uiRunStatus= CFE_ES_APP_ERROR;
    }

    return (iStatus);
}

/*=====================================================================================
** Name: ARDrone2_ProcessNewCmds
**
** Purpose: To process incoming command messages for ARDrone2 application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    CFE_SB_RcvMsg
**    CFE_SB_GetMsgId
**    CFE_EVS_SendEvent
**    ARDrone2_ProcessNewAppCmds
**
** Called By:
**    ARDrone2_RcvMsg
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    None
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Pseudo-code or description of basic algorithm
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDrone2_ProcessNewCmds()
{
    int iStatus = CFE_SUCCESS;
    CFE_SB_Msg_t*   CmdMsgPtr=NULL;
    CFE_SB_MsgId_t  CmdMsgId;

    while (1)
    {
        
        iStatus = CFE_SB_RcvMsg(&CmdMsgPtr, g_ARDrone2_AppData.CmdPipeId, 
                                CFE_SB_POLL);
        if (iStatus == CFE_SUCCESS)
        {
            CmdMsgId = CFE_SB_GetMsgId(CmdMsgPtr);
            switch (CmdMsgId)
            {
                case ARDRONE2_CMD_MID:
                    ARDrone2_ProcessNewAppCmds(CmdMsgPtr);
                    break;

                case ARDRONE2_SEND_HK_MID:
                    ARDrone2_ReportHousekeeping();
                    break;

                default:
                    CFE_EVS_SendEvent(MSGID_ERR_EID, CFE_EVS_ERROR,
                                      "ARDrone2 - Recvd invalid CMD msgId "
                                      "(0x%08X)", CmdMsgId);
                    break;
            }
        }
        else if (iStatus == CFE_SB_NO_MESSAGE)
        {
            break;
        }
        else 
        {
            CFE_EVS_SendEvent(PIPE_ERR_EID, CFE_EVS_ERROR,
                  "ARDrone2: CMD pipe read error (0x%08X)", 
                  iStatus);
            g_ARDrone2_AppData.uiRunStatus = CFE_ES_APP_ERROR;
        }
    }
}

/*=====================================================================================
** Name: ARDrone2_ProcessNewAppCmds
**
** Purpose: To process incoming CFE APP command messages for ARDrone2 application
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**
** Called By:
**    ARDrone2_ProcessNewCmds
**
** Global Inputs/Reads:
**
** Global Outputs/Writes:
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**    Pseudo-code or description of basic algorithm
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDrone2_ProcessNewAppCmds(CFE_SB_Msg_t *MsgPtr)
{
    uint32  uiCmdCode = 0;

    if (MsgPtr != NULL)
    {
        uiCmdCode = CFE_SB_GetCmdCode(MsgPtr);
        switch (uiCmdCode)
        {
            case ARDRONE2_NOOP_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt++; 
                break;

            case ARDRONE2_RESET_COUNTERS_CC:
                g_ARDrone2_AppData.HkTlm.usCmdCnt = 0;
                g_ARDrone2_AppData.HkTlm.usCmdErrCnt = 0;
                break;

            default:
                g_ARDrone2_AppData.HkTlm.usCmdErrCnt++;
                CFE_EVS_SendEvent(MSGID_ERR_EID, CFE_EVS_ERROR,
                                  "ARDrone2 - Recvd invalid APP cmd Code (%d)", 
                                  (int)uiCmdCode);
                break;
        }
    }
}

/*=====================================================================================
** Name: ARDrone2_ReportHousekeeping
**
** Purpose: To send housekeeping message
**
** Arguments:
**    None
**
** Returns:
**    None
**
** Routines Called:
**    TBD
**
** Called By:
**    ARDrone2_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Pseudo-code or description of basic algorithm
**
** Author(s):  GSFC, Mathew Benson
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
void ARDrone2_ReportHousekeeping()
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_ARDrone2_AppData.HkTlm);
    CFE_SB_SendMsg((CFE_SB_Msg_t*)&g_ARDrone2_AppData.HkTlm);
}

/*=====================================================================================
** Name: ARDrone2_SendOutData
**
** Purpose: To Send message out to SB
**
** Arguments:
**
** Returns:
**
** Routines Called:
**
** Called By:
**    ARDrone2_ProcessNewCmds
**
** Global Inputs/Reads:
**
** Global Outputs/Writes:
**
** Limitations, Assumptions, External Events, and Notes:
**
** Algorithm:
**
** Author(s):  Guy de Carufel
**
** History:  Date Written  2012-10-01
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
int32  ARDrone2_SendOutData(void)
{
	int32 iStatus = CFE_SUCCESS;

    iStatus = OS_MutSemTake(g_ARDrone2_AppData.NavDataMutexId);
    /* Send if not stale */
    if (g_ARDrone2_AppData.NavDataStale == FALSE)
    {
    	/* Send out the TO Nav Data Select */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t*)&g_ARDrone2_AppData.NavDataSelectMsg);

        CFE_SB_SetMsgId((CFE_SB_Msg_t*)&g_ARDrone2_AppData.NavDataSelectMsg,
        		ARDRONE2_NAVDATA_SELECT_MID);

    	iStatus = CFE_SB_SendMsg((CFE_SB_Msg_t *) &g_ARDrone2_AppData.NavDataSelectMsg);

    	/* The software bus return code was bad */
    	if (iStatus != CFE_SUCCESS) {
    		/*
    		 ** Send an event message to report the invalid command status
    		 */
    		CFE_EVS_SendEvent(MSG_SEND_ERR_EID,
    				CFE_EVS_ERROR,
    				"ARDrone2: SendOutData SendMsg - Software Bus return was bad");
    	}
    }
    /* Set to stale until next data received */
    g_ARDrone2_AppData.NavDataStale = TRUE;
    iStatus = OS_MutSemGive(g_ARDrone2_AppData.NavDataMutexId);

	return iStatus;
}

/*=====================================================================================
** Name: ARDrone2_VerifyCmdLength
**
** Purpose: To verify command length for a particular command message
**
** Arguments:
**    CFE_SB_Msg_t*  MsgPtr      - command message pointer
**    uint16         usExpLength - expected command length
**
** Returns:
**    boolean bResult - result of verification
**
** Routines Called:
**    TBD
**
** Called By:
**    ARDrone2_ProcessNewCmds
**
** Global Inputs/Reads:
**    None
**
** Global Outputs/Writes:
**    TBD
**
** Limitations, Assumptions, External Events, and Notes:
**    1. List assumptions that are made that apply to this function.
**    2. List the external source(s) and event(s) that can cause this function to execute.
**    3. List known limitations that apply to this function.
**    4. If there are no assumptions, external events, or notes then enter NONE.
**       Do not omit the section.
**
** Algorithm:
**    Pseudo-code or description of basic algorithm
**
** Author(s):  Mathew Benson 
**
** History:  Date Written  2012-11-18
**           Unit Tested   yyyy-mm-dd
**=====================================================================================*/
boolean ARDrone2_VerifyCmdLength(CFE_SB_Msg_t* MsgPtr,
                           uint16 usExpectedLen)
{
    boolean bResult=FALSE;
    uint16  usMsgLen=0;

    if (MsgPtr != NULL)
    {
        usMsgLen = CFE_SB_GetTotalMsgLength(MsgPtr);

        if (usExpectedLen != usMsgLen)
        {
            CFE_SB_MsgId_t MsgId = CFE_SB_GetMsgId(MsgPtr);
            uint16 usCmdCode = CFE_SB_GetCmdCode(MsgPtr);

            CFE_EVS_SendEvent(MSGLEN_ERR_EID, CFE_EVS_ERROR,
                              "ARDrone2 - Rcvd invalid msgLen: msgId=0x%08X, cmdCode=%d, "
                              "msgLen=%d, expectedLen=%d",
                              MsgId, usCmdCode, usMsgLen, usExpectedLen);
            g_ARDrone2_AppData.HkTlm.usCmdErrCnt++;
        }
    }

    return (bResult);
}

/*=======================================================================================
** End of file ardrone2_app.c
**=====================================================================================*/
