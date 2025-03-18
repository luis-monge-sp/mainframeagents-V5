/**************************************************************
*  No corresponding CTSCMBX in CTS.CSRC                       *
**************************************************************/
/**************************************************************
*                                                             *
* Title            : Mailbox services                         *
*                                                             *
* File Name        : ctsombx.c                                *
*                                                             *
* Author           : Alexander Shvartsman                     *
*                                                             *
* Creation Date    : 25/10/94                                 *
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
* dc2105   Doron    21/5/95  Support rc=54 from mbxwrit       *
*                            (gateway shutdown).              *
* SAS2IBMN NuritY   05/10/16 SAS/C to IBM C Conversion.       *
*    "       "      23/01/17 Replcae sleep with ctsaslp.      *
* SAS2IBMT SeligT   01/11/16 SAS/C to IBM C Conversion.       *
* SAS2IBMA AvnerL   07/12/16 Set CC includes per IS0060.      *
**************************************************************/

#include   <globs.h>

/* SAS2IBMN #include  LCLIB   */

#include   ESA_OS_MBX
#include   ESA_DIAG
#include   ESA_INIT

/* #include   MVS_COMP    - IS0060 */

#include   MVS_C_COMP      /* IS0060  */

#define    MBXATTC_FUNC "MBXATTC "
#define    MBXDETC_FUNC "MBXDETC "
#define    MBXCRE_FUNC  "MBXCRE  "
#define    MBXDELT_FUNC "MBXDELT "
#define    MBXGETS_FUNC "MBXGETS "
#define    MBXREAD_FUNC "MBXREAD "
#define    MBXWRIT_FUNC "MBXWRIT "

/*
 *   Mailbox assembler implementation routine
 */

 /* SAS2IBMT prototype changed for IBM C
 int   ctsambx( char    * opcode , ... );                            */
 int   ctsambx();                                         /* SAS2IBMT */
 int   ctsaslp();                                         /* SAS2IBMN */

/**********************************************************
* PROCEDURE NAME : ESA_MBX_attach
* DESCRIPTION    : This function assigns to the exist
*                  mailbox specified by
*                  mailbox name.
* INPUT          : mail_name
* OUTPUT         : handle
* RETURN VALUE   : ESA_RC ( ESA_OK, ESA_ERR, ESA_MBX_NOT_EXIST )
************************************************************/

ESA_RC ESA_MBX_attach( MBX_QUE_NAME_D_typ    mail_name,
                       MBX_QUE_HDL_D_typ   * handle  )
{

 /*
  *   Variables
  */

  static char func[]="ESA_MBX_attach";
  ESA_RC rc;
  int    r15;
  int    debug_level = 0;

 /*
  *   Initialize
  */

  ESA_DIAG_enter(ESA_COMP_OS_MBX_ATT  , 1, func);

  ESA_DIAG_printf(ESA_COMP_OS_MBX_ATT ,1,
                  "Attaching mailbox %s", mail_name );

 /*
  *   Call assembelr routine and process return code
  */

  if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_MBX_ATT ) GE 3)
     debug_level = 1;

  /* SAS2IBMT
  r15 = ctsambx( MBXATTC_FUNC, mail_name, handle, &debug_level);     */
  r15 = (*(ASM_RTN_TYP *)&ctsambx)                        /* SAS2IBMT */
               ( MBXATTC_FUNC, mail_name, handle, &debug_level);
  ESA_DIAG_printf(ESA_COMP_OS_MBX_ATT ,1, "mbxattc rc=%d", r15);

  switch(r15) {
    case   1  : rc = ESA_OK;
                break;
    case   2  : rc = ESA_ERR;
                break;
    case  29  : rc = ESA_MBX_NOT_EXIST;
                break;
    case  33  : rc = ESA_MBX_BUFFEROVF;
                break;
    case  36  : rc = ESA_MBX_ALREADY_EXIST;
                break;
    default   : rc = ESA_FATAL;
                ESA_DIAG_printf(ESA_COMP_OS_MBX_ATT ,0,
                   "Undefined rc=%d. Assumed ESA_FATAL", r15);
  }

 /*
  *   Finish
  */

  ESA_DIAG_exit( ESA_COMP_OS_MBX_ATT , 1, func, rc);
  return rc;

}

/***********************************************************
* PROCEDURE NAME : ESA_MBX_detach
* DESCRIPTION    : This function dassigns the mailbox spec. by
*                  mailbox handle .
* INPUT          : handle
* OUTPUT         : none
* RETURN VALUE   : ESA_RC ( ESA_OK, ESA_ERR )
**************************************************************/

