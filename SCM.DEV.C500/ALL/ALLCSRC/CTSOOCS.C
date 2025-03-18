 /**************************************************************
 *                                                             *
 *  <!><!> ATTENTION <!><!>                                    *
 *  =-=-=-=-=-=-=-=-=-=-=-=                                    *
 *                                                             *
 *  When updating this member, do not forget to update         *
 *  CTSCOCS in CTS.CSRC as well.                               *
 *                                                             *
 *                                                             *
 **************************************************************/
 /**************************************************************
 *                                                             *
 * Title            : OS functions for CS, CD                  *
 *                                                             *
 * File Name        : ctscocs.c                                *
 *                                                             *
 * Author           : Alexander Shvartsman                     *
 *                                                             *
 * Creation Date    : 20/07/94                                 *
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
 * PS0082   AS       10/03/97 Support external rss types table *
 * PS0107   AS       16/06/97 Support intercept set from script*
 * ps0173   AS       04/08/97 os_attach improvement            *
 * ws2305   AS       19/02/98 make cs,cd none-swappable        *
 * ps0246   dc       05/03/98 make cs,cd none-swappable        *
 * PS0311   Alex     16/09/98 List fields handling             *
 *                            (See BS2363 for V210)            *
 * ws2349   Alex     02/06/99 Operate with Started Task auth   *
 *                            for "GET" operation script       *
 * ws2355   YB       12/08/99 Allow for USAAPI modules in a    *
 *                            non-APF authorized library       *
 * bs2398   YB       14/09/99 fix bug in ws2355 (len of        *
 *                            load_ddnam/load_ddpad = 9)       *
 * bs2400   YB       21/09/99 fix bug in ws2355 (take rss name *
 *                            from parm not from admin params) *
 * bs2427   Alex     05/08/00 Operate with Started Task auth   *
 *                            for all TSS scripts              *
 * bs2428   Yoni     14/08/00 Fix rc=8 in TSS commands using   *
 *                            RSS adminstrator (cancel bs2427) *
 * ws2375   Michael  11/10/00 Execute user USAAPI modules in   *
 *                            non-APF authorized mode          *
 * WS0363   Alex     11/26/00 "SEND RSSPARM TO SCRIPT" SUPPORT *
 * bs2452   Michael  31/01/01 Fix bug in WS2375.               *
 *                            Support multi non-APF RSSs.      *
 * bs2504   Yoni     24/12/01 not all script lines are printed *
 * is0216   AvnerL   16/01/04 Drop is0225 with OS_localTime.   *
 * SAS2IBMT SeligT   12/07/16 SAS/C to IBM C Conversion Project*
 * SAS2IBMN NuritY   09/08/16 SAS/C to IBM C Conversion:       *
 *                            remove loadm calls to SAS/C      *
 *                            modules:                         *
 *                            LSCALMT, LSCIDDN, LSCRSTD,       *
 *                            LSCIDSN, LSCOKVS, LSCKIO         *
 * SAS2IBMA AvnerL   07/12/16 Set CC includes per IS0060.      *
 * CIQ#6    SeligT   10/05/17 Account Aggr includes Connections*
 * IS10174  NuritY   15/01/18 Dynamic EXECOUT support.         *
 * IS10184  NuritY   31/08/21 Improve scripts performance      *
 * WS10082  MauriC   24/11/22 drop CTSOADI and recompile       *
 **************************************************************/

 #include   <globs.h>

 /*
  *   Standard include files
  */

 #include   STDIO
 #include   STDLIB
 #include   STRING
 #include   TIME
 #include   ERRNO                                         /* SAS2IBMT */
 /* #include   LCLIB                                         SAS2IBMT */

 /*
  *   ESA include files
  */

 #include   ESA_API
 #include   ESA_INIT
 #include   ESA_CS_OS
 #include   ESA_DIAG
 #include   ESA_CTSAMSG
 #include   ESA_API_CODES
 /* #include   API_AUTH   IS0060 */
 #include   API_C_AUTH        /*  IS0060  */

 /*
  *   MVS include files
  */
 /* IS0060 do not use USA-API H files
 #include   MVS_OS_CLI
 #include   MVS_OS_MVS
 #include   MVS_OS_DYNAM
 #include   MVS_CODES
 */
 /*  IS0060  - Use CC / ALL H files */
 #include   MVS_OSC_CLI
 #include   MVS_OSC_MVS
 #include   MVS_OSC_DYNAM
 #include   MVS_C_CODES
 /* IS0060 - end*/

 /*
  *   RACF include files
  */
 /* do not use USA-API H files
 #include   RACF_CODES
 #include   API_ADDINFO
  * IS0060 - use CC / ALL H files */
 #include   RACF_C_CODES
 /* WS10082 #include   API_C_ADDINFO         CTSOADI is obsolete */

 /* IS0060 -end */

 /*
  *   Defaults
  */

 #define MAX_OS_ATTACH_TABLE   20

 /*
  *   PS0082
  */

 #define RSSTABLE_DD           "DD:RSSTABL"   /* added DD:   SAS2IBMT */
 #define MAX_RSS_TABLE         20
 #define USAAPI_LIB_NAME       "USAAPI_LIB_NAME"          /* ws2355 */
 #define ESA_COMP_NON_APF      150                        /* ws2375 */
 struct  _rss_t {
     char    rss_type[20];
     char    api_module[10];
 };

 /*
  *  ps0173.  Attach service structure
  */

 typedef struct s_os_attach {
      char               api_module[10]; /* api module      */
      int                attached ;      /* attached flag   */
      FUNC_PTR_rec_typ   func_ptr ;      /* ptrs to API     */
      RSS_LIMITS_rec_typ rss_limits;     /* rss limits      */
 } OS_ATTACH_typ, *OS_ATTACH_ptr ;

 typedef struct s_os_attach_NA {                          /* bs2452 */
      FUNC_PTR_rec_typ   func_ptr ;  /* ptrs to API     *//* bs2452 */
 } OS_ATTACH_NA_typ;                                      /* bs2452 */

 static  struct _rss_t  rss_table[MAX_RSS_TABLE];
 static  int            rss_table_recs = 0;

 static ESA_RC OS_RSSTABL_init(CTSAMSG_DEST_TABLE_rec_ptr dest,
                               CTSAMSG_HANDLE_rec_ptr     msgs);

 static ESA_RC OS_RSSTABL_find (char *rss_type, char   *api_module);

 static char * convert_obj_type(INTERCEPT_obj_typ   intercept_obj_type);

 /*
  * Assembler routine to make started task none-swappable
  */

 extern void  ctsaswp(void);      /* ws2305 */

 static  char component[]="ESAOSCS";

 static FUNC_PTR_rec_typ func_ptr_area; /* NA ptrs area ws2375        */
 static FUNC_PTR_rec_typ *func_ptr_NA = &func_ptr_area;
 /* ptrs to internal api ws2375 */

/*****************************************************************
*  WS2375 - NON APF authorized USAAPI front end
*  ============================================
*
* Following are front end functions for the NON APF authorized
* USA-API. Logic is as follows :
*
* When parameter 'USAAPI_LIB_NAME' is specified in the RSSPARM,
* OS_attach_rss is looking for the USA-API in the specified DD
* name (and not as usual, in STEPLIB). The USA-API is loaded,
* and the function pointers are saved in func_ptr_NA. Then the
* front end is called, and returns a set of functions pointers to
* the front end USA-API. Whenever a transaction occurs in the CS,
* it calls the front end appropriate routine, and the front end
* turns off authorization, and calls the real USA-API function -
* using the function pointer saved in func_ptr_NA.
* After its execution the front end turns on authorization,
* and returns.
*
*****************************************************************/
/**************************************************************
*
* Function Name  : CTSAPIInitNA
*
* Description    : Call CTSAPIInit in non-apf mode
*
***************************************************************/

