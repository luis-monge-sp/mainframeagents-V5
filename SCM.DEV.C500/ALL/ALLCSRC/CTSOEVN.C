/**************************************************************
*  No corresponding CTSCEVN in CTS.CSRC                       *
**************************************************************/
/****************************************************
 * Title            : Event broadcasting and checks
 * File Name        : ctscevn.c
 * Author           : Doron Cohen
 * Creation Date    : 24/1/95
 * Description      :
 * Assumptions and
 *   Considerations :
 *****************************************************/

/********************************************************
 * Mod.ID      Who      When       Description
 * ------------------------------------------------------
 * ws2356     Yonatan  23/8/99  change the subsystem
 *                              mechanism for name/token.
 * PS0426     Yonatan  12/10/00 remove reference to SUBSYS_NAME
 * SAS2IBMT   SeligT   09/10/16 SAS/C to IBM C Conversion Project
 * SAS2IBMA   AvnerL   07/12/16 Set CC includes per IS0060.      *
 ********************************************************/

#include "globs.h"

#include STRING
#include STDIO
#include STDLIB

#include ESA_API
#include ESA_INIT
#include ESA_DIAG
#include ESA_API_CODES
#include ESA_OS_EVENT

/* IS0060
#include MVS_CODES
#include MVS_OS_MVS  */

/* IS0060 - renaming  */
#include MVS_C_CODES
#include MVS_OSC_MVS
/*IS0060 - end */

#define XMM_SSN_SIZE    4
#define XMM_JBN_SIZE    8
#define XMM_MAIN_SIZE   8
#define XMM_NO_JOBNAME  "********"
#define BLANK_JBN       "        "
#define INSTAL_PARAM_NAME  "CTSA_ID"                    /* ws2356 */

static char comp[] = "OS_EVENT" ;

/* SAS2IBMT prototype changed for IBM C
extern int ctsaevn( char *evn_op, char *evn_ssn, int *evn_dbg,
                    int *evn_rc, int *evn_rs, ...);                  */
extern int ctsaevn();                                     /* SAS2IBMT */

/* SAS2IBMT prototype changed for IBM C
extern int ctsasri( char *sri_op, char *sri_ssn, char *sri_jbn,
                    int *sri_dbg, int *sri_rc, int *sri_rs );        */
extern int ctsasri();                                     /* SAS2IBMT */

static void chk_main_name(ESA_MAIN_NAME_typ main_name,
                          char              mnm[XMM_MAIN_SIZE+1],
                          char              jbn[XMM_JBN_SIZE+1],
                          int             * mnm_flag );

typedef struct {
  char ssn[XMM_SSN_SIZE+1];
  char jbn[XMM_JBN_SIZE+1];
  char mnm[XMM_MAIN_SIZE+1];
  void *xmm;
} OS_EVENT_handle_rec_typ;

static char sri_init[]   = "INIT    ";
static char sri_term[]   = "TERM    ";
static char evn_reset[]  = "RESET   ";
static char evn_get[]    = "GET     ";
static char evn_brdcst[] = "BRDCST  ";
static char evn_clr[]    = "CLEAR   ";
static char evn_myxmm[]  = "MYXMMH  ";
static int  evn_stat_on  = 1;
static int  evn_stat_off = 0;

/**********************************************************
 PROCEDURE NAME : OS_EVENT_init
 DESCRIPTION    : This function initialize events services
                  for the process.
 INPUT          : msg_params - ctsamsg parameters
 OUTPUT         : handle     - event services handle
 RETURN VALUE   : ( ESA_OK, ESA_ERR )
************************************************************/

