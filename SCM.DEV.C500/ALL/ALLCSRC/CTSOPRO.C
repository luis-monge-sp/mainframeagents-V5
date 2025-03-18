/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSCPRO in CTS.CSRC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
 /**************************************************************
 *                                                             *
 * Title            : OS Process functions                     *
 *                                                             *
 * File Name        : ctscpro.c                                *
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
 * Mod.Id   Who    When       Description                      *
 * -------- ------ ---------- -------------------------------- *
 * ps0186   Alex   13/08/1997 Removed func : OS_DIAG_file_mod  *
 * ps0254   Guy    19/04/1998 Added func   : get_process_name  *
 *                            Added parm   : os_proc_handle    *
 * ps0295   Guy    09/08/1998 Added func   : OS_PROC_run       *
 * ps0393   Guy    18/11/1999 Two new empty functions          *
 * ps0454   Yoni   02/05/2001 signal functions activated       *
 * ws2446   Yoni   28/11/2001 HANDLE_ABENDS default is N       *
 * SAS2IBMT SeligT 30/06/2016 SAS/C to IBM C Conversion Project*
 * SAS2IBMA AvnerL 07/12/16 Set CC includes per IS0060.        *
 * SAS2IBMN NuritY 01/01/2017 SAS/C to IBM C Conversion:       *
 *                            Remove signal processing because *
 *                            it depends on the HANDLE_ABENDS  *
 *                            RSSPARM ALL_RSS parameter which  *
 *                            is always N. If/when recovery is *
 *                            needed, the appropriate recovery *
 *                            services will have to be used.   *
 * BS10074  NuritY 16/11/17   Enable calling OS_MVS_ddinfo     *
 *                            from CC.                         *
 * IS10174  NuritY 15/01/18   Dynamic EXECOUT support.         *
 **************************************************************/


#include <globs.h>

#include STRING
#include STDLIB

#include ESA_API
#include ESA_OS_PROC

/* IS0060 - Do not use USA-API H files
#include MVS_COMP
#include MVS_OS_MVS              */

/*  IS0060  - Use CC / ALL H files */
#include MVS_C_COMP        /* IS0060 */
#include MVS_OSC_MVS       /* IS0060 */

/* SAS2IBMT #include <lcsignal.h>                             ps0454 */

/* SAS2IBMN static int   Last_exception ;                    * ps0454 */
static int   Last_exception = 0;                          /* SAS2IBMN */

/****************************************************
 * Procedure Name: OS_PROC_fmt_name
 * Description   : Given a format string with a single %s in it,
 *                 replace the %s with a unique string for the
 *                 calling process/task.
 * Input         : const char *, buffer max length
 * Output        :
 * Input/Output  : Buffer
 * Return Value  : ESA_ERR if input buffer is too short
 *                 ESA_OK  if OK
 * Side Effects  :
 * Comments      : It's not an error to pass a format without
 *                 an '%s' in it. In this case, the output will
 *                 be an exact copy of the format.
 ***************************************************/

ESA_RC OS_PROC_fmt_name (const char * format,
                         int          buflen,
                         char       * fname,
                         void       * os_proc_handle)     /* PS0254 */
  {

   ESA_RC rc=ESA_OK;

  /*
   *  Check entering format
   */

   if ( buflen GE ( strlen(format) + 1 ) )
       strcpy(fname, format ) ;
   else
       rc=ESA_ERR;

   return rc ;

  }

/****************************************************
 * Procedure Name: OS_DIAG_file_mod
 * Description   : Check whether DIAG levels file was modified since
 *                 last call to this function.
 * Input         : full pathname
 * Output        :
 * Input/Output  : admin
 * Return Value  : ESA_WARN  file has not changed
 *                 ESA_OK    file has changed
 *                 ESA_FATAL failed to get file status
 * Side Effects  : Status is changed in static area.
 * Comments      : Returns ESA_OK on first time call
 ***************************************************/

 /*
  *  Removed by ps0186
  *
  *  ESA_RC OS_DIAG_file_mod (const char           * fname,
  *                           ADMIN_PARAMS_rec_typ * admin_param)
  *  {
  *
  *   ESA_RC rc=ESA_OK;
  *
  *   rc=ESA_OK;
  *
  *   return rc ;
  *
  *  }
  */