ESA_RC ESA_MBX_detach( MBX_QUE_HDL_D_typ handle ) {

 /*
  *   Variables
  */

  static char func[]="ESA_MBX_detach";
  ESA_RC rc;
  int    r15;

 /*
  *   Initialize
  */

  ESA_DIAG_enter(ESA_COMP_OS_MBX_GNRL , 1, func);

 /*
  *   Call assembelr routine and process return code
  */

  /* SAS2IBMT
  r15 = ctsambx( MBXDETC_FUNC, handle);                              */
  r15 = (*(ASM_RTN_TYP *)&ctsambx)                        /* SAS2IBMT */
               ( MBXDETC_FUNC, (void *)handle);
  ESA_DIAG_printf(ESA_COMP_OS_MBX_GNRL,1, "mbxdetc rc=%d", r15);

  switch(r15) {
     case 1 : rc = ESA_OK;
                 break;
     default: rc = ESA_FATAL;
              ESA_DIAG_printf(ESA_COMP_OS_MBX_GNRL, 0,
                    "Undefined rc=%d. Assumed ESA_FATAL", r15);
  }

 /*
  *   Finish
  */

  ESA_DIAG_exit(ESA_COMP_OS_MBX_GNRL, 1, func, rc);
  return rc;

}

/***********************************************************
* PROCEDURE NAME : ESA_MBX_create
* DESCRIPTION    : This function creates a read/write
*                  mailbox messages. The mailbox can be create as
*                  unique mailbox in the system (parameter CONF_FLG =
*                  ESA_MBX_NEW). In this case checks if the mailbox is
*                  exist. If it exist forces a name generator to search
*                  unique mailbox name.  (in this case parameter
*                  REALLY_NAME returns new maibox name).  If parametr
*                  CONF_FLG is ESA_MBX_EXIST no check of the existence
*                  name perform.
* INPUT          : mail_name, mail_key, mail_size, mess_size, conf_flg
* OUTPUT         : really_name, handle
* RETURN VALUE   : ESA_RC ( ESA_OK, ESA_ERR, ESA_MBX_NAMENOGEN )
************************************************************/

ESA_RC ESA_MBX_create( MBX_QUE_NAME_D_typ   mail_name,
                       MBX_QUE_KEY_D_typ    mail_key,
                       MBX_QUE_SIZE_D_typ   mail_size,
                       MBX_QUE_SIZE_D_typ   mess_size,
                       MBX_QUE_CONF_D_typ   conf_flg,
                       MBX_QUE_NAME_D_typ   really_name,
                       MBX_QUE_HDL_D_typ  * handle )

 {

 /*
  *   Variables
  */

  static char func[]="ESA_MBX_create";
  ESA_RC rc;
  int    r15;

 /*
  *   Initialize
  */

  ESA_DIAG_enter(ESA_COMP_OS_MBX_CRE  , 1, func);

  ESA_DIAG_printf(ESA_COMP_OS_MBX_ATT ,1,
                  "Creating mailbox %s", mail_name );
 /*
  *   Call assembelr routine and process return code
  */

  /* SAS2IBMT
  r15 = ctsambx( MBXCRE_FUNC, mail_name, mail_key, mail_size,        */
  r15 = (*(ASM_RTN_TYP *)&ctsambx)                        /* SAS2IBMT */
               ( MBXCRE_FUNC, mail_name, mail_key, mail_size,
                 mess_size, conf_flg, really_name, handle);
  ESA_DIAG_printf(ESA_COMP_OS_MBX_CRE,1, "mbxcre rc=%d", r15);

  switch(r15) {
     case 1 : rc = ESA_OK;
              break;
     default: rc = ESA_FATAL;
              ESA_DIAG_printf(ESA_COMP_OS_MBX_CRE ,0,
                   "Undefined rc=%d. Assumed ESA_FATAL", r15);
  }

 /*
  *   Finish
  */

  ESA_DIAG_exit(ESA_COMP_OS_MBX_CRE  , 1, func, rc);
  return rc;

}

/***********************************************************
* PROCEDURE NAME : ESA_MBX_delete
* DESCRIPTION    : This function deletes a mailbox spec. by the
*                  handle ( channel ).
* INPUT          : handle
* OUTPUT         : none
* RETURN VALUE   : ESA_RC (ESA_OK, ESA_MBX_NOT_EXIST, ESA_ERR )
************************************************************/

