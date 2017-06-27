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
 **  Author: Allen Brown, Odyssey Space Research, LLC
 **
 **
 ** Description:
 ** This file provides macro definitions helpful for creating compile-time
 ** CCSDS message header components suitable for struct initialization.
 **
 ** Various components in cFS use differnt struct member layouts to 
 ** encode the primary and secondary CCSDS message headers.  Note that
 ** the CCSDS secondary headers have platform-dependent endianness.
 **
 ** Dependencies: 
 ** SOFTWARE_BIG_BIT_ORDER or SOFTWARE_BIG_BIT_ORDER must be defined.
 **
 *************************************************************************/

#ifndef CT_CCSDS_H
#define CT_CCSDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ccsds.h"

/*****************************
 * Helpful macros for command definition
 *****************************/

/*
 * Command packet segment flags and sequence counter
 * - 2 bits of segment flags (0xC000 = start and end of packet)
 * - 14 bits of sequence count (unused for command packets)
 * (big endian)
 */
#define CMD_FLAGS_BE     CFE_MAKE_BIG16(0xC000)

/* Macros for conversion and placement of a uint16 into
   uint8[2] where the uint16 is platform-dependent. */
#ifdef SOFTWARE_BIG_BIT_ORDER
 #define INPLACE_BE_HI(value)   (((value) >> 8))
 #define INPLACE_BE_LO(value)   (((value) & 0xff))
#elif defined SOFTWARE_LITTLE_BIT_ORDER
 #define INPLACE_BE_LO(value)   (((value) >> 8))
 #define INPLACE_BE_HI(value)   (((value) & 0xff))
#else
 #error "SOFTWARE_?_BIT_ORDER must be defined!"
#endif /* SOFTWARE_BIG_BIT_ORDER */

/* Properly place a native uint8 command code into the correct position
   when defining the CCSDS command secondary header as uint8[2]. 
   The checksum is set to 0x00. */
#ifdef SOFTWARE_BIG_BIT_ORDER
 #define CC_NATIVE_1ST(code)         ((code) & 0x7f)
 #define CC_NATIVE_2ND(code)         ((code) & 0x00)
#else
 #define CC_NATIVE_1ST(code)         ((code) & 0x00)
 #define CC_NATIVE_2ND(code)         ((code) & 0x7f)
#endif /* SOFTWARE_BIG_BIT_ORDER */


/*****************************
 * (End of macros)
 *****************************/

#ifdef __cplusplus
}
#endif

#endif /* CT_CCSDS_H */

/************************/
/*  End of File Comment */
/************************/
