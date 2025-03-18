/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSCCLI in CTS.CMAC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
 /**************************************************************
 *                                                             *
 * Title            : Command Line services                    *
 *                                                             *
 * File Name        : ctsccli.h                                *
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
 * -------- -------- -------- -------------------------------- *
 * sasc     NuritY   24/08/16 Adapt to CC changes.             *
 * WS10062  SeligT   14/11/17 Refresh GDB At Start of Group    *
 *                            Aggregation: Create new routine  *
 *                            ESA_CLI_fetch_output.            *
 * WS10067  SeligT   29/01/18 Update GDB dynamically when      *
 *                            access or resource rules are     *
 *                            added or modified                *
 *                                                             *
 * IS10174  NuritY   15/01/18 Dynamic EXECOUT support.         *
 * WS10076  SeligT   05/01/20 Manage Permissions as Account    *
 *                            and Group Attributes - Phase 2   *
 * WS10078  AvnerL   05/04/20 Support XREF records             *
 * BS10111  NuritY   13/12/20 Support ruob above the bar.      *
 **************************************************************/
 #ifndef OS_CLI_H_included
 #define OS_CLI_H_included

 /* sasc - not needed anymore because it is defined in GLOBS
 #ifdef EXT
 #undef EXT
 #endif
 #ifndef OS_CLI_owner
 #define EXT extern
 #else
 #define EXT
 #endif
 */

 #define ESA_CLI_init_tso               ccli010
 #define ESA_CLI_exec_wait              ccli011
 #define ESA_CLI_get_output             ccli012
 #define ESA_CLI_clr_output             ccli013
 #define ESA_CLI_delete_output          ccli014
 /* WS10076 #define ESA_CLI_fetch_output           ccli015            */
 /* BS10111 - the following 2 routines are removed
 #define ESA_CLI_get_RUOB_line          ccli015            /@ WS10076 @/
 #define ESA_CLI_del_RUOB               ccli016            /@ WS10076 @/
    end of removed lines - BS10111 */

 #include ESA_CTSAMSG

 typedef char *CMDLINE_D_typ;
 typedef char *NODEID_D_typ;


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

 EXT ESA_RC ESA_CLI_init_tso( int        *r15,
                              int        *rc,
                              int        *rs );

 /**************************************************************
 *                                                             *
 * PROCEDURE NAME : ESA_CLI_exec_wait                          *
 *                                                             *
 * DESCRIPTION    : Execute a shell command, wait for its      *
 *                  completion .                               *
 *                                                             *
 * INPUT          : ddn              -  ddn                    *
 *                  p_cmd            -  command                *
 *                  admin_params     -                         *
 *                                                             *
 * OUTPUT         : p_shell_rc       -  shell rc               *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 **************************************************************/

 EXT ESA_RC ESA_CLI_exec_wait(char                    * ddn,
                          CMDLINE_D_typ                 p_cmd,
                          int                         * p_shell_rc,
         /* IS10174 */    ADMIN_PARAMS_rec_typ        * admin_params);
         /* IS10174       CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
         /* IS10174       CTSAMSG_HANDLE_rec_typ      * msgs );       */

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
 *                  admin_params     -                         *
 *                                                             *
 * OUTPUT         :                                            *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 **************************************************************/

 EXT ESA_RC ESA_CLI_get_output(char                   * ddn,
                           char                       * p_buff,
                           short                      * p_len,
         /* IS10174 */    ADMIN_PARAMS_rec_typ        * admin_params);
         /* IS10174       CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
         /* IS10174       CTSAMSG_HANDLE_rec_typ      * msgs );       */


/*  WS10076N - this routine is not used any more
/@ * * * * * * * WS10062 Start * * * * * * * @/
/@*************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_fetch_output                       *
*                                                             *
* DESCRIPTION    : Obtain shell output from last command in   *
*                  'byteseek' mode and return it to the       *
*                  caller in a supplied buffer                *
*                                                             *
* INPUT          : ddn              -  DDN of file            *
*                  stream           -  file stream handle     *
*                  p_buff           -  buffer address         *
*                  p_len            -  buffer len requested   *
*                  admin_params     -                         *
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
         /@ IS10174 @/      ADMIN_PARAMS_rec_typ        * admin_params);
         /@ IS10174         CTSAMSG_DEST_TABLE_rec_ptr   dest,       @/
         /@ IS10174         CTSAMSG_HANDLE_rec_ptr       msgs );     @/
/@ * * * * * * * WS10062 End * * * * * * * @/
*/


 /**************************************************************
 *                                                             *
 * PROCEDURE NAME : ESA_CLI_clr_output                         *
 *                                                             *
 * DESCRIPTION    : Clear shell output file                    *
 *                                                             *
 * INPUT          : ddn              -  DDN of file            *
 *                  admin_params     -                         *
 *                                                             *
 * OUTPUT         :                                            *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 **************************************************************/

 EXT ESA_RC ESA_CLI_clr_output( char                        *ddn,
        /* IS10174 */    ADMIN_PARAMS_rec_typ        * admin_params);
        /* IS10174       CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
        /* IS10174       CTSAMSG_HANDLE_rec_typ      * msgs );       */

