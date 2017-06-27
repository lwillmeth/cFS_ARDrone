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
** File Name:  ardrone2_platform_cfg.h
**
** Title:  Platform Configuration Header File for ARDrone2 Application
**
**
** Purpose:  This header file contains declarations and definitions of all ardrone2's
**           platform-specific configurations.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2012-11-18 | Mathew Benson | Build #: Code Started
**
**=====================================================================================*/
    
#ifndef _ARDRONE2_PLATFORM_CFG_H_
#define _ARDRONE2_PLATFORM_CFG_H_

/*
** Pragmas
*/

/*
** Local Defines
*/
#define ARDRONE2_CAL_TABLE_FILENAME  					"/cf/apps/ARDrone2_caltbl.tbl"

#define ARDRONE2_USE_PARROT_CONTROLLER	0
#define ARDRONE2_DEBUG_NAVDATA 1
#define ARDRONE2_NAVDATA_MAX_CUSTOM_TIME_SAVE 	20
#define ARDRONE2_NB_NAVDATA_DETECTION_RESULTS 	4

#define ARDRONE2_NAVDATA_SEQUENCE_DEFAULT  		1
#define ARDRONE2_CMD_PORT                 		5554
#define ARDRONE2_NAV_DATA_PORT            		5554
#define ARDRONE2_MAX_NAV_BUFFER_SIZE			1024
#define ARDRONE2_MAX_AT_BUFFER_SIZE				1024

#define ARDRONE2_AT_PORT   					    5556
#define ARDRONE2_WATCHDOG_PERIOD				1000000

//#define ARDrone2_NAVDATA_DEBUG

/*
** Include Files
*/
#include "ardrone2_platform_ip_cfg.h"

/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Prototypes
*/

#endif /* _ARDRONE2_PLATFORM_CFG_H_ */

/*=======================================================================================
** End of file ardrone2_platform_cfg.h
**=====================================================================================*/