static ESA_RC CTSAPIInitNA (ADMIN_PARAMS_rec_typ * admin_params,
                    ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAPIInitNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->apiinit_ptr(admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSAPIInit is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*
* Function Name  : CTSAPITermNA
*
* Description    : Call CTSAPITerm in non-apf mode
*
***************************************************************/

static ESA_RC CTSAPITermNA (ADMIN_PARAMS_rec_typ * admin_params,
                            ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAPITermNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->apiterm_ptr(admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSAPITerm is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* PROCEDURE NAME : CTSAddUserNA                               *
*                                                             *
* DESCRIPTION    : Call CTSAddUser in non-apf mode            *
*                                                             *
**************************************************************/

static ESA_RC CTSAddUserNA (USER_PARAMS_rec_typ  * user_params,
                     ADDINFO_rec_typ      * addinfo,
                     ADMIN_PARAMS_rec_typ * admin_params,
                     ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAddUserNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->adduser_ptr(user_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSADDUSER is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* PROCEDURE NAME : CTSUpdUserNA                               *
*                                                             *
* DESCRIPTION    : Call CTSUpdUser in non-apf mode            *
*                                                             *
**************************************************************/

static ESA_RC CTSUpdUserNA (USER_PARAMS_rec_typ  * user_params,
                     ADDINFO_rec_typ      * addinfo,
                     ADMIN_PARAMS_rec_typ * admin_params,
                     ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSUpdUserNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->upduser_ptr(user_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSUPDUSER is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* PROCEDURE NAME : CTSDelUserNA                               *
*                                                             *
* DESCRIPTION    : Call CTSDelUser in non-apf mode            *
*                                                             *
**************************************************************/

static ESA_RC CTSDelUserNA (USER_PARAMS_rec_typ  * user_params,
                     ADDINFO_rec_typ      * addinfo,
                     ADMIN_PARAMS_rec_typ * admin_params,
                     ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSDelUserNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->deluser_ptr(user_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSDELUSER is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* PROCEDURE NAME : CTSRevokeUserNA                            *
*                                                             *
* DESCRIPTION    : Call CTSRevokeUser in non-apf mode         *
*                                                             *
**************************************************************/

static ESA_RC CTSRevokeUserNA (USER_PARAMS_rec_typ  * user_params,
                     ADDINFO_rec_typ      * addinfo,
                     ADMIN_PARAMS_rec_typ * admin_params,
                     ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSRevokeUserNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->revuser_ptr(user_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSREVOKEUSER is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* PROCEDURE NAME : CTSUpdPasswordNA                           *
*                                                             *
* DESCRIPTION    : Call CTSUpdPassword in non-apf mode        *
*                                                             *
**************************************************************/

static ESA_RC CTSUpdPasswordNA (USER_PARAMS_rec_typ  * user_params,
                     ADDINFO_rec_typ      * addinfo,
                     ADMIN_PARAMS_rec_typ * admin_params,
                     ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSUpdPasswordNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->updpass_ptr(user_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSUpdPassword is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;

}
/**************************************************************
*                                                             *
* Function Name : CTSAddUGNA                                  *
*                                                             *
* Description    : call CTSAddUG in non-apf mode              *
*                                                             *
**************************************************************/

static ESA_RC CTSAddUGNA (UG_PARAMS_rec_typ    * ug_params,
                   ADDINFO_rec_typ      * addinfo,
                   ADMIN_PARAMS_rec_typ * admin_params,
                   ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAddUGNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->addug_ptr(ug_params,
                                addinfo,
                                admin_params,
                                err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSAddUG is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* Function Name : CTSUpdUGNA                                  *
*                                                             *
* Description    : call CTSUpdUG in non-apf mode              *
*                                                             *
**************************************************************/

static ESA_RC CTSUpdUGNA (UG_PARAMS_rec_typ    * ug_params,
                   ADDINFO_rec_typ      * addinfo,
                   ADMIN_PARAMS_rec_typ * admin_params,
                   ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSUpdUGNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->updug_ptr(ug_params,
                                addinfo,
                                admin_params,
                                err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSUpdUG is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}


/**************************************************************
*                                                             *
* Function Name : CTSDelUGNA                                  *
*                                                             *
* Description    : call CTSDelUG in non-apf mode              *
*                                                             *
**************************************************************/

static ESA_RC CTSDelUGNA (UG_PARAMS_rec_typ    * ug_params,
                   ADDINFO_rec_typ      * addinfo,
                   ADMIN_PARAMS_rec_typ * admin_params,
                   ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSDelUGNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->delug_ptr(ug_params,
                                addinfo,
                                admin_params,
                                err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSDelUG is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}


/**************************************************************
*                                                             *
* Function Name : CTSAddUserToUGNA                            *
*                                                             *
* Description    : Call CTSAddUserToUG in non-apf mode        *
*                                                             *
**************************************************************/

static ESA_RC CTSAddUserToUGNA (U2UG_PARAMS_rec_typ  * u2ug_params,
                         ADDINFO_rec_typ      * addinfo,
                         ADMIN_PARAMS_rec_typ * admin_params,
                         ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAddUserToUGNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->addu2ug_ptr(u2ug_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSAddUserToUG  is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* Function Name : CTSUpdUserToUGNA                            *
*                                                             *
* Description    : call CTSUpdUserToUG in non-apf mode        *
*                                                             *
**************************************************************/

static ESA_RC CTSUpdUserToUGNA (U2UG_PARAMS_rec_typ  * u2ug_params,
                         ADDINFO_rec_typ      * addinfo,
                         ADMIN_PARAMS_rec_typ * admin_params,
                         ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSUpdUserToUGNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->updu2ug_ptr(u2ug_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSUpdUserToUG  is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}


/**************************************************************
*                                                             *
* Function Name : CTSDelUserFromUGNA                          *
*                                                             *
* Description    : call CTSDelUserFromUG in non-apf mode      *
*                                                             *
**************************************************************/

static ESA_RC CTSDelUserFromUGNA (U2UG_PARAMS_rec_typ  * u2ug_params,
                           ADDINFO_rec_typ      * addinfo,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSDelUserFromUGNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->delu2ug_ptr(u2ug_params,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSDelUserFromUG is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* Function Name  : CTSAddACENA                                *
*                                                             *
* Description    : Call CTSAddACE in non-apf mode             *
*                                                             *
**************************************************************/

static ESA_RC CTSAddACENA (RES_PARAMS_rec_typ   * res_params,
                           ACE_rec_typ          * new_ace,
                           ADDINFO_rec_typ      * new_addinfo,
                           ACE_POS_typ            ace_place,
                           ACE_rec_typ          * rel_ace,
                           ADDINFO_rec_typ      * rel_addinfo,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAddACENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->addace_ptr(res_params,
                                 new_ace,
                                 new_addinfo,
                                 ace_place,
                                 rel_ace,
                                 rel_addinfo,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSAddACE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}


/**************************************************************
*                                                             *
* Function Name  : CTSDelACENA                                *
*                                                             *
* Description    : call CTSDelACE in non-apf mode             *
*                                                             *
**************************************************************/

static ESA_RC CTSDelACENA (RES_PARAMS_rec_typ   * res_params,
                           ACE_rec_typ          * ace,
                           ADDINFO_rec_typ      * addinfo,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSDelACENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->delace_ptr(res_params,
                                 ace,
                                 addinfo,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSDelACE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* Function Name  : CTSUpdAceNA                                *
*                                                             *
* Description    : call CTSUpdAce in non-apf mode             *
*                                                             *
**************************************************************/

static ESA_RC CTSUpdACENA (RES_PARAMS_rec_typ   * res_params,
                           ACE_rec_typ          * new_ace,
                           ADDINFO_rec_typ      * new_addinfo,
                           ACE_rec_typ          * old_ace,
                           ADDINFO_rec_typ      * old_addinfo,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSUpdACENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->updace_ptr(res_params,
                                 new_ace,
                                 new_addinfo,
                                 old_ace,
                                 old_addinfo,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSUpdACE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
* Procedure Name: CTSAddResNA
* Description   : Call CTSAddRes in non-apf mode
***************************************************/

static ESA_RC CTSAddResNA (RES_PARAMS_rec_typ   * res_params,
                           ADDINFO_rec_typ      * addinfo_data,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAddResNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->addres_ptr(res_params,
                                 addinfo_data,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSAddRes is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/**************************************************************
*                                                             *
* PROCEDURE NAME : CTSUpdResNA                                *
*                                                             *
* DESCRIPTION    : call CTSUpdRes in non-apf mode             *
*                                                             *
**************************************************************/

static ESA_RC CTSUpdResNA (RES_PARAMS_rec_typ   * res_params,
                           ADDINFO_rec_typ      * addinfo,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSUpdResNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->updres_ptr(res_params,
                                 addinfo,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSUpdRes is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
* Procedure Name: CTSDelResNA
* Description   : call CTSDelRes in non-apf mode
***************************************************/

static ESA_RC CTSDelResNA (RES_PARAMS_rec_typ   * res_params,
                           ADDINFO_rec_typ      * addinfo_data,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSDelResNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->delres_ptr(res_params,
                                 addinfo_data,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSDelRes is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
* Procedure Name : CTSSetRSSParamsNA
* Description    : call CTSSetRSSParams in non-apf mode
***************************************************/

static ESA_RC CTSSetRSSParamsNA (RSS_PARAMS_rec_typ   * rss_params,
                                 ADDINFO_rec_typ      * addinfo,
                                 ADMIN_PARAMS_rec_typ * admin_params,
                                 ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSSetRSSParamsNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->strsprm_ptr(rss_params,
                                 addinfo,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSSetRSSParams is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
* Procedure Name : CTSGetRSSParamsNA
* Description    : call CTSGetRSSParams in non-apf mode
***************************************************/

static ESA_RC CTSGetRSSParamsNA (RSS_PARAMS_rec_typ   * rss_params,
                                 ADDINFO_rec_typ      * addinfo,
                                 ADMIN_PARAMS_rec_typ * admin_params,
                                 ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSGetRSSParamsNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->gtrsprm_ptr(rss_params,
                                 addinfo,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSGetRSSParams is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/***************************************************************
*
* Procedure Name: CTSGetUsersNA
*
* Description   : call CTSGetUsers in non-apf mode
*
***************************************************************/
static ESA_RC CTSGetUsersNA (GET_USER_MODE          mode,
                             OE_typ                 oe,
                             short                  max_users,
                             short                * actual_num,
                             HAVE_MORE_typ        * have_more,
                             void                ** handle,
                             short                  num_users_in,
      /*SAS2IBMT*/           USER_PARAMS_rec_typ    user_params_in[1],
      /*SAS2IBMT*/           USER_PARAMS_rec_typ    user_params[1],
      /*SAS2IBMT*/           ADDINFO_rec_ptr        addinfo[1],
      /*SAS2IBMT*/           OBJ_EXISTS_typ         objs_exist[1],
                             ADMIN_PARAMS_rec_typ * admin_params,
                             ERR_STRUCT_rec_typ   * err,
                             char                   get_conn) /*CIQ#6*/
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSGetUsersNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->gtusers_ptr(mode,
                                  oe,
                                  max_users,
                                  actual_num,
                                  have_more,
                                  handle,
                                  num_users_in,
                                  user_params_in,
                                  user_params,
                                  addinfo,
                                  objs_exist,
                                  admin_params,
                                  err,
                                  get_conn); /* CIQ#6 */

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSGetUsers is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/*********************************************************************
*
* Function      : CTSGetUGsNA
*
* Description   : call CTSGetUGs in non-apf mode
*
 ********************************************************************/

static ESA_RC CTSGetUGsNA (GET_GROUP_MODE         mode,
                           OE_typ                 oe,
                           short                  max_ugs,
                           short                * actual_num,
                           HAVE_MORE_typ        * have_more,
                           void                ** handle,
                           short                  num_ugs_in,
      /*SAS2IBMT*/         UG_PARAMS_rec_typ      ug_params_in[1],
      /*SAS2IBMT*/         UG_PARAMS_rec_typ      ug_params[1],
      /*SAS2IBMT*/         ADDINFO_rec_ptr        addinfo[1],
      /*SAS2IBMT*/         OBJ_EXISTS_typ         objs_exist[1],
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSGetUGsNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

     rc = func_ptr_NA->getugs_ptr(mode,
                                  oe,
                                  max_ugs,
                                  actual_num,
                                  have_more,
                                  handle,
                                  num_ugs_in,
                                  ug_params_in,
                                  ug_params,
                                  addinfo,
                                  objs_exist,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSGetUGs  is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/*********************************************************************
*
* Function Name: CTSGetConnsNA
*
* Description   : call CTSGetConns in non-apf mode
*
 ****************************************************/

 static ESA_RC CTSGetConnsNA (GET_CONN_MODE          mode,
                              short                  max_conns,
                              short                * actual_num,
                              HAVE_MORE_typ        * have_more,
                              void                ** handle,
                              short                  num_ugs_in,
                              short                  num_users_in,
           /*SAS2IBMT*/       UG_typ                 ugs_in[1],
           /*SAS2IBMT*/       USER_typ               users_in[1],
           /*SAS2IBMT*/       U2UG_PARAMS_rec_typ    u2ug_params[1],
           /*SAS2IBMT*/       ADDINFO_rec_ptr        addinfo[1],
           /*SAS2IBMT*/       OBJ_EXISTS_typ         objs_exist[1],
                              ADMIN_PARAMS_rec_typ * admin_params,
                              ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSGetConnsNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->gtug2uc_ptr(mode,
                                  max_conns,
                                  actual_num,
                                  have_more,
                                  handle,
                                  num_ugs_in,
                                  num_users_in,
                                  ugs_in,
                                  users_in,
                                  u2ug_params,
                                  addinfo,
                                  objs_exist,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSGetConns is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}
/****************************************************
* Procedure Name: CTSGetResNA
* Description   : call CTSGetRes in non-apf mode
 ****************************************************/

static ESA_RC CTSGetResNA (GET_RESOURCE_MODE      mode,
                           OE_typ                 oe,
                           short                  max_ress,
                           short                * actual_num,
                           HAVE_MORE_typ        * have_more,
                           void                ** handle,
                           RES_PARAMS_rec_typ   * res_params_in,
                           ADDINFO_rec_typ      * addinfo_in,
        /*SAS2IBMT*/       RES_PARAMS_rec_typ     res_params[1],
       /*SAS2IBMT*/        ADDINFO_rec_ptr        addinfo_out[1],
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSGetResNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

     rc = func_ptr_NA->getres_ptr(mode,
                                  oe,
                                  max_ress,
                                  actual_num,
                                  have_more,
                                  handle,
                                  res_params_in,
                                  addinfo_in,
                                  res_params,
                                  addinfo_out,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSGetRes is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}
/**************************************************************
*
* Function Name : CTSGetResACLNA
*
* Description   : call CTSGetResACL in non-apf mode
*
**************************************************************/

 static ESA_RC CTSGetResACLNA (GET_ACL_MODE            mode,
                               short                   max_aces,
                               short                 * actual_num,
                               HAVE_MORE_typ         * have_more,
                               void                 ** handle,
                               RES_PARAMS_rec_ptr      res_params,
            /*SAS2IBMT*/       ACE_rec_typ             ace[1],
            /*SAS2IBMT*/       ADDINFO_rec_ptr         addinfo[1],
                               ADMIN_PARAMS_rec_typ  * admin_params,
                               ERR_STRUCT_rec_typ    * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSGetResACLNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->gtrsacl_ptr(mode,
                                  max_aces,
                                  actual_num,
                                  have_more,
                                  handle,
                                  res_params,
                                  ace,
                                  addinfo,
                                  admin_params,
                                  err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSGetResACL is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}


/*****************************************************************
*
* Function Name : CTSRSSLoginNA
*
* Description   : call CTSRSSLogin in non-apf mode
*
*****************************************************************/
static ESA_RC CTSRSSLoginNA (RSS_typ                rss_name,
                             USER_typ               admin,
                             UG_typ                 admin_group,
                             PASSWD_typ             admin_passwd,
                             LOGIN_MODE_typ         login_mode,
                             void                ** handle,
                             ADMIN_PARAMS_rec_typ * admin_params,
                             ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSRSSLoginNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->rss_login_ptr(rss_name,
                                    admin,
                                    admin_group,
                                    admin_passwd,
                                    login_mode,
                                    handle,
                                    admin_params,
                                    err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSRSSLogin is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}
/*****************************************************************
*
* Function Name: CTSRSSLogoutNA
*
* Description   : call CTSRSSLogout in non-apf mode
*
*****************************************************************/

static ESA_RC CTSRSSLogoutNA ( RSS_typ                rss_name,
                               USER_typ               admin,
                               UG_typ                 admin_group,
                               LOGIN_MODE_typ         logout_mode,
                               void                ** handle,
                               ADMIN_PARAMS_rec_typ * admin_params,
                               ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSRSSLogoutNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->rss_logout_ptr(rss_name,
                                    admin,
                                    admin_group,
                                    logout_mode,
                                    handle,
                                    admin_params,
                                    err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSRSSLogout is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}
/***************************************************************
*
* Function Name : CTSRSSCheckNA
*
* Description   : call CTSRSSCheck in non-apf mode
***************************************************************/

static ESA_RC CTSRSSCheckNA (RSS_typ                     rss_name,
                             RSS_STATUS_typ            * status,
                             ADMIN_PARAMS_rec_typ      * admin_params,
                             ERR_STRUCT_rec_typ        * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSRSSCheckNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->rss_check_ptr(rss_name,
                                    status,
                                    admin_params,
                                    err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSRSSCheck is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}
/*****************************************************************
*
* Function Name : CTSInterceptorStartNA
*
* Description   : call CTSInterceptorStart in non-apf mode
*****************************************************************/

static ESA_RC CTSInterceptorStartNA (RSS_typ        rss_name,
                                     char         * host_name,
                             ADMIN_PARAMS_rec_typ * admin_params,
                             ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSInterceptorStartNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->rss_start_intercept_ptr(rss_name,
                                              host_name,
                                              admin_params,
                                              err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSInterceptorStart is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
 * Procedure Name: CTSAddOENA
 * Description   : call CTSAddOE in non-apf mode
 ***************************************************/

 static ESA_RC CTSAddOENA (OE_PARAMS_rec_typ    * oe_params,
                           ADDINFO_rec_typ      * addinfo_data,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSAddOENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->addoe_ptr(oe_params,
                                addinfo_data,
                                admin_params,
                                err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSAddOE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
 * Procedure Name: CTSUpdOENA
 * Description   : call CTSUpdOE in non-apf mode
 ***************************************************/

 static ESA_RC CTSUpdOENA (OE_PARAMS_rec_typ    * oe_params,
                           ADDINFO_rec_typ      * addinfo_data,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSUpdOENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->updoe_ptr(oe_params,
                                addinfo_data,
                                admin_params,
                                err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSUpdOE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
 * Procedure Name: CTSDelOENA
 * Description   : call CTSDelOE in non-apf mode
 ***************************************************/

 static ESA_RC CTSDelOENA (OE_PARAMS_rec_typ    * oe_params,
                           ADDINFO_rec_typ      * addinfo_data,
                           ADMIN_PARAMS_rec_typ * admin_params,
                           ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSDelOENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->deloe_ptr(oe_params,
                                addinfo_data,
                                admin_params,
                                err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSDelOE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/****************************************************
 * Procedure Name: CTSGetOEsNA
 * Description   : call CTSGetOEs in non-apf mode
 ****************************************************/

 static ESA_RC CTSGetOEsNA  (GET_OE_MODE            mode,
                             OE_typ                 oe,
                             short                  max_oe,
                             short                * actual_num,
                             HAVE_MORE_typ        * have_more,
                             void                ** handle,
                             short                  num_oe_in,
           /*SAS2IBMT*/      OE_PARAMS_rec_typ      oe_params_in[1],
           /*SAS2IBMT*/      OE_PARAMS_rec_typ      oe_params[1],
           /*SAS2IBMT*/      ADDINFO_rec_typ      * addinfo[1],
           /*SAS2IBMT*/      OBJ_EXISTS_typ         objs_exist[1],
                             ADMIN_PARAMS_rec_typ * admin_params,
                             ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSGetOEsNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->getoes_ptr(mode,
                                 oe,
                                 max_oe,
                                 actual_num,
                                 have_more,
                                 handle,
                                 num_oe_in,
                                 oe_params_in,
                                 oe_params,
                                 addinfo,
                                 objs_exist,
                                 admin_params,
                                 err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSGetOEs is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/*****************************************************************
*
* Function Name : CTSIsUserInOENA
*
* Description   : call CTSIsUserInOE in non-apf mode
*
*****************************************************************/

static ESA_RC CTSIsUserInOENA( USER_typ               user,
                               OE_typ                 oe_mask,
                               ADMIN_PARAMS_rec_typ * admin_params,
                               ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSIsUserInOENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->isuserinoe_ptr(user,
                                     oe_mask,
                                     admin_params,
                                     err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSIsUserInOE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/*****************************************************************
*
* Function Name : CTSIsUGInOENA
*
* Description   : Call CTSIsUGInOE in non-apf mode
*
*****************************************************************/

static ESA_RC CTSIsUGInOENA( UG_typ                 ug,
                             OE_typ                 oe_mask,
                             ADMIN_PARAMS_rec_typ * admin_params,
                             ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSIsUGInOENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->isuginoe_ptr(ug,
                                   oe_mask,
                                   admin_params,
                                   err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSIsUGInOE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/*****************************************************************
*
* Function Name : CTSIsOEInOENA
*
* Description   : call CTSIsOEInOE in non-apf mode
*
*****************************************************************/

static ESA_RC CTSIsOEInOENA( OE_typ                 oe_name,
                             OE_typ                 oe,
                             ADMIN_PARAMS_rec_typ * admin_params,
                             ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSIsOEInOENA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->isoeinoe_ptr(oe_name,
                                   oe,
                                   admin_params,
                                   err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSIsOEInOE is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/*****************************************************************
*
* Function Name : CTSIsMaskMatchNA
*
* Description   : Call CTSIsMaskMatch in non-apf mode
*
*****************************************************************/

static ESA_RC CTSIsMaskMatchNA(char                 * object  ,
                               char                 * mask,
                               OBJECT_TYPE_typ        object_type,
                               MASK_TYPE_typ          mask_type,
                               ADMIN_PARAMS_rec_typ * admin_params,
                               ERR_STRUCT_rec_typ   * err)
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSIsMaskMatchNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA->ismaskmatch_ptr(object     ,
                                      mask,
                                      object_type,
                                      mask_type,
                                      admin_params,
                                      err);

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSIsMaskMatch is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

/*****************************************************************
*
* Function Name : CTSTransactionEventNA
*
* Description   : Call CTSTransactionEvent in non-apf mode
*
*****************************************************************/

static ESA_RC CTSTransactionEventNA(INTERCEPT_obj_typ      object_type,
                                    OE_typ                 oe,
                                    USER_typ               user,
                                    UG_typ                 ug,
                                    TRNSEVNT_UPCON_FLAG_typ connflag,
                                    ADMIN_PARAMS_rec_typ * admin_params,
                                    ERR_STRUCT_rec_typ   * err )
{
 ESA_RC                       rc;
 ESA_RC                       rc_auth;
 static char func[]="CTSTransactionEventNA";
 ESA_DIAG_enter(ESA_COMP_NON_APF,1, func );

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from NON_APF is %d", rc_auth);

 rc = func_ptr_NA-> trnsevnt_ptr (object_type,
                                 oe,
                                 user,
                                 ug,
                                 connflag,
                                 admin_params,
                                 err );

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
                  "rc from CTSTransactionEvent is %d", rc);

 /* SAS2IBMT
 rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;                   */
 rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"APF_AUT") ;

 ESA_DIAG_printf(ESA_COMP_NON_APF,2,
               "rc from APF_AUT is %d", rc_auth);

 ESA_DIAG_exit(ESA_COMP_NON_APF,1, func, rc );
 return rc;
}

 /****************************************************
 * Procedure Name: apiloadNA
 * Description   : Main api routine to fill func_ptr with
 *                 the non-APF routines addresses.
 * Input         : ptr to api_functions
 * Output        : none
 * Input/Output  :
 * Return Value  : ESA_OK
 * Side Effects  :
 *
 * Comments      :
 *
 ****************************************************/
 static ESA_RC apiloadNA (RSS_typ                rss_name,
                          FUNC_PTR_rec_typ     * func_ptr ,
                          RSS_LIMITS_rec_typ   * rss_limits_ptr,
                          ADMIN_PARAMS_rec_typ * admin_params,
                          ERR_STRUCT_rec_typ   * err)
 {


  /*
   *   Variables
   */

   ESA_RC rc = ESA_OK ;
 /*typedef  __local void (*LOCAL_FUNC_PTR_typ)();*/

  /*
   *  Create API functions addresses structure
   */

   func_ptr->apiinit_ptr       =  NULL;
   func_ptr->apiterm_ptr       =  NULL;
   func_ptr->strsprm_ptr       =  NULL;
   func_ptr->adduser_ptr       =  NULL;
   func_ptr->upduser_ptr       =  NULL;
   func_ptr->deluser_ptr       =  NULL;
   func_ptr->revuser_ptr       =  NULL;
   func_ptr->updpass_ptr       =  NULL;
   func_ptr->addug_ptr         =  NULL;
   func_ptr->updug_ptr         =  NULL;
   func_ptr->delug_ptr         =  NULL;
   func_ptr->addu2ug_ptr       =  NULL;
   func_ptr->updu2ug_ptr       =  NULL;
   func_ptr->delu2ug_ptr       =  NULL;
   func_ptr->addace_ptr        =  NULL;
   func_ptr->delace_ptr        =  NULL;
   func_ptr->updace_ptr        =  NULL;
   func_ptr->addres_ptr        =  NULL;
   func_ptr->updres_ptr        =  NULL;
   func_ptr->delres_ptr        =  NULL;
   func_ptr->gtrsprm_ptr       =  NULL;
   func_ptr->gtusers_ptr       =  NULL;
   func_ptr->getugs_ptr        =  NULL;
   func_ptr->gtug2uc_ptr       =  NULL;
   func_ptr->getres_ptr        =  NULL;
   func_ptr->gtrsacl_ptr       =  NULL;
   func_ptr->rss_login_ptr     =  NULL;
   func_ptr->rss_logout_ptr    =  NULL;
   func_ptr->rss_check_ptr     =  NULL;
   func_ptr->rss_start_intercept_ptr = NULL;
   func_ptr->addoe_ptr         =  NULL;
   func_ptr->updoe_ptr         =  NULL;
   func_ptr->deloe_ptr         =  NULL;
   func_ptr->getoes_ptr        =  NULL;
   func_ptr->isuserinoe_ptr    =  NULL;
   func_ptr->isuginoe_ptr      =  NULL;
   func_ptr->isoeinoe_ptr      =  NULL;
   func_ptr->ismaskmatch_ptr   =  NULL;
   func_ptr->trnsevnt_ptr      =  NULL;

 /*===========================================================*/
 if (func_ptr_NA->apiinit_ptr NE NULL)
     func_ptr   ->apiinit_ptr       = &CTSAPIInitNA;
 if (func_ptr_NA->apiterm_ptr NE NULL)
     func_ptr   ->apiterm_ptr       = &CTSAPITermNA;
 if (func_ptr_NA->strsprm_ptr NE NULL)
     func_ptr   ->strsprm_ptr       = &CTSSetRSSParamsNA;
 if (func_ptr_NA->adduser_ptr NE NULL)
     func_ptr   ->adduser_ptr       = &CTSAddUserNA;
 if (func_ptr_NA->upduser_ptr NE NULL)
     func_ptr   ->upduser_ptr       = &CTSUpdUserNA;
 if (func_ptr_NA->deluser_ptr NE NULL)
     func_ptr   ->deluser_ptr       = &CTSDelUserNA;
 if (func_ptr_NA->revuser_ptr NE NULL)
     func_ptr   ->revuser_ptr       = &CTSRevokeUserNA;
 if (func_ptr_NA->updpass_ptr NE NULL)
     func_ptr   ->updpass_ptr       = &CTSUpdPasswordNA;
 if (func_ptr_NA->addug_ptr NE NULL)
     func_ptr   ->addug_ptr         = &CTSAddUGNA;
 if (func_ptr_NA->updug_ptr NE NULL)
     func_ptr   ->updug_ptr         = &CTSUpdUGNA;
 if (func_ptr_NA->delug_ptr NE NULL)
     func_ptr   ->delug_ptr         = &CTSDelUGNA;
 if (func_ptr_NA->addu2ug_ptr NE NULL)
     func_ptr   ->addu2ug_ptr       = &CTSAddUserToUGNA;
 if (func_ptr_NA->updu2ug_ptr NE NULL)
     func_ptr   ->updu2ug_ptr       = &CTSUpdUserToUGNA;
 if (func_ptr_NA->delu2ug_ptr NE NULL)
     func_ptr   ->delu2ug_ptr       = &CTSDelUserFromUGNA;
 if (func_ptr_NA->addace_ptr NE NULL)
     func_ptr   ->addace_ptr        = &CTSAddACENA;
 if (func_ptr_NA->delace_ptr NE NULL)
     func_ptr   ->delace_ptr        = &CTSDelACENA;
 if (func_ptr_NA->updace_ptr NE NULL)
     func_ptr   ->updace_ptr        = &CTSUpdACENA;
 if (func_ptr_NA->addres_ptr NE NULL)
     func_ptr   ->addres_ptr        = &CTSAddResNA;
 if (func_ptr_NA->updres_ptr NE NULL)
     func_ptr   ->updres_ptr        = &CTSUpdResNA;
 if (func_ptr_NA->delres_ptr NE NULL)
     func_ptr   ->delres_ptr        = &CTSDelResNA;
 if (func_ptr_NA->gtrsprm_ptr NE NULL)
     func_ptr   ->gtrsprm_ptr       = &CTSGetRSSParamsNA;
 if (func_ptr_NA->gtusers_ptr NE NULL)
     func_ptr   ->gtusers_ptr       = &CTSGetUsersNA;
 if (func_ptr_NA->getugs_ptr NE NULL)
     func_ptr   ->getugs_ptr        = &CTSGetUGsNA;
 if (func_ptr_NA->gtug2uc_ptr NE NULL)
     func_ptr   ->gtug2uc_ptr       = &CTSGetConnsNA;
 if (func_ptr_NA->getres_ptr NE NULL)
     func_ptr   ->getres_ptr        = &CTSGetResNA;
 if (func_ptr_NA->gtrsacl_ptr NE NULL)
     func_ptr   ->gtrsacl_ptr       = &CTSGetResACLNA;
 if (func_ptr_NA->rss_login_ptr NE NULL)
     func_ptr   ->rss_login_ptr     = &CTSRSSLoginNA;
 if (func_ptr_NA->rss_logout_ptr NE NULL)
     func_ptr   ->rss_logout_ptr    = &CTSRSSLogoutNA;
 if (func_ptr_NA->rss_check_ptr NE NULL)
     func_ptr   ->rss_check_ptr     = &CTSRSSCheckNA;
 if (func_ptr_NA->rss_start_intercept_ptr NE NULL)
     func_ptr   ->rss_start_intercept_ptr = &CTSInterceptorStartNA;
 if (func_ptr_NA->addoe_ptr NE NULL)
     func_ptr   ->addoe_ptr         = &CTSAddOENA;
 if (func_ptr_NA->updoe_ptr NE NULL)
     func_ptr   ->updoe_ptr         = &CTSUpdOENA;
 if (func_ptr_NA->deloe_ptr NE NULL)
     func_ptr   ->deloe_ptr         = &CTSDelOENA;
 if (func_ptr_NA->getoes_ptr NE NULL)
     func_ptr   ->getoes_ptr        = &CTSGetOEsNA;
 if (func_ptr_NA->isuserinoe_ptr NE NULL)
     func_ptr   ->isuserinoe_ptr    = &CTSIsUserInOENA;
 if (func_ptr_NA->isuginoe_ptr NE NULL)
     func_ptr   ->isuginoe_ptr      = &CTSIsUGInOENA;
 if (func_ptr_NA->isoeinoe_ptr NE NULL)
     func_ptr   ->isoeinoe_ptr      = &CTSIsOEInOENA;
 if (func_ptr_NA->ismaskmatch_ptr NE NULL)
     func_ptr   ->ismaskmatch_ptr   = &CTSIsMaskMatchNA;
 if (func_ptr_NA->trnsevnt_ptr NE NULL)
     func_ptr   ->trnsevnt_ptr      = &CTSTransactionEventNA;

  /*
   *   Finish
   */

   return rc ;

  }    /* apiloadna */

  /****************************************************
   * ws2375 end of front end routines
   ****************************************************/

/****************************************************
 * Procedure Name: OS_CS_script
 * Description   : Execute the platform native language script
 * Input         : func_id - The api call name as define in esaapi.h
 *               : action_id - pre or post process
 *               : script_name - the name of the script to execute.
 *               : script_dir - the name of the library    * IS10184 *
 *               :              from which to execute the  * IS10184 *
 *               :              script or "SCRIPT_DIR".    * IS10184 *
 *               :              When "SCRIPT_DIR" is       * IS10184 *
 *               :              specifed, the routine      * IS10184 *
 *               :              will take the library      * IS10184 *
 *               :              name from the SCRIPT_DIR   * IS10184 *
 *               :              RSSPARM parameter or from  * IS10184 *
 *               :              the DD statement           * IS10184 *
 *               :              USRSCRPT.                  * IS10184 *
 * Input/Output  : entity_info - basic attribute of the entity and
 *               :        admin. information (as addinfo structure)
 *               : addinfo
 * Output        : err
 *
 * Return Value  : ESA_RC
 * Comments      :
 * Scope         :
 ****************************************************/
/***************************************************************
 *                                                             *
 * IS10184 - Improve scripts performance                       *
 * =====================================                       *
 *                                                             *
 * 1. Do not logout/login when called for a GET request.       *
 *    The responsibility to set the appropriate "admin" was    *
 *    moved to the callers of this routine. Otherwise, when    *
 *    a script was called for GET operation we did 4 unneeded  *
 *    login or logout requests:                                *
 *    - The caller switched to admin before callaing us.       *
 *    - Here we switched to STC before activating the script.  *
 *    - We switched back to admin before returning to our      *
 *      caller.                                                *
 *    - our caller switched back to SC before retuninng to its *
 *      caller.                                                *
 *    All these "switches" are not needed because GET requests *
 *    are performed under STC userid.                          *
 *    Therefore, the caller was changed not to switch to the   *
 *    admin userid when the request is GET so the logout/login *
 *    done here are not needed any more.                       *
 *                                                             *
 * 2. Use a pre-allocated scripts library.                     *
 *    When the scripts library is pre-allocated, MVS uses the  *
 *    pre-allocated DD and does not allocate/free the library  *
 *    each time the script is executed.                        *
 *    With this change, we allow removing the SCRIPT_DIR       *
 *    RSSPARM parameter so that the script library name will   *
 *    be defined in one place only.                            *
 *    To support all combinations (with/without parmaeter and  *
 *    with/without DD), the callers of this routine are        *
 *    changed not to retrieve the SCRIPT_DIR RSSPARM parameter *
 *    and pass it as script_dir any more.  Instead they will   *
 *    pass "SCRIPT_DIR" in the script_dir parameter, letting   *
 *    this routine decide where to take the script library     *
 *    name from.                                               *
 *    (Callers that use this routine to execute clist from     *
 *    other libraries can still pass the library name in the   *
 *    script_dir parameter.                                    *
 *                                                             *
 *    In this routine, when script_dir parameter contains      *
 *    "SCRIPT_DIR", we take the library name from the          *
 *    common_parmas. the initialization process processed      *
 *    the USRSCRPT DD statement and thr SCRIPT_DIR RSSPARM     *
 *    parameter and determined on the proper name to use.      *
 *    Ifno name - error.                                       *
 *    If script_dir parameter does not contain "SCRIP_DIR", we *
 *    take it value as the scripts library name.               *
 *                                                             *
 * 3. Use RUOB to hold the script messages instead of EXECOUT. *
 *    This saves 3 open and close calls for EXECOUT (clear,    *
 *    put and get) for each script.                            *
 *    To implement this, clist CTSASCR is changed to call      *
 *    CTSCGRO to save the script output in RUOB in storage,    *
 *    instead of calling EXECIO to write it to EXECOUT.        *
 *                                                             *
 *    In this routine we prepare the parameters to CTSCGRO and *
 *    pass them to CTSASCR instead of the EXECOUT ddname.      *
 *    After CTSASCR completion we call CTSARUH to get the      *
 *    script output (instead of reading from EXECOUT).         *
 *    No real I/O is done in this process.                     *
 *                                                             *
 **************************************************************/

 ESA_RC OS_CS_script (RSS_typ                  rss_name,
                      RSS_typ                  rss_type,
                      SCRIPT_NAME_typ          script_dir,
                      char                   * script_name,
                      int                      func_id,
                      SCRIPT_ACTION_typ        action,
                      ADDINFO_rec_typ        * entity_info,
                      ADDINFO_rec_typ        * addinfo,
                      ADDINFO_rec_typ        * cur_addinfo,
                      ADDINFO_rec_typ        * RssPrmInfo,
                      ADMIN_PARAMS_rec_typ   * admin_params)

 {

 /*
  *   Variables
  */

   ESA_RC                     rc=ESA_OK ;
   ESA_RC                     rc_notify;
   ERR_STRUCT_rec_typ         err;

   CTSAMSG_HANDLE_rec_ptr     msgs;
   CTSAMSG_DEST_TABLE_rec_ptr dest;
   OS_HEADER_typ              header;
   char                       header_ptr[10];
   int                        sum_symb ;
   short                      l;
   char                       command[80];
   char                       max_script_notify_p[80];
   int                        max_script_notify =
                                            MAX_SCRIPT_NOTIFY_DEFAULT;
   int                        tmp;
   int                        i;
   int                        tso_rc;

   /* IS10184 int                        logout_done = 0; /@ ws2349 @/*/

   int                        cblnk;            /* ps0246 */
   short                      output_len;
   /* char                       output_buffer[8192]; bs2504 */
   char                       output_buffer[32768]; /*bs2504*/
   int                        coff, clen;
   ESA_RC                     output_rc;
   char                       ddn[9] = EXECOUT_DDNAME;   /* IS10174 */
   SCRIPT_NAME_typ            script_file = "";           /* IS10184 */
   char                       errmsg[150] = "";           /* IS10184 */
   COMMON_PARAMS_rec_typ     *cmnprms = NULL;             /* IS10184 */
   static char func[]="OS_CS_script";

  /*
   *  Initialize
   */

   ESA_DIAG_enter(ESA_COMP_OS_SCRIPT,1, func );

   msgs = admin_params->ctsamsg_handle;
   dest = admin_params->ctsamsg_dest;

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,                  /* IS10184 */
                   "script=%s(%s) ",                      /* IS10184 */
                    script_dir, script_name);             /* IS10184 */

   /* IS10184 - the code below is removed. The decision which
                authority to set was moved to the callers of
                this routine. This way we save uneeded login/logout
                that were done for GET requests.

   /@ WS2349
    * Before activating the script for "get" operations,
    * set "Started task" authority
    @/

   if ( (func_id EQ FUNC_GTUSERS)  OR                      /@ WS2349 @/
        (func_id EQ FUNC_GTRSPRM)  OR                      /@ WS2349 @/
        (func_id EQ FUNC_GETUGS )  OR                      /@ WS2349 @/
        (func_id EQ FUNC_GTUG2UC)  OR                      /@ WS2349 @/
        (func_id EQ FUNC_GETRES )  OR                      /@ WS2349 @/
        (func_id EQ FUNC_GTRSACL)  OR                      /@ WS2349 @/
        (func_id EQ FUNC_GETOES ) ) {                      /@ WS2349 @/
   /@   (memcmp( rss_type, "TSS", 3) EQ 0) ) {                bs2427 @/
     rc = CTSCRSS_set_authority( &logout_done,             /@ WS2349 @/
                                 SET_TO_STARTED_TASK_AUTH, /@ WS2349 @/
                                 admin_params, &err );     /@ WS2349 @/
     if ( rc NE ESA_OK )                                   /@ WS2349 @/
         goto exit;                                        /@ WS2349 @/
                                                           /@ WS2349 @/
   }                                                       /@ WS2349 @/
       end of code removed by IS10184 */

  /*
   *  Initialize header structure
   */

   memcpy(header.eyecatcher, SCR_EYECATCHER, 4 );
   header.rss_name           = rss_name;
   header.rss_type           = rss_type;
   header.func_id            = func_id;
   header.action             = action;
   header.entity_info        = entity_info;
   header.addinfo            = addinfo;
   header.cur_addinfo        = cur_addinfo;
   header.RssPrmInfo         = RssPrmInfo;   /* WS0363 */
   header.get_rsskwd_typ     = admin_params->cs_func.rsskwd_typ_ptr;

   /* PS0107 */
   header.intercept_set_func = admin_params->cs_func.intercept_set_ptr;
   header.num_intercept_rec  = 0;
   header.intercept_rec      = NULL;
   header.num_intercept_rec_alloc = 0;

   header.msgs               = msgs;
   header.dest               = dest;
   header.rc_script          = ESA_FATAL;
   header.debug_level    = ESA_DIAG_get_debug_level(ESA_COMP_OS_SCRIPT);

   /* PS0311 */
   header.rssprm_get_opt     = admin_params->cs_func.rssprm_get_opt_ptr;
   header.admin_params       = admin_params;

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
                   "msgs=%X dest=%X admin_params=%X",
                   msgs, dest, admin_params);

  /*
   *  Obtain MAX_SCRIPT_NOTIFY value and allocate area
   */

   rc = admin_params->cs_func.rssprm_get_ptr(ALL_RSS,
                                           MAX_SCRIPT_NOTIFY_KEYWORD,
                                           sizeof(max_script_notify_p),
                                           max_script_notify_p);
   if ( rc EQ ESA_OK ) {
     tmp = atoi(max_script_notify_p);
     if ( tmp GT 0 )
        max_script_notify = tmp;
   }
   header.num_intercept_rec_alloc = max_script_notify;

   header.intercept_rec = (INTERCEPT_rec_typ *)
                           malloc( max_script_notify *
                                   sizeof(INTERCEPT_rec_typ) );
   if ( header.intercept_rec EQ NULL ) {
     CTSAMSG_print(ERR_MALLOC, msgs, NULL, dest,
                   "Script Notify Buffer",
                   max_script_notify * sizeof(INTERCEPT_rec_typ) );
     rc = ESA_FATAL ;
     goto exit ;
   }

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
        "Script Notify Buffer Allocated At %X.For %d Elements",
        header.intercept_rec, max_script_notify);

  /*
   *  Convert header ptr to char for pass to REXX program
   */

   sprintf(header_ptr,"%08x",&header) ;

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
                   "header=%x header_ptr(char)=%s",
                   &header,header_ptr);

  /*
   *  Calculate control symbol
   */

   rc = OS_MVS_checksum(header_ptr,&sum_symb);
   if ( rc NE ESA_OK )
     {
        CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                      component, func,
                      "Invalid parameter list",16,__LINE__);
        rc=ESA_ERR ;
        goto exit ;
     }

   l=strlen(header_ptr);
   header_ptr[l]=sum_symb    ;
   header_ptr[l+1]=NULL_CHAR ;

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
                   "script=%s,%s header=%x header_ptr(char)=%s",
                    script_dir, script_name,&header,header_ptr);

   /* IS10184 - start */
   /*
    *  Decide which library name to use
    *  If "SCRIPT_DIR" is passed as script_dir, take the library
    *  name from common_params.
    *  Othwewise, use the name passsed in script_dir as the
    *  library name.
    *
    */
   if (strcmp(script_dir, SCRIPT_DIR) EQ 0)  /* No script file      */
   {
     GET_P_CMNPRMS(admin_params, cmnprms, rc, errmsg)
     if (rc EQ ESA_OK)    /* common_params exist   */
     {
       if (cmnprms->script_lib_error EQ 0xFF) /* Error at init ? */
       {
         /* Issue message prepared during init */
         CTSAMSG_print(cmnprms->script_lib_error_code,
                       msgs, NULL, dest,
                       SCRIPT_DIR_DDN);
         rc = ESA_FATAL;
         goto exit;
       }
       else
         /* if field is empty - error.              */
         if (cmnprms->script_libname[0] EQ NULL_CHAR  OR
             cmnprms->script_libname[0] EQ ' ')
         {
           CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                       component, func,
                       "Scripts environment was not initialized ",
                       rc, __LINE__);
           rc = ESA_FATAL;
           goto exit;
         }
         else   /* all is good - script library name exists, use it. */
           strcpy(script_file, cmnprms->script_libname);
     }
     else                 /* environmental error   */
     {
       CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                     component, func, errmsg,
                     rc, __LINE__);
       rc = ESA_FATAL;
       goto exit;
     }
   }                                  /*                            */
   else                             /* script file name provided   */
     strcpy(script_file, script_dir);    /* use it.                */
   /* IS10184 - end */

  /*
   *  Invoke user REXX program under TSO
   */

   /* IS10174 sprintf(command, "%%CTSASCR  %s(%s) %s ",
           script_dir, script_name, header_ptr);   */
   sprintf(command, "%%CTSASCR  %s(%s) %s %s ",          /* IS10174 */
           script_file, script_name, ddn, header_ptr);   /* IS10184 */
 /* IS10184 script_dir, script_name, ddn, header_ptr);  /@ IS10174 @/ */

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
                   "command = %s", command);

   /* IS10174
   rc = ESA_CLI_exec_wait(EXECOUT_DDNAME, command, &tso_rc, dest, msgs);
   */
   rc = ESA_CLI_exec_wait(ddn, command, &tso_rc,         /* IS10174 */
                          admin_params);                 /* IS10174 */
   if (rc EQ ESA_OK) {
      if (tso_rc LT 4)
         rc = ESA_OK;
      else if (tso_rc LT 8)
         rc = ESA_ERR;
      else rc = ESA_FATAL;
   }
   else rc = ESA_FATAL;

   /* IS10174 - start */
   /*
   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
                   "ESA_CLI_exec_wait rc=%d TSO rc=%d script rc=%d",
                   rc,tso_rc,header.rc_script);
   */
   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
       "ESA_CLI_exec_wait ddn=%s  rc=%d  TSO-rc=%d  script-rc=%d",
        ddn, rc, tso_rc, header.rc_script);               /* IS10184 */
   /* IS10184  rc, ddn, tso_rc, header.rc_script);  */
   /* IS10174 - end   */

  /*
   *  Obtain output
   */

   output_len =  sizeof(output_buffer);
   /* IS10174
   output_rc = ESA_CLI_get_output(EXECOUT_DDNAME, output_buffer,
                                  &output_len, dest, msgs );

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,1,
                   "Shell Output rc=%d",output_rc);
   */
   output_rc = ESA_CLI_get_output(ddn, output_buffer,    /* IS10174 */
                                  &output_len,           /* IS10174 */
                                  admin_params);         /* IS10174 */

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,1,                 /* IS10174 */
                   "Shell Output ddn=%s   rc=%d",        /* IS10174 */
                   ddn, output_rc);                      /* IS10174 */

  /*
   *  Issue output messages
   */

   if (output_rc EQ ESA_OK) {
      clen = strlen(output_buffer);                        /* ps0246 */
      cblnk = strspn(output_buffer , " \n");               /* ps0246 */
      if ((output_len NE clen+1) OR (cblnk NE clen)) { /* ps0246 */

          for (coff = 0; coff LT output_len; coff += clen ) {
              clen = strlen(&output_buffer[coff])+1;

              /* suppress newline markers */
              if ( (clen GT 2) AND
                 (output_buffer[coff+clen-2] EQ '\n'))
                 output_buffer[coff+clen-2] = NULL_CHAR;

                  CTSAMSG_print(RACF_OUTPUT_LINE, msgs, NULL, dest,
                                &output_buffer[coff] );
          } /* for */
      } /* if not an empty line */
   }                                                       /* ps0246 */
   else {
      CTSAMSG_print(RACF_OUTPUT_ERR, msgs, NULL, dest);
      rc = ESA_ERR;
   }

   rc = header.rc_script ;

  /*
   *  Interceptor set
   */

   ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
                   "Interceptor set area %X . %d elements",
                   header.intercept_rec, header.num_intercept_rec);

   if ( header.intercept_rec NE NULL) {
     for (i=0; i LT header.num_intercept_rec; i++) {

        ESA_DIAG_printf(ESA_COMP_OS_SCRIPT,2,
                     "Notify %d. %d/%s oe=%s user=%s group=%s",
                     i, header.intercept_rec[i].type,
                     convert_obj_type(header.intercept_rec[i].obj_type),
                     header.intercept_rec[i].oe,
                     header.intercept_rec[i].user,
                     header.intercept_rec[i].ug);

        rc_notify = (admin_params->cs_func.intercept_set_ptr)
                           (header.intercept_rec[i].type,
                            header.intercept_rec[i].obj_type,
                            header.intercept_rec[i].oe,
                            header.intercept_rec[i].user,
                            header.intercept_rec[i].ug);
        if ( rc_notify NE ESA_OK ) {
          CTSAMSG_print(OS_CS_NOTIFY_FAILED,
                    msgs, NULL, dest,
                    convert_obj_type(header.intercept_rec[i].obj_type),
                    header.intercept_rec[i].oe,
                    header.intercept_rec[i].user,
                    header.intercept_rec[i].ug);
          goto exit;
        }
     }    /* for (i=0;  */
   }    /*  if ( header.intercept_rec NE NULL) */

  /*
   *  Finish
   */

  /* WS2349
   * If script have been activated for "get" and
   * now Agent running under "Started task" authority,
   * return back the current authority
   */

   /* IS10184 - the code below is removed. The decision which
                authority to set was moved to the callers of
                this routine. This way we save uneeded login/logout
                that were done for GET requests.
   if (logout_done)                                        /@ WS2349 @/
     CTSCRSS_set_authority( &logout_done,                  /@ WS2349 @/
                            RETURN_FROM_STARTED_TASK_AUTH, /@ WS2349 @/
                            admin_params, &err );          /@ WS2349 @/
      end of removed code   IS10184 */

   exit:;

   if ( header.intercept_rec NE NULL )
       free (header.intercept_rec);

   ESA_DIAG_exit(ESA_COMP_OS_SCRIPT,1, func, rc );

   return rc ;
 }

/****************************************************
 * Procedure Name: OS_CS_attach_rss
 * Description   : load api functions
 *
 * Input         : RSS_name and type
 *                 CTSAMSG stuff
 *
 * Output        : func ptr structure
 *                 CTSAMSG error structure
 *
 * Return Value  : ESA_OK     upon success
 *                 ESA_FATAL  on any fail situation
 *
 ****************************************************/

 ESA_RC OS_CS_attach_rss (RSS_typ                  rss_name,
                          RSS_typ                  rss_type,
                          FUNC_PTR_rec_typ       * func_ptr ,
                          RSS_LIMITS_rec_typ     * rss_limits_ptr,
                          ADMIN_PARAMS_rec_typ   * admin_params,
                          ERR_STRUCT_rec_typ     * err_pre,
                          void                  ** handle)
 {

 /*
  *   Variables
  */

   int     i;
   int     rss_found;
   int     Not_APF_auth = FALSE;                           /* bs2452 */
   int     load_rc;                                        /* SAS2IBMN */

   typedef ESA_RC (*FP_API)(RSS_typ                rss_name,
                            FUNC_PTR_rec_typ     * func_ptr,
                            RSS_LIMITS_rec_typ   * rss_limits_ptr,
                            ADMIN_PARAMS_rec_typ * admin_params,
                            ERR_STRUCT_rec_typ   * err)  ;

   static char func[]="OS_CS_attach_rss";
   static OS_ATTACH_typ       attach_table[MAX_OS_ATTACH_TABLE];
                                                         /* bs2452 */
   static OS_ATTACH_NA_typ    attach_table_NA[MAX_OS_ATTACH_TABLE];
   static int                 attached_counter=0;
   /* SAS2IBMN static FP                fp_ptr; */
   static FP_API              api_ptr;
   static char                api_module[10] = "";
   static char                load_ddnam[9 ] ;              /*bs2398*/
   static char                load_ddpad[9 ] = "        " ; /*bs2398*/


   ESA_RC                     rc = ESA_OK ;
   ESA_RC                     rc_auth = ESA_OK ;           /* ws2375 */
   CTSAMSG_HANDLE_rec_ptr     msgs;
   CTSAMSG_DEST_TABLE_rec_ptr dest;

  /*
   *  Initialize
   */

   ESA_DIAG_enter(ESA_COMP_OS_ATTACH,1, func);

   msgs = admin_params->ctsamsg_handle;
   dest = admin_params->ctsamsg_dest;

  /*
   *  If first entry initialize attached table.
   */

   if ( attached_counter EQ 0 ) {
      for ( i=0; i LT MAX_OS_ATTACH_TABLE; i++ )
          attach_table[i].attached = 0 ;

      rc = OS_RSSTABL_init(dest,msgs);                 /* PS0082 */
      if ( rc NE ESA_OK ) {                            /* PS0082 */
         CTSAMSG_print(ERR_API_LOAD, msgs, NULL, dest, /* PS0082 */
                       rss_name, api_module);          /* PS0082 */
         goto exit;                                    /* PS0082 */
      }                                                /* PS0082 */
   }

  /*
   *  Determine RSS module/lib
   */

   ESA_DIAG_printf(ESA_COMP_OS_ATTACH,1,
                   "rss_name=%s(%d) rss_type=%s(%d)",
                    rss_name, strlen(rss_name),
                    rss_type,strlen(rss_type) );

   rc = OS_RSSTABL_find (rss_type, api_module);
   if ( rc NE ESA_OK ) {
      CTSAMSG_print(ERR_API_UNKNOWN_TYPE,msgs, NULL, dest,
                    rss_name, rss_type);
      rc=ESA_FATAL ;
      goto exit ;
   }

  /*
   *  Search api_module in attach table.
   *  Definition : need load module or use previously loaded
   */

   rss_found = -1 ;
   for ( i=0 ; i LT attached_counter ; i++) {

       ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,
                       "api_module=%s attach_table(%d).api_module=%s",
                        api_module, i,  attach_table[i].api_module);
       /* ps0173 */
       if (memcmp(api_module, attach_table[i].api_module,8) EQ 0) {
          rss_found = i;
          break;
       }
   }

   ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2, "rss_found=%d", rss_found );

  /*
   *  RSS found , and attached
   */

   if ((rss_found GE 0) AND (attach_table[rss_found].attached)) {
      memcpy(func_ptr, &(attach_table[rss_found].func_ptr),
             sizeof(FUNC_PTR_rec_typ));
  /*-----------------------------------------------------*/ /* bs2452 */
  /* copy "real" usa-api addresses from table.           */
  /*-----------------------------------------------------*/
    memcpy(func_ptr_NA, &(attach_table_NA[rss_found].func_ptr),
          sizeof(FUNC_PTR_rec_typ ));
  /*-----------------------------------------------------*/ /* bs2452 */

      memcpy(rss_limits_ptr, &(attach_table[rss_found].rss_limits),
             sizeof(RSS_LIMITS_rec_typ));

      ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,
                    "rss_type=%s found and attached", rss_type);
      goto exit;
   }

  /*
   *  Load API main module
   */
   rc = admin_params->cs_func.rssprm_get_opt_ptr(         /* ws2355 */
                    rss_name,                             /* bs2400 */
                    USAAPI_LIB_NAME,                      /* ws2355 */
                    sizeof(load_ddnam),                   /* ws2355 */
                    load_ddnam,                           /* ws2355 */
                    OPT_TRUE,                             /* bs2398 */
                    OPT_TRUE);                            /* bs2398 */
                                                          /* ws2355 */
 ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,                    /* bs2398 */
           "Load API  rc = %d  load_ddnam = %s",          /* bs2398 */
           rc,load_ddnam) ;                               /* bs2398 */

   if (rc NE ESA_OK)                                      /* ws2355 */
   {                                                      /* ws2375 */
      Not_APF_auth = FALSE;                               /* bs2452 */
      /* SAS2IBMN fp_ptr = OS_DYNAM_load(api_module, NULL ); * ws2355 */
      /* SAS2IBMN api_ptr = (FP_API)fp_ptr;                * ws2375 */
      /* SAS2IBMN if ( api_ptr NE NULL)                    * ws2375 */
      /* SAS2IBMT
      load_rc = ctsaldm(api_module, "       ", &api_ptr); // SAS2IBMN */
      load_rc = (*(ASM_RTN_TYP *)&ctsaldm)                /* SAS2IBMT */
                       (api_module, "       ", &api_ptr); /* SAS2IBMN */
      if (load_rc EQ 0)                                   /* SAS2IBMN */
         rc = (*api_ptr)(rss_name,                        /* ws2375 */
                         func_ptr, rss_limits_ptr,        /* ws2375 */
                         admin_params, err_pre);          /* ws2375 */
      else {                                              /* ws2375 */
         CTSAMSG_print(ERR_API_LOAD, msgs,                /* ws2375 */
                       NULL, dest,rss_name,api_module);   /* ws2375 */
         rc = ESA_FATAL ;                                 /* ws2375 */
      }                                                   /* ws2375 */

   }                                                      /* ws2375 */
   else {      /* user USAAPI requires non APF mode */    /* ws2355 */
      Not_APF_auth = TRUE;                                /* bs2452 */
      memcpy(load_ddpad, load_ddnam, strlen(load_ddnam)); /* ws2355 */

      /* SAS2IBMT
      rc = OS_DYNAM_call("CTSAATH ",1,"NON_APF") ;        // ws2355 */
      rc = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        ("CTSAATH ",1,"NON_APF") ;        /* ws2355 */

      ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,               /* bs2398 */
                    "rc from NON_APF is %d", rc);         /* bs2398 */

      /* SAS2IBMN - start */
      /*
      fp_ptr = OS_DYNAM_load(api_module, load_ddpad) ;     * ws2355 *
      */
      /* SAS2IBMT
      load_rc = ctsaldm(api_module, load_ddpad, &api_ptr);           */
      load_rc = (*(ASM_RTN_TYP *)&ctsaldm)                /* SAS2IBMT */
                       (api_module, load_ddpad, &api_ptr);
      /* SAS2IBMN - end  */

      ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,               /* bs2398 */
           "api module=%s load_ddpad=%s ddpad_len=%d",    /* bs2398 */
           api_module,load_ddpad,strlen(load_ddpad)) ;    /* bs2398 */

      /* SAS2IBMN api_ptr = (FP_API)fp_ptr;               /* ws2375 */

      /* SAS2IBMN if ( api_ptr NE NULL)                    * ws2375 */
      if (load_rc EQ 0)                                   /* SAS2IBMN */
         rc = (*api_ptr)(rss_name, func_ptr_NA,           /* ws2375 */
                         rss_limits_ptr,                  /* ws2375 */
                         admin_params, err_pre);          /* ws2375 */
      else {                                              /* ws2375 */
         CTSAMSG_print(ERR_API_LOAD, msgs,                /* ws2375 */
                       NULL, dest,rss_name,api_module);   /* ws2375 */
         rc = ESA_FATAL ;                                 /* ws2375 */
      }                                                   /* ws2375 */

      /* SAS2IBMT
      rc_auth = OS_DYNAM_call("CTSAATH ",1,"APF_AUT") ;   // ws2355 */
      rc_auth = (*(ASM_RTN_TYP *)&OS_DYNAM_call)          /* SAS2IBMT */
                             ("CTSAATH ",1,"APF_AUT") ;   /* ws2355 */

      ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,               /* bs2398 */
                    "rc from APF_AUT is %d", rc_auth);    /* bs2398 */

      if ( rc EQ ESA_OK )                                 /* ws2375 */
          rc = apiloadNA (rss_name, func_ptr,             /* ws2375 */
                          rss_limits_ptr,                 /* ws2375 */
                          admin_params, err_pre);         /* ws2375 */

   }
  /*
   *  Save API ptrs, rss_limits in attach table
   */

   if (rss_found LT 0) {
      attached_counter++;
      if ( attached_counter GE MAX_OS_ATTACH_TABLE ) {
         ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,
            "Rss_type=%s maximum of rss was reached. Attach_counter=%d",
            rss_type,attached_counter-1);
         attached_counter--;

         CTSAMSG_print(ERR_NUMBER_RSS,msgs,NULL,dest,
                       rss_name, MAX_OS_ATTACH_TABLE);
         rc = ESA_FATAL;
         goto exit;
      }
      else rss_found = attached_counter - 1 ;
   }

   ESA_DIAG_printf(ESA_COMP_OS_ATTACH,2,
        "rss_type=%s loaded.Attach_table indx=%d attach_counter=%d",
         rss_type,rss_found, attached_counter );

   memcpy(&(attach_table[rss_found].func_ptr), func_ptr ,
          sizeof(FUNC_PTR_rec_typ ));

  /*------------------------------------------------*/      /* bs2452 */
  /* copy "real" usa-api addresses to table         */
  /*------------------------------------------------*/
   if (Not_APF_auth) {
    memcpy(&(attach_table_NA[rss_found].func_ptr), func_ptr_NA ,
          sizeof(FUNC_PTR_rec_typ ));
   }
  /*-------------------------------------------------*/     /* bs2452 */

   memcpy(&(attach_table[rss_found].rss_limits), rss_limits_ptr,
          sizeof(RSS_LIMITS_rec_typ));

   strcpy(attach_table[rss_found].api_module, api_module); /* ps0173 */
   attach_table[rss_found].attached = 1;

   exit :;

   ESA_DIAG_exit(ESA_COMP_OS_ATTACH,1, func, rc);

   return rc;

 }

/****************************************************
 * Procedure Name: OS_CS_detach_rss
 * Description   : unload api functions
 *
 * Input         : Handle
 *                 CTSAMSG stuff
 *                 Admin params
 *
 * Output        : func ptr structure
 *                 CTSAMSG error structure
 *
 * Return Value  : ESA_OK     upon success
 *                 ESA_FATAL  on any fail situation
 *
 ****************************************************/

 ESA_RC OS_CS_detach_rss (ADMIN_PARAMS_rec_typ   * admin_params,
                          ERR_STRUCT_rec_typ     * err,
                          void                  ** handle)
 {
  return ESA_OK;
 }

/****************************************************
 *
 * Procedure Name: OS_CS_get_passwd
 *
 * Description   : Request by prompt_str to enter password
 *
 * Input         : prompt_str
 * Output        : passwd_str. See comments below
 *
 * Return Value  : ESA_OK     upon success
 *                 ESA_FATAL  on any other fail situation
 *
 * Comments      : Max length of passwd_str is MAX_RSS_PASSWD_LEN
 *                 as defined in platform.h
 *
 ****************************************************/

 ESA_RC OS_CS_get_passwd (char * prompt_str,
                          char * passwd_str)
 {

   ESA_RC  rc = ESA_OK;

   gets(passwd_str);

   return rc;
 }

/****************************************************
 * Procedure Name: OS_CS_init
 * Description   : CONTROL-SA init
 *
 * Input         : void
 *
 * Return Value  : ESA_OK     upon success
 *                 ESA_FATAL  on any fail situation
 *
 * Comments      :
 * Scope         :
 ****************************************************/

 ESA_RC OS_CS_init (const char                  * main_name,
                    CTSAMSG_PARAMS_rec_typ      * cts_handle,
                    void                       ** handle )

 {

   int r15,rc,rs;
   ESA_RC init_rc;
   static char func[]="OS_CS_init";

   /***** Initialize *******/

   ESA_DIAG_enter( ESA_COMP_ESAOSCS, 1, func );

   /***** Call tso initialization *******/

   init_rc = ESA_CLI_init_tso(&r15, &rc, &rs);
   ESA_DIAG_printf( ESA_COMP_ESAOSCS, 1,
                    "Init tso rc=%d. Tso r15=%d rc=%d rs=%d",
                    init_rc, r15, rc, rs );

   if (init_rc NE ESA_OK) {
      init_rc = ESA_FATAL;
      goto exit;
   }

  /*
   *  make CS, CD started tasks none-swappable
   */

   if ( (strcmp(main_name, ESA_MAIN_CS) EQ 0 ) OR   /* ws2305 */
        (strcmp(main_name, ESA_MAIN_CD) EQ 0 )  )   /* ws2305 */
      /* SAS2IBMT
      ctsaswp();                                    // ws2305 */
      (*(ASM_RTN_TYP *)&ctsaswp)();                       /* SAS2IBMT */

   /***** Finish *******/

   exit: ;

   ESA_DIAG_exit( ESA_COMP_ESAOSCS, 1, func, init_rc );

   return init_rc;

 }

 /* IS10184 - start */
/****************************************************
 * Procedure Name: OS_CS_init_cmnprms
 * Description   : CONTROL-SA init
 *
 *                 Create the common_params, if not already
 *                 created.  The common_params is pointed by the
 *                 SPI params so if SPI params does not exist
 *                 we first have to allocate the dummy_params as
 *                 a replacement and then create the common_params.
 *
 *                 This routine might be called when:
 *                 - No SPI params block.
 *                   We need to alllocate dummy_params and
 *                   common_params, and put the address of the
 *                   dummy_params block in admin_params as if it
 *                   is SPI params.
 *
 *                 - SPI params exists, but common_params does not.
 *                   (when called from offline...)
 *                   We need to allocate only common_params and
 *                   put its address in SPI params.
 *
 *                 - Both exists
 *                   Nothing to do.
 *
 * Input         : common_params owner
 *               : admin_params
 *
 * Return Value  : ESA_OK     upon success
 *                 ESA_FATAL  on any fail situation
 *
 * Comments      :
 * Scope         :
 ****************************************************/

 ESA_RC OS_CS_init_cmnprms(
                    char                          owner,
                    ADMIN_PARAMS_rec_typ        * admin_params)

 {

   ESA_RC rc = ESA_OK;
   static char func[]="OS_CS_init_cmnprms";

   DUMMY_PARAMS_rec_typ * p_dummy_params = NULL;
   char   errmsg[81] = "";

   /***** Initialize *******/

   ESA_DIAG_enter( ESA_COMP_ESAOSCS, 1, func );

   /* Create the common_params, if not already created. */
   /* The common_params is pointed by the SPI params    */
   /* so we first have to allocate the dummy_params as  */
   /* first have to allocate the dummy_params as a      */
   /* replacement and then create the common_params.    */

   if (admin_params->apiinit_handle EQ NULL)
   {
     /* allocate the dummy_params */
     p_dummy_params = calloc(1, sizeof(DUMMY_PARAMS_rec_typ));
     if (p_dummy_params EQ NULL)
     {
        sprintf(errmsg,"DUMMY_PARAMS in fn=%s", func);
        CTSAMSG_print(ERR_MALLOC, admin_params->ctsamsg_handle, NULL,
                      admin_params->ctsamsg_dest,
                      errmsg, sizeof(DUMMY_PARAMS_rec_typ));
        rc = ESA_FATAL;
        goto exit;
     };
   }
   else
     p_dummy_params =
                (DUMMY_PARAMS_rec_typ *)admin_params->apiinit_handle;

     /* allocate and initilize the common_params */
   if (p_dummy_params->common_params EQ NULL)
   {
     rc = Common_params_Handle ("INIT", owner,
                                &p_dummy_params->common_params,
                                admin_params);
     ESA_DIAG_printf(ESA_COMP_ESAOSCS, 1,
                    "Common_params_Handle(INIT): rc = %d   area=%8X",
                    rc, p_dummy_params->common_params);
     if (rc NE ESA_OK)
     {
       rc = ESA_FATAL;
       goto exit;
     };

     p_dummy_params->common_params->PARAMS_type = PARAMS_TYPE_DUMMY;

   };

   /* If dummy_params weas allocated, return its address. */
   if (admin_params->apiinit_handle EQ NULL)
     admin_params->apiinit_handle = p_dummy_params;

   /***** Finish *******/

   exit: ;

   ESA_DIAG_exit( ESA_COMP_ESAOSCS, 1, func, rc );

   return rc;

 }
 /* IS10184 - end  */

/****************************************************
 * Procedure Name: OS_CS_term
 * Description   : CONTROL-SA term
 *
 * Input         : void
 *
 * Return Value  : ESA_OK     upon success
 *                 ESA_FATAL  on any fail situation
 *
 * Comments      :
 * Scope         :
 ****************************************************/

 ESA_RC OS_CS_term (const char                  * main_name,
                    CTSAMSG_PARAMS_rec_typ      * cts_handle,
                    void                       ** handle )

 {

   return ESA_OK;

 }

/* IS10184 - start */
/****************************************************
 * Procedure Name: OS_CS_term_cmnprms
 * Description   : CONTROL-SA term
 *                 Free the common_params and dummy_params,
 *                 if created by us.
 *
 * Input         : common_params owner
 *               : admin_params
 *
 * Return Value  : ESA_OK     upon success
 *                 ESA_FATAL  on any fail situation
 *
 * Comments      :
 * Scope         :
 ****************************************************/

 ESA_RC OS_CS_term_cmnprms(
                    char                          owner,
                    ADMIN_PARAMS_rec_typ        * admin_params)

 {
   static char func[]="OS_CS_term_cmnprms";
   ESA_RC rc = ESA_OK;
   DUMMY_PARAMS_rec_typ * p_dummy_params = NULL;
   COMMON_PARAMS_rec_typ * cmnp = NULL;

   /* Free the common_params and dummy_params it exist  */
   /* and was created by us.                            */

   p_dummy_params = admin_params->apiinit_handle;
   if (p_dummy_params NE NULL)
   {
     /* Free the common_params         */
     /* Common_Params_handle returns ESA_NOT_SUPP if the owner  */
     /* passed is not equal to the owner set in the block.      */
     ESA_DIAG_printf(ESA_COMP_ESAOSCS, 1,
                    "Common_params_Handle(TERM):  area=%8X",
                     p_dummy_params->common_params);
     rc = Common_params_Handle ("TERM", owner,
                       &p_dummy_params->common_params,
                       admin_params);
     ESA_DIAG_printf(ESA_COMP_ESAOSCS, 1,
                    "Common_params_Handle(TERM): rc = %d", rc);

     /* free the dummy params only if common_params is ours.    */
     if (rc NE ESA_NOT_SUPP)
     {
       free (p_dummy_params);
       admin_params->apiinit_handle = NULL;
     }

   };

   return ESA_OK;

 }
 /* IS10184 - end  */

/*
 *  PS0082
 */

/****************************************************
 * Procedure Name: OS_RSSTABL_init
 * Description   : Initialize rss table
 *
 * Input         : void
 *
 * Return Value  : none
 *
 * Comments      :
 * Scope         :
 ****************************************************/

 static ESA_RC OS_RSSTABL_init(CTSAMSG_DEST_TABLE_rec_ptr dest,
                               CTSAMSG_HANDLE_rec_ptr     msgs)
 {

   ESA_RC    rc = ESA_OK;
   static    char func[] = "OS_RSSTABL_init";
   FILE      *f_ptr = NULL;
   int       n_rec = 0;
   int       i;
   char      *token;
   char      err_msg[200];
   char      err_msg_1[] = "RSSTBL:Invalid record # %d.%.80s";
   char      err_msg_2[] = "RSSTBL:Records number exceed maximum %d";
   char      record[100];
   char      rss_type[20];
   char      api_module[10];

  /*
   *  Initialize
   */

   ESA_DIAG_enter(ESA_COMP_OS_ATTACH,1, func);

   rss_table_recs = 0;  /* Initialize table counter */

   f_ptr = fopen(RSSTABLE_DD, FILE_OPEN_READ_TEXT);
   if (f_ptr EQ NULL) {
      CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                    "fopen", func, RSSTABLE_DD, strerror(errno));
     rc = ESA_FATAL;
     goto exit;
   }

   while ( fgets(record, sizeof(record)-1, f_ptr) ) {

      /*   Handle EOF */

      if ( feof(f_ptr) NE 0) {
         rc = ESA_OK;
         goto exit;
      }

      /*   Handle errors */

      if (ferror(f_ptr)) {
         CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                       "fgets", func, RSSTABLE_DD, strerror(errno));
         rc = ESA_FATAL;
         goto exit;
      }

      n_rec++;

      ESA_DIAG_printf(ESA_COMP_OS_ATTACH,5, "%d.%s", n_rec, record);

      /*   Handle comments */

      if ( record[0] EQ '*' )
        continue;

      /*   Get rss_type */

      token = strtok(record, " \n");
      if ( NOT token ) {
          sprintf(err_msg, err_msg_1, n_rec, record);
          CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                        component, func, err_msg, 1,__LINE__);
          continue;
      }

      if (strlen(token) GT sizeof(rss_type) ) {
          sprintf(err_msg, err_msg_1, n_rec, record);
          CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                        component, func, err_msg, 2,__LINE__);
          continue;
      }
      else
        strcpy(rss_type, token);

      /*   Get api_module */

      token = strtok(NULL, " \n");
      if ( NOT token ) {
          sprintf(err_msg, err_msg_1, n_rec, record);
          CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                        component, func, err_msg, 3,__LINE__);
          continue;
      }

      if (strlen(token) GT sizeof(api_module) ) {
          sprintf(err_msg, err_msg_1, n_rec, record);
          CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                        component, func, err_msg, 4,__LINE__);
          continue;
      }
      else
          strcpy(api_module, token);

      if ( (rss_table_recs + 1) GE MAX_RSS_TABLE) {
         sprintf(err_msg, err_msg_2, MAX_RSS_TABLE);
         CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,
                       component, func, err_msg, 1,__LINE__);
         rc = ESA_FATAL;
      }

      i = rss_table_recs;
      strcpy( rss_table[i].rss_type   , rss_type);
      strcpy( rss_table[i].api_module , api_module);

      ESA_DIAG_printf(ESA_COMP_OS_ATTACH,1,
                      "%d. %s api module=%s", i, rss_type, api_module);

      rss_table_recs++;

   }    /* while fgets */

 exit: ;

 if (f_ptr)
   fclose(f_ptr);

 ESA_DIAG_exit(ESA_COMP_OS_ATTACH,1, func, rc);

 return rc;

 }

/****************************************************
 * Procedure Name: OS_RSSTABL_find
 * Description   : Find api_module
 *
 * Input         : rss_type
 *
 * Return Value  : api_module
 *
 * Comments      :
 * Scope         :
 ****************************************************/

 static ESA_RC OS_RSSTABL_find (char *rss_type, char   *api_module)
 {

   ESA_RC    rc = ESA_OK;
   static    char func[] = "OS_RSSTABL_find";
   int       i;

  /*
   *  Initialize
   */

   ESA_DIAG_enter(ESA_COMP_OS_ATTACH,1, func);

   ESA_DIAG_printf(ESA_COMP_OS_ATTACH,1,
                   "Obtained rss_type=%s", rss_type);

   for (i=0; i LT rss_table_recs; i++) {
    if (strcmp( rss_table[i].rss_type , rss_type) EQ 0 ) {
      memset( api_module, ' ', 8 );
      *(api_module + 9) = NULL_CHAR;
      memcpy( api_module, rss_table[i].api_module,
                          strlen(rss_table[i].api_module)) ;
      ESA_DIAG_printf(ESA_COMP_OS_ATTACH,1,
                      "%d. %s api module=%s",
                      i, rss_type, rss_table[i].api_module);
      goto exit;
    }
   }

   rc = ESA_ERR;   /* Module not found */

  exit :;

   ESA_DIAG_exit(ESA_COMP_OS_ATTACH,1, func, rc);

   return rc;

 }

/****************************************************
 * Procedure Name: convert_obj_type
 * Description   : Convert interceprot obj type to string
 *
 * Input         : intercept_obj_type
 *
 * Return Value  : intercept_onj_type string
 *
 * Comments      :
 * Scope         :
 ****************************************************/

 static char * convert_obj_type(INTERCEPT_obj_typ   intercept_obj_type)
 {

   switch (intercept_obj_type)  {
     case INTERCEPT_USER       :  return ("USER");
                                  break;
     case INTERCEPT_GROUP      :  return ("GROUP");
                                  break;
     case INTERCEPT_CONTAINER  :  return ("OE");
                                  break;
     case INTERCEPT_CONNECTION :  return ("CONN");
                                  break;
     default  :                   return ("UNKNOWN");
   }
   return NULL;
 }

