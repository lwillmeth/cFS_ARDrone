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
** File Name:  ardrone2_navdata.c
**
** Title:  Function Definitions ARDRONE2 NAVDATA
**
** Purpose:  Pack and Unpack ARDRONE NAVDATA
**
** Functions Defined:
** ARDrone2_NavdataPackOption()
** ARDrone2_NavdataUnpackOption()
** ARDrone2_NavDataComputeCks()
** ARDrone2_NavDataSearchOption()
** ARDrone2_GetMaskFromState()
** ARDrone2_ResetUpdateCounters()
**
** Limitations, Assumptions, External Events, and Notes:
**
** Modification History:
**   Date | Author | Description
**   ---------------------------
**   2013-09-15 | Mathew Benson | Code Started
**
**=====================================================================================*/

/*
** Pragmas
*/

/*
** Include Files
*/
#include <string.h>
#include "ardrone2_app.h"
#include "ardrone2_navdata.h"

/*
** Local Defines
*/


/*
** Local Structure Declarations
*/

/*
** External Global Variables
*/
extern AppData_t g_ARDrone2_AppData;

/*
** Global Variables
*/

/*
** Local Variables
*/

/*
** Local Function Definitions
*/


uint8* ARDrone2_NavdataPackOption( uint8* navdata_ptr, uint8* data, uint32 size )
{
	uint32 copy_size = size - sizeof(NavDataHeader_t);
	uint8* dest_ptr = navdata_ptr + sizeof(NavDataHeader_t);
	uint8* src_ptr = data + sizeof(uint16) + sizeof(uint16);
	memcpy(dest_ptr, src_ptr, copy_size);
	// TODO:  Need to finish this.
	exit(-1);
	return (navdata_ptr + size);
}


NavData_Option_t * ARDrone2_NavdataUnpackOption(NavData_Option_t* navdata_ptr, 
                                                NavDataHeader_t* unpack_ptr, 
                                                uint32 unpack_opt_size)
{
    unpack_ptr->update_count++;
    uint32 srcSize = navdata_ptr->size - 4;
    uint32 destSize = unpack_opt_size - sizeof(NavDataHeader_t);

    uint32 minSize = (srcSize < destSize ? srcSize : destSize);

    char * destPtr = (char *)unpack_ptr + sizeof(NavDataHeader_t);
    char * srcPtr = (char *)navdata_ptr + 4;

    memcpy(destPtr, srcPtr, minSize);
    
    char * nextOptPtr = (char *)navdata_ptr + navdata_ptr->size;
    return ((NavData_Option_t *) nextOptPtr);
}

uint32 ARDrone2_NavDataComputeCks( int8* nv, int32 size )
{
	int32 i;
	uint32 cks;
	uint32 temp;

	cks = 0;

	for( i = 0; i < size; i++ )
	{
		temp = nv[i];                                              
		cks += temp;
	}

	return cks;
};

NavData_Option_t* ARDrone2_NavDataSearchOption( NavData_Option_t* navdata_options_ptr, NavData_Tag_t tag )
{
  uint8* ptr;

  while( navdata_options_ptr->tag != tag )
  {
    ptr  = (uint8*) navdata_options_ptr;
    ptr += navdata_options_ptr->size;

    navdata_options_ptr = (NavData_Option_t*) ptr;
  }

  return navdata_options_ptr;
}


