/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
**This member is obsolete by WS10082. no one uses it anymore.**
* ==========================================================  *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSCADI in CTS.CMAC as well.                               *
*                                                             *
*                                                             *
***************************************************************
***************************************************************
*                                                             *
* Title            : Additional Info Manipulation             *
*                                                             *
* File Name        : addinfo.h                                *
*                                                             *
* Author           : Doron Cohen                              *
*                                                             *
* Creation Date    : 21/03/94                                 *
*                                                             *
* Description      :                                          *
*                                                             *
* Assumptions and                                             *
*   Considerations :                                          *
*                                                             *
***************************************************************

***************************************************************
* Mod.Id   Who      When     Description                      *
* -------- -------- -------- -------------------------------- *
*                                                             *
* spr125   Alex     07/09/95 Add new enum value KDT_KEYG      *
* PS0288   Alex     09/07/98 Add new param ADDINFO_dup_check  *
*                                       ADDINFO_prefix_search *
* ps0292   AS       23/07/98 Digital certificate support      *
* PS0349   Guy      06/12/98 Add new enum value KDT_KEYM      *
* IS0208   AmirH    30/12/03 Add ADDINFO_value_addval function*
* IS10100A NuritY   14/10/13 Add ADDINFO_Split_by_Prefix      *
*                            to support RACF command          *
*                            limitations.                     *
* sasc     NuritY   24/08/16 Adapt to CC changes.             *
* WS10070  KailasP  13/03/18 Add ADDINFO_list_getnextvalue    *
* WS10082  AvnerL   29/11/22 Member is now obsolete           *
***************************************************************
#ifndef ADDINFO_H_included
#define ADDINFO_H_included

  * sasc - not needed anymore because it is defined in GLOBS
#ifdef EXT
#undef EXT
#endif
#ifndef ADDINFO_owner
#define EXT extern
#else
#define EXT
#endif
*

#define ADDINFO_search                 radi010
#define ADDINFO_dup_check              radi011
#define ADDINFO_prefix_search          radi012
#define ADDINFO_insert                 radi013
#define ADDINFO_insert_cond            radi014
#define ADDINFO_addval                 radi015
#define ADDINFO_updval                 radi016
#define ADDINFO_delete                 radi017
#define ADDINFO_dump                   radi018
#define ADDINFO_alloc                  radi019
#define ADDINFO_free                   radi020
#define ADDINFO_copy                   radi021
#define ADDINFO_kdt_search             radi022
#define ADDINFO_kdt_modify             radi023
#define ADDINFO_mkt_check              radi024
#define ADDINFO_rkt_check              radi025
#define ADDINFO_concat                 radi026
#define ADDINFO_list_search            radi027
#define ADDINFO_list_diff              radi028
#define ERROR_dump                     radi029
#define ERROR_add                      radi030
#define ERROR_print                    radi031
#define ADDINFO_clear_value            radi032
#define ADDINFO_clear                  radi033
#define ADDINFO_get_value              radi034
#define ADDINFO_strip_prefix           radi035
#define ADDINFO_get_next               radi036
#define ADDINFO_copy_by_prefix         radi037
#define ADDINFO_convert_list_sep       radi038
#define ADDINFO_masupd                 radi039
#define ADDINFO_expand_from_buffer     radi040
#define ADDINFO_pack_to_buffer         radi041

#define ADDINFO_insert_dgt             radi051   * ps0292 *
#define ADDINFO_updval_dgt             radi052   * ps0292 *
#define ADDINFO_addval_dgt             radi053   * ps0292 *
#define ADDINFO_value_addval           radi054   * IS0208 *
#define ADDINFO_split_by_prefix        radi055   * IS10100A *
#define ADDINFO_count_by_prefix        radi056   * IS10100 *
#define ADDINFO_list_getnextvalue      radi057   * WS10070 *

#include ESA_API

#define KWD_FLAG_ON       "Y"
#define KWD_FLAG_OFF      "N"
#define ADDINFO_MAX_ST 1025
#define ERROR_MAX_ST   1025
#define ADDINFO_LIST_MAX_ITEM 513
#define ADDINFO_LIST_INSEP    ", "
#define ADDINFO_LIST_OUTSEP   ","
#define ADDINFO_LIST_TEXT_SEP_STR   ","
#define ADDINFO_LIST_HEX_SEP1_CHAR  0x01
#define ADDINFO_LIST_HEX_SEP2_CHAR  0x02