ESA_RC ESA_MBX_delete( MBX_QUE_HDL_D_typ handle )
{

 /*
  *   Variables
  */

  static char func[]="ESA_MBX_delete";
  ESA_RC rc;
  int    r15;

 /*
  *   Initialize
  */

  ESA_DIAG_enter(ESA_COMP_OS_MBX_GNRL , 1, func);

 /*
  *   Call assembelr routine and process return code
  */

  /* SAS2IBMT
  r15 = ctsambx( MBXDELT_FUNC, handle);                              */
  r15 = (*(ASM_RTN_TYP *)&ctsambx)                        /* SAS2IBMT */
               ( MBXDELT_FUNC, (void *)handle);
  ESA_DIAG_printf(ESA_COMP_OS_MBX_GNRL, 1, "mbxdelt rc=%d", r15);

  switch(r15)     {
     case 1 : rc = ESA_OK;
              break;
     default: rc = ESA_FATAL;
              ESA_DIAG_printf(ESA_COMP_OS_MBX_GNRL,0,
                   "Undefined rc=%d. Assumed ESA_FATAL", r15);
  }

 /*
  *   Finish
  */

  ESA_DIAG_exit(ESA_COMP_OS_MBX_GNRL , 1, func, rc);
  return rc;

}

/**********************************************************
* PROCEDURE NAME : ESA_MBX_get_status
* DESCRIPTION    : This function gets status of the mailbox
*                  specified by the hendle ( channel )
* INPUT          : handle
* OUTPUT         : none
* RETURN VALUE   : ESA_RC (ESA_OK, ESA_MBX_EMPTY,
*                         ESA_MBX_NOT_EXIST, ESA_ERR )
************************************************************/

ESA_RC ESA_MBX_get_status( MBX_QUE_HDL_D_typ handle )
{

 /*
  *   Variables
  */

  static char func[]="ESA_MBX_get_status";
  ESA_RC rc;
  int    r15;

 /*
  *   Initialize
  */

  ESA_DIAG_enter(ESA_COMP_OS_MBX_GNRL, 1, func);

 /*
  *   Call assembelr routine and process return code
  */

  /* SAS2IBMT
  r15 = ctsambx( MBXGETS_FUNC, handle);                              */
  r15 = (*(ASM_RTN_TYP *)&ctsambx)                        /* SAS2IBMT */
               ( MBXGETS_FUNC, (void *)handle);
  ESA_DIAG_printf(ESA_COMP_OS_MBX_GNRL, 1, "mbxgets rc=%d", r15);

  switch(r15) {
     case 1 : rc = ESA_OK;
              break;
     default: rc = ESA_FATAL;
              ESA_DIAG_printf(ESA_COMP_OS_MBX_GNRL, 0,
                       "Undefined rc=%d. Assumed ESA_FATAL", r15);
  }

 /*
  *   Finish
  */

  ESA_DIAG_exit(ESA_COMP_OS_MBX_GNRL, 1, func, rc);
  return rc;

}


/********************************************************
* PROCEDURE NAME : ESA_MBX_read
* DESCRIPTION    : This function reads message from the Mailbox
*                  specified by the handle ( channel )
* INPUT          : handle, str, size, p_wait
* OUTPUT         : str, size
* RETURN VALUE   : ESA_RC ( ESA_OK, ESA_ERR, ESA_MBX_NOT_EXIST,
*                          ESA_MBX_EMPTY )
*************************************************************/

ESA_RC ESA_MBX_read( MBX_QUE_HDL_D_typ   handle,
                     char              * str ,
                     int               * size,
                     int                 p_wait)
{

 /*
  *   Variables
  */

  static char func[]="ESA_MBX_read";
  ESA_RC rc;
  int    r15;
  int    debug_level = 0;

 /*
  *   Initialize
  */

  ESA_DIAG_enter(ESA_COMP_OS_MBX_RD, 1, func);

 /*
  *   Call assembelr routine and process return code
  */

  if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_MBX_RD ) GE 3)
     debug_level = 1;

  /* SAS2IBMT
  r15 = ctsambx( MBXREAD_FUNC, handle, str , size, p_wait,           */
  r15 = (*(ASM_RTN_TYP *)&ctsambx)                        /* SAS2IBMT */
               ( MBXREAD_FUNC, (void *)handle, str , (void *)size,
                 (void *)p_wait, &debug_level);
  if (r15 EQ 1)
     ESA_DIAG_printf(ESA_COMP_OS_MBX_RD  ,1,
                     "mbxread rc=%d len=%d mh=%.*s",
                     r15, *size, sizeof(RSS_MSG_HDR_rec_typ), str );
  else
     ESA_DIAG_printf(ESA_COMP_OS_MBX_RD  ,1, "mbxread rc=%d", r15);

  switch(r15) {
     case  1: rc = ESA_OK;
              if (*str EQ 'N')
                 rc = ESA_CANCEL_SERVICE;
              break;
     case  2: rc = ESA_TERM_CTSA; /* changed by doron */
              break;
     case 29: rc = ESA_MBX_NOT_EXIST;
              break;
     case 30: rc = ESA_MBX_EMPTY;
              break;
     case 33: rc = ESA_MBX_BUFFEROVF;
              break;
     case 36: rc = ESA_FATAL;   /* protocol vioalation */
              *size=0;
              break;
     case 40: rc = ESA_CANCEL_SERVICE;  /* comm down */
              break;
     case 44: rc = ESA_CANCEL_SERVICE;  /* timeout */
              break;
     default: rc = ESA_FATAL;
              *size=0;
              ESA_DIAG_printf(ESA_COMP_OS_MBX_RD  ,0,
                    "Undefined rc=%d. Assumed ESA_FATAL", r15);
  }

 /*
  *   Finish
  */

  ESA_DIAG_exit(ESA_COMP_OS_MBX_RD, 1, func, rc);
  return rc;

}

