/************************************************************************
**
**
**  Copyright 2016 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Johnson Space Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
**  elf2cfetbl test: an empty SC RTS table
**
**  Author: Allen Brown, Odyssey Space Research, LLC
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */


/*
** Sample cFE Table Header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "RTS_Table001", "SC.RTS_TBL001", "Unit Test Table A",
    "ut_rtsA.tbl", (SC_RTS_BUFF_SIZE * sizeof(uint16))
};


/*
** Sample RTS Table Data
*/
uint16 RTS_Table001[SC_RTS_BUFF_SIZE] =
{
};

/************************/
/*  End of File Comment */
/************************/