boolean ARDrone2_NavDataUnpackAll(NavData_Unpacked_t* navdata_unpacked, NavData_t* navdata, uint32* cks)
{
	boolean success = TRUE;
	NavData_Cks_t navdata_cks;
	navdata_cks.cks = 0;

	NavData_Option_t* navdata_option_ptr;

	navdata_option_ptr = (NavData_Option_t*) &navdata->options[0];

	navdata_unpacked->nd_seq   			= navdata->sequence;
	navdata_unpacked->ardrone_state   	= navdata->ardrone_state;
	navdata_unpacked->vision_defined  	= navdata->vision_defined;

	while( navdata_option_ptr != 0 )
	{
		// Check if we have a valid option
		if( navdata_option_ptr->size == 0 )
		{
			/* TODO:  Replace this with an error event. */
			//PRINT("One option (%d) is not a valid option because its size is zero\n", navdata_option_ptr->tag);
			navdata_option_ptr = 0;
			success = FALSE;
		}
		else
		{
			if( navdata_option_ptr->tag <= ARDRONE2_NAVDATA_NUM_TAGS)
			{
			   #ifdef DEBUG_NAVDATA_C
    	  	 	OS_printf("[%d]",navdata_option_ptr->tag);
			   #endif
    	  	 	navdata_unpacked->last_navdata_refresh |= 1 << (navdata_option_ptr->tag);
			}

			switch( navdata_option_ptr->tag )
			{
				case ARDRONE2_NAVDATA_DEMO_TAG:
				{
					uint32 dstSize = sizeof(g_ARDrone2_AppData.NavDataDemoMsg.payload);
					uint32 srcSize = navdata_option_ptr->size;
					uint32 minSize = (dstSize < srcSize ? dstSize : srcSize);
                    
					memcpy(&g_ARDrone2_AppData.NavDataDemoMsg.payload, &navdata_option_ptr->data[0], minSize);
					
                    /*For some unknown reason, the message received is 500bytes 
                      instead of 292, so lets just jump to 500... */
                    //char *addr = (char *) navdata;
                    //addr +=500;
                    //navdata_option_ptr = (NavData_Option_t *) addr;
                    
                    char *addr = (char *)navdata_option_ptr;
                    navdata_option_ptr = (NavData_Option_t *)(addr + srcSize);

                    //addr += 500;
                    //navdata_option_ptr = navdata_option_ptr + srcSize;
					break;
				}

				case ARDRONE2_NAVDATA_TIME_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->time ,
                    sizeof(navdata_unpacked->time) );
					break;
				}

				case ARDRONE2_NAVDATA_RAW_MEASURES_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->raw_measures ,
                    sizeof(navdata_unpacked->raw_measures) );
					break;
				}

				case ARDRONE2_NAVDATA_PHYS_MEASURES_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->phys_measures ,
                    sizeof(navdata_unpacked->phys_measures) );
					break;
				}

				case ARDRONE2_NAVDATA_GYROS_OFFSETS_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->gyros_offsets ,
                    sizeof(navdata_unpacked->gyros_offsets) );
					break;
				}

				case ARDRONE2_NAVDATA_EULER_ANGLES_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->euler_angles ,
                    sizeof(navdata_unpacked->euler_angles) );
					break;
				}

				case ARDRONE2_NAVDATA_REFERENCES_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->references ,
                    sizeof(navdata_unpacked->references) );
					break;
				}

				case ARDRONE2_NAVDATA_TRIMS_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->trims,
                    sizeof(navdata_unpacked->trims));
                    break;
				}

				case ARDRONE2_NAVDATA_RC_REFERENCES_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->rc_references ,
                    sizeof(navdata_unpacked->rc_references) );
					break;
				}

				case ARDRONE2_NAVDATA_PWM_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->pwm ,
                    sizeof(navdata_unpacked->pwm) );
					break;
				}

				case ARDRONE2_NAVDATA_ALTITUDE_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->altitude ,
                    sizeof(navdata_unpacked->altitude) );
					break;
				}

				case ARDRONE2_NAVDATA_VISION_RAW_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->vision_raw ,
                    sizeof(navdata_unpacked->vision_raw) );
					break;
				}

				case ARDRONE2_NAVDATA_VISION_OF_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->vision_of ,
                    sizeof(navdata_unpacked->vision_of) );
					break;
				}

				case ARDRONE2_NAVDATA_VISION_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->vision ,
                    sizeof(navdata_unpacked->vision) );
					break;
				}

				case ARDRONE2_NAVDATA_VISION_PERF_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->vision_perf ,
                    sizeof(navdata_unpacked->vision_perf) );
					break;
				}

				case ARDRONE2_NAVDATA_TRACKERS_SEND_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->trackers_send ,
                    sizeof(navdata_unpacked->trackers_send) );
					break;
				}

				case ARDRONE2_NAVDATA_VISION_DETECT_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->vision_detect ,
                    sizeof(navdata_unpacked->vision_detect) );
					break;
				}

				case ARDRONE2_NAVDATA_WATCHDOG_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->watchdog ,
                    sizeof(navdata_unpacked->watchdog) );
					break;
				}

				case ARDRONE2_NAVDATA_ADC_DATA_FRAME_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->adc_data_frame ,
                    sizeof(navdata_unpacked->adc_data_frame) );
					break;
				}

				case ARDRONE2_NAVDATA_VIDEO_STREAM_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->video_stream ,
                    sizeof(navdata_unpacked->video_stream) );
					break;
				}

				case ARDRONE2_NAVDATA_GAMES_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->games ,
                    sizeof(navdata_unpacked->games) );
					break;
				}

				case ARDRONE2_NAVDATA_PRESSURE_RAW_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->pressure_raw ,
                    sizeof(navdata_unpacked->pressure_raw) );
					break;
				}

				case ARDRONE2_NAVDATA_MAGNETO_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->magneto ,
                    sizeof(navdata_unpacked->magneto) );
					break;
				}

				case ARDRONE2_NAVDATA_WIND_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->wind_speed ,
                    sizeof(navdata_unpacked->wind_speed) );
					break;
				}

				case ARDRONE2_NAVDATA_KALMAN_PRESSURE_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->kalman_pressure ,
                    sizeof(navdata_unpacked->kalman_pressure) );
					break;
				}

				case ARDRONE2_NAVDATA_HDVIDEO_STREAM_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->hdvideo_stream ,
                    sizeof(navdata_unpacked->hdvideo_stream) );
					break;
				}

				case ARDRONE2_NAVDATA_WIFI_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->wifi ,
                    sizeof(navdata_unpacked->wifi) );
					break;
				}

				case ARDRONE2_NAVDATA_ZIMMU_3000_TAG:
				{
					navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    (NavDataHeader_t *) &navdata_unpacked->zimmu_3000 ,
                    sizeof(navdata_unpacked->zimmu_3000) );
					break;
				}

				case ARDRONE2_NAVDATA_CKS_TAG:
				{
					//navdata_option_ptr = ARDrone2_NavdataUnpackOption( navdata_option_ptr,
                    //(NavDataHeader_t *) &navdata_unpacked->demo ,
                    //sizeof(navdata_unpacked->demo) );
					*cks = navdata_cks.cks;
					navdata_option_ptr = 0; // End of structure
					break;
				}

				default:
				{
					// TODO:  Replace the following with an error event.
					//PRINT("Tag %d is an unknown navdata option tag\n", (int) navdata_option_ptr->tag);
					navdata_option_ptr = (NavData_Option_t*)(((uint32)navdata_option_ptr) + navdata_option_ptr->size);
					break;
				}
			}
		}
	}

   #ifdef DEBUG_NAVDATA_C
	printf("\n");
   #endif

	return success;
}