/**********************************************************
* PROCEDURE NAME : ESA_MBX_write
* DESCRIPTION    : This function writes message to the mailbox
*                  specified by the handle (channel ).
* INPUT          : handle, str, size, p_wait
* OUTPUT         : none
* RETURN VALUE   : ESA_RC ( ESA_OK, ESA_ERR, ESA_MBX_FULL,
*                           ESA_MBX_NOT_EXIST )
*************************************************************/

ESA_RC ESA_MBX_write( MBX_QUE_HDL_D_typ   handle,
                      char              * str,
                      int                 size,
                      int                 p_wait )

{

 /*
  *   Variables
  */

  static char func[]="ESA_MBX_write";
  ESA_RC rc;
  int    r15;
  int    debug_level = 0;
  int    i;
  char   ctsaslp_time_type = 'S';          /* S(conds)      SAS2IBMN */

 /*
  *   Initialize
  */

  ESA_DIAG_enter(ESA_COMP_OS_MBX_WR   , 1, func);

 /*
  *   Call assembelr routine and process return code
  */

  if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_MBX_WR ) GE 3)
     debug_level = 1;

  /* SAS2IBMT
  r15 = ctsambx( MBXWRIT_FUNC, handle, str, size, &debug_level);     */
  r15 = (*(ASM_RTN_TYP *)&ctsambx)                        /* SAS2IBMT */
              ( MBXWRIT_FUNC, (void *)handle, str, (void *)size,
              &debug_level);

  ESA_DIAG_printf(ESA_COMP_OS_MBX_WR  ,1,
                  "mbxwrit rc=%d len=%d mh=%.*s",
                  r15, size, sizeof(RSS_MSG_HDR_rec_typ), str );

  /* MBX Full situation (retry 60 times (appx. 30 mins) */

  for (i=1; (i LT 60) AND (r15 EQ 48); i++ ) {
     /* SAS2IBMN sleep(30);   */
     r15 = (*(ASM_RTN_TYP *)&ctsaslp)                    /* SAS2IBMN */
             (30, &ctsaslp_time_type, &debug_level);     /* SAS2IBMN */
  /* SAS2IBMT
     r15 = ctsambx( MBXWRIT_FUNC, handle, str, size, &debug_level);  */
     r15 = (*(ASM_RTN_TYP *)&ctsambx)                     /* SAS2IBMT */
                  ( MBXWRIT_FUNC, (void *)handle, str, (void *)size,
                    &debug_level);
     ESA_DIAG_printf(ESA_COMP_OS_MBX_WR, 1,
                     "mbxwrit retry no. %d rc=%d", i, r15);
  }

  switch(r15) {
     case  1: rc = ESA_OK;
              break;
     case  2: rc = ESA_TERM_CTSA; /* changed by doron           */
              break;
     case 29: rc = ESA_MBX_NOT_EXIST;
              break;
     case 36: rc = ESA_FATAL;     /* protocol vioalation        */
              break;
     case 40: rc = ESA_CANCEL_SERVICE;
              break;
     case 48: rc = ESA_FATAL;     /* mailbox full too much time */
              break;
     case 52: rc = ESA_FATAL;     /* mismatched message header  */
              break;
     case 54: rc = ESA_TERM_CTSA; /* gateway in shutdown        */
              break;
     default: rc = ESA_FATAL;
              ESA_DIAG_printf(ESA_COMP_OS_MBX_WR  ,0,
                    "Undefined rc=%d. Assumed ESA_FATAL", r15);
  }

 /*
  *   Finish
  */

  ESA_DIAG_exit(ESA_COMP_OS_MBX_WR, 1, func, rc);
  return rc;

}

/**********************************************************
* PROCEDURE NAME : ESA_MBX_act_msg_size
* DESCRIPTION    : This function retrieves a specific mailbox's
*                  message size.
* INPUT          : handle
* OUTPUT         : msg_size - the mailbox's defined message size
* RETURN VALUE   : ESA_RC ( ESA_OK, ESA_ERR, ESA_MBX_NOT_EXIST )
*************************************************************/

ESA_RC ESA_MBX_act_msg_size (MBX_QUE_HDL_D_typ  handle,
                             int                *msg_size)
{
   *msg_size = 32000;
   return ESA_OK;
}