ESA_RC OS_EVENT_init ( void                     ** handle,
                       ESA_MAIN_NAME_typ           main_name,
                       ADMIN_PARAMS_rec_typ      * admin_params,
                       CTSAMSG_PARAMS_rec_typ    * msg_params)
{

  /*
   *   Variables
   */

   static char        func[]="OS_EVENT_init";
   ESA_RC             rc = ESA_OK;
   int                dbgl;
   int                sri_rc, sri_rs;
   int                evn_rc, evn_rs;
   char               ssn[XMM_SSN_SIZE+1];
   char               jbn[XMM_JBN_SIZE+1];
   char               mnm[XMM_MAIN_SIZE+1];
   int                mnm_flag;
   void             * xmm = NULL;

   OS_EVENT_handle_rec_typ * hp;

   char               parm_get[81] = "";
   int                parm_len;

  /*
   *   Initialize
   */

   ESA_DIAG_enter (ESA_COMP_OS_EVENT, 1, func );

   if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_EVENT) GT 1)
      dbgl = 1;

  /*
   *   Check handle parameter
   */

   if (handle EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "NO HANDLE", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

   hp = (OS_EVENT_handle_rec_typ *)(*handle);
   if ( hp NE NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "INVALID HANDLE", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

  /*
   *   Obtain subsystem name from RSSPARM file
   */

   rc = admin_params->cs_func.rssprm_get_opt_ptr(ALL_RSS,    /*ws2356*/
                                            INSTAL_PARAM_NAME,
                                             sizeof(parm_get),
                                             parm_get,
                                             OPT_TRUE,       /*ws2356*/
                                             OPT_TRUE) ;     /*ws2356*/
   if (rc NE ESA_OK) {
        CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                      NULL, msg_params->ctsamsg_dest, comp, func,
                      "NO CTSAID NAME", 16, __LINE__ );
        rc = ESA_FATAL;
        goto exit;
   }
   ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
                    "subsystem parameter=%s", parm_get );

  /*
   *   Check subsystem name validity
   */

   parm_len = strlen(parm_get);
   if ( (parm_len GT sizeof(ssn)-1) OR (parm_len LT 1)) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "INVALID SSNAME", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

   memset(ssn, BLANK, sizeof(ssn)-1 );
   ssn[sizeof(ssn)-1] = NULL_CHAR;
   memcpy(ssn, parm_get, parm_len);

  /*
   *   Check if main can recieve events or just broadcast
   */

   chk_main_name(main_name, mnm, jbn, &mnm_flag);

  /*
   *   If main can also accept events - create xmm block for it
   */

   if (mnm_flag) {

     /*
      *   Check APF authority
      */

      rc = OS_MVS_check_apf( msg_params->ctsamsg_handle,
                             msg_params->ctsamsg_dest   );
      if (rc NE ESA_OK) {
         rc = ESA_FATAL;
         goto exit;
      }

     /*
      *   Initialize xmm header block (if main supported)
      */

      /* SAS2IBMT
      sri_rc = ctsasri(sri_init, ssn, jbn, &dbgl, &sri_rc, &sri_rs); */
      sri_rc = (*(ASM_RTN_TYP *)&ctsasri)(sri_init, ssn,  /* SAS2IBMT */
                       jbn, &dbgl, &sri_rc, &sri_rs);
      ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
                       "ctsasri op=%s ssn=%s jbn=%s rc=%d rs=%d",
                       sri_init, ssn, jbn, sri_rc, sri_rs);
      if ( sri_rc NE 0 ) {
         CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                       NULL, msg_params->ctsamsg_dest, comp, func,
                       "CREATE XMM", sri_rc, __LINE__ );
         rc = ESA_FATAL;
         goto exit;
      }

     /*
      *   Obtain xmm block details
      */

      /* SAS2IBMT
      evn_rc = ctsaevn( evn_myxmm, ssn, &dbgl, &evn_rc, &evn_rs,     */
      evn_rc = (*(ASM_RTN_TYP *)&ctsaevn)(evn_myxmm, ssn, /* SAS2IBMT */
                        &dbgl, &evn_rc, &evn_rs,
                        jbn, &xmm);
      jbn[sizeof(jbn)-1] = NULL_CHAR;

      ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
                       "ctsaevn xmmh ssn=%s jbn=%s xmm=%X rc=%d rs=%d",
                        ssn, jbn, xmm, evn_rc, evn_rs);

      if ( evn_rc NE 0 ) {
         CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                       NULL, msg_params->ctsamsg_dest, comp, func,
                       "MYXMMH", evn_rc, __LINE__ );
         rc = ESA_FATAL;
         goto exit;
      }

     /*
      *   Reset events flags
      */

      /* SAS2IBMT
      evn_rc = ctsaevn( evn_reset, ssn, &dbgl, &evn_rc, &evn_rs,     */
      evn_rc = (*(ASM_RTN_TYP *)&ctsaevn)(evn_reset, ssn, /* SAS2IBMT */
                        &dbgl, &evn_rc, &evn_rs,
                        xmm, jbn, mnm);
      ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
             "ctsaevn reset ssn=%s jbn=%s xmm=%X main=%s rc=%d rs=%d",
             ssn, jbn, xmm, mnm, evn_rc, evn_rs);
      if ( evn_rc NE 0 ) {
         CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                       NULL, msg_params->ctsamsg_dest, comp, func,
                       "RESET EVENT", evn_rc, __LINE__ );
         rc = ESA_FATAL;
         goto exit;
      }

   }    /* main can accept events */

  /*
   *   Main can only brodacst events
   */

   else {

      strcpy(jbn, XMM_NO_JOBNAME );
      xmm = NULL;

   }    /* main can only brodcast */

  /*
   *   Create handle
   */

   hp = malloc(sizeof(OS_EVENT_handle_rec_typ));
   if (hp NE NULL) {
      memcpy( hp->ssn, ssn, sizeof(hp->ssn));
      memcpy( hp->jbn, jbn, sizeof(hp->jbn));
      memcpy( hp->mnm, mnm, sizeof(hp->mnm) );
      hp->xmm = xmm;
      *handle = hp;
   }
   else {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                     "MALLOC HANDLE", 16, __LINE__ );
      rc = ESA_FATAL ;
      goto exit;
   }

  /*
   *   Finish
   */

   exit :;

   ESA_DIAG_exit(ESA_COMP_OS_EVENT, 1 , func , rc );

   return rc;

}