#define ADDINFO_size(num)                          \
        sizeof(ADDINFO_rec_typ)+((num-1)*sizeof(ONE_FIELD_rec_typ))

#define ADDINFO_empty(addinfo)                     \
        addinfo->num_pairs = 0

#define BUILD_STRING_SEPARATOR( str_sep, char_sep )  \
   (str_sep)[0] = (char_sep);                        \
   (str_sep)[1] = NULL_CHAR;

typedef enum {
 UNIQUE, MULTIPLE
 } ADDINFO_DUPLICATE;

* Keyword types for KDT *

typedef enum {
 KDT_KEY, KDT_FLG, KDT_IGN ,
 KDT_KEYG,  * spr125 *
 KDT_KEYM   * PS0349 *
 } ADDINFO_KWD_typ;

*
 *  MKTE - Mandatory Keywords Table Entry definition
 *

typedef struct {
   char kwd[MAX_RSS_ADDINFO_KWD_LEN+1];
 } ADDINFO_MKTE_rec_typ, *ADDINFO_MKTE_rec_ptr;

*
 *  RKTE - Forbiddeb Keywords Table Entry definition
 *

typedef struct {
   char kwd[MAX_RSS_ADDINFO_KWD_LEN+1];
 } ADDINFO_RKTE_rec_typ, *ADDINFO_RKTE_rec_ptr;

*
 *  KDTE - Keyword Description Entry definition
 *

typedef struct {
   char                  kwd[MAX_RSS_ADDINFO_KWD_LEN+1];
   ADDINFO_KWD_typ       type;
   char                  neg_kwd[MAX_RSS_ADDINFO_KWD_LEN+1];
   char                  *txtl;
   char                  *txtr;
 } ADDINFO_KDTE_rec_typ, *ADDINFO_KDTE_rec_ptr;

*
 *  PKB  - Primary Keyword Buffer definition
 *

typedef struct {
  int    no_pkb;
  char   name[MAX_RSS_ADDINFO_KWD_LEN+1];
  char   *buff;
  short  size;
 } ADDINFO_PKB_rec_typ, *ADDINFO_PKB_rec_ptr ;

typedef enum {
 ADDINFO_masupd_Merge,
 ADDINFO_masupd_NoMerge
} ADDINFO_masupd_mode;

static int i_snap;
static int j_snap;
static char m_snap;
#define    SNAP_AREA( title, ptr, len )                       \
  printf("\n>>> %s addr=%x len=%d <<<\n", title, ptr, len );  \
  for( i_snap=0; i_snap LT len; i_snap++)   {                 \
     if ( ( i_snap % 16 ) EQ 0 )                              \
       printf("%4.4X ",i_snap);                               \
     printf("%2.2X", *((char *)ptr+i_snap) );                 \
     if ( ( ( i_snap + 1 ) % 4 ) EQ 0 )                       \
           printf(" ");                                       \
     if ( ( ( i_snap + 1 ) % 16) EQ 0 )                       \
      {                                                       \
      for (j_snap=0;j_snap LT 16;j_snap++)                    \
       {                                                      \
       m_snap = (char) *((char *)ptr +i_snap-15+j_snap);      \
       if (isprint(m_snap) EQ 0)                              \
        printf(".");                                          \
       else                                                   \
        printf("%c",m_snap);                                  \
        };                                                    \
      printf("\n");                                           \
      };                                                      \
  };                                                          \
  printf("\n");                                               \
  printf("\n");


