/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSCCLI in CTS.CSRC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
/**************************************************************
*                                                             *
* Title            : Command Line Services                    *
*                                                             *
* File Name        : ctsocli.c                                *
*                                                             *
* Author           : Doron Cohen                              *
*                                                             *
* Creation Date    : 23/02/94                                 *
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
*                   dd/mm/yy                                  *
*                                                             *
* BASR1009 Alex     25/04/96 Correct creation of  //systsin   *
*                            file for non dynamic TSO envir   *
* ps0240   Alex     16/02/98 supress empties lines printing   *
* SAS2IBMT SeligT   30/06/16 SAS/C to IBM C Conversion Project*
* SAS2IBMN NuritY   10/10/16 SAS/C to IBM C Conversion:       *
*                            1. Replace access() with fopen   *
*                               and fgets.                    *
*                            2. Replace remove(ddn) with      *
*                               fopen() and fclose() to clear *
*                               the file.                     *
*                            3. Replace remove(dsn) with      *
*                               remove(ddn) to delete the     *
*                               file.                         *
*                            4. ddn: -> dd:                   *
* SAS2IBMA AvnerL   07/12/16 Set CC includes per IS0060.      *
* WS10062  SeligT   05/11/17 Refresh GDB At Start of Group    *
*                            Aggregation: Create new routine  *
*                            (similar to ESA_CLI_get_output), *
*                            ESA_CLI_fetch_output, which      *
*                            reads blocks of output, one at   *
*                            a time - in 'byteseek' mode -    *
*                            and returns them to the caller   *
*                            for processing.                  *
* WS10067  SeligT   29/01/18 Update GDB dynamically when      *
*                            access or resource rules are     *
*                            added/modified                   *
* IS10174  NuritY   15/01/18 Dynamic EXECOUT support.         *
* WS10076  SeligT   06/01/20 Manage Permissions as Account    *
*                            and Group Attributes - Phase 2   *
* WS10076A AvnerL   23/01/20 Add ESA_CLI_del_RUOB             *
* WS10078  AvnerL   01/04/20 support x-rol & x-sgp            *
* WS10078KGKailasP  28/04/20 Issue of garbage char at the end *
*                            of line in ESA_CLI_get_RUOB_line *
* BS10111  NuritY   16/12/20 Support ruob above th bar.       *
* IS10183  ThomaS   10/05/21 Don't print password in clear    *
* IS10184  NuritY   31/08/21 Improve scripts performance      *
**************************************************************/
#include   <globs.h>

/*
 *   Standard include files
 */

#include   STDIO
#include   STDLIB
#include   STRING
#include   FCNTL
#include   ERRNO                                          /* SAS2IBMT */
#include   UNISTD                                         /* SAS2IBMT */

/*
 *   CONTROL-SA include files
 */

#include   ESA_DIAG
#include   ESA_API                                       /* IS10174 */
#include   ESA_API_CODES

/* IS0060 - Do not use USA-API H files
#include   MVS_COMP
#include   MVS_OS_DYNAM
#include   MVS_OS_CLI
#include   MVS_OS_MVS

#include   RACF_CODES
*/

/*  IS0060 - use H files of CC */
#include   API_C_AUTH                                      /* IS10174 */
#include   MVS_C_COMP
#include   MVS_OSC_DYNAM
#include   MVS_OSC_CLI
#include   MVS_OSC_MVS
#include   MVS_C_CODES                                   /* WS10076 */

#include   RACF_C_CODES
/* IS0060 - END  */

/*
 *   definitions
 */

#define CLI_MAX_ST            81
#define CLI_MAX_OUTPUT_ST     8192
#define CLI_MAX_EFFECTIVE_ST  71          /* BASR1009 */
#define SYSTSIN_RECORD_LINE   80          /* BASR1009 */
#define TSO_INIT_MODULE       "CTSAINI "
/* IS10184 #define TSO_SERVICES_MODULE   "CTSATSO "  */

/*
 *   static variables
 */

/* SAS2IBMN - start */
/*
static char  shell_input_file_name[CLI_MAX_ST] = "DDN:SYSTSIN";
*/
static char  shell_input_file_name[CLI_MAX_ST] = "DD:SYSTSIN";
/* SAS2IBMN - end */
static char  component[] = "OS_MVS";

/*
 *   Assembler function declarations
 */

 extern int ctsacli(void);

 typedef int TSO_SERVICES_func(char *op, ... );


/* move it to CTSCCLI mac to be used by Get_XREF_Record * WS10078 *
 * WS10076 - start *
typedef struct MEMORY_HANDLE_STRUCT {
  REXX@UTL_output_blk_rec_typ *block;
  int len;
  int iseof;
} Get_RUOBline_handle_rec_typ;
/* WS10076 - end *
 * WS10078 end */

 /* IS10174 - start */
 /*
  *  GETDD
  *  Get EXECOUT ddname for current userid.
  *
  */
 #define GETDD(d, retcod)                                      \
                                                               \
  if (strcmp((d), EXECOUT_DDNAME) EQ 0)                        \
  {                                                            \
    (retcod) = CTSCRSS_Admin_GetEXECOUTDD(NULL, (d),           \
                                         PLT_COMP_OS_CLI,      \
                                         admin_params);        \
    ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,                       \
          "CTSCRSS_Admin_GetEXECOUTDD: rc = %s   ddn = %s",    \
          ESA_rc2str((retcod)), (d) );                         \
    if ((retcod) NE ESA_FATAL)                                 \
      (retcod) = ESA_OK;                                       \
  }                                                            \
  else                                                         \
    (retcod) = ESA_OK;
 /* IS10174 - end   */



/* BASR1009 */
/**************************************************************
*                                                             *
* PROCEDURE NAME : PutSYSTSIN                                 *
*                                                             *
* DESCRIPTION    : Create SYSTSIN file for non dynamic        *
*                  TSO environment                            *
*                                                             *
* INPUT          : p_cmd, i_start, cmd_len                    *
*                  dest, msgs                                 *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************/
static ESA_RC PutSYSTSIN(char                        * p_cmd,
                         int                           i_start,
                         int                           cmd_len,
                         CTSAMSG_DEST_TABLE_rec_typ  * dest,
                         CTSAMSG_HANDLE_rec_typ      * msgs )
{

 /*
  *         Vars
  */
  static char func[] = "PutSYSTSIN";

  ESA_RC rc = ESA_OK;
  FILE   *shell_input_file = NULL;
  char   systsin_record[SYSTSIN_RECORD_LINE];
  int    len;

 /*
  *         Initialize
  */

  ESA_DIAG_enter( PLT_COMP_OS_CLI, 1, func );

  shell_input_file = fopen( shell_input_file_name,
                            FILE_OPEN_WRITE_TEXT);
  if (shell_input_file EQ NULL) {
      CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                    "open","TSO services",
                    shell_input_file_name, strerror(errno));
      rc = ESA_FATAL;
      goto exit;
  }

  len    = cmd_len;  /* Command len */
  p_cmd += i_start;  /* Ptr to first non blank sybmol */

  /*
   *  Build records with continues
   */

  while ( len GT CLI_MAX_EFFECTIVE_ST ) {

     memcpy(systsin_record, p_cmd, CLI_MAX_EFFECTIVE_ST);
     systsin_record[CLI_MAX_EFFECTIVE_ST]   = '-';
     systsin_record[CLI_MAX_EFFECTIVE_ST+1] = NEWLINE;
     systsin_record[CLI_MAX_EFFECTIVE_ST+2] = NULL_CHAR;

     ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
                      "line=%s", systsin_record);

     /* write the record with continues */

     fputs(systsin_record,shell_input_file);
     if (ferror(shell_input_file)) {
        CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                      "put","TSO services",
                      shell_input_file_name, strerror(errno));
        rc = ESA_FATAL;
        goto exit;
     } /* if */

     len   -= CLI_MAX_EFFECTIVE_ST;
     p_cmd += CLI_MAX_EFFECTIVE_ST;

  }

  /*
   *    Write last record
   */

   if ( len GT 0 ) {
     strcpy(systsin_record, p_cmd);
     ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
                      "line=%s", systsin_record);

     /* write the record with continues */

     fputs(systsin_record,shell_input_file);
     if (ferror(shell_input_file)) {
        CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                      "put","TSO services",
                      shell_input_file_name, strerror(errno));
        rc = ESA_FATAL;
        goto exit;
     } /* if */

   }

  exit: ;

  if (shell_input_file);
     fclose(shell_input_file);

  ESA_DIAG_exit( PLT_COMP_OS_CLI, 1, func, rc );

  return rc;

}

