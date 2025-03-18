/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSCCMP in CTS.CMAC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
/****************************************************
 * Title            : Diag Components definition for
 *                    COntrol-SA/MVS
 * File Name        : ctcccmp.h
 * Author           : Doron
 * Creation Date    : 02-Aug-94
 * Description      :
 * Assumptions and
 *   Considerations :
 ******************************************************/

/***************************************************************
 * Mod.ID   Who    When     Description                        *
 * ------------------------------------------------------------*
 * IS0358   YoniMa 17/08/06 test IS0358 in TSS (BS2698)        *
 * SAS2IBMN NuritY 09/08/16 SAS/C to IBM C Conversion:         *
 *                          OS_DYNAM_LOAD and OS_DYNAM_DELETE  *
 *                          are removed because fetch() and    *
 *                          release() are used instead.        *
 * WS10075  AvnerL 21/11/19 Support permissions in accounts.   *
 * WS10078  AvnerL 30/03/20 Support roles & sources.           *
 * WS10078  SeligT 19/05/20 Added PLT_COMP_ACF2_MEMORY (same   *
 *                          as PLT_COMP_TSS_MEMORY)            *
 **************************************************************/

#ifndef PLT_COMP_H_INCLUDED
#define PLT_COMP_H_INCLUDED

/****************************************************
 * API components
 ****************************************************/

#define PLT_COMP_API_ADDINFO  ESA_FIRST_PLATFORM_COMP+1
#define PLT_COMP_API_RACF     ESA_FIRST_PLATFORM_COMP+2
#define PLT_COMP_API_UT100    ESA_FIRST_PLATFORM_COMP+3
#define PLT_COMP_API_RCFRES   ESA_FIRST_PLATFORM_COMP+4


/****************************************************
 * OS components
 ****************************************************/

#define PLT_COMP_OS_CONSOLE   ESA_FIRST_PLATFORM_COMP+10
#define PLT_COMP_OS_CLI       ESA_FIRST_PLATFORM_COMP+11
/* SAS2IBMN #define PLT_COMP_OS_DYNAM   ESA_FIRST_PLATFORM_COMP+12 */
#define PLT_COMP_OS_MBX       ESA_FIRST_PLATFORM_COMP+13
#define PLT_COMP_OS_PROC      ESA_FIRST_PLATFORM_COMP+14
#define PLT_COMP_OS_MVS       ESA_FIRST_PLATFORM_COMP+15
#define PLT_COMP_CTSC100      ESA_FIRST_PLATFORM_COMP+16

/****************************************************
 * Extractor components
 ****************************************************/

#define PLT_COMP_RCFUSR_EXT   ESA_FIRST_PLATFORM_COMP+17
#define PLT_COMP_RCFGRP_EXT   ESA_FIRST_PLATFORM_COMP+18
#define PLT_COMP_RCFCNN_EXT   ESA_FIRST_PLATFORM_COMP+19

/****************************************************
 * TSS API components
 ****************************************************/

#define PLT_COMP_TSS_MEMORY   ESA_FIRST_PLATFORM_COMP+22 /*150-IS0358*/
#define PLT_COMP_ACF2_MEMORY  ESA_FIRST_PLATFORM_COMP+22 /*150-WS10078*/

#define PLT_COMP_PERMISSION   ESA_FIRST_PLATFORM_COMP+23 /*151-WS10075*/

/****************************************************
 * ACF2 API components
 ****************************************************/

#define PLT_COMP_API_ACF2     ESA_FIRST_PLATFORM_COMP+30
#define PLT_COMP_CTSSHOWV     ESA_FIRST_PLATFORM_COMP+31
/*#define ESA_COMP_BATCH ESA_FIRST_PLATFORM_COMP+32 *160-WS10078*/
#define PLT_COMP_XREF         ESA_FIRST_PLATFORM_COMP+33 /*161-WS10078*/

#endif /* PLT_COMP_H_INCLUDED */