/**************************************************************
*                                                             *
* PROCEDURE NAME : ESA_CLI_delete_output                      *
*                                                             *
* DESCRIPTION    : Delete/Clear shell output file             *
*                                                             *
* INPUT          : ddn              -  DDN of file            *
*                  admin_params     -                         *
*                                                             *
* OUTPUT         :                                            *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************/
EXT ESA_RC ESA_CLI_delete_output(char                        *ddn,
        /* IS10174 */    ADMIN_PARAMS_rec_typ        * admin_params);
        /* IS10174       CTSAMSG_DEST_TABLE_rec_typ  * dest,         */
        /* IS10174       CTSAMSG_HANDLE_rec_typ      * msgs );       */

/* ****** ****** ****** ** WS10076 Start ** ****** ****** ****** */
/*  The definitions below were moved from CTSBACF  */

/*  BS10111 - remove the definiions below - not needed any more
/@
 *   Internal subroutines
 @/
typedef ESA_RC GET_LINE_TYP(char                * function,
                       void                     * inp_desc_blk,
                       void                    ** handle,
                       char                     * get_line_buffer,
                       int                       get_line_buffer_size,
                       ADMIN_PARAMS_rec_typ     * admin_params);

typedef GET_LINE_TYP * GET_LINE_PTR;   /@ pointer to func @/

/@ WS10076 extern  GET_LINE_TYP   Get_line_from_memory;    @/
extern  GET_LINE_TYP   ESA_CLI_get_RUOB_line;              /@ WS10076 @/
/@   End of move      @/
/@
 *   REXX/Utility Output Block header structure.  This header is
 *   immediately followed by rule lines as returned by ACFRPTRX
 *   and LIST RULES.
 *
 *   Each line of data is preceded by 1 byte of length. This
 *   byte contains the length of the data (not including itself).
 *   The REXX/Utility Output Blocks are chained.  The last block,
 *   created by CTS2RXO, contains 0 in the hnext field.
 *
 *   The type of hnext is REXX@UTL_output_blk_rec_typ *.
 *   The type of all other fields is int.
 *
 *   ***  ATTENTION ***
 *   Any change in this structure should be first done in the
 *   CTSARUOB member in the MAC library, and EDCDSECT should be
 *   run to get the matching structure definition to be put here.
 *
 @/
#pragma pack(packed)

typedef struct ruo {
  unsigned char  ruoheyec[4]; /@ EYE CATCHER "RUOB"       WS10076 @/
  void          *ruohnext;    /@ -> NEXT BLOCK.                   @/
  int            ruohlen;     /@ BLOCK LENGTH.                    @/
  unsigned char  ruohsp;      /@ SUBPOOL.                         @/
  unsigned char  ruohkey;     /@ KEY.                             @/
  unsigned char  ruohloc;     /@ LOCATION (24/31)                 @/
  unsigned char  _filler1;    /@ FFU.                             @/
  int            ruohdlen;    /@ DATA LENGTH.                     @/
  unsigned char  ruodata;     /@ START OF DATA.                   @/
  } REXX@UTL_output_blk_rec_typ, * REXX@UTL_output_blk_rec_ptr;

/@ Values for field "ruohdlen" @/
#define _ruohlen 0x14 /@ HEADER LENGTH @/

typedef struct ruol {
  unsigned char  ruollen;  /@ LINE DATA LENGTH.     @/
  unsigned char  ruoldata; /@ VARIABLE LENGTH DATA, @/
  } REXX@UTL_output_line_rec_typ, * REXX@UTL_output_line_rec_ptr;

/@ Values for field "ruoldata" @/
#define _ruosp 131  /@ SUBPOOL=131 - OWNER=JSTCB @/
#define _ruokey 8    /@ KEY=8.                    @/
#define _ruoloc 31   /@ LOCATION=ABOVE.           @/

#pragma pack(reset)
   end of removed code.  - BS10111  */

/*
 *   The following structure is used for ignoring extraneous
 *   output lines returned by ACFRPTRX and LIST RULES.
 *
 *   ***  ATTENTION ***
 *   Any change in this structure should be first done in the
 *   CTSARUOI member in the MAC library, and EDCDSECT should be
 *   run to get the matching structure definition to be put here.
 */
#pragma pack(packed)

typedef struct ignlist {
  unsigned char  ignlen;       /* TEXT LENGTH        */
                               /* SHOULD BE 1 WHEN   */
                               /* IGNFULL IS FF      */
  unsigned char  ignfull;      /* 'FULL' INDICATION: */
                               /* - 00 - EXACT TEXT. */
                               /* - FF - THE WHOLE   */
                               /*        LINE SHOULD */
                               /*        CONTAIN THE */
                               /*        CHARACTER   */
                               /*        SET IN TEXT.*/
  unsigned char  igntext[133]; /* TEXT TO IGNORE.    */
  } REXX@UTL_output_ignlist_rec_typ, * REXX@UTL_output_ignlist_rec_ptr;

