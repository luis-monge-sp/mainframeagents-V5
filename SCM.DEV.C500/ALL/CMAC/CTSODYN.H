/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSCDYN in CTS.CMAC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
 /**************************************************************
 *                                                             *
 * Title            : Dynamic moudle loading functions         *
 *                                                             *
 * File Name        : ctscdyn.h                                *
 *                                                             *
 * Author           : Doron Cohen                              *
 *                                                             *
 * Creation Date    : 02/03/94                                 *
 *                                                             *
 * Description      :                                          *
 *                                                             *
 * Assumptions and                                             *
 *   Considerations :                                          *
 *                                                             *
 **************************************************************/

 /**************************************************************
 * Mod.Id   Who      When     Description                      *
 * -------- -------- -------- -------------------------------- *
 * SAS2IBMN NuritY   09/08/16 SAS/C to IBM C Conversion:       *
 *                            replace loadm, buildm, unloadm,  *
 *                            OS_DYNAM_Load and OS_DYNAM_Delete*
 *                            with calls to ctsaldm and ctsadlm*
 * sasc     NuritY   24/08/16 Adapt to CC changes.             *
 * SAS2IBMT SeligT   30/10/16 SAS/C to IBM C Conversion Project*
 *                                                             *
 **************************************************************/
 #ifndef OS_DYNM_H_included
 #define OS_DYNM_H_included

 /* sasc - not needed anymore because it is defined in GLOBS
 #ifdef EXT
 #undef EXT
 #endif
 #ifndef OS_DYNM_owner
 #define EXT extern
 #else
 #define EXT
 #endif
 */

 #define OS_DYNAM_call                  ctsaldc

 /* SAS2IBMN #define OS_DYNAM_load                cdyn010  */
 /* SAS2IBMN #define OS_DYNAM_delete              cdyn011  */

 /* SAS2IBMN typedef ESA_RC (*FP)();                       */

 /* SAS2IBMN - start */
 /**************************************************************
 *                                                             *
 * PROCEDURE NAME : ctsaldm (assembler)                        *
 *                                                             *
 * DESCRIPTION    : Dynamically load a load module.            *
 *                                                             *
 * INPUT          : See ctsaldm.                               *
 *                                                             *
 * OUTPUT         : See ctsaldm.                               *
 *                                                             *
 * RETURN VALUE   : Return code indicating is load             *
 *                                                             *
 **************************************************************/
 EXT    int ctsaldm();

 /**************************************************************
 *                                                             *
 * PROCEDURE NAME : ctsadlm (assembler)                        *
 *                                                             *
 * DESCRIPTION    : Dynamically delete a loaded module.        *
 *                                                             *
 * INPUT          : See ctsaldm.                               *
 *                                                             *
 * OUTPUT         : See ctsaldm.                               *
 *                                                             *
 * RETURN VALUE   : Return code indicating is load             *
 *                                                             *
 **************************************************************/
 EXT    int ctsadlm();
 /* SAS2IBMN - end */


 /**************************************************************
 *                                                             *
 * PROCEDURE NAME : OS_DYNM_call                               *
 *                                                             *
 * DESCRIPTION    : Dynamically load and execute a module      *
 *                  via link macro                             *
 *                                                             *
 * INPUT          : 1. module name                             *
 *                  2. parameters flag                         *
 *                  3. called module parameters                *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : Return code of called function             *
 *                                                             *
 **************************************************************/

 /* SAS2IBMT prototype changed for IBM C
 EXT int OS_DYNAM_call(char *mod, int more, ... );                   */
 EXT int OS_DYNAM_call();                                 /* SAS2IBMT */

 /* SAS2IBMN
  **************************************************************
 *                                                             *
 * PROCEDURE NAME : OS_DYNM_load                               *
 *                                                             *
 * DESCRIPTION    : Dynamically load a module                  *
 *                                                             *
 * INPUT          : 1. module name                             *
 *                  2. ddname containing the module            *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : function pointer to the loaded module      *
 *                  or NULL                                    *
 *                                                             *
 **************************************************************

 EXT FP OS_DYNAM_load(char *p_mod, char *p_lib);

  **************************************************************
 *                                                             *
 * PROCEDURE NAME : OS_DYNM_delete                             *
 *                                                             *
 * DESCRIPTION    : delete a dynamically loaded module         *
 *                                                             *
 * INPUT          : 1. module name                             *
 *                  2. pointer to function pointer             *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : function pointer to the loaded module      *
 *                  is set to null and the module is deleted   *
 *                  from storage                               *
 *                                                             *
 **************************************************************

 EXT ESA_RC OS_DYNAM_delete(char *p_mod, FP *p_fp);
 */

 #endif  /* OS_DYNM_H_included */
