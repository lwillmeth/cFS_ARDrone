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
**        Created by Michael Rosburg, mrosburg@odysseysr.com
**
******************************************************************************/

/*=======================================================================================
** File Name:  ardwd_msgids.h
**
** Title:  Message ID Header File for ARDWD Application
**
**
** Purpose:  This header file contains declarations and definitions of all ARDWD's
**           Message IDs.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2016-05-05 | Michael Rosburg | Build #: Code Started
**
**=====================================================================================*/

#ifndef _ARDWD_MSGIDS_H_
#define _ARDWD_MSGIDS_H_

/***** TODO:  These Message ID values are default and may need to be changed by the developer  *****/

/**********************************
** ardwd Command Message IDs
***********************************/

#define ARDWD_CMD_MID                  	    0x1832
#define ARDWD_SEND_HK_MID              	    0x1833
#define ARDWD_WAKEUP_MID                    0x1834


/***************************
** ardwd Telemetry Message IDs
****************************/

#define ARDWD_HK_TLM_MID                    0x0833


#endif /* _ARDWD_MSGIDS_H_ */

/*=======================================================================================
** End of file ardwd_msgids.h
**=====================================================================================*/
    