/**********************************************************
 PROCEDURE NAME : OS_EVENT_term
 DESCRIPTION    : This function terminates events services
                  for the process.
 INPUT          : msg_params - ctsamsg parameters
 OUTPUT         : handle     - event services handle
 RETURN VALUE   : ( ESA_OK, ESA_ERR )
************************************************************/

ESA_RC OS_EVENT_term ( void                     ** handle,
                       CTSAMSG_PARAMS_rec_typ    * msg_params)
{

  /*
   *   Variables
   */

   static char        func[]="OS_EVENT_term";
   ESA_RC             rc = ESA_OK;
   int                dbgl;
   int                sri_rc, sri_rs;
   OS_EVENT_handle_rec_typ * hp;

  /*
   *   Initialize
   */

   ESA_DIAG_enter (ESA_COMP_OS_EVENT, 1, func );

   if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_EVENT) GT 1)
      dbgl = 1;

  /*
   *   Check handle parameter
   */

   if (handle EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "NO HANDLE", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

   hp = (OS_EVENT_handle_rec_typ *)(*handle);
   if (hp EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "INVALID HANDLE", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

  /*
   *   Terminate xmm block
   */

   if (hp->xmm NE NULL) {

      /* SAS2IBMT
      sri_rc = ctsasri( sri_term, hp->ssn, hp->jbn, &dbgl, &sri_rc,  */
      sri_rc = (*(ASM_RTN_TYP *)&ctsasri)(sri_term,       /* SAS2IBMT */
                        hp->ssn, hp->jbn, &dbgl, &sri_rc,
                        &sri_rs );
      ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
                       "ctsasri term ssn=%s jbn=%s rc=%d rs=%d",
                       hp->ssn, hp->jbn, sri_rc, sri_rs);
      if ( sri_rc NE 0 ) {
         CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                       NULL, msg_params->ctsamsg_dest, comp, func,
                       "TERMINATE XMM", sri_rc, __LINE__ );
         rc = ESA_FATAL;
         goto exit;
      }
   }

  /*
   *   Free handle
   */

   free(hp);
   *handle = NULL;

  /*
   *   Finish
   */

   exit :;

   ESA_DIAG_exit(ESA_COMP_OS_EVENT, 1 , func , rc );

   return rc;

}