**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_search                             *
*                                                             *
* DESCRIPTION    : Search a keyword in additional info        *
*                                                             *
* INPUT          : kwd              -  keyword                *
*                  pair             -  entry to start search  *
*                  limit            -  max entries to search  *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ONE_FIELD_rec_ptr (or NULL if not found)   *
*                                                             *
**************************************************************
 EXT ONE_FIELD_rec_ptr ADDINFO_search (char *p_kwd,
                                       int   max_kwdlen,
                                       ONE_FIELD_rec_ptr p_pair,
                                       short p_limit) ;

 **************************************************************
 *                                                             *
 * Procedure Name : ADDINFO_dup_check                          *
 *                                                             *
 * Description    : Check duplicate keywords in ADDINFO        *
 *                                                             *
 * Input          : addinfo                                    *
 *                  keyword                                    *
 *                                                             *
 * Output         : ptr to found record                        *
 *                                                             *
 * Input/Output   :                                            *
 *                                                             *
 * Return Value   : ESA_RC, ESA_FATAL - Duplicate keywords     *
 *                                                             *
 * Side Effects   :                                            *
 *                                                             *
 * Comments       :                                            *
 *                                                             *
 **************************************************************

 ESA_RC ADDINFO_dup_check(ADDINFO_rec_typ           * addinfo,
                          char                      * kwd,
                          int                         max_kwdlen,
 * PS0288 *               int                         exists_when_empty,
                          ONE_FIELD_rec_ptr         * pair,
                          CTSAMSG_DEST_TABLE_rec_ptr  dest,
                          CTSAMSG_HANDLE_rec_ptr      msgs);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_prefix_search                      *
*                                                             *
* DESCRIPTION    : Search a keyword in additional info        *
*                                                             *
* INPUT          : kwd              -  keyword prefix         *
*                  pair             -  entry to start search  *
*                  limit            -  max entries to search  *
*                                                             *
* OUTPUT         : None                                       *
*                                                             *
* RETURN VALUE   : ONE_FIELD_rec_ptr (or NULL if not found)   *
*                                                             *
**************************************************************
ONE_FIELD_rec_ptr ADDINFO_prefix_search (char            *kwd,
                                      int          max_kwdlen,
 * PS0288 *                           int          exists_when_empty,
                                      ONE_FIELD_rec_ptr p_pair,
                                      short             p_limit);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_get_next                           *
*                                                             *
* DESCRIPTION    : Get next item from ADDINFO                 *
*                                                             *
* INPUT          : pair             -  entry to start loop    *
*                                                             *
* OUTPUT         : None                                       *
*                                                             *
* RETURN VALUE   : ONE_FIELD_rec_ptr (or NULL if finished)    *
*                                                             *
**************************************************************
EXT ONE_FIELD_rec_ptr ADDINFO_get_next(ADDINFO_rec_typ  *addinfo,
                                       ONE_FIELD_rec_ptr p_pair);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_insert                             *
*                                                             *
* DESCRIPTION    : Insert a keyword in additional info        *
*                                                             *
* INPUT          : type             -  addinfo type           *
*                  kwd              -  keyword                *
*                  value            -  value                  *
*                  addinfo          -  additional info        *
*                  dupflag          -  allow duplicate flag   *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************

EXT ESA_RC ADDINFO_insert  (ADDINFO_typ        type,
                            char               *kwd,
                            char               *val,
                            ADDINFO_rec_ptr    addinfo,
                            int                max_kwdlen,
                            int                max_vallen,
                            ADDINFO_DUPLICATE  dupflag);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_updval                             *
*                                                             *
* DESCRIPTION    : Update a value of an exiting entry         *
*                                                             *
* INPUT          : value     - Value to be added              *
*                  pair      - Addinto entry                  *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_updval (char               *addval,
                           int                max_vallen,
                           ONE_FIELD_rec_ptr  pair);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_addval                             *
*                                                             *
* DESCRIPTION    : Conactenate a value to an exiting one      *
*                                                             *
* INPUT          : addval    - Value to be added              *
*                  addsep    - Sperator between value list    *
*                  pair      - Addinto entry                  *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_addval (char                       *addval,
                           int                         max_vallen,
                           char                        addsep,
                           ONE_FIELD_rec_ptr           pair,
                           CTSAMSG_DEST_TABLE_rec_ptr  dest,
                           CTSAMSG_HANDLE_rec_ptr      msgs);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_delete                             *
*                                                             *
* DESCRIPTION    : Delete from addtional info area            *
*                  and compress                               *
*                                                             *
* INPUT          : Addinfo          -  additional info        *
*                  keyword          -  keyword                *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
ESA_RC ADDINFO_delete(ADDINFO_rec_ptr addinfo,
                      char            *keyword) ;

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_dump                               *
*                                                             *
* DESCRIPTION    : Dump Additional info area                  *
*                                                             *
* INPUT          : addinfo          -  additional info        *
*                  print_empty      -  1 print in any case    *
*                                   -  0 print only if value  *
*                                        length > 0           *
*                                                             *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_dump (ADDINFO_rec_ptr   p_addinfo,
                         int               print_empty);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_alloc                              *
