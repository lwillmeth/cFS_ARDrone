/************************************************************************
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
**  elf2cfetbl test: not enough room for ATS sentinel word
**
**  Author: Allen Brown, Odyssey Space Research, LLC
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"

#include "sc_platform_cfg.h"    /* defines table buffer size */
#include "sc_tbldefs.h"

/*****************************
 * Include files (for the app commands)
 *****************************/

/* For SC APP */
#include "sc_msgdefs.h"         /* defines SC command code values */
#include "sc_msgids.h"          /* defines SC packet msg ID's */
#include "sc_msg.h"             /* defines SC command structs */

/* Required items for SC table creation */
#include "ct_sc_table.h"

/* Helpful macros for command definition */
#include "ct_ccsds.h"

/*****************************
 * (end include files)
 *****************************/

/*****************************
 * Table ID and Type Information
 *****************************/
/*
 * Sample ATS_TBL1 Table Header
 * (Required)
 */
static CFE_TBL_FileDef_t CFE_TBL_FileDef RETAIN_SYMBOL = {
		"ATS_Table1", "SC.ATS_TBL1", "SC Sample ATS_TBL1",
		"ut_ats_2big.tbl", (SC_ATS_BUFF_SIZE * sizeof(uint16)) };

/*
 * Sample ATS Table Data
 * (Required)
 * (Note, no data will be used from this table, it will be assembled
 *  with information below.)
 */
uint16 ATS_Table1[SC_ATS_BUFF_SIZE] = { };

/* The type of table (used by the post-processor).
 * This is used since the naming convention is under the control of sc_platform_cfg.h.
 * 0=ATS, 1=RTS */
static uint8 SCTableType RETAIN_SYMBOL = 0; /* ATS */

/*****************************
 * (End table ID and type info)
 *****************************/

/*****************************
 * Begin command definitions
 *****************************/


/* Lets make fill it up with the largest message we can: 
  Typically SC_PACKET_MAX_SIZE is 250 (sc_platform_cfg.h) 
        and SC_ATS_BUFF_SIZE is 8000 16-bit words = 16000 bytes
  so to fill up the table is 64 messages 
  Also SC_MAX_ATS_CMDS is 1000 */
#define LARGEST_SIZE_BE     CFE_MAKE_BIG16(250U-7U) /* size */

typedef struct myLargestMsg_t {
	CFE_SB_CmdHdr_t cmdHdr; /* 8 bytes */
	uint8           payload[(250U-8U)];
} myLargestMsg_t;

static myLargestMsg_t bigmsg RETAIN_SYMBOL = { { {
/* primary message header */
{ INPLACE_BE_HI(CFE_MAKE_BIG16(SC_CMD_MID)),
  INPLACE_BE_LO(CFE_MAKE_BIG16(SC_CMD_MID)) }, /* MID */
{ INPLACE_BE_HI(CMD_FLAGS_BE), INPLACE_BE_LO(CMD_FLAGS_BE) }, /* Seq/Flags */
{ INPLACE_BE_HI(LARGEST_SIZE_BE), INPLACE_BE_LO(LARGEST_SIZE_BE) } /* Length */
},
/* secondary command header, native endian */
{ 0x0000 }
},
{} /* zeros for the payload */
};


typedef struct aSmallerMsg_t {
	CFE_SB_CmdHdr_t cmdHdr; /* 8 bytes */
	uint8           payload[(250U-8U-1U)]; /* one byte smaller */
} aSmallerMsg_t;

static myLargestMsg_t smallermsg RETAIN_SYMBOL = { { {
/* primary message header */
{ INPLACE_BE_HI(CFE_MAKE_BIG16(SC_CMD_MID)),
  INPLACE_BE_LO(CFE_MAKE_BIG16(SC_CMD_MID)) }, /* MID */
{ INPLACE_BE_HI(CMD_FLAGS_BE), INPLACE_BE_LO(CMD_FLAGS_BE) }, /* Seq/Flags */
{ INPLACE_BE_HI(LARGEST_SIZE_BE-1U), INPLACE_BE_LO(LARGEST_SIZE_BE-1U) } /* Length */
},
/* secondary command header, native endian */
{ 0x0000 }
},
{} /* zeros for the payload */
};


/*****************************
 * (End command definitions)
 *****************************/

/*****************************
 * SC Table Command Content
 *****************************/

/* Set the number of commands to insert in the table here: */
#define NUM_CMDS (64U)

/*
** Arbitrary spacecraft time for start of sample ATS
*/
#define TEST_TIME     1000000


/* The command order and relative timing to place into the table.
 * Fields:
 *       The execution time for each command (spacecraft time)
 *       The complete command message (symbol) name (must match above)
 *       The size of the entire message
 * NOTE: The symbol "SCCommandList" is the trigger for external processing of the
 *       table. */
static ATS_List_Entry SCCommandList[NUM_CMDS] RETAIN_SYMBOL =
    { { (TEST_TIME +  1),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  2),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  3),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  4),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  5),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  6),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  7),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  8),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME +  9),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 10),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 11),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 12),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 13),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 14),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 15),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 16),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 17),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 18),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 19),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 20),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 21),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 22),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 23),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 24),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 25),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 26),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 27),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 28),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 29),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 30),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 31),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 32),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 33),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 34),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 35),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 36),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 37),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 38),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 39),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 40),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 41),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 42),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 43),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 44),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 45),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 46),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 47),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 48),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 49),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 50),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 51),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 52),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 53),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 54),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 55),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 56),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 57),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 58),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 59),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 60),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 61),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 62),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 63),  "bigmsg",     sizeof(bigmsg)     },
      { (TEST_TIME + 64),  "bigmsg",     sizeof(smallermsg)     } };

/* The number of commands in the above table (the post-processor needs this size). */
static const uint16 SCCommandList_NUM_CMDS RETAIN_SYMBOL = (NUM_CMDS);

/*****************************
 * (end SC Table Command Content)
 *****************************/
 
#ifdef __cplusplus
}
#endif

/************************/
/*  End of File Comment */
/************************/