/***********************************************************
 PROCEDURE NAME : OS_EVENT_broadcast
 DESCRIPTION    : This function set the events and broadcast it
                  to the requested process
 INPUT          : event_num  - number of events to broadcast
                  event      - events to broadcast
                  main_name  - to whom to broadcast the event
                  handle     - event services handle
                  msg_params - ctsamsg parameters
 OUTPUT         : none
 RETURN VALUE   : ( ESA_OK, ESA_ERR )
**************************************************************/

ESA_RC OS_EVENT_broadcast (int                      event_num,
                           ESA_EVENT_typ            event[1],
                           int                      main_num,
                           ESA_MAIN_NAME_typ        main_name[1],
                           void                   * event_data[1],
                           void                   * handle,
                           CTSAMSG_PARAMS_rec_typ * msg_params)
{

  /*
   *   Variables
   */

   static char    func[]="OS_EVENT_brodacast";
   ESA_RC         rc = ESA_OK;
   int            evn_rc, evn_rs;
   int            dbgl;
   int            i,j;
   char           mnm[XMM_MAIN_SIZE+1] = "";
   int            mnm_flag;
   OS_EVENT_handle_rec_typ * hp;

  /*
   *   Initialize
   */

   ESA_DIAG_enter (ESA_COMP_OS_EVENT, 1, func );

   if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_EVENT) GT 1)
      dbgl = 1;

  /*
   *   Check handle parameter
   */

   hp = (OS_EVENT_handle_rec_typ *)(handle);
   if (hp EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "INVALID HANDLE", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

  /*
   *   Check APF authority
   */

   rc = OS_MVS_check_apf( msg_params->ctsamsg_handle,
                          msg_params->ctsamsg_dest   );
   if (rc NE ESA_OK) {
      rc = ESA_ERR;
      goto exit;
   }

  /*
   *   Loop on events and broadcast each of them to main requested
   */

   for (i=0; i LT event_num; i++)  {
       for (j=0; j LT main_num; j++)  {
           chk_main_name(main_name[j], mnm, NULL, &mnm_flag);
           /* SAS2IBMT
           evn_rc = ctsaevn( evn_brdcst, hp->ssn, &dbgl, &evn_rc,    */
           evn_rc = (*(ASM_RTN_TYP *)&ctsaevn)(evn_brdcst, /* SAS2IBMT*/
                             hp->ssn, &dbgl, &evn_rc,
                             &evn_rs, hp->xmm, hp->jbn, mnm,
                             &event[i], &evn_stat_on );
           ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
                  "ctsaevn brdcst ssn=%s main=%s event=%d rc=%d rs=%d",
                  hp->ssn, mnm, event[i], evn_rc, evn_rs);
           if ( evn_rc GT 4 ) {
              CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                        NULL, msg_params->ctsamsg_dest, comp, func,
                        "BROADCAST", evn_rc, __LINE__ );
              rc = ESA_ERR;
              goto exit;
           }
       }
   }

  /*
   *   Finish
   */

   exit :;

   ESA_DIAG_exit(ESA_COMP_OS_EVENT, 1 , func , rc );

   return rc;

}

/***********************************************************
 PROCEDURE NAME : OS_EVENT_clear
 DESCRIPTION    : This function clears the event for the
                  current process and returns the event status before
                  it was cleared .
 INPUT          : event_num  - number of events to clear
                  event      - events to clear
                  handle     - event services handle
                  msg_params - ctsamsg parameters
 OUTPUT         : event_stat - status of the events before cleared
                : event_data - user data of the events before cleared
 RETURN VALUE   : ( ESA_OK, ESA_ERR )
**************************************************************/

