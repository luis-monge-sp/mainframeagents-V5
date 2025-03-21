/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSBVRS in CTS.CSRC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
/**************************************************************
*                                                             *
* Title            : ACF2 Version programs                    *
*                                                             *
* File Name        : ctsbvrs.c                                *
*                                                             *
* Author           : Guy Shavitt                              *
*                                                             *
* Creation Date    : 22/10/98                                 *
*                                                             *
* Description      :                                          *
*                                                             *
* Assumptions and                                             *
*   Considerations :                                          *
*                                                             *
**************************************************************/

/**************************************************************
* Mod.Id   Who      When     Description                      *
* -------- -------- -------  -------------------------------- *
* ws2404   rami     30/7/01  retrieved debug_level for CTS2VER*
* IS10119  AvnerL   18/3/14  Support ACCREL# as new source.   *
* SAS2IBMT SeligT   1/11/16  SAS/C to IBM C Conversion Project*
* SAS2IBMA AvnerL   07/12/16 Set CC includes per IS0060.      *
* IS10174  NuritY   25/12/17 Admins list support.             *
* WS10075  AvnerL   16/12/19 Fake from ALLCSRC due to ALL.CMAC*
**************************************************************/

/*  IS10174 - remove the code as the routine below is not
              required any more.

              We leave this program as empty in order to save
              the need to change many linkcards. On next version,
              CTSOVRS and CTS2VER should ne removed from all
              LINKCARDs.

              Whoever needs this routine should call CTS2VRS
              directly (dynamically, or statically).

#include   <globs.h>

 *
 *   Standard include files
 *

#include   STDIO
#include   STDLIB
#include   STRING
#include   CTYPE
#include   FCNTL

 *
 *   CONTROL-SA include files
 *

#include ESA_API
#include ESA_DIAG
#include ESA_CTSAMSG
#include ESA_API_CODES

 * IS0060 - Do not use USA-API H files
#include MVS_COMP

#include API_ADDINFO
#include API_DATES

#include ACF2_CODES
#include ACF2
*

 * IS0060 begin: Use CC H files *
#include MVS_C_COMP

#include API_C_ADDINFO

#include ACF2_C_CODES
#include ACF2_C
 * IS0060  - end   *
*/

static char comp[]="CTSBVRS";

/**************************************************************
*                                                             *
* PROCEDURE NAME : ACF2_version_get                           *
*                                                             *
* DESCRIPTION    : Retrieve ACF2 version                      *
*                                                             *
* INPUT          : none                                       *
*                                                             *
* OUTPUT         : version                                    *
*                                                             *
* RETURN VALUE   : ESA_OK,  ESA_FATAL                         *
*                                                             *
**************************************************************/

/* IS10174 C ACF2_version_get(char       *version,
                        ADMIN_PARAMS_rec_typ *admin_params)   *IS10119*
*/
static void ACF2_version_get(void)                         /* IS10174 */
{

 /* IS10174 - start */
  static char func[]="this is an obsolete source module";
 /* IS10174 - end   */

 /* IS10174- remove the whole routine
  *
  *   Variables
  *

  ESA_RC      rc = ESA_OK;
  int         arc ;
  CTSAMSG_HANDLE_rec_typ     * msgs;                      * SAS2IBMT *
  CTSAMSG_DEST_TABLE_rec_typ * dest;                      * SAS2IBMT *

  *
  *   Assembler module. Get ACF2 version
  *

   * SAS2IBMT prototype changed for IBM C
  extern ESA_RC cts2ver(int         *debug_level,
                        char        *acf2_real_nm_version);          *
  extern ESA_RC cts2ver();                                 * SAS2IBMT *

  int    debug_level=0;             * debug level       *

  char   acf2_real_nm_version[5];    * real version name *
  static         char func[]="ACF2_version_get";           *IS10119*

  *
  *   Obtain ACF2 version
  *
  admin_params->cs_func.DIAG_enter_ptr(                    *IS10119*
     ESA_COMP_APIINIT, 1, func );
  debug_level = (int)ESA_DIAG_get_debug_level(ESA_COMP_USAAPI);

  msgs = admin_params->ctsamsg_handle;                    * SAS2IBMT *
  dest = admin_params->ctsamsg_dest;                      * SAS2IBMT *

   * SAS2IBMT
  arc=cts2ver(&debug_level,           // Input   debug level     *
  arc=(*(ASM_RTN_TYP *)&cts2ver)                           * SAS2IBMT *
             (&debug_level,            * Input   debug level     *
              acf2_real_nm_version);   * Outp    real vers name  *
  admin_params->cs_func.DIAG_printf_ptr(                  *IS10119*
     ESA_COMP_APIINIT,6,
     "ACF2 Version is <%s>", acf2_real_nm_version);

  if ( arc NE 0 ) {
      * IS10119 rc = ESA_FATAL; *
     rc = ESA_OK;                                             *IS10119*
      admin_params->cs_func.DIAG_printf_ptr(                  *IS10119*
        ESA_COMP_APIINIT,0,
        "Version %s identified for rss_type %s. Operation continues.",
        acf2_real_nm_version, admin_params->rss_type);
       * SAS2IBMT
      admin_params->cs_func.MSG_printf_ptr(               //IS10119//
        ACF2_NEW_VER,acf2_real_nm_version);                          *
      * IS10119 goto exit; *
      CTSAMSG_print(ACF2_NEW_VER, msgs, NULL, dest,
                    acf2_real_nm_version);                * SAS2IBMT *
  }

  acf2_real_nm_version[4]=NULL_CHAR;   * real version name *

  strcpy (version, acf2_real_nm_version);

  exit:;

  admin_params->cs_func.DIAG_exit_ptr(                     *IS10119*
      ESA_COMP_APIINIT, 1, func,rc);
  return rc;

*/

}