*                                                             *
* DESCRIPTION    : Allocate addtional info area               *
*                                                             *
* INPUT          : max_pairs        -  max_pairs              *
*                : addinfo_ptr      -  additional info pointer*
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_alloc (char                      * func,
                          short                       max_pairs,
                          ADDINFO_rec_ptr           * addinfo_ptr,
                          CTSAMSG_DEST_TABLE_rec_ptr  dest,
                          CTSAMSG_HANDLE_rec_ptr      msgs);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_free                               *
*                                                             *
* DESCRIPTION    : Free addtional info area                   *
*                                                             *
* INPUT          : addinfo_ptr      -  additional info pointer*
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_free (ADDINFO_rec_ptr *addinfo_ptr);


**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_copy                               *
*                                                             *
* DESCRIPTION    : Copy additional info area                  *
*                                                             *
* INPUT          : to               -  additional info        *
*                  from             -  additional info        *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_copy  (ADDINFO_rec_ptr to,
                          ADDINFO_rec_ptr from);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_copy_by_prefix                     *
*                                                             *
* DESCRIPTION    : Copy ADDINFO pairs by prefix               *
*                                                             *
* INPUT          : addinfo_in                                 *
*                  prefix
*                                                             *
* OUTPUT         : addinfo_out                                *
*                                                             *
* RETURN VALUE   : ONE_FIELD_rec_ptr (or NULL if finished)    *
*                                                             *
**************************************************************
ESA_RC ADDINFO_copy_by_prefix(char              *prefix,
                              int                max_kwdlen,
                              int                max_vallen,
                              int                drop_prefix,
                              ADDINFO_rec_typ   *addinfo_in,
                              ADDINFO_rec_typ   *addinfo_out);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_kdt_search                         *
*                                                             *
* DESCRIPTION    : Locate a keyword in KDT (Keyword           *
*                  Description Table )                        *
*                                                             *
* INPUT          : p_kwd            - keyword to be searched  *
*                  p_desc           - KDTE to start search    *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ADDINFO_KDTE_rec_ptr                       *
*                                                             *
**************************************************************
EXT ADDINFO_KDTE_rec_ptr ADDINFO_kdt_search   ( char *kwd,
                              ADDINFO_KDTE_rec_ptr   p_kdte);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_mkt_check                          *
*                                                             *
* DESCRIPTION    : Check that all mandatory keywords were     *
*                  specified                                  *
*                                                             *
* INPUT          : p_func        - function name              *
*                  p_numaddi     - number of addinfo buffers  *
*                  p_addinfo     - pointer to addinfo vector  *
*                  p_mkte        - MKTE to start with         *
*                  p_kdt         - KDT for commnad            *
*                  p_dest        - Vector of error msg dests  *
*                  p_msgs        - Error messages buffer      *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_mkt_check (char                   *p_func,
                              short                  p_numaddi,
                              ADDINFO_rec_ptr        *p_addinfo,
                              ADDINFO_MKTE_rec_ptr   p_mkte,
                              ADDINFO_KDTE_rec_ptr   p_kdt,
                              CTSAMSG_DEST_TABLE_rec_ptr p_dest,
                              CTSAMSG_HANDLE_rec_ptr p_msgs);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_kdt_modify                         *
*                                                             *
* DESCRIPTION    : Modify the textl, tetxr for specified      *
*                  entry in KDT table                         *
*                                                             *
* INPUT          : kwd              - keyword                 *
*                  p_kdte           - KDTE table              *
*                  char             - text left               *
*                  char             - text right              *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_OK, ESA_ERR - specified keyword        *
*                                    not found                *
*                                                             *
**************************************************************
ESA_RC ADDINFO_kdt_modify(char                 *kwd,
                          ADDINFO_KDTE_rec_ptr  p_kdte,
                          char                 *txtl,
                          char                 *txtr) ;

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_rkt_check                          *
*                                                             *
* DESCRIPTION    : verify that no reserved keywords are used  *
*                                                             *
* INPUT          : func          - function name              *
*                  numaddi       - addinfo buffer             *
*                  addinfo       - addinfo vector pointer     *
*                  rkte          - RKTE to start with         *
*                  p_dest        - Vector of error msg dests  *
*                  p_msgs        - Error messages buffer      *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_rkt_check(char                     *p_func,
                            short                      p_numaddi,
                            ADDINFO_rec_ptr           *p_addinfo,
                            ADDINFO_RKTE_rec_ptr       p_rkte,
                            CTSAMSG_DEST_TABLE_rec_ptr p_dest,
                            CTSAMSG_HANDLE_rec_ptr     p_msgs);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_concat                             *