/****************************************************
 * Procedure Name: OS_PROC_get_pid
 * Description   : Get caller's PID (process id), or some other
 *                 entity that uniquely identifies the process
 *                 in the OS.
 * Input         : None
 * Output        : Pid string. NULL is legal
 *                 if a meaningful value can not be returned.
 * Input/Output  :
 * Return Value  : ESA_RC
 * Side Effects  :
 ***************************************************/

ESA_RC OS_PROC_get_pid (OS_pid_str_typ pid,
                        void         * os_proc_handle)    /* PS0254 */
  {

  /*
   *  Variables
   */

   ESA_RC      rc=ESA_OK;
   char        jobname[10] = "";
   char        jobid[10]   = "";
   char        userid[10]  = "";
   char        sysid[9]  = "";
   void       *acee;

  /*
   *    Get jobname, jobid and userid who sent a job
   */

   OS_MVS_whoami( jobname, jobid, userid, sysid, &acee );

   sprintf( pid , "%s/%s", jobname, jobid);

   return rc ;

  }

/* PS0295 */
/****************************************************
 * Procedure Name: OS_PROC_run
 * Description   : This function runs a detached process
 *                 and transfers parameters to the new
 *                 process using the logical names in
 *                 the logical names group table .
 * Input         : proc_path - In case the value of proc_path is
 *                             NULL - the path is <CTSA>\EXE
 *                 proc_name - Including extention
 *                 param_list - A null terminated list of
 *                              parameters separated by space
 * Output        : procid
 * Input/Output  :
 * Return Value  : ESA_RC
 * Side Effects  :
 * Comments      :
 ***************************************************/

ESA_RC OS_PROC_run (char           * application,
                    char           * proc_path,
                    char           * param_list,
                    OS_pid_str_typ   pid,
                    void           * os_proc_handle)

  {

  return ESA_NOT_SUPP;

  }

/* PS0254 */
/****************************************************
 * Procedure Name: OS_PROC_get_process_name
 * Description   : Get caller's process name
 * Input         : None
 * Output        : Process name
 * Input/Output  :
 * Return Value  : ESA_RC
 * Side Effects  :
 ***************************************************/

ESA_RC OS_PROC_get_process_name(char * proc_name,
                                short  max_len,
                                void * os_proc_handle)
  {

  /*
   *  Variables
   */

   ESA_RC      rc=ESA_OK;
   char        jobname[10] = "";
   char        jobid[10]   = "";
   char        userid[10]  = "";
   char        sysid[9]  = "";
   void       *acee;

  /*
   *    Get jobname, jobid and userid who sent a job
   */

   OS_MVS_whoami( jobname, jobid, userid, sysid, &acee );

   if (strlen(jobname) GT max_len)
     strncpy ( proc_name, jobname, max_len );
   else
     strcpy( proc_name, jobname );

   return rc ;

  }

/* End of PS0254 */

/*********************************************************
* Procedure Name: OS_PROC_get_host_name
* Description   : Get caller's Host name.
* Input         : None
* Output        : Host string. NULL is ilegal
* Input/Output  :
* Return Value  : ESA_RC
* Side Effects  :
*********************************************************/

ESA_RC OS_PROC_get_host_name (OS_host_name_typ host_name,
                              void           * os_proc_handle)
                                                          /* PS0254 */
{

  /*
   *  Variables
   */

   ESA_RC          rc=ESA_OK;
   char            jobname[10] = "";
   char            jobid[10]   = "";
   char            userid[10]  = "";
   char            sysid[9]  = "";
   void           *acee;

  /*
   *  obtain info
   */

   OS_MVS_whoami( jobname, jobid, userid, sysid, &acee );

   strcpy(host_name, sysid);

   return rc ;

}