/* Values for field "igntext" */
#define _ignlen 0x87

#pragma pack(reset)

/* BS10111 - the definitions below are not needed any more.
/@******************************************************************
* Input Descriptor Block for the ESA_CLI_get_RUOB_line routine     *
******************************************************************@/
typedef struct get_from_RUOB_parm {
   REXX@UTL_output_blk_rec_typ  * blk;
} IDB_GETRUOBLINE_rec_typ  , *IDB_GETRUOBLINE_rec_ptr;

/@ copied from CTSCCLI CSRC to be used by Get_XREF_Record WS10078 @/
typedef struct MEMORY_HANDLE_STRUCT {
  REXX@UTL_output_blk_rec_typ *block;
  int len;
  int iseof;
} Get_RUOBline_handle_rec_typ;
   end of removed code  - BS10111  */

/**********************************************************************
 *  The following typedef from WS10075 was changed in WS10076S as     *
 *  follows:                                                          *
 *     - every reference to PBB was changed to GRO                    *
 *     - the reference to CTSBPBB was changed to CTSCGRO              *
 *     - changes to individual variables as documented below          *
 *     - move the entire typedef to CTSCCLI / CTSOCLI                 *
 *     - the following fields were removed:  rss_name, rss_type,      *
 *       msgs, dest.                                                  *
 *                                                                    *
 *  typedef for the CTSCGRO routine                                   *
 *  This is a structure for passing parameters from a 'C' program to  *
 *  another 'C' program, CTSCGRO, via a REXX program, CTSBRLST, and   *
 *  an Assembler program, CTSAVAR.                                    *
 *********************************************************************/

#define GRO_EYECATCHER                    "GRO "

typedef struct s_os_gro {
     char                       eyecatcher[4]; /* eyecatcher 'GRO' */
     int                        RUOB_size;     /* RUOB size.       */
                                               /* added in WS10076S*/
     /* BS10111
     REXX@UTL_output_blk_rec_typ    ** RUOBptr; /@ ptr to 1st RUOB @/ */
                                               /* added in WS10076S*/
     char                     * RUOBptr; /* -> 1st RUOB addr  BS10111 */
     REXX@UTL_output_ignlist_rec_typ * ignore_list; /* ignore list */
                                               /* added in WS10076S*/
     int                        debug_level;   /* debug level      */
     ESA_DIAG_COMP_typ          comp;          /* diag component   */
                                               /* added in WS10076S*/
     ADMIN_PARAMS_rec_typ     * admin_params;  /* admin params     */
} OS_GRO_typ, *OS_GRO_ptr ;
/* ****** ****** ****** ** WS10076 End **** ****** ****** ****** */

/*                                                          * BS10111 *
 *   Define all CTSARUH request and debug types so that     * BS10111 *
 *   they will always be 8-characters long with trailing    * BS10111 *
 *   blanks.                                                * BS10111 *
 */                                                        /* BS10111 */
#define  CTSARUH_REQ_STARTIO     "STARTIO "                /* BS10111 */
#define  CTSARUH_REQ_PUT         "PUT     "                /* BS10111 */
#define  CTSARUH_REQ_GETFIRST    "GETFIRST"                /* BS10111 */
#define  CTSARUH_REQ_GETNEXT     "GETNEXT "                /* BS10111 */
#define  CTSARUH_REQ_TERMIO      "TERMIO  "                /* BS10111 */
#define  CTSARUH_REQ_TERMNFRE    "TERMNFRE"                /* BS10111 */
#define  CTSARUH_REQ_FREERUOB    "FREERUOB"                /* BS10111 */
#define  CTSARUH_DBGTYPE_PRTDBG  "PRTDBG  "                /* BS10111 */
#define  CTSARUH_DBGTYPE_WTO     "WTO     "                /* BS10111 */

/*  BS10111 - this routine is not needed any more.
/@*********************** WS10076A *********************************
* PROCEDURE NAME : ESA_CLI_del_RUOB                                *
* DESCRIPTION    : Delete REXX / Utility Output Block deletes all  *
*                  blocks. The only parameter is the address of the*
*                  first block to be deleted.                      *
* INPUT          : ddn              -  DDN of file                 *
*                  admin_params     -                              *
* OUTPUT         :                                                 *
* RETURN VALUE   : ESA_RC                                          *
******************************************************************@/
ESA_RC ESA_CLI_del_RUOB(REXX@UTL_output_blk_rec_typ  * ruoh,
                        ADMIN_PARAMS_rec_typ         * admin_params);
/@*******************WS10076A end *********************************@/
 end of rmoved code - BS10111  */

 #endif  /* OS_CLI_H_included */