/**************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_init_tso                           *
*                                                             *
* DESCRIPTION    : Initialize tso services in the address     *
*                  space .                                    *
*                                                             *
* INPUT          : None                                       *
*                                                             *
* OUTPUT         : r15 , rc ,rs                               *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************/

ESA_RC ESA_CLI_init_tso( int        *r15,
                         int        *rc,
                         int        *rs )
{

  static char func[] = "ESA_CLI_init_tso";


 /*
  *         Variables
  */

  ESA_RC            init_rc = ESA_OK;
  int               ini_dbg = 0 ;
  char              ini_opcode[] =  "INIT    " ;

 /*
  *         Initialize
  */

  ESA_DIAG_enter( PLT_COMP_OS_CLI, 1, func );

  if ( ESA_DIAG_get_debug_level(PLT_COMP_OS_CLI) GT 0)
     ini_dbg = 1;

  /* SAS2IBMT
  (*r15) = OS_DYNAM_call(TSO_INIT_MODULE, 1, ini_opcode, &ini_dbg,   */
  (*r15) = (*(ASM_RTN_TYP *)&OS_DYNAM_call)               /* SAS2IBMT */
                        (TSO_INIT_MODULE, 1, ini_opcode, &ini_dbg,
                         rc, rs);
  ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
                   "Call CTSAINI r15=%d rc=%d rs=%d",
                   *r15, *rc, *rs );

  if ( (*r15 GT 4) AND (*r15 NE 16) )
       init_rc = ESA_FATAL;

 /*
  *   Termination
  */

  exit: ;

  ESA_DIAG_exit( PLT_COMP_OS_CLI, 1, func, init_rc );

  return init_rc;

}