/* SAS2IBMN - start  */
/*****************************************************************
*                                                                *
* SAS2IBMN - Remove signal processing                            *
*                                                                *
* Signal processing is removed because it depends on the         *
* HANDLE_ABENDS RSSPARM ALL_RSS parameter which is always N.     *
* If/when recovery is needed, these routines will have to be     *
* written using the appropriate IBM C or LE services.            *
*                                                                *
* Considerations:                                                *
* 1. The siginfo is not availabe in IBM C.                       *
* 2. LE has its own recovery so we should find a way to control  *
*    LE dump and backtrace according to our needs.               *
*                                                                *
******************************************************************
 *****************************************************************
* Procedure Name: OS_PROC_trslt_signal
* Description   : Translates signal number to signal
*                 name.
* Input         : Signal number.
* Output        : Signal string.
* Input/Output  :
* Return Value  : none.
* Side Effects  :
* Comments      : When the signal is unknown/illegal -
*                 the function should return a string
*                 of the signal number.
*                 The returned string must be terminated
*                 by NULL character.
*
* PS0454 - routine written
******************************************************************

void OS_PROC_trslt_signal (int           signum ,
                           OS_sigstr_typ sigstr,
                           void        * os_proc_handle)   * PS0254 *
{
 ABND_t *abend_data ;
 ABRT_t *abort_data ;
 FPE_t  *fpe_data ;
 ILL_t  *ill_data ;
 SEGV_t  *segv_data ;
 unsigned short   diag_comp = ESA_COMP_ESAOSCS;
 char   * fn="OS_PROC_trslt_signal";

 ESA_DIAG_enter (diag_comp , 1 , fn);
 sprintf (sigstr , "Exception #%X (Translation was not found)" ,
          signum) ;
 switch ((unsigned int)signum)
  {
   case SIGABND:
    abend_data = siginfo() ;
    if (abend_data NE NULL)
      sprintf (sigstr , "Abended, code= %.5s",
               abend_data->ABEND_str);
    break;
   case SIGABRT:
    abort_data = siginfo() ;
    if (abort_data NE NULL)
      sprintf (sigstr , "Abended, code= %.5s",
               abort_data->ABEND_str);
    break;
   case SIGFPE:
    fpe_data = siginfo() ;
    if (fpe_data NE NULL)
      sprintf (sigstr , "exception SIGFPE. code=%X",
                fpe_data->int_code);
    break;
   case SIGILL:
    ill_data = siginfo() ;
    if (ill_data NE NULL)
      sprintf (sigstr , "exception SIGILL. code=%X",
                ill_data->int_code);
    break;
   case SIGSEGV:
    segv_data = siginfo() ;
    if (segv_data NE NULL)
      sprintf (sigstr , "exception SIGSEGV. code=%X",
                segv_data->int_code);
    break;
    }
  ESA_DIAG_exit (diag_comp, 1 , fn, ESA_OK);
  return;
}

 ****************************************************************
* Procedure Name: OS_PROC_set_signal
* Description   : Activate platform's interesting signals
* Input         : Exit handler's function address.
*                 Main process type.
* Output        :
* Input/Output  :
* Return Value  : ESA_FATAL failed to activate signal
* Side Effects  :
* Comments      :
* PS0454  - routine written.
****************************************************************

ESA_RC OS_PROC_set_signal (void               (* func)(int),
                           ESA_MAIN_NAME_typ  main_type,
                           void             * os_proc_handle)
                                                           * PS0254 *
{
 #define  HANDLE_ABENDS       "HANDLE_ABENDS"
 #define  ALL_RSS             "ALL_RSS"
 #define  NO_ABEND_HANDLE     "N"
 unsigned short   diag_comp = ESA_COMP_ESAOSCS;
 char           * fn="OS_PROC_set_signal";
 ESA_RC          rc=ESA_OK;
 ESA_RC      prm_rc=ESA_OK;
 char          * abend_handle = " ";

 ESA_DIAG_enter (diag_comp , 1 , fn);
 Last_exception = 0;
 prm_rc = rssprm_get_opt(
                  ALL_RSS,
                  HANDLE_ABENDS,
                  sizeof(abend_handle),
                  abend_handle,
                  OPT_TRUE,
                  OPT_FALSE);    * ws2446 *

 ESA_DIAG_printf(diag_comp,2,
                "rc from prm_get: %d, abend_handle: %s",
                prm_rc,abend_handle);
  * ws2446 if ((prm_rc NE ESA_OK) OR
     ((prm_rc EQ ESA_OK) AND (abend_handle[0] NE 'N'))) *
  if ((prm_rc EQ ESA_OK) AND (abend_handle[0] EQ 'Y'))  * ws2446 *
   {
    if (func) {
       ESA_DIAG_printf(diag_comp,2,
                     "Turning on signals");
       signal(SIGABND,func) ;
       signal(SIGABRT,func) ;
       signal(SIGFPE,func) ;
       signal(SIGILL,func) ;
       signal(SIGSEGV,func) ;
    }
    else
    rc=ESA_FATAL ;
   }

 ESA_DIAG_exit (diag_comp, 1 , fn, ESA_OK);
 return rc;
}
*/
/* SAS2IBMN - end    */

