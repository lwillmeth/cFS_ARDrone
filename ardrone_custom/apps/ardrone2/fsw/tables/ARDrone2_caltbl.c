/*=======================================================================================
** File Name:  ardrone2_caltbl.c
**
** Title:  ARDrone2 Driver sensor calibration coefficients
**
** Purpose:  This source file defines calibration coefficients for the AR Drone 2.0
** sensors.  This includes the accelerometers, gyros, and altimeter.
**=====================================================================================*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "tbldefs.h"


/*
** Local Defines
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

/* Table file header */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuninitialized"
static CFE_TBL_FileDef_t CFE_TBL_FileDef =
{
    "ARDrone2_DefaultCalTable", "ARDRONE2.CALS", "ARDRONE2 cal table",
    "ARDrone2_caltbl.tbl", sizeof(CalTable_t)
};
#pragma GCC diagnostic push

/* Default schedule table data */
CalTable_t ARDrone2_DefaultCalTable =
{
	{ 2048, 2048, 2048 }, /* Accelerometer bias (X, Y, Z) */
	{ 1024, 1024, 1024 }, /* Accelerometer gain (X, Y, Z) */
	{ 1350/0.806, 1350/0.806, 1350/0.806 }, /* Gyro bias (X, Y, Z) */
	{ 6.9786031e-03, 7.1979444e-03, 3.8175473e-03 }, /* Gyro gain (X, Y, Z) */
	{ 1350/0.806, 1350/0.806 }, /* Gyro 110 bias (X, Y) */
	{ 1.5517747e-03, 1.5981209e-03 }, /* Gyro 110 gain (X, Y) */
	-30.0,   /* Acceleromter temp bias */
	0.5, /* Accelerometer temp gain (C/lsb) */
	1250, /* Gyro temp (mV at room temp) */
	0.806, /* Gyro temp ( mv/lsb ) */
	4.0, /* Gyro temp ( mV/°C ) */
	20.0, /* Gyro temp ( room temperature in °C ) */
	0.0340	/* Height (acoustic altimeter) gain */
};

/*
** Local Variables
*/

/*
** Function Prototypes
*/

/*
** Function Definitions
*/

/*=======================================================================================
** End of file ardrone2_caltbl.c
**=====================================================================================*/
    