/**************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_exec_wait                          *
*                                                             *
* DESCRIPTION    : Execute a shell command, wait for its      *
*                  completion .                               *
*                                                             *
* INPUT          : ddn              -  DDN of file            *
*                  p_in_cmd         -  command                *
*                  dest             -  destinations table     *
*                  msgs             -  messages buffer        *
*                                                             *
* OUTPUT         : p_shell_rc       -  shell rc               *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************/
ESA_RC ESA_CLI_exec_wait(char                        * ddn,
        /* IS10174 */    CMDLINE_D_typ                 p_in_cmd,
                         int                         * p_shell_rc,
        /* IS10174 */    ADMIN_PARAMS_rec_typ        * admin_params)
        /* IS10174       CMDLINE_D_typ                 p_cmd,        */
        /* IS10174       CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
        /* IS10174       CTSAMSG_HANDLE_rec_typ      * msgs )        */
{

  static char func[] = "ESA_CLI_exec_wait";

  ESA_RC exec_rc = ESA_OK;
  int    coff = 0 ;
  int    cleft;

  int    tso_r15, tso_rc, tso_rs, tso_ab;
  char   tso_chkenv[] = "CHKENV  ";
  char   tso_cmd   [] = "CMD     ";
  int    tso_dbg =0;
  struct tso_buff_struct {
    int  len;
    char cmd[CLI_MAX_OUTPUT_ST];
  } tso_buff;

  char   p_cmd[CLI_MAX_OUTPUT_ST];                       /* IS10174 */
  char  *cmd_ddn;     /* ddname to be replaced in cmd       IS10174 */
  char   ddname[9];                                      /* IS10174 */
  int    i = 0;                                          /* IS10174 */
  char   err_msg[80];                                    /* IS10174 */
  char  *p  =NULL;                                       /* IS10183 */
  char  *p1 =NULL;                                       /* IS10183 */
  char  *p2 =NULL;                                       /* IS10183 */
  char   astrx[100];                                     /* IS10183 */
  int    j = 0;                                          /* IS10183 */
  int    pl= 0;                                          /* IS10183 */
  ASM_RTN_TYP * p_ctsatso;                               /* IS10184 */
  char          errmsg[170] = "";                        /* IS10184 */
  COMMON_PARAMS_rec_typ  * cmnprms = NULL;               /* IS10184 */

  CTSAMSG_HANDLE_rec_typ     * msgs;                     /* IS10174  */
  CTSAMSG_DEST_TABLE_rec_typ * dest;                     /* IS10174  */

 /*
  *         Initialize
  */

  ESA_DIAG_enter( PLT_COMP_OS_CLI, 1, func );

  msgs = admin_params->ctsamsg_handle;                   /* IS10174  */
  dest = admin_params->ctsamsg_dest;                     /* IS10174  */

 /* IS10183 - start */
 p = strstr(p_in_cmd, "PASSWORD(");
 if (p EQ NULL)
     p = strstr(p_in_cmd, "PWPHRASE(");
 if (p EQ NULL)
     p = strstr(p_in_cmd, "PHRASE(");
 if (p NE NULL)
 {
     p1= strstr(p, "(");
     p2= strstr(p1,")");
     if (  (p1 NE NULL) AND (p2 NE NULL) )
     {
       pl = p2 - p1 - 1;
       pl = pl < sizeof(astrx) ? pl : sizeof(astrx)-2;
       for (j=0; j LE pl-1; j++)
           astrx[j] = '*';
        astrx[j] = NULL_CHAR;
        ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
          "command=%d/%.*s%.*s%s",
          strlen(p_in_cmd), p1-p_in_cmd+1, p_in_cmd, pl, astrx,p2);
     }
 }
 else
 /* IS10183 - end   */
  ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
    /* IS10174 */  "command=%d/%s", strlen(p_in_cmd), p_in_cmd );
    /* IS10174     "command=%d/%s", strlen(p_cmd), p_cmd );         */
  ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,                   /* IS10174 */
                   "ddname = %s", ddn);                  /* IS10174 */

  if ( ESA_DIAG_get_debug_level(PLT_COMP_OS_CLI) GE 3 )
     tso_dbg = 1;

  /* IS10174 - start */
  strcpy(ddname, ddn);
  GETDD(ddname, exec_rc)

  if (exec_rc NE ESA_OK)
    goto exit;

  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "Retrieved ddname = %s", ddname);

  if (strcmp(ddn, ddname) NE 0)     /* if ddname has to be changed  */
  {
    cmd_ddn = strstr(p_in_cmd, ddn); /* look for ddname to replaced.*/
    if (cmd_ddn NE NULL)
    {
      ESA_DIAG_printf(PLT_COMP_OS_CLI, 1,
                   "DDNAME to be replaced:  %s <-> %s ", ddn, ddname);

      strncpy(p_cmd, p_in_cmd, cmd_ddn-p_in_cmd);
      strcpy(p_cmd+(cmd_ddn-p_in_cmd), ddname);  /* set ddname in cmd */
      strcat(p_cmd, cmd_ddn+strlen(ddn) );
    }
    else   /* No ddname to replace, copy the whole command           */
      strcpy(p_cmd, p_in_cmd);

    strcpy(ddn, ddname);    /* return ddname to caller.              */
  }
  else   /* No ddname to replace, copy the whole command             */
    strcpy(p_cmd, p_in_cmd);

  /* IS10174 - end   */

 /*
  *         If dynamic tso supported , use it instead
  */

  /* SAS2IBMT
  tso_r15 = OS_DYNAM_call(TSO_SERVICES_MODULE, 1, tso_chkenv,        */
  /* IS10184
  tso_r15 = (*(ASM_RTN_TYP *)&OS_DYNAM_call)              /@ SAS2IBMT @/
                         (TSO_SERVICES_MODULE, 1, tso_chkenv,
                          &tso_rc, &tso_rs, &tso_ab, &tso_dbg); */

  /* Get CTSATSO routine address and call it.                IS10184 */
  GET_CMNPRMS_PROG(admin_params, cmnprms, p_ctsatso,      /* IS10184 */
                   p_ctsatso, exec_rc, errmsg)            /* IS10184 */
  if (exec_rc NE ESA_OK)   /* Emvironmental error ? */    /* IS10184 */
  {                                                       /* IS10184 */
    CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest,        /* IS10184 */
                  component, func, errmsg,                /* IS10184 */
                  exec_rc, __LINE__);                     /* IS10184 */
    goto exit;                                            /* IS10184 */
  };                                                      /* IS10184 */
  tso_r15 = (*p_ctsatso)(tso_chkenv,                      /* IS10184 */
                         &tso_rc, &tso_rs,                /* IS10184 */
                         &tso_ab, &tso_dbg);              /* IS10184 */

  ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
                   "CHECKENV CTSATSO rc=%d", tso_r15 );
  if (tso_r15 EQ 0) {

    /*
     *   Clear output file
     */

     /* IS10174 exec_rc = ESA_CLI_clr_output(ddn, dest, msgs) ;  */
     exec_rc = ESA_CLI_clr_output(ddn, admin_params);    /* IS10174 */
     if (exec_rc NE ESA_OK) {
        exec_rc = ESA_FATAL;
        goto exit;
     }

    /*
     *   Call dynamic services
     */

     tso_buff.len = strlen(p_cmd);
     strncpy(tso_buff.cmd, p_cmd, sizeof(tso_buff.cmd));
     /* SAS2IBMT
     tso_r15 = OS_DYNAM_call(TSO_SERVICES_MODULE, 1, tso_cmd, &tso_rc,*/
     /* IS10184
     tso_r15 = (*(ASM_RTN_TYP *)&OS_DYNAM_call)           /@ SAS2IBMT @/
                       (TSO_SERVICES_MODULE, 1, tso_cmd, &tso_rc,
                       &tso_rs, &tso_ab, &tso_dbg, &tso_buff);  */
     tso_r15 = (*p_ctsatso)(tso_cmd, &tso_rc,             /* IS10184 */
                            &tso_rs, &tso_ab, &tso_dbg,   /* IS10184 */
                            &tso_buff);                   /* IS10184 */
     ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
                      "CTSATSO r15=%d, rc=%d, rs=%d, ab=%d",
                      tso_r15, tso_rc, tso_rs, tso_ab  );

     switch (tso_r15) {
        case 0:
             *p_shell_rc = 0;
             exec_rc = ESA_OK;
             break;
        case 4:
             *p_shell_rc = tso_rc;
             exec_rc = ESA_OK;
             break;
        case 12:
             *p_shell_rc = tso_ab;
             exec_rc = ESA_OK;
             break;
        default:
             *p_shell_rc = 0;
             exec_rc = ESA_FATAL;
             break;
     }
     goto exit;

  } /* dynamic tso support */

 /*
  *   No dynamic tso services, TMP will be attached
  *   therefore, input is to be prepared in systsin
  */

 /*
  *  Ignore trailing blanks
  */

  for ( cleft = strlen(p_cmd);
      ( (cleft GT 0) AND (p_cmd[cleft-1] EQ BLANK) );
        cleft-- ) ;

  coff  = 0;
  if (cleft GT ZERO) {

     /* skip leading blanks */

     for ( ;
           ((cleft GT 0) AND (p_cmd[coff] EQ BLANK)) ;
           coff++ ) cleft--;

     ESA_DIAG_printf( PLT_COMP_OS_CLI, 1,
                      "i_start=%d,length=%d", coff,cleft);

     /* Prepare file */

     exec_rc = PutSYSTSIN( p_cmd, coff, cleft, dest, msgs );
     if ( exec_rc NE ESA_OK ) {
        exec_rc = ESA_FATAL;
        goto exit;
     }
  }
  else  {
    CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest, component, func,
                  "command buffer is empty", 16, __LINE__ );
    exec_rc = ESA_FATAL;
    goto exit;
  }

 /*
  *    Call shell
  */

  /* SAS2IBMT (*p_shell_rc) = ctsacli();                             */
  (*p_shell_rc) = (*(ASM_RTN_TYP *)&ctsacli)();           /* SAS2IBMT */
  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "ctsacli rc=%d", *p_shell_rc);

 /*
  *   Termination
  */

 exit: ;

 ESA_DIAG_exit(PLT_COMP_OS_CLI, 1, func, exec_rc );

 return exec_rc;

}

