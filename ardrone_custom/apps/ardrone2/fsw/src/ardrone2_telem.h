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

/*==============================================================================
** File Name:  ardrone2_telem.h
**
** Title:  Telemetry Header File for ARDrone2 Application
**
**
** Purpose:  This header file contains declarations and definitions of all
**           ARDrone2's telemetry.
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2013-09-15 | Mathew Benson | Code Started
**
**===========================================================================*/
#ifndef _ARDRONE2_TELEM_H_
#define _ARDRONE2_TELEM_H_


void ARDrone2_TlmMain(void);
void ARDrone2_ResetTlmConnection(void);

uint32 ARDrone2_TlmUpdates;

#endif /* ARDRONE2_TELEM_H_ */