*                                                             *
* DESCRIPTION    : Concatenate Additional Info buffers to     *
*                  command buffer                             *
*                                                             *
* INPUT          :                                            *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_concat( char                      *p_func,
                           char                      *p_cmd,
                           short                      p_numaddi,
                           ADDINFO_rec_ptr           *p_addinfo,
                           ADDINFO_KDTE_rec_ptr       p_kdt,
                           CTSAMSG_DEST_TABLE_rec_ptr p_dest,
                           CTSAMSG_HANDLE_rec_ptr     p_msgs);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_list_search                        *
*                                                             *
* DESCRIPTION    : Locate an item in addinfo list value       *
*                                                             *
* INPUT          : list     - list string (items spearted by  *
*                             commas)                         *
*                  item     - searched item                   *
*                  sep      - string of list seperators       *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : Pointer to list item or NULL               *
*                                                             *
**************************************************************
EXT char *ADDINFO_list_search (char *list, char *item, char  sep);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_list_diff                          *
*                                                             *
* DESCRIPTION    : Build list of items found in first         *
*                  list, but not in second list .             *
*                                                             *
* INPUT          : inlist1  - first list                      *
*                  inlist2  - second list                     *
*                  outlist  - list of added items             *
*                  insep    - string of list sperators        *
*                  outsep   - sepearator in output list       *
*                                                             *
* OUTPUT         : outlist  - list of added items             *
*                                                             *
*                  is a preallocated output buffer in the size*
*                  of the first input list.                   *
*                                                             *
* RETURN VALUE   : ESA_OK                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_list_diff (char *inlist1, char *inlist2,
                              char *outlist, char  insep,
                              char  outsep);

**************************************************************
*                                                             *
* PROCEDURE NAME : ERROR_add                                  *
*                                                             *
* DESCRIPTION    : Add error message to error buffer          *
*                                                             *
* INPUT          : p_line   - line to add to buffer           *
*                  p_err    - error buffer                    *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ERROR_add (char                 *p_line,
                      ERR_STRUCT_rec_ptr   p_err   );

**************************************************************
*                                                             *
* PROCEDURE NAME : ERROR_dump                                 *
*                                                             *
* DESCRIPTION    : Dump error message buffer                  *
*                                                             *
* INPUT          : p_err    - error buffer                    *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ERROR_dump (ERR_STRUCT_rec_ptr p_err) ;

**************************************************************
*                                                             *
* PROCEDURE NAME : ERROR_print                                *
*                                                             *
* DESCRIPTION    : Print error message buffer to stdout       *
*                                                             *
* INPUT          : p_err    - error buffer                    *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ERROR_print (ERR_STRUCT_rec_ptr p_err) ;


**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_clear_value                        *
*                                                             *
* DESCRIPTION    : Clear value of entering pair               *
*                                                             *
* INPUT          : pair      - Addinto entry                  *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_clear_value(ONE_FIELD_rec_ptr  pair);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_clear                              *
*                                                             *
* DESCRIPTION    : Clear Addtional info area                  *
*                                                             *
* INPUT          : Addinfo          -  additional info        *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************
EXT ESA_RC ADDINFO_clear(ADDINFO_rec_ptr addinfo);

**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_insert_cond                        *
*                                                             *
* DESCRIPTION    : Search a kwd1 with val1 in addinfo1        *
*                  and if it matches, add kwd2 with val2      *
*                  to addinfo2 .                              *
*                                                             *
* INPUT          : kwd1                                       *
*                  val1                                       *
*                  addinfo1                                   *
*                  kwd2                                       *
*                  val2                                       *
*                                                             *
* OUTPUT         : addinfo2                                   *
*                                                             *
* RETURN VALUE   : none                                       *
*                                                             *
**************************************************************

