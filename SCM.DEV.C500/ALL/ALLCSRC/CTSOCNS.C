/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSCCNS in CTS.CSRC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
/**************************************************************
*                                                             *
* Title            : OS console function                      *
*                                                             *
* File Name        : ctsocns.c                                *
*                                                             *
* Author           : Alexander Shvartsman                     *
*                                                             *
* Creation Date    : 03/08/94                                 *
*                                                             *
* Description      :                                          *
*                                                             *
* Assumptions and                                             *
*   Considerations :                                          *
*                                                             *
**************************************************************/

/**************************************************************
* Mod.Id   Who     When     Description                       *
* -------- ------- -------- ----------------------------------*
* IS0003   ShmuelK 01/2003  Add dummy  OS_GetFreeDiskSpace    *
* SAS2IBMT SeligT  09/10/16 SAS/C to IBM C Conversion Project *
* SAS2IBMA AvnerL  07/12/16 Set CC includes per IS0060.       *
* BS10058  SeligT  10/08/17 Part of Instruction Became Comment*
**************************************************************/

 #include   <globs.h>

 #include   STDLIB
 #include   STRING

 #include   ESA_DIAG
 #include   ESA_OS_CONSOLE

 /* IS0060 -
 #include   MVS_COMP
 #include   MVS_OS_CLI  */

 /* IS0060 -  rename of header files */
 #include   MVS_C_COMP
 #include   MVS_OSC_CLI
 /* IS0060 - end */
 #define    MAX_MSG_LEN    120   /* Don't change !!!! */
                                 /* Assembler program */
                                 /* work only with max*/
                                 /* length 120 byte    */

/*
 *     Assembler routine issue message ( WTO macro )
 */

 /* SAS2IBMT
 extern int ctsawto(int *msg_len,const char *message, char *roll );  */
 extern int ctsawto();                                    /* SAS2IBMT */


/****************************************************
 * Procedure Name: OS_CONSOLE_print
 * Description   : Print a message to system console/log
 * Input         : Message
 *                 rollable - OS_CONS_ROLLABLE_typ
 *                 severity - OS_CONS_SEVERITY_typ
 * Output        :
 * Input/Output  :
 * Return Value  : ESA_RC
 * Side Effects  : Message is written to system console
 *                 and/or to system log.
 ***************************************************/

ESA_RC OS_CONSOLE_print (const char           * message,
                         OS_CONS_ROLLABLE_typ   rollable,
                         OS_CONS_SEVERITY_typ   severity)


{

  static char func[] = "OS_CONSOLE_print";

 /*
  *    Variables
  */

  ESA_RC    rc = ESA_OK ;
  int       msg_pos;
  int       r15;
  int       i,l;
  div_t     num_lines;
  int       msg_len ;
  char      roll_flag = 'Y' ;

 /*
  *    Initialize
  */

  ESA_DIAG_enter(PLT_COMP_OS_CONSOLE, 1, "OS_CONSOLE_print");

 /*
  *    Set roll_flag for ctsawto
  */

  if (rollable EQ OS_CONS_ROLLABLE_NO)
     roll_flag = 'N' ;

 /*
  *    Calculate number of lines need to send message
  */

  msg_pos=0;

  l = strlen(message);       /* message len                */
  num_lines = div(l,MAX_MSG_LEN);    /* def quotient and remainder */

 /*
  *      Quotients
  */

  if ( num_lines.quot NE 0 ) {
    msg_len = MAX_MSG_LEN ;
    for (i=0;  i LT num_lines.quot; i++ , msg_pos += msg_len )
        /* SAS2IBMT and BS10058
        r15 = ctsawto(&msg_len, &message[msg_pos], &roll_flag);       */
        r15 = (*(ASM_RTN_TYP *)&ctsawto)                  /* SAS2IBMT */
                     (&msg_len, &message[msg_pos], &roll_flag);
        if (r15 GT 0) {
           rc = ESA_FATAL;
           goto exit;
        }
  }

 /*
  *      Remainder
  */

  if ( num_lines.rem NE 0 ) {
    msg_len = l - num_lines.quot * MAX_MSG_LEN ;
    /* SAS2IBMT
    r15 = ctsawto(&msg_len, &message[msg_pos], &roll_flag);          */
    r15 = (*(ASM_RTN_TYP *)&ctsawto)                      /* SAS2IBMT */
                 (&msg_len, &message[msg_pos], &roll_flag);
    if (r15 GT 0) {
       rc = ESA_FATAL;
       goto exit;
    }
  }

 /*
  *      Finish
  */

  exit:;

  ESA_DIAG_exit(PLT_COMP_OS_CONSOLE, 1, func, rc);

  return rc ;

}
 /****************************************************
  * ------- start of IS0003 ----
  * Procedure Name: OS_GetFreeDiskSpace
  * Description     This is a dummy function (for z/OS)
  * Input         : log file name
  * Output        :
  * Input/Output  :
  * Return Value  : Greater Than 0
  * Side Effects  : Needed from Common Code 3.1.05 up
  ***************************************************/
int   OS_GetFreeDiskSpace(const char * log_file_name)
{
   return 1; /*return greater than 0 */
}/* OS_GetFreeDiskSpace. end of IS0003*/