ESA_RC OS_EVENT_clear     ( int                       event_num,
                            ESA_EVENT_typ             event[1],
                            ESA_EVENT_STATUS_typ      event_stat[1],
                            void                    * event_data[1],
                            void                    * handle,
                            CTSAMSG_PARAMS_rec_typ  * msg_params)
{

  /*
   *   Variables
   */

   static char        func[]="OS_EVENT_clear";
   ESA_RC             rc = ESA_OK;
   int                dbgl;
   int                evn_rc, evn_rs;
   OS_EVENT_handle_rec_typ * hp;
   int                i;
   int                event_st;

  /*
   *   Initialize
   */

   ESA_DIAG_enter (ESA_COMP_OS_EVENT, 1, func );

   if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_EVENT) GT 1)
      dbgl = 1;

  /*
   *   Check handle parameter
   */

   hp = (OS_EVENT_handle_rec_typ *)(handle);
   if (hp EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "INVALID HANDLE", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }
   else if (hp->xmm EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "NO INIT", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

  /*
   *   Loop on events and test the event mask for each of them
   *   and clear each successfuly tested bit
   */

   for (i=0; i LT event_num; i++)  {

       ESA_DIAG_printf (ESA_COMP_OS_EVENT, 3,
               "%d: clearing event %d status", i, event[i] );

       event_stat[i] = EVENT_OFF;

       /***                                                     ***/
       /*** really test the flags only for supported main names ***/
       /*** and only for events in the supported range          ***/
       /***                                                     ***/

       if ( (event[i] GE 1) AND (event[i] LE 32) AND
            (hp->xmm NE NULL) ) {

           /* SAS2IBMT
           evn_rc = ctsaevn( evn_clr, hp->ssn, &dbgl, &evn_rc, &evn_rs,
                            hp->xmm, hp->jbn, hp->mnm, &event[i],    */
           evn_rc = (*(ASM_RTN_TYP *)&ctsaevn)(evn_clr,    /* SAS2IBMT*/
                            hp->ssn, &dbgl, &evn_rc, &evn_rs,
                            hp->xmm, hp->jbn, hp->mnm, &event[i],
                            &event_st );
           ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
               "ctsaevn clr ssn=%s jbn=%s event=%d rc=%d rs=%d st=%d",
               hp->ssn, hp->jbn, event[i], evn_rc, evn_rs, event_st);
           if ( evn_rc GT 0 ) {
              CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                           NULL, msg_params->ctsamsg_dest, comp, func,
                           "GET EVENTS", evn_rc, __LINE__ );
              rc = ESA_ERR;
              goto exit;
           }
           if (event_st NE 0)
              event_stat[i] = EVENT_ON;

       } /* event in range */

       ESA_DIAG_printf (ESA_COMP_OS_EVENT, 3,
            "Event %d status was set to %d", event[i], event_stat[i] );

   } /* next event */

  /*
   *   Finish
   */

   exit :;

   ESA_DIAG_exit(ESA_COMP_OS_EVENT, 1 , func , rc );

   return rc;
}

/***********************************************************
 PROCEDURE NAME : ESA_EVENT_test
 DESCRIPTION    : This function tests the status of events
                  for the current process
 INPUT          : event_num  - number of events to test
                  event      - events to test
                  handle     - event services handle
                  msg_params - ctsamsg parameters
 OUTPUT         : event_stat - status of the events
                : event_data - user data of the events
 RETURN VALUE   : ( ESA_OK, ESA_ERR )
**************************************************************/

