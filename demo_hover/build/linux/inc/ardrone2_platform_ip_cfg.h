/*=======================================================================================
** File Name:  ardrone2_platform_ip_cfg.h
**
** Title:  Platform-Specific Configuration Header File for PROXY Application IP Address
**
** $Author:    Allen Brown
** $Revision:  $
** $Date:      2015-05-01
**
** Purpose:  This header file contains declartions and definitions for PROXY's 
**           platform-specific IP address.
**
**
**=====================================================================================*/
    
#ifndef _ARDRONE2_PLATFORM_IP_CFG_H_
#define _ARDRONE2_PLATFORM_IP_CFG_H_

/*
** Pragmas
*/

/*
** Local Defines
*/
/* The ARDRONE2_WIFI_IP sets the IP where the ARDrone or Trick simulation
 * listens to this application.  Use the loopback, 127.0.0.1, when this
 * CFS app runs on the same platform.  When the CFS app runs on a separate
 * platform, use the IP of the AR Drone.
 * Use the loopback with the Trick simulation on Linux.  Use the AR Drone 2.0
 * IP address when this build talks to the actual drone across wifi. */
#define ARDRONE2_WIFI_IP 	                    "127.0.0.1"

/*
** Include Files
*/

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

#endif /* _ARDRONE2_PLATFORM_IP_CFG_H_ */

/*=======================================================================================
** End of file ardrone2_platform_ip_cfg.h
**=====================================================================================*/
    