EXT void ADDINFO_insert_cond( char              * kwd1,
                              char              * val1,
                              ADDINFO_rec_typ   * addinfo1,
                              char              * kwd2,
                              char              * val2,
                              int                 max_kwdlen,
                              int                 max_vallen,
                              ADDINFO_rec_typ   * addinfo2 ) ;

 **************************************************************
 *                                                             *
 * PROCEDURE NAME : ADDINFO_get_value                          *
 *                                                             *
 * DESCRIPTION    : Get parameter from ADDINFO                 *
 *                                                             *
 * INPUT          : addinfo                                    *
 *                  parameter_name                             *
 *                                                             *
 * OUTPUT         : parameter_value                            *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 **************************************************************
 EXT ESA_RC ADDINFO_get_value(ADDINFO_rec_typ    * addinfo,
                              char               * parameter_name,
                              char               * parameter_val,
                              int                  max_kwdlen,
                              int                  max_vallen);

 **************************************************************
 *                                                             *
 * PROCEDURE NAME : ADDINFO_strip_prefix                       *
 *                                                             *
 * DESCRIPTION    : Copy prefix keyword from source addinfo    *
 *                  to target addinfo with removing prefix from*
 *                  from keyword                               *
 *                                                             *
 * INPUT          : addinfo                                    *
 *                  keyword                                    *
 *                                                             *
 * OUTPUT         : laddinfo                                   *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 **************************************************************
 EXT ESA_RC ADDINFO_strip_prefix(char               * keyword,
                                 int                  max_kwdlen,
                                 int                  max_vallen,
                                 ADDINFO_rec_typ    * addinfo,
                                 ADDINFO_rec_typ    * laddinfo);

 **************************************************************
 *                                                             *
 * PROCEDURE NAME : ADDINFO_convert_list_sep                   *
 *                                                             *
 * DESCRIPTION    : Convert string containing old seperators   *
 *                  to new seperators                          *
 *                                                             *
 * INPUT          : src_entry_sep - source entry sep           *
 *                  src_subf_sep  - source subfield  sep       *
 *                  trg_entry_sep - target entry sep           *
 *                  trg_subf_sep  - target subfield  sep       *
 *                                                             *
 * input/output   : buff    - string buffer                    *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 **************************************************************

 EXT ESA_RC ADDINFO_convert_list_sep(char    * buff,
                                     char      src_entry_sep,
                                     char      src_subf_sep,
                                     char      trg_entry_sep,
                                     char      trg_subf_sep);

 **************************************************************
 *                                                             *
 * PROCEDURE NAME : ADDINFO_masupd                             *
 *                                                             *
 * DESCRIPTION    : Massive updates of field in addinfo_to by  *
 *                  values of same_name fields in addinfo_from *
 *                                                             *
 * RETURN VALUE   : NONE                                       *
 *                                                             *
 **************************************************************

EXT void ADDINFO_masupd(ADDINFO_rec_ptr              addinfo_to,
                        ADDINFO_rec_ptr              addinfo_from,
                        ADDINFO_masupd_mode          mode,
                        CTSAMSG_DEST_TABLE_rec_typ * dest,
                        CTSAMSG_HANDLE_rec_typ     * msgs);

 **************************************************************
 *                                                             *
 * PROCEDURE NAME : ADDINFO_expand_from_buffer                 *
 *                                                             *
 * DESCRIPTION    : build addinfo object from a buffer of      *
 *                  null-terminated pairs (kwd,type,value).    *
 *                                                             *
 * RETURN VALUE   : NONE                                       *
 *                                                             *
 **************************************************************

ESA_RC ADDINFO_expand_from_buffer (char *buffer,
                                   int  buffer_length,
                                   ADDINFO_rec_ptr addinfo);

 **************************************************************
 *                                                             *
 * PROCEDURE NAME : ADDINFO_pack_to_buffer                     *
 *                                                             *
 * DESCRIPTION    : build a buffer of null-terminated strings  *
 *                  from addinfo object.                       *
 *                                                             *
 * RETURN VALUE   : NONE                                       *
 *                                                             *
 **************************************************************

ESA_RC ADDINFO_pack_to_buffer (char *buffer,
                              int  buffer_length,
                              ADDINFO_rec_ptr addinfo,
                              int *actual_buffer_length);

* ps0292 *

ESA_RC ADDINFO_insert_dgt  (ADDINFO_typ        type,
                            char               *kwd,
                            char               *val,
                            int                vallen,
                            ADDINFO_rec_ptr    addinfo,
                            int                max_kwdlen,
                            int                max_vallen,
                            ADDINFO_DUPLICATE  dupflag);