boolean ARDrone2_GetMaskFromState( uint32 state, uint32 mask )
{
  return state & mask ? TRUE : FALSE;
}



void ARDrone2_ResetUpdateCounters(NavData_Unpacked_t* navdata_unpacked)
{
//	navdata_unpacked->demo.header.update_count = 0;
	navdata_unpacked->time.header.update_count = 0;
	navdata_unpacked->raw_measures.header.update_count = 0;
	navdata_unpacked->phys_measures.header.update_count = 0;
	navdata_unpacked->gyros_offsets.header.update_count = 0;
	navdata_unpacked->euler_angles.header.update_count = 0;
	navdata_unpacked->references.header.update_count = 0;
	navdata_unpacked->trims.header.update_count = 0;
	navdata_unpacked->rc_references.header.update_count = 0;
	navdata_unpacked->pwm.header.update_count = 0;
	navdata_unpacked->altitude.header.update_count = 0;
	navdata_unpacked->vision_raw.header.update_count = 0;
	navdata_unpacked->vision_of.header.update_count = 0;
	navdata_unpacked->vision.header.update_count = 0;
	navdata_unpacked->vision_perf.header.update_count = 0;
	navdata_unpacked->trackers_send.header.update_count = 0;
	navdata_unpacked->vision_detect.header.update_count = 0;
	navdata_unpacked->watchdog.header.update_count = 0;
	navdata_unpacked->adc_data_frame.header.update_count = 0;
	navdata_unpacked->video_stream.header.update_count = 0;
	navdata_unpacked->games.header.update_count = 0;
	navdata_unpacked->pressure_raw.header.update_count = 0;
	navdata_unpacked->magneto.header.update_count = 0;
	navdata_unpacked->wind_speed.header.update_count = 0;
	navdata_unpacked->kalman_pressure.header.update_count = 0;
	navdata_unpacked->hdvideo_stream.header.update_count = 0;
	navdata_unpacked->wifi.header.update_count = 0;
	navdata_unpacked->zimmu_3000.header.update_count = 0;
}