/**************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_get_output                         *
*                                                             *
* DESCRIPTION    : Obatin shell output from last command to   *
*                  a supplied buffer                          *
*                                                             *
* INPUT          : ddn              -  DDN of file            *
*                  p_buff           -  buffer address         *
*                  p_len            -  buffer length          *
*                  dest             -  destinations table     *
*                  msgs             -  messages buffer        *
*                                                             *
* OUTPUT         :                                            *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************/
ESA_RC ESA_CLI_get_output(char                       * ddn,
                          char                       * p_buff,
                          short                      * p_len,
        /* IS10174 */    ADMIN_PARAMS_rec_typ        * admin_params)
        /* IS10174       CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
        /* IS10174       CTSAMSG_HANDLE_rec_typ      * msgs )        */
{

  static char func[] = "ESA_CLI_get_output";

 /* Variables */

  int    cblnk;            /* ps0240 */
  ESA_RC output_rc ;
  short  coff = 0 ;
  short  clen;
  short  cleft;
  char   line[CLI_MAX_OUTPUT_ST];
  /* IS10174 char   file_name[80];                                  */
  FILE   *shell_output_file = NULL;
  char   ddn_with_dd[12] = "DD:";                        /* SAS2IBMN */
  char   ddname[9] = "";                                 /* IS10174  */

  CTSAMSG_HANDLE_rec_typ     * msgs;                     /* IS10174  */
  CTSAMSG_DEST_TABLE_rec_typ * dest;                     /* IS10174  */

 /*
  *   Initialization
  */

  ESA_DIAG_enter(PLT_COMP_OS_CLI, 1, func );

  msgs = admin_params->ctsamsg_handle;                   /* IS10174  */
  dest = admin_params->ctsamsg_dest;                     /* IS10174  */

  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "Buffer at %X length %d",
                  p_buff, *p_len );
  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "DDNAME = %s",     /* IS10174 */
                  ddn);                                  /* IS10174 */

 /* IS10174 - start */
 strcpy(ddname, ddn);
 GETDD(ddname, output_rc)

 if (output_rc NE ESA_OK)
   goto exit;

 ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "Retrieved ddname = %s",
                 ddname);

 if (strcmp(ddn, ddname) NE 0)
   strcpy(ddn, ddname);
 /* IS10174 - end   */

 /* SAS2IBMN - start *
 /* IS10174 - start */
 /* Remove the commands below, the ddname will always be w/o DD:
  if (strncmp(ddn, ddn_with_dd, 3) EQ 0)   * ddname contains dd: ? *
    strcpy(ddn_with_dd, ddn);              * yes - use it.         *
  else
 */
 /* IS10174 - end   */
 strcat(ddn_with_dd, ddn);                /* no- add DD: to ddname */
 /* SAS2IBMN - end   */

 /*
  *   Check ddname existance
  */
  /*SAS2IBMNoutput_rc=OS_MVS_ddinfo(ddn, file_name, TRUE, dest, msgs);*/
  /* IS10174
  output_rc = OS_MVS_ddinfo(ddn_with_dd, file_name, TRUE, * SAS2IBMN *
                            dest, msgs);               * SAS2IBMN *  */
  output_rc = OS_MVS_ddinfo(ddn, NULL,                   /* IS10174 */
                            TRUE, dest, msgs, 0);        /* IS10174 */
  if ( output_rc NE ESA_OK ) {
     *p_len = 0;
     goto exit;
  }

 /* SAS2IBMN - remove call to access
  *
  *   Check output file existance
  *

  if ( access(ddn, 0) NE 0) {
     *p_len = 0;
     output_rc = ESA_OK;
     goto exit;
  }
  */

 /*
  *   Open shell output file
  */

  /* SAS2IBMN shell_output_file = fopen( ddn, FILE_OPEN_READ_TEXT);  */
  shell_output_file = fopen( ddn_with_dd,                /* SAS2IBMN */
                             FILE_OPEN_READ_TEXT);       /* SAS2IBMN */
  if (shell_output_file EQ NULL) {
      CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                    "open","TSO services",
                    ddn, strerror(errno));
      output_rc = ESA_FATAL;
      goto exit;
  }


 /* SAS2IBMN - start */
 /* Verify that the file is not empty (as done by SAS/C access() ) */
  fgets(line, sizeof(line), shell_output_file);

  if ( feof(shell_output_file) )       /* check if file is empty */
  {
     *p_len = 0;
     output_rc = ESA_OK;
     goto exit;
  }
 /* SAS2IBMN - end */

 /*
  *   Copy shell output to buffer
  */

  cleft = *p_len - 1;  /* one character reserved for overflow */
  *p_len = 0;
  coff  = 0 ;

  /* SAS2IBMNN fgets(line, sizeof(line), shell_output_file);   */

  while (  (cleft GT 0) AND (feof(shell_output_file) EQ 0) ) {

     /* check i/o errors */

     if (ferror(shell_output_file)) {
        CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                      "get","TSO services",
                      ddn, strerror(errno) );
        output_rc = ESA_FATAL;
        goto exit;
     }

     /* copy output line (ignore the first ready prompt) */

     clen = strlen(line)+1;
     if (clen GT cleft)
        clen = cleft;
     strncpy(p_buff+coff, line, clen);
     cleft -= clen;
     coff += clen;

     ESA_DIAG_printf(PLT_COMP_OS_CLI, 3,
                        "Read %d chars, next %d left %d",
                        clen, coff, cleft );

     /* next output line */

     fgets(line, sizeof(line), shell_output_file);

  } /* while */

  /* if overflow occured - terminate the last string */

  if (cleft EQ 0)
     p_buff[coff] = NULL_CHAR;

  *p_len = coff;

  clen  = strlen(p_buff);                              /* ps0240 */
  cblnk = strspn(p_buff, " \n");                       /* ps0240 */
  ESA_DIAG_printf(PLT_COMP_OS_CLI, 3,                  /* ps0240 */
                  "coff=%d clen=%d cblnk=%d",          /* ps0240 */
                  coff, clen, cblnk);                  /* ps0240 */
  if ((coff  EQ clen+1) AND        /* One line only */ /* ps0240 */
      (cblnk EQ clen)     )        /* Empty line    */ /* ps0240 */
    *p_len = 0;                                        /* ps0240 */

  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1,
                  "Buffer filled with %d chars", *p_len );

  output_rc = ESA_OK;

 /*
  *   Termination
  */

  exit: ;

  if (shell_output_file NE NULL)
     fclose(shell_output_file);

  ESA_DIAG_exit(PLT_COMP_OS_CLI, 1, func, output_rc);

  return output_rc;

}