/****************************************************
 * Procedure Name: OS_PROC_init
 * Description   : Perform OS-related initialization steps
 *                 upon process initialization.
 * Input         : Main_type - the main process type.
 * Output        :
 * Input/Output  : handle.
 * Return Value  : ESA_FATAL if failed.
 * Side Effects  :
 * Comments      :
 ***************************************************/

ESA_RC OS_PROC_init (ESA_MAIN_NAME_typ    main_type,
                     void              ** handle,
                     int                  argc_prm,
                     char              ** argv_prm)
{

  ESA_RC             rc;

  rc = OS_MVS_init(handle);

  return rc;
}

 /* BS10074 - start */
/****************************************************
 * Procedure Name: OS_PROC_ddinfo
 * Description   : Retrieve dsname and member name
 *                 of a re-allocated DD.
 * Input         : OS-MVS-ddinfo parameters
 * Output        :
 * Input/Output  :
 * Return Value  : Same as OS_MVS_ddinfo_do
 * Side Effects  :
 * Comments      :
 ***************************************************/

ESA_RC OS_PROC_ddinfo ( char                        * ddn,
                        char                        * file_name,
                        int                           issue_err_msg,
                        CTSAMSG_DEST_TABLE_rec_typ  * dest,
                        CTSAMSG_HANDLE_rec_typ      * msgs,
        /* IS10174 */   int                           parm_num,
        /* IS10174 */   ... )
{

  ESA_RC             rc;
  va_list            arg_list;                           /* IS10174 */

  va_start (arg_list, parm_num);                         /* IS10174 */

  /* IS10174
  rc = OS_MVS_ddinfo (ddn, file_name, issue_err_msg,
                      dest, msgs);                                  */
  rc = OS_MVS_ddinfo_do (ddn, file_name, issue_err_msg,  /* IS10174 */
                      dest, msgs, parm_num,              /* IS10174 */
                      arg_list);                         /* IS10174 */

  va_end (arg_list);                                     /* IS10174 */

  return rc;
}
/* BS10074 - end  */

/****************************************************
 * Procedure Name: OS_PROC_term
 * Description   : Perform OS-related termination steps
 *                 upon process termination.
 * Input         : Main_type - the main process type.
 * Output        :
 * Input/Output  : handle
 * Return Value  : ESA_FATAL if failed.
 * Side Effects  :
 * Comments      :
 ***************************************************/

ESA_RC OS_PROC_term (ESA_MAIN_NAME_typ    main_type,
                     void              ** handle)
{

  if ( (*handle) NE NULL)
    free ( (*handle) );

  return ESA_OK;
}

/****************************************************
 * Procedure Name: OS_PROC_exit
 * Description   : Invoke OS' exit activities.
 * Input         : signum - the signal that caused the activation
 *                          of exit handler.
 *                 Main_type - Main process type.
 * Output        :
 * Input/Output  :
 * Return Value  : None
 * Side Effects  :
 * Comments      :
 ***************************************************/

void OS_PROC_exit (int               signum,
                   ESA_MAIN_NAME_typ main_type,
                   void            * os_proc_handle)      /* PS0254 */
{
 exit(signum);                                            /* PS0454 */
}

/* PS0393 */
/****************************************************
 * Procedure Name: OS_PROC_save_exception
 * Description   : Save last exception code
 * Input         : Exception code
 *
 * Output        :
 * Input/Output  :
 * Return Value  : ESA_SUCCESS always
 * Side Effects  :
 * Comments      :
 ***************************************************/
ESA_RC OS_PROC_save_exception (int   signum)
{
 Last_exception = signum;
 return ESA_OK ;
}

/****************************************************
 * Procedure Name: OS_PROC_get_exception
 * Description   : Retrieve last exception id save by
 *                 OS_PROC_save_exception
 * Input         :
 * Output        :
 * Input/Output  :
 * Return Value  : Last exception code
 * Side Effects  :
 * Comments      :
 ***************************************************/
int OS_PROC_get_exception ()
{
 return Last_exception ;
}
/* End of PS0393 */
