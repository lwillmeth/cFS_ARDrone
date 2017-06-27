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
** File Name:  ardrone2_msgids.h
**
** Title:  Ardrone2 Header
**
**
** Purpose:  This header file contains declarations and definitions of all ardrone
** I/O Application Message IDs
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2012-11-18 | Guy de Carufel | Build #: Code Started
**
**=====================================================================================*/
#ifndef _ardrone2_msgids_h_
#define _ardrone2_msgids_h_

/**********************************
** ARDrone2 Command Message IDs
***********************************/
#define ARDRONE2_CMD_MID                    0x1820
#define ARDRONE2_SEND_HK_MID                0x1821
#define ARDRONE2_READ_INPUTS_MID            0x1822
#define ARDRONE2_WRITE_OUTPUTS_MID          0x1823
#define ARDRONE2_PLAT_CMD_MID               0x1824
#define ROTWING_GENERIC_CMD_MID             0x1825
#define ARDRONE2_SEND_ATCMDS_MID            0x1826 // FIXME: unused? remove
#define ARDRONE2_WAKEUP_MID                 0x1827

/***************************
** ARDRONE2 Telemetry Message IDs
****************************/
#define ARDRONE2_HK_TLM_MID                 0x0820
#define ARDRONE2_NAVDATA_DEMO_MID           0x0821  
#define ARDRONE2_TO_NAVDATA_DEMO_MID	    0x0822  
#define ARDRONE2_TO_NAVDATA_MID	            0x0823  
#define ARDRONE2_NAVDATA_SELECT_MID         0x0824

#endif /* _ardrone2_msgids_h_ */

/************************/
/*  End of File Comment */
/************************/