/*  WS10076  - ESA_CLI_fetch_output is no longer used - remove
/@ * * * * * * * WS10062 Start * * * * * * * @/
/@*************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_fetch_output                       *
*                                                             *
* DESCRIPTION    : Obtain shell output from last command in   *
*                  'byteseek' mode and return it to the       *
*                  caller in a supplied buffer.               *
*                                                             *
* INPUT          : ddn              -  DDN of file            *
*                  stream           -  file stream handle     *
*                  p_buff           -  buffer address         *
*                  p_len            -  buffer len requested   *
*                  admin_params                               *
*                                                             *
* OUTPUT (first  : blksize          -  blksize         WS10067*
* call)            lrecl            -  lrecl           WS10067*
*                                                             *
* OUTPUT (after  : p_buff           -  filled buffer          *
* first call)      p_len            -  buffer len returned    *
*                                                             *
* RETURN VALUE   : ESA_OK                                     *
*                  ESA_EOF                                    *
*                  ESA_FATAL                                  *
*                                                             *
* With WS10067, this routine is called for the first time in  *
* order to open the output file and to return the file's      *
* blksize and lrecl. On each subsequent call, it returns the  *
* next block and length.                                      *
*                                                             *
*************************************************************@/
ESA_RC ESA_CLI_fetch_output(char                       * ddn,
                            FILE                      ** stream,
                            char                       * p_buff,
                            short                      * p_len,
                            long               * blksize, /@ WS10067 @/
                            long               * lrecl,   /@ WS10067 @/
         /@ IS10174 @/      ADMIN_PARAMS_rec_typ        * admin_params)
         /@ IS10174         CTSAMSG_DEST_TABLE_rec_ptr   dest,       @/
         /@ IS10174         CTSAMSG_HANDLE_rec_ptr       msgs );     @/
{

  static char func[] = "ESA_CLI_fetch_output";

#define READ_BS  "rb,abend=recover,byteseek,recfm=*"

 /@ Variables @/

  ESA_RC output_rc ;
  short  clen;
  char   file_name[80];
  char   sample[80];
  FILE   * file          = NULL;
  char   ddn_with_dd[12] = "DD:";
  fldata_t info;                                          /@ WS10067 @/
  char   ddname[9] = "";                                 /@ IS10174  @/
  CTSAMSG_HANDLE_rec_typ     * msgs;                     /@ IS10174  @/
  CTSAMSG_DEST_TABLE_rec_typ * dest;                     /@ IS10174  @/

 /@
  *   Routine entry messages
  @/

  ESA_DIAG_enter(PLT_COMP_OS_CLI, 1, func );

  msgs = admin_params->ctsamsg_handle;                   /@ IS10174  @/
  dest = admin_params->ctsamsg_dest;                     /@ IS10174  @/

  /@ WS10067
  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1,
                  "Input: buffer at %X,  buffer length = %d",
                  p_buff, *p_len );                                  @/

 /@
  *   Initialization (if needed)
  @/

  if (*stream EQ NULL) {
     ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,
                      "stream is null - about to do open");
     /@ IS10174 - start @/
     strcpy(ddname, ddn);
     GETDD(ddname, output_rc)

     if (output_rc NE ESA_OK)
       goto exit;

     ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "Retrieved ddname = %s",
                     ddname);

     if (strcmp(ddn, ddname) NE 0)
       strcpy(ddn, ddname);
     /@ IS10174 - end   @/


     /@ IS10174 strcpy(ddn_with_dd, ddn);                           @/
     strcat(ddn_with_dd, ddn);                          /@ IS10174  @/

    /@
     *   Check ddname existence
     @/
     /@ IS10174
     output_rc = OS_MVS_ddinfo(ddn_with_dd, file_name, TRUE, dest,msgs);
     @/
     output_rc = OS_MVS_ddinfo(ddn, NULL,                /@ IS10174 @/
                               TRUE, dest, msgs, 0);     /@ IS10174 @/
     if (output_rc NE ESA_OK) {
        ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,
                         "file %s does not exist prog=%s line=%d",
                         ddn, __FILE__, __LINE__);       /@ IS10174 @/
             /@ IS10174  ddn_with_dd, __FILE__, __LINE__);    @/
        output_rc = ESA_FATAL;
        goto exit;
     }

    /@
     *   Open shell output file and prepare for the first read
     @/

     file = fopen(ddn_with_dd, READ_BS);
     if (file EQ NULL) {
         CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                       "fopen","TSO services",
                       ddn, strerror(errno));
         ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,
                          "fopen() failed prog=%s line=%d",
                          __FILE__, __LINE__);
         output_rc = ESA_FATAL;
         goto exit;
     }
     *stream = file;
     ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,
                      "fopen() worked fine");

     output_rc = fseek(file, 0L, SEEK_SET);
     if (output_rc NE ESA_OK) {
        ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,
                        "fseek to start of file failed prog=%s line=%d",
                        __FILE__, __LINE__);
        output_rc = ESA_FATAL;
        goto exit;
     }
     ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,
                      "fseek() worked fine");

    /@
     *   Return blksize and lrecl to the caller              WS10067
     @/

     output_rc = fldata(file, file_name, &info);          /@ WS10067 @/
     if (output_rc NE ESA_OK) {                           /@ WS10067 @/
        ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,              /@ WS10067 @/
                        "fldata failed prog=%s line=%d",  /@ WS10067 @/
                        __FILE__, __LINE__);              /@ WS10067 @/
        output_rc = ESA_FATAL;                            /@ WS10067 @/
        goto exit;                                        /@ WS10067 @/
     }                                                    /@ WS10067 @/
     else {                                               /@ WS10067 @/
        *blksize = info.__blksize;                        /@ WS10067 @/
        *lrecl   = info.__maxreclen;                      /@ WS10067 @/
        ESA_DIAG_printf(PLT_COMP_OS_CLI, 6,               /@ WS10067 @/
                        "fldata blksize=%lu lrecl=%lu",   /@ WS10067 @/
                        *blksize, *lrecl);                /@ WS10067 @/
        goto exit;                                        /@ WS10067 @/
     }                                                    /@ WS10067 @/
  }
  else
     file = *stream;

 /@
  *   Read first/next block of shell output file
  @/

  memset(p_buff,0X00,*p_len);             /@ clear the entire buffer @/

  clen = fread(p_buff, 1, *p_len, file);  /@ read the next buffer in @/
  ESA_DIAG_printf(PLT_COMP_OS_CLI, 6,
                  "fread obtained %d characters", clen);
  if (clen EQ *p_len) {
     ESA_DIAG_printf(PLT_COMP_OS_CLI, 6,
                     "clen = *p_len = %d characters", clen);
     memset(sample,0X00,sizeof(sample));
     strncpy(sample,p_buff,75);     /@ copy first 75 bytes of buffer @/
     ESA_DIAG_printf(PLT_COMP_OS_CLI, 6,
                     "first 75 chars of buffer = %s", sample);
     output_rc = ESA_OK;
     goto exit;
  }
  else if (clen LT *p_len) { /@ We got the last (partial) block and   @/
     if (feof(file)) {       /@ the feof indicator.  This means that  @/
        fclose(file);        /@ the caller must process the last      @/
        *p_len = clen;       /@ (partial) block and should not return @/
        output_rc = ESA_EOF; /@ because the file will already be      @/
        goto exit;           /@ closed.                               @/
     }
     else if (ferror(file)) {
        CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                      "fread","TSO services",
                      ddn, strerror(errno));
        ESA_DIAG_printf (PLT_COMP_OS_CLI, 6,
                         "fread() failed prog=%s line=%d",
                         __FILE__, __LINE__);
        fclose(file);
        output_rc = ESA_FATAL;
        goto exit;
     }
     else {
        ESA_DIAG_printf(PLT_COMP_OS_CLI, 6,
                        "clen (%d) LT *p_len (%d)", clen, *p_len);
        *p_len = clen;
        ESA_DIAG_printf(PLT_COMP_OS_CLI, 6,
                        "Warning: clen LT *p_len but not EOF");
       CTSAMSG_print(ERR_3_STRINGS, msgs, NULL, dest,
       "Warning:clen LT *p_len but not EOF.Processing continues","","");
       output_rc = ESA_OK;
       goto exit;
     }
  }

 /@
  *   Termination
  @/

  exit: ;
  ESA_DIAG_exit(PLT_COMP_OS_CLI, 1, func, output_rc);

  return output_rc;

}
/@ * * * * * * * WS10062 End * * * * * * * @/
    end of removed code     WS10076 */