ESA_RC ADDINFO_updval_dgt (char               *addval,
                           int                vallen,
                           int                max_vallen,
                           ONE_FIELD_rec_ptr  pair);

ESA_RC ADDINFO_addval_dgt (char                      *addval,
                           int                        vallen,
                           int                        max_vallen,
                           char                       addsep_c,
                           ONE_FIELD_rec_ptr          pair,
                           CTSAMSG_DEST_TABLE_rec_ptr p_dest,
                           CTSAMSG_HANDLE_rec_ptr     p_msgs);

ESA_RC ADDINFO_value_addval (char                     *addval,
                             int                       addval_len,
                             char                     *current_val,
                             int                      *current_val_len,
                             int                       max_vallen,
                             char                      addsep_c,
                             ADMIN_PARAMS_rec_typ     *admin_params);

* IS10100A - start   *
**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_split_by_prefix                    *
*                                                             *
* DESCRIPTION    : split the addinfo to 2 or more addinfos:   *
*                  - entries matching the prefix are copied   *
*                    to to_prefix.                            *
*                  - all other entries are copied to          *
*                    to_general.                              *
*                                                             *
* INPUT          : from - input addinfo.                      *
*                  to_prefix_num - number of addinfos in      *
*                                  to_prefix vector.          *
*                  prefix - prefix for split.                 *
*                                                             *
* OUTPUT         : to_general - addinfo for non-prefix        *
*                               entries.                      *
*                  to_prefix - vector of pointers to addinfos *
*                              for prefix entries.            *
*                                                             *
* RETURN VALUE   : ESA_OK - when all OK.                      *
*                  ESA_ERR - when output overflow.            *
*                                                             *
*                                                             *
**************************************************************
ESA_RC  ADDINFO_split_by_prefix(ADDINFO_rec_ptr  from,
                                ADDINFO_rec_ptr  to_general,
                                ADDINFO_rec_ptr *to_prefix,
                                int              to_prefix_num,
                                char            *prefix);
* IS10100A - end     *
* IS10100 - start *
**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_count_by_prefix                    *
*                                                             *
* DESCRIPTION    : Count the number of entries with a         *
*                  specific prefix.                           *
*                                                             *
* INPUT          : addinfo - ut addinfo.                      *
*                  prefix - prefix for count.                 *
*                  prefix_with_data - count only pairs with   *
*                                     data (TRUE) or all      *
*                                     (FALSE).                *
*                  other_with_data - count only pairs with    *
*                                    data (TRUE) or all       *
*                                    (FALSE).                 *
*                                                             *
* OUTPUT         : prefix_num - number of entries with        *
*                               prefix.                       *
*                  other_num - number of other entries.       *
*                                                             *
* RETURN VALUE   :                                            *
*                                                             *
**************************************************************
void  ADDINFO_count_by_prefix(ADDINFO_rec_ptr  addinfo,
                              char            *prefix,
                              int             *prefix_num,
                              int             *other_num,
                              int              prefix_with_data,
                              int              other_with_data);

* IS10100 - end    *
* WS10070 - start *
**************************************************************
*                                                             *
* PROCEDURE NAME : ADDINFO_list_getnextvalue                  *
*                                                             *
* DESCRIPTION    : Get one value from list field.             *
*                  The routine can be used to get list values *
*                  one by one                                 *
*                  Note:                                      *
*                  This routine does not handle sub-fields.   *
*                                                             *
*                                                             *
* INPUT          : pair - add_info pair to be parsed          *
*                  field_separator - delimiter to get field   *
*                                    from list                *
*                  lasts - lasts position to continue getting *
*                          values lasts must be NULL          *
*                          when called for first time         *
*                                                             *
* OUTPUT         : token - A copy of the next value from the  *
*                          list-type value, NULL_CHAR when    *
*                          end of values. Token should be     *
*                          defined by the caller as a string  *
*                          variable with length enough to get *
*                          the longest expected value.        *
*                                                             *
* RETURN VALUE   : ESA_OK                                     *
*                                                             *
**************************************************************
ESA_RC ADDINFO_list_getnextvalue(ONE_FIELD_rec_ptr pair,
                                char field_separator,
                                char *token,
                                char **lasts);

* WS10070 - end    *
#endif  * ADDINFO_H_included *