ESA_RC OS_EVENT_test ( int                       event_num,
                       ESA_EVENT_typ             event[1],
                       ESA_EVENT_STATUS_typ      event_stat[1],
                       void                    * event_data[1],
                       void                    * handle,
                       CTSAMSG_PARAMS_rec_typ  * msg_params)
{

  /*
   *   Variables
   */

   static char        func[]="OS_EVENT_test";
   ESA_RC             rc = ESA_OK;
   int                dbgl;
   int                evn_rc, evn_rs;
   OS_EVENT_handle_rec_typ * hp;
   int                i;
   int                event_mask = 0 ;
   int                temp_mask = 0;

  /*
   *   Initialize
   */

   ESA_DIAG_enter (ESA_COMP_OS_EVENT, 1, func );

   if (  ESA_DIAG_get_debug_level(ESA_COMP_OS_EVENT) GT 1)
      dbgl = 1;

  /*
   *   Check handle parameter
   */

   hp = (OS_EVENT_handle_rec_typ *)(handle);
   if (hp EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "INVALID HANDLE", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }
   else if (hp->xmm EQ NULL) {
      CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "NO INIT", 16, __LINE__ );
      rc = ESA_FATAL;
      goto exit;
   }

  /*
   *   Get events flag
   */

   if (hp->xmm NE NULL) {
      /* SAS2IBMT
      evn_rc = ctsaevn( evn_get, hp->ssn, &dbgl, &evn_rc, &evn_rs,   */
      evn_rc = (*(ASM_RTN_TYP *)&ctsaevn)(evn_get, hp->ssn, /*SAS2IBMT*/
                        &dbgl, &evn_rc, &evn_rs,
                        hp->xmm, hp->jbn, hp->mnm, &event_mask );
      ESA_DIAG_printf (ESA_COMP_OS_EVENT, 2,
                   "ctsaevn get ssn=%s jobname=%s mask=%X rc=%d rs=%d",
                   hp->ssn, hp->jbn, event_mask, evn_rc, evn_rs);
      if ( evn_rc GT 0 ) {
         CTSAMSG_print(ERR_INTERNAL2, msg_params->ctsamsg_handle,
                    NULL, msg_params->ctsamsg_dest, comp, func,
                    "GET EVENTS", evn_rc, __LINE__ );
         rc = ESA_ERR;
         goto exit;
      }
   } /* xmm block exists */

  /*
   *   Loop on events and test the event mask for each of them
   */

   for (i=0; i LT event_num; i++)  {

       ESA_DIAG_printf (ESA_COMP_OS_EVENT, 3,
               "%d: setting event %d status", i, event[i] );
       event_stat[i] = EVENT_OFF;
       if ( (event[i] GE 1) AND (event[i] LE 32) AND
            (event_mask NE 0) ) {

          temp_mask = event_mask << (event[i]-1);
          ESA_DIAG_printf (ESA_COMP_OS_EVENT, 3,
                           "Mask after shift left = %X", temp_mask);
          temp_mask = temp_mask >> 31;
          ESA_DIAG_printf (ESA_COMP_OS_EVENT, 3,
                           "Mask after shift right = %X", temp_mask);
          if (temp_mask NE 0)
             event_stat[i] = EVENT_ON;

       }
       ESA_DIAG_printf (ESA_COMP_OS_EVENT, 3,
                        "Event %d status was set to %d",
                        event[i], event_stat[i] );
   }

  /*
   *   Finish
   */

   exit :;

   ESA_DIAG_exit(ESA_COMP_OS_EVENT, 1 , func , rc );

   return rc;

}

/***********************************************************
 PROCEDURE NAME : convert_main_name
 DESCRIPTION    :
 INPUT          :
 OUTPUT         :
 RETURN VALUE   : none
**************************************************************/

static void chk_main_name(ESA_MAIN_NAME_typ main_name,
                          char              mnm[XMM_MAIN_SIZE+1],
                          char              jbn[XMM_JBN_SIZE+1],
                          int             * mnm_flag )
{

  *mnm_flag = 0;
  if (jbn NE NULL)
     strcpy(jbn, BLANK_JBN);

  switch (main_name) {

     case ESA_ACS_PROC:
           strcpy(mnm,"MAINACS ");
           *mnm_flag = 1;
           break;

     case ESA_ACD_PROC:
           strcpy(mnm,"MAINACD ");
           *mnm_flag = 1;
           break;

     case ESA_ONLI_PROC:
           strcpy(mnm,"MAINONI ");
           strcpy(jbn, ONLI_XMM_NAME);
           *mnm_flag = 1;
           break;

     case ESA_OFLI_PROC:
           strcpy(mnm,"MAINOFI ");
           break;

     case ESA_ACSDIAG_PROC:
           strcpy(mnm,"MAINDIAG");
           break;

     case ESA_ACSPRC_PROC:
           strcpy(mnm,"MAINDFR ");
           break;

     case ESA_KGEN_PROC:
           strcpy(mnm,"MAINKGN ");
           break;

     case ESA_QCR_PROC:
           strcpy(mnm,"MAINQCR ");
           break;

     case ESA_QPR_PROC:
           strcpy(mnm,"MAINQPR ");
           break;

     case ESA_OTHER_PROC:
           strcpy(mnm,"MAINOTHR");
           break;

     case ESA_ALL_PROC:
           strcpy(mnm,"*ALL*   ");
           break;

     default:
           strcpy(mnm,"MAINDFLT");

  } /* switch */

}