/**************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_clr_output                         *
*                                                             *
* DESCRIPTION    : Clear shell output file                    *
*                                                             *
* INPUT          : ddn              -  DDN of file            *
*                  dest             -  destinations table     *
*                  msgs             -  messages buffer        *
*                                                             *
* OUTPUT         :                                            *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************/
ESA_RC ESA_CLI_clr_output(char                        *ddn,
        /* IS10174 */     ADMIN_PARAMS_rec_typ        * admin_params)
        /* IS10174        CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
        /* IS10174        CTSAMSG_HANDLE_rec_typ      * msgs )        */
{

  static char func[] = "ESA_CLI_clr_output";
  /* IS10174 char   file_name[80];                                   */
  char   ddn_with_dd[12] = "DD:";                        /* SAS2IBMN */
  char   ddname[9] = "";                                 /* IS10174  */

  ESA_RC rc = ESA_OK ;
  FILE   *shell_output_file = NULL;

  CTSAMSG_HANDLE_rec_typ     * msgs;                     /* IS10174  */
  CTSAMSG_DEST_TABLE_rec_typ * dest;                     /* IS10174  */

 /*
  *   Initialization
  */

  ESA_DIAG_enter(PLT_COMP_OS_CLI, 1, func );

  msgs = admin_params->ctsamsg_handle;                   /* IS10174  */
  dest = admin_params->ctsamsg_dest;                     /* IS10174  */

  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "DDNAME = %s",     /* IS10174 */
                  ddn);                                  /* IS10174 */

 /* IS10174 - start */
 strcpy(ddname, ddn);
 GETDD(ddname, rc)

 if (rc NE ESA_OK)
   goto exit;

 ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "Retrieved ddname = %s",
                 ddname);

 if (strcmp(ddn, ddname) NE 0)
   strcpy(ddn, ddname);
 /* IS10174 - end   */
 /* SAS2IBMN - start */
 /* IS10174 - start */
 /* Remove the commands below, the ddname will always be w/o DD:
 if (strncmp(ddn, ddn_with_dd, 3) EQ 0)   * ddname contains dd: ? *
   strcpy(ddn_with_dd, ddn);              * yes - use it.         *
 else
 */
 /* IS10174 - end   */
 strcat(ddn_with_dd, ddn);                /* no- add DD: to ddname */
 /* SAS2IBMN - end   */
 /*
  *   Check ddname existance
  */
  /* SAS2IBMN rc = OS_MVS_ddinfo(ddn, file_name, FALSE, dest, msgs); */
  /* IS10174
  rc = OS_MVS_ddinfo(ddn_with_dd, file_name, FALSE,   * SAS2IBMN *
                     dest, msgs);                     * SAS2IBMN *   */
  rc = OS_MVS_ddinfo(ddn, NULL, FALSE,                   /* IS10174 */
                     dest, msgs, 0);                     /* IS10174 */
  if ( rc NE ESA_OK ) {
     rc = ESA_OK;
     goto exit;
  }

 /*
  *   Open shell output file
  */

  /* SAS2IBMN shell_output_file = fopen( ddn, FILE_OPEN_WRITE_TEXT); */
  shell_output_file = fopen( ddn_with_dd,                /* SAS2IBMN */
                             FILE_OPEN_WRITE_TEXT);      /* SAS2IBMN */
  if (shell_output_file EQ NULL) {
      CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                    "open","TSO services",
                    ddn, strerror(errno));
      rc = ESA_FATAL;
      goto exit;
  }

 /* IS10184 - remove the code below.
              opening a file for output and closing it
              empties the file. there is n oneed to write
              an empty line.
 /@
  *   write empty line
  @/

  fputs( " ",  shell_output_file);
  if (ferror(shell_output_file)) {
     CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                   "put","TSO services",
                    ddn, strerror(errno) );
      rc = ESA_FATAL;
      goto exit;
  }
    end of removed code - IS10184 */

 /*
  *   Finish
  */

  exit: ;

  if (shell_output_file NE NULL)
     fclose(shell_output_file);

  ESA_DIAG_exit(PLT_COMP_OS_CLI, 1, func, rc);

  return rc;

}

/**************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_delete_output                      *
*                                                             *
* DESCRIPTION    : Delete/Clear shell output file             *
*                                                             *
* INPUT          : ddn              -  DDN of file            *
*                  dest             -  destinations table     *
*                  msgs             -  messages buffer        *
*                                                             *
* OUTPUT         :                                            *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************/
ESA_RC ESA_CLI_delete_output(char                        *ddn,
        /* IS10174 */    ADMIN_PARAMS_rec_typ        * admin_params)
        /* IS10174       CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
        /* IS10174       CTSAMSG_HANDLE_rec_typ      * msgs )        */
{

  static char func[] = "ESA_CLI_delete_output";
  /* SAS2IBMN char file_name[80] = "";  */
  /* SAS2IBMN char dsn[80];             */
  FILE  *fptr;                                             /* SAS2IBMN*/
  int    rem_rc;
  char   err_msg[80];
  char   ddn_with_dd[12] = "DD:";                        /* SAS2IBMN */
  ESA_RC rc = ESA_OK ;
  char   ddname[9];                                      /* IS10174 */
  int    is_execout = FALSE;                             /* IS10174 */

  CTSAMSG_HANDLE_rec_typ     * msgs;                     /* IS10174 */
  CTSAMSG_DEST_TABLE_rec_typ * dest;                     /* IS10174 */

 /*
  *   Initialization
  */

  ESA_DIAG_enter(PLT_COMP_OS_CLI, 1, func );

  msgs = admin_params->ctsamsg_handle;                   /* IS10174 */
  dest = admin_params->ctsamsg_dest;                     /* IS10174 */

  ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "DDNAME = %s",     /* IS10174 */
                  ddn);                                  /* IS10174 */

 /*
  * IS10174 - Remove the commands below, will be done later.
  * SAS2IBMN - start *
 if (strncmp(ddn, ddn_with_dd, 3) EQ 0)   * ddname contains dd: ? *
   strcpy(ddn_with_dd, ddn);              * yes - use it.         *
 else
 strcat(ddn_with_dd, ddn);                * no- add DD: to ddname *
  * SAS2IBMN - end   *
   end of IS10174 */

 /*
  *   Delete file if DDN != EXECOUT_DDNAME
  *   "Make empty" in another case
  */

  /* SAS2IBMN if ( strcmp( ddn, EXECOUT_DDNAME ) EQ 0 )   */
  /* IS10174 - remove the command below - will be done later
  if ( strcmp( ddn_with_dd, EXECOUT_DDNAME ) EQ 0 )       * SAS2IBMN *
    end of IS10174  */
   /* SAS2IBMN - start */
   /* The code below is replaced because:
    * 1. Unike SAS/C, in IBM C remove(ddn) deletes the file.
    * 2. To clear the file,we need to fopen() and fclose() it.
    rem_rc = remove( ddn );
  else {
    *
    *   Check ddname existance
    *
    rc = OS_MVS_ddinfo(ddn, file_name, FALSE, dest, msgs);
    if ( rc NE ESA_OK ) {
       rc = ESA_OK;
       goto exit;
    }

    *
    *  Build dsn style of file name
    *

    strcpy( dsn, "dsn:" );
    strcat( dsn, file_name);

    rem_rc = remove( dsn );  * file must exist and must be closed *
  }
  */
  /* IS10174 - start */
  /*
   *   Determine if this is execout
   */
  if ( (strcmp(ddn, EXECOUT_DDNAME) EQ 0) )
  {    /* if EXECOUT dummay name - get real name */
    strcpy(ddname, ddn);
    GETDD(ddname, rc)

    if (rc NE ESA_OK)
      goto exit;

    ESA_DIAG_printf(PLT_COMP_OS_CLI, 1, "Retrieved ddname = %s",
                 ddn);

    is_execout = TRUE;
  }
  else
    if ( (strcmp(ddn, EXECOUT_DDNAME_STATIC) EQ 0) OR
         (strncmp(ddn, EXECOUT_DDNAME_PREFIX,
                     strlen(EXECOUT_DDNAME_PREFIX)) EQ 0) )
    {
      strcpy(ddname, ddn);
      is_execout = TRUE;
    }

  /*
   *   If execout, clear the file. All other, delete.
   */
  if (is_execout)
  {
    strcat(ddn_with_dd, ddname);
    /* IS10174 - end   */
    /* SAS2IBMN fptr = fopen(ddn, FILE_OPEN_WRITE_TEXT);  */

    fptr = fopen(ddn_with_dd, FILE_OPEN_WRITE_TEXT);     /* SAS2IBMN */
    if (fptr EQ NULL)
    {
      CTSAMSG_print(ERR_FILE, msgs, NULL, dest,
                    "open","Command Line Services (CLI)",
                    ddn, strerror(errno));
      rc = ESA_FATAL ;
    }
    else
      fclose(fptr);
  }
  else   /* else, delete the file */
  {
    /* SAS2IBMN rem_rc = remove( ddn );  */
    strcat(ddn_with_dd, ddn);                            /* IS10174 */
    rem_rc = remove( ddn_with_dd );                      /* SAS2IBMN */

    if ( rem_rc LT 0 )
    {
      /* SAS2IBMN sprintf( err_msg, "remove() failed. ddn/dsn=%s/%s",
               ddn, file_name); */
      sprintf( err_msg, "remove() failed. ddn=%s", ddn); /*SAS2IBMN*/

      CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest, component, func,
                    err_msg , 16, __LINE__ );
      rc = ESA_FATAL ;
    }
  }
  /* SAS2IBMN - end */

 /*
  *   Finish
  */

  exit: ;

  ESA_DIAG_exit(PLT_COMP_OS_CLI, 1, func, rc);

  return rc;

}


/* BS10111 - the routines below are removed because they are
             not used any more.
/@ * * * * * * * WS10076 Start * * * * * * * @/

/@********************************************************************
 * Procedure Name: ESA_CLI_get_RUOB_line
 *********************************************************************
 * Description   : Get REXX / Utility Output Block line routine gets
 *                 the data from REXX or from utility.
 *                 The data is saved in a structure mapped in C & ASM:
 *                 RUOHNEXT -> next block
 *                 RUOHLEN  -  block total length
 *                 RUOHDLEN -  block actual data length
 *                 RUODATA  -  where data starts in the block
 *                 RUOLLEN  -  line data length
 *                 RUOLDATA -  where line data starts
 *                 The above structure is for one block the address of
 *                 next block is saved in ruohnext.
 *
 * Input         : function (GETFIRST / GETNEXT / TERM) (I)
 *                 Input descriptor block (I)
 *                 handle (I/O)
 *                 get line buffer length (I)
 *
 * Output        : handle (I/O)
 *                 get line buffer (O)
 *                 get line size (O)
 *
 * Return Value  : OK - Return OK with line populated in buffer
 *                 EOF - line is blank with return EOF to represent
 *                       end of buffer
 *                 ERR - Line is blank and error is returned if
 *                       there are any errors
 *******************************************************************@/
ESA_RC ESA_CLI_get_RUOB_line(
                 char                     * function,
                 void                     * p_inp_desc_blk,
                 void                    ** handle,
                 char                     * get_line_buffer,
                 int                        get_line_buffer_size,
                 ADMIN_PARAMS_rec_typ     * admin_params)
{
   static char                  func[] = "ESA_CLI_get_RUOB_line";
   ESA_RC                         rc = ESA_OK;
   IDB_GETRUOBLINE_rec_typ      * inp_desc_blk;
   REXX@UTL_output_blk_rec_typ  * data_buff = NULL;
   REXX@UTL_output_line_rec_typ * line;
   Get_RUOBline_handle_rec_typ  * this_handle = NULL;
   int                            len = 0;
   char                           ruobeyec[5] = "RUOB";  /@ WS10076A @/
   char                           errmsg[80] = "";       /@ WS10076n @/

   CTSAMSG_HANDLE_rec_typ       * msgs;
   CTSAMSG_DEST_TABLE_rec_typ   * dest;

   ESA_DIAG_enter(PLT_COMP_PERMISSION, 2, func);
   msgs = admin_params->ctsamsg_handle;
   dest = admin_params->ctsamsg_dest;

   inp_desc_blk = (IDB_GETRUOBLINE_rec_typ *)p_inp_desc_blk;

   ESA_DIAG_printf (PLT_COMP_PERMISSION, 9,
          "function-%s,inp_desc_blk-%p,*handle-%p",
           function, inp_desc_blk, *handle);

   if ( strcmp(function,"TERM") EQ 0 )    /@ TERM @/
   {
     if (*handle NE NULL)
     {                                                   /@ WS10076N @/
       free(*handle);
       *handle = NULL;                                   /@ WS10076N @/
     }                                                   /@ WS10076N @/
     rc = ESA_OK;
     ESA_DIAG_printf (PLT_COMP_PERMISSION, 10,
                      "free handle in TERM function");
     goto exit;
   }
   /@ If handle is NULL then we start reading from first record
      irrespective of function is GETFIRST or GETNEXT             @/
   if ( *handle EQ NULL )
   {
     if (inp_desc_blk->blk EQ NULL)
     {
       rc=ESA_FATAL;
       CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest, component,
                     func,"No data block",
                     rc, __LINE__ );
       goto exit;
     }

     *handle = (void *) calloc(sizeof(Get_RUOBline_handle_rec_typ), 1);
     ESA_DIAG_printf (PLT_COMP_PERMISSION, 10,
                      "Handle (size - %i) %p allocated",
                       sizeof(Get_RUOBline_handle_rec_typ), *handle);

     if( *handle EQ NULL )
     {
       rc=ESA_FATAL;
       CTSAMSG_print(ERR_MALLOC, msgs, NULL, dest,
                      "ESA_CLI_get_RUOB_line handle",
                       sizeof(Get_RUOBline_handle_rec_typ));
       goto exit;
     }

     this_handle = (Get_RUOBline_handle_rec_typ *) (*handle);
     this_handle->block = inp_desc_blk->blk;
     this_handle->len = 0;
     this_handle->iseof = 0;
   }
   else
   {
     /@ If handle is not NULL then check if function is GETFIRST
        For GETFIRST start from first record                     @/
     this_handle = (Get_RUOBline_handle_rec_typ *) (*handle);
     ESA_DIAG_printf (PLT_COMP_PERMISSION, 6,            /@ WS10078S @/
                   "this_handle = %p", this_handle);     /@ WS10078S @/
     if ( strcmp(function,"GETFIRST") EQ 0)
     {
        this_handle->block = inp_desc_blk->blk;
        this_handle->len = 0;
        this_handle->iseof = 0;
     }
     /@ WS10076N - start @/
     else
       if ( strcmp(function,"GETNEXT") NE 0)
       {
         rc = ESA_FATAL;
         sprintf(errmsg, "Invalid function received %s", function);
         CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest, component,
                       func, errmsg, rc, __LINE__ );
         goto exit;
       }
     /@ WS10076N - end   @/
   }

   data_buff = (REXX@UTL_output_blk_rec_typ *)this_handle->block;
   len = this_handle->len;
   ESA_DIAG_printf (PLT_COMP_PERMISSION, 10,
                   "first_buf = %p   curr_buf = %p  read_len = %d",
                   inp_desc_blk->blk, data_buff, len);

   /@  If EOF already reached, just return EOF rc           @/
   if (this_handle->iseof EQ 1)
   {
     rc = ESA_EOF;
     ESA_DIAG_printf (PLT_COMP_PERMISSION, 6,
                      "Read request after EOF.");
     goto exit;
   }

   /@  Is it real RUOB ?  WS10076A   @/
   if (strncmp(data_buff->ruoheyec,ruobeyec,4) NE 0)
   {
        /@ No RUOB eye catcher - wrong data @/
        rc = ESA_ERR;
        ESA_DIAG_printf (PLT_COMP_PERMISSION, 6,
                         "No RUOB eye catcher. Data=<%.40s>",
                         data_buff);
        goto exit;
     }

   /@  Are we at the end of the current block ?             @/
   if (data_buff->ruohdlen LE len)
   {
     data_buff = (REXX@UTL_output_blk_rec_typ *) data_buff->ruohnext;
     if (data_buff EQ NULL)
     {
        /@ No more data. Set EOF to 1 and return @/
        this_handle->iseof = 1;
        rc = ESA_EOF;
        ESA_DIAG_printf (PLT_COMP_PERMISSION, 6,
                         "No more data in buffer.");
        goto exit;
     }
     else
     {
       this_handle->block = data_buff;  /@ Keep current block @/
       len = 0;                  /@ Start from the beginning  @/
     }
   }

   /@  set start point   @/
   line = (REXX@UTL_output_line_rec_typ *)(&data_buff->ruodata + len);

   if (get_line_buffer_size LT line->ruollen)
   {
     rc=ESA_FATAL;
     CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest, component,
                   func,"Buffer for rule line is too short",
                   rc, __LINE__ );
     goto exit;
   }

   /@ Copy the line into buffer and update handle                 @/
   memcpy(get_line_buffer, &line->ruoldata, line->ruollen);
 /@*(get_line_buffer + line->ruollen + 1) = NULL_CHAR; WS10078KG *
    * WS10087KG - There is extra character at the end hence
      NULL_CHAR should be set before garbage character.       @/
   *(get_line_buffer + line->ruollen ) = NULL_CHAR;    /@ WS10078KG @/

   ESA_DIAG_printf (PLT_COMP_PERMISSION, 16,
                    "line read: (%d) %s",
                    line->ruollen, get_line_buffer);

   /@ Point line to next record @/
   this_handle->len = len + line->ruollen + 1;

   ESA_DIAG_printf (PLT_COMP_PERMISSION, 16,
                    "len after read : %d", this_handle->len);

   exit:;

      ESA_DIAG_exit(PLT_COMP_PERMISSION, 2, func, rc);
      return rc;
}
/@ * * * * * * * WS10076 End * * * * * * * @/
/@ * * * * * * * WS10076A Start * * * * * * * @/
/@********************************************************************
 * Procedure Name: ESA_CLI_del_RUOB
 *********************************************************************
 * Description   : Delete REXX / Utility Output Block deletes all
 *                 blocks. The only parameter is the address of the
 *                 first block to be deleted.
 *                 Structure includes eye catacher and address of
 *                 next block.
 *                 We do the following flow in a loop:
 *                 - Continue as long as block address is not NULL
 *                 - Verify eye catcher is correct
 *                 - Take next block address
 *                 - Free current block
 *                 RUOHEYEC =  "RUOB" - eye catcher
 *                 RUOHNEXT -> next block
 *                 RUOHLEN  -  block total length
 *                 RUOHDLEN -  block actual data length
 *                 RUODATA  -  where data starts in the block
 *                 RUOLLEN  -  line data length
 *                 RUOLDATA -  where line data starts
 *                 The above structure is for one block the address of
 *                 next block is saved in ruohnext.
 *
 * Input         : First buffer address (I)
 *                 admin_params
 *
 * Return Value  : OK - Return OK when all elements were free
 *                 ERR - At least one of the buffers was not free
 *******************************************************************@/
ESA_RC ESA_CLI_del_RUOB(
                 REXX@UTL_output_blk_rec_typ  * ruoh,
                 ADMIN_PARAMS_rec_typ         * admin_params)
{
   static char                    func[] = "ESA_CLI_del_RUOB";
   static ASM_RTN_TYP           * p_ctsasto = NULL;
   ESA_RC                         rc = ESA_OK;
   int                            int_rc = 0;
   int                            sto_sp = 0;
   int                            sto_key = 0;
   int                            sto_loc = 0;
   int                            sto_msg[257];
   int                            sto_msg_size;
   int                            dbglvl = 0;
   char                           ruobeyec[5] = "RUOB";
   REXX@UTL_output_blk_rec_ptr    ruohnext,ruob;
   char   err_msg[80];
   CTSAMSG_HANDLE_rec_typ       * msgs;
   CTSAMSG_DEST_TABLE_rec_typ   * dest;

   ESA_DIAG_enter(PLT_COMP_PERMISSION, 2, func);
   msgs = admin_params->ctsamsg_handle;
   dest = admin_params->ctsamsg_dest;

   sto_msg_size = sizeof(sto_msg);

   ESA_DIAG_printf (PLT_COMP_PERMISSION, 6,
                    "input ruob = %p -> %4s",
                    ruoh,
                    ruoh EQ NULL? "" : ruoh->ruoheyec);

   if (p_ctsasto EQ NULL)
   {
      int_rc = (*(ASM_RTN_TYP *)&ctsaldm)            /@ load ctsasto @/
                ("CTSASTO ", "       ", &p_ctsasto);
      if (int_rc NE 0)
      {
         /@ display error message CTS4020E @/
         CTSAMSG_print(ERR_API_LOAD, msgs, NULL, dest,
                       admin_params->rss_name, "CTSASTO");
         rc = ESA_FATAL;
         goto exit;
      };

      ESA_DIAG_printf(PLT_COMP_PERMISSION, 6,
                      "p_CTSASTO = %p", p_ctsasto);
   }

   dbglvl = (int)ESA_DIAG_get_debug_level(PLT_COMP_PERMISSION);
   if (dbglvl GT 0)
     dbglvl = 1;

   ruob = ruoh;
   while (ruob NE NULL) {
    if(strncmp(ruob->ruoheyec,ruobeyec,4) NE 0) {
     ESA_DIAG_printf (PLT_COMP_PERMISSION, 6,
       "Wrong eye-catcher in RUOB: <%s> buffer after=<%40x>",
        ruob->ruoheyec,ruob->ruohnext);
     rc=ESA_FATAL;
     sprintf(err_msg,
"Bad eye-catcher in RUOB:<%4x> buffer=<%40x> func=%s comp=%s stmt=%d",
      ruob->ruoheyec, ruob->ruohnext, func, component, __LINE__);
     CTSAMSG_print(ERR_TL_STRING, msgs, NULL, dest, err_msg);
     goto exit;
    }
    ruohnext = ruob->ruohnext;        /@ keep next block address @/
    sto_sp = ruob->ruohsp;      /@  They are unsigned char...  @/
    sto_key = ruob->ruohkey;    /@  ...but we need them...     @/
    sto_loc = ruob->ruohloc;    /@  ...as int.                 @/
    sto_msg[0] = NULL_CHAR;
     ESA_DIAG_printf (PLT_COMP_PERMISSION, 6,
       "Go free: %p,  %d, %d, %d, %d",
        ruob, ruob->ruohlen, sto_sp, sto_key, sto_loc);
    int_rc = (*p_ctsasto)("RELEASE",
                          &(ruob->ruohlen),
                          &ruob,
                          &sto_sp,
                          &sto_key,
                          &sto_loc,
                          &sto_msg,
                          &sto_msg_size,
                          &dbglvl);
    if (int_rc NE 0)
    {
       CTSAMSG_print(ERR_INTERNAL2, msgs, NULL, dest, component,
                     func, sto_msg, int_rc, __LINE__ );
       rc=ESA_FATAL;
       goto exit;
    }
    ruob = ruohnext;                  /@ set next as current @/
   }

   exit:;
   ESA_DIAG_exit(PLT_COMP_PERMISSION, 2, func, rc);
   return rc;
}
/@ * * * * * * * WS10076 End * * * * * * * @/
    end of removed code  - BS10111   */
