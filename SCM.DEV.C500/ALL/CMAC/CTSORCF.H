/* IS10174

    This member is obsolete

*/
/**************************************************************
*                                                             *
*  <!><!> ATTENTION <!><!>                                    *
*  =-=-=-=-=-=-=-=-=-=-=-=                                    *
*                                                             *
*  When updating this member, do not forget to update         *
*  CTSRRCF in CTS.CMAC as well.                               *
*                                                             *
*                                                             *
**************************************************************/
/**************************************************************
*                                                             *
* Title            : Racf                                     *
*                                                             *
* File Name        : racf.h                                   *
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
**************************************************************/

/**************************************************************
* Mod.Id   Who      When     Description                      *
* -------- -------- -------- -------------------------------- *
* dc1005   Doron    10/05/95 Add function RACF_extract_defug  *
* r0003    Doron    28/05/95 Increase size of RACF output line*
* WSAR1001 Alex     29/04/96 Add Get RACF options facility    *
* ps0231   Alex     28/12/97 RACF extract res/resACL functions*
* ps0238   Alex     16/02/98 Issue SETROPTS for GENERIC prf   *
* ps0292   Alex     22/07/98 Digital certificate support      *
* ps0320   Guy      22/10/98 Removed RACF_CMDOUT_typ and      *
*                            RACF_CMD_RC_typ                  *
* ps0460   ShmuelK  23/04/01 Support PROTECTED RACF user      *
* PS0491   ShmuelK  18/11/01 Support RACF_version_at_least    *
* ws2427   ShmuelK  13/02/02 Support RACF Universal Groups    *
* ps0499   ShmuelK  07/03/02 Support RACF for z/os 1.3        *
* ps0511   ShmuelK  24/06/02 Support RACF for z/os 1.4        *
* ws2532   AmirH    12/05/04 Support RACF for z/os 1.5        *
* IS0272   Michael  10/06/04 Update C31B libs with old fixes  *
* IS0263   AvnerL   05/08/04 Support RACF for z/os 1.6        *
* WS2350   AmirH    22/09/04 Add script output message tag    *
* IS0322   AvnerL   03/08/05 Support RACF for z/os 1.7        *
* ws2549   Yonatan  09/11/05 New parm MSG_CONNGRP_ERR.        *
* BS2703   yonim    26/02/06 Support  Multi subgroup keyword  *
* IS0369   AvnerL   06/09/06 Support RACF for z/os 1.8        *
* IS10002  AvnerL   31/01/07 set VERIFY_PASSWORD_BY_LOGIN val *
* WS10004  AvnerL   05/06/07 enlarge ICHEINTY output to 64K.  *
* BS10006  MeirS    28/08/07 REVOKE/RESUME for z/os 1.7       *
* WS10013  AvnerL   06/11/07 Add LOGIN_INTERCEPT parm.        *
* BS10008  AvnerL   24/03/08 Pass max_connect_number.         *
* WS10018  AvnerL   07/07/08 Support for RACF Z/OS 1.10 & 1.9 *
* IS10061  AvnerL   12/09/11 Support zOS 1.11,1.12,1.13       *
* IS10070  AvnerL   07/02/12 Update RACF versions supported.  *
* IS10076  Nurity   07/05/12 Support Custom Fields.           *
* IS10080  AvnerL   24/01/13 Support zOS 2.1                  *
* IS10100  NuritY   21/07/13 Support CTSA custom fields and   *
*                            revoke reason.                   *
* IS10100A NuritY   14/10/13 Support RACF command limitations.*
* WS10043  NuritY   06/01/14 Enhance locked accounts support  *
*                            to display the revoke reason.    *
* IS10132  AvnerL   11/06/15 Support zOS 2.2                  *
* WS10054  SeligT   26/07/16 Add ENQ/DEQ SYSTEMS RNL Parameter*
* IS10139  SeligT   28/08/16 Update OS Functions for Agents   *
* sasc     NuritY   24/08/16 Copy from CTSRRCF and adapt to   *
*                            CC changes.                      *
* SAS2IBMT SeligT   15/02/17 SAS/C to IBM C Conversion Project*
* groups   AvnerL   25/06/17 5K groups cause CTS3223E.        *
**************************************************************/

/* IS10174
#ifndef RACF_H_included
#define RACF_H_included

 @* sasc - not needed anymore because it is defined in GLOBS
#ifdef EXT
#undef EXT
#endif
#ifndef RACF_owner
#define EXT extern
#else
#define EXT
#endif
*@

#include ESA_API
#include ESA_INIT                                         @* IS10100 *@
@* sasc   #include MVS_OS_CLI                             *@
@* asac   #include API_ADDINFO                            *@
#include MVS_OSC_CLI                                       @* sasc   *@
#include API_C_ADDINFO                                     @* sasc   *@
#include MVS_C_SRV                                         @* IS10139 *@

#define RACF_get_list_count            rrcf010
#define RACF_alloc_output_lines        rrcf011
#define RACF_extract                   rrcf012
#define RACF_extract_field             rrcf013
#define RACF_extract_defug             rrcf014
#define RACF_output_to_addinfo         rrcf015
#define RACF_get_field                 rrcf016
#define RACF_get_next_parameter        rrcf017
#define RACF_get_parameter             rrcf018
@* SAS2IBMT #define RACF_get_classes   rrcf019                       *@
#define RACF_revoke_date               rrcf020
#define RACF_version_get               rrcf022
#define RACF_version_check             rrcf023
#define RACF_chk_user                  rrcf024
#define RACF_chk_group                 rrcf025
#define RACF_chk_conn                  rrcf026
#define RACF_chk_res                   rrcf027
#define RACF_chk_hilvds                rrcf028
#define RACF_chk_ace                   rrcf029
#define RACF_chk_cond_ace              rrcf030
#define RACF_get_option                rrcf031  @* WSAR1001 *@
#define RACF_get_option_value          rrcf032  @* WSAR1001 *@
#define RACF_refresh_class             rrcf033  @* WSAR1001 *@
#define RACF_is_raclisted_class        rrcf034  @* WSAR1001 *@
#define RACF_is_global_class           rrcf035  @* WSAR1001 *@
#define RACF_is_generic_class          rrcf036  @* WSAR1001 *@
#define RACF_refresh                   rrcf037  @* WSAR1001 *@
#define RACF_extract_res               rrcf038  @* ps0231   *@
#define RACF_extract_ace               rrcf039  @* ps0231   *@
#define RACF_chk_res_class             rrcf040  @* ps0231   *@
#define RACF_is_active_class           rrcf041  @* ps0231   *@
#define RACF_getdig_info               rrcf042  @* ps0292   *@
#define RACF_Dig_cert_add              rrcf043  @* ps0292   *@
#define RACF_Dig_cert_upd              rrcf044  @* ps0292   *@
#define RACF_strtok                    rrcf045  @* ps0292   *@
#define RACF_Dig_cert_get_info         rrcf046  @* ps0292   *@
#define RACF_pro_user                  rrcf047  @* ps0460   *@
#define RACF_version_at_least          rrcf048  @* ps0491   *@
#define RACF_uni_group                 rrcf049  @* ws2427   *@
@* IS10100 #define RACF_Create_CField_Blocks   rrcf050   * IS10076  *@
#define RACF_Init_Custom_Fields        rrcf050  @* IS10100  *@
#define RACF_Free_CField_Blocks        rrcf051  @* IS10076  *@
#define RACF_Init_Dynamic_KDTs         rrcf052  @* IS10076  *@
#define RACF_Free_Dynamic_KDT          rrcf053  @* IS10076  *@
#define RACF_Search_CField_Block       rrcf054  @* IS10076  *@
#define RACF_Free_CTSACF_Block         rrcf055  @* IS10100  *@
#define RACF_IsAct_CTSACF_Feature      rrcf056  @* IS10100  *@
#define RACF_Update_Revoke_Reason      rrcf057  @* IS10100  *@
#define RACF_Handle_CTSACF_Features    rrcf058  @* IS10100  *@
#define RACF_do_AddAlt_cmd             rrcf059  @* IS10100  *@
#define RACF_IsIt_CTSACF_cf            rrcf060  @* IS10100  *@
#define RACF_Revoke_Reason_Code2Name   rrcf061  @* WS10043  *@

#define MSG_CONNGRP_ERR             "MSG_CONNGRP_ERR" @* ws2549 *@
#define MSG_CONNGRP_ERR_ON          'Y'               @* ws2549 *@
#define MSG_CONNGRP_ERR_OFF         'N'               @* ws2549 *@

 @* BS10006 REVOKE & RESUME parameters         ==> start        *@
 @*         RESUME_CLEARS_REVOKEDATE if = 'Y' clears the        *@
 @*         REVOKE_DATE when RESUME requested                   *@
 @*         REVOKE_CLEARS_REVOKEDATE if = 'Y' clears the        *@
 @*         RESUME_DATE when REVOKE requested                   *@
#define RES_CLR_REV                 "RESUME_CLEARS_REVOKEDATE"
#define REV_CLR_RES                 "REVOKE_CLEARS_RESUMEDATE"
#define RES_CLR_REV_ON              'Y'
#define RES_CLR_REV_OFF             'N'
#define REV_CLR_RES_ON              'Y'
#define REV_CLR_RES_OFF             'N'
 @* BS10006 REVOKE & RESUME parameters         ==> end          *@
#define LOCKED_ACCOUNT_CF "LOCKED_ACCOUNT_CFNAME"   @* IS10100 *@

#define DIG_CERT_INDEX_FIELD        "%%CERTCT"        @* ps0292   *@
#define DIG_CERT_DATA_FIELD         "DCERT.CERTDATA"  @* ps0292   *@
#define DIG_CERT_PROFILE_PRFX       "DCERT.CERTNAME." @* ps0292   *@
#define MAX_API_ST                   4096
#define RACF_OUTPUT_SIZE           131072    @*groups  for user/group*@
@*groups #define RACF_OUTPUT_SIZE   65536     *WS10004 for user/group*@
@*WS10004 #define RACF_OUTPUT_SIZE   32768    @*BS2703 for user/group*@

@*BS2703 #define RACF_OUTPUT_SIZE            16384 *@
                            @* r0003 increased to 16k from 4k  *@
#define RACF_CNN_OUTPUT_SIZE          512  @* for connection   *@
                            @* r0003 increased to 512 from 500 *@
#define RACF_OPERATION_NEXT    "next    "
#define RACF_OPERATION_LOCATE  "locate  "
#define RACF_USER_PROFILE      "user    "
#define RACF_GROUP_PROFILE     "group   "
@*ws2427              *@
#define RACF_UNIGROUP_PROFILE  "unigroup"
#define RACF_UNIDIGIT_MAX      6

@*PS0491:racf releases*@
@* IS10070  #define RACF190 "1090"          drop old RACF version *@
@* IS10070  #define RACF192 "1092"          drop old RACF version *@
@* IS10070  #define RACF21  "2010"          drop old RACF version *@
@* IS10070  #define RACF22  "2020"          drop old RACF version *@
@* IS10070  #define RACF23  "2030"          drop old RACF version *@
@* IS10070  #define RACF24  "2040"          drop old RACF version *@
@* IS10070  #define RACF26  "2060"          drop old RACF version *@
@* IS10070  #define RACF28  "2608"          drop old RACF version *@
@* IS10070  #define RACF210 "7703"          drop old RACF version *@
@* IS10070  #define RACFZ12 "7705"          drop old RACF version *@
@* IS10070  #define RACFZ13 "7706"          drop old RACF version *@
#define RACFZ14 "7707"
#define RACFZ15 "7708"      @* WS2532 *@
#define RACFZ16 "7709"      @* is0263 *@
#define RACFZ17 "7720"      @* is0322 *@
#define RACFZ18 "7730"      @* is0369 *@
#define RACFZ19 "7740"      @* ws10018*@
#define RACFZ10 "7750"      @* ws10018*@
#define RACFZ111 "7760"      @* ws10061*@
#define RACFZ112 "7770"      @* ws10061*@
#define RACFZ113 "7780"      @* IS10061*@
#define RACFZ21  "7790"      @* IS10080 *@
#define RACFZ22  "77A0"      @* IS10132 *@
@*PS0491: end racf releases*@

@* Script output message tag (See CTSASRCH script)             WS2350 *@
#define RACF_SCRIPT_OUTMSG_TAG "<MSG>"
#define RACF_SCRIPT_OUTMSG_TAG_LEN sizeof( RACF_SCRIPT_OUTMSG_TAG ) - 1

typedef enum {
   RACF_NO, RACF_YES
} RACF_FLAG_typ;

@* PS0320 - Deleted
typedef enum {
   CMDOUT_YES, CMDOUT_NO, CMDOUT_ERR
} RACF_CMDOUT_typ;

typedef enum {
   MAX_OK_RC_0 = 0, MAX_OK_RC_4 = 4
} RACF_CMD_RC_typ;
*@

@* access to racf data base *@

typedef struct {
  int   max_len;                            @* max  length   *@
  int   used_len;                           @* used length   *@
  char  buffer[RACF_OUTPUT_SIZE];           @* output buffer *@
  } RACF_OUTPUT_LINE_rec_typ, *RACF_OUTPUT_LINE_rec_ptr;

typedef char ruser[RCF_USER_NAME_LEN];      @* ws2427        *@
typedef struct {
  int   max_users;                          @* max  length   *@
  int   used_users;                         @* used length   *@
  ruser users[1];                           @* users list    *@
  } RACF_USERS_LIST_rec_typ, *RACF_USERS_LIST_rec_ptr;

typedef struct {
  int   max_len;                            @* max  length   *@
  int   used_len;                           @* used length   *@
  char  buffer[RACF_CNN_OUTPUT_SIZE];       @* output buffer *@
  } RACF_CNN_OUTPUT_LINE_rec_typ, *RACF_CNN_OUTPUT_LINE_rec_ptr;

@* ps0231 *@
#define RACF_ACE_AREA_SIZE       512  @* for ace's *@

typedef struct {
  int   max_len;                            @* max  length   *@
  int   used_len;                           @* used length   *@
  char  buffer[RACF_ACE_AREA_SIZE];         @* output buffer *@
} RACF_ACE_AREA_rec_typ, *RACF_ACE_AREA_rec_ptr;

 @* WSAR1001 *@
#define RACF_OPTIONS_OUTPUT_BUF_LEN   8096
typedef struct {
  int   max_len;                            @* max  length   *@
  int   used_len;                           @* used length   *@
  char  *buffer;                            @* output buffer *@
 } RACF_OPTIONS_rec_typ, RACF_OPTIONS_rec_ptr;

 @*  Custom Fields Support (IS10076) - Start                        *@

 @*   Custom Fields type table.                                   *@
 @*   This list was created according to the list of values in    *@
 @*   "OS Security Server RACF Macros and Interfaces" Appendix D  *@
 @*   The type received from RACF is used as an index (-1) to     *@
 @*   this table to get the character representation of the field *@
 @*   type.                                                       *@
static char  RACF_CFEILD_type_tab[] = {'C', 'F', 'B', 'H'};
static int   RACF_CFEILD_type_tab_count =
        sizeof(RACF_CFEILD_type_tab) / sizeof(RACF_CFEILD_type_tab[0]);

 @* RACF Custom Field output buffer   *@
#define RACF_CFIELD_OUTPUT_BUF_LEN   512
typedef struct {
  int   max_len;                            @* max  length   *@
  int   used_len;                           @* used length   *@
  char  buffer[0];                          @* output buffer *@
  } RACF_CFIELD_OUTPUT_rec_typ, *RACF_CFIELD_OUTPUT_rec_ptr;

typedef struct {
  char  name[16];                    @* field name CSDATA.name      *@
  char  type;                        @* field type: B, C, F, H      *@
  char  label[41];                   @* display label               *@
  short max_length;                  @* maximum length (for C and H *@
  int   min_value;                   @* minimum value (for F)       *@
  int   max_value;                   @* minimum value (for F)       *@
  char  first[9];                    @* first character rule (for C)*@
  char  other[9];                    @* other character rule (for C)*@
  char  mixed;                       @* mixed case (Y / N)          *@
 } RACF_CUSTOM_FIELD_rec_typ, *RACF_CUSTOM_FIELD_rec_ptr;

#define RACF_CUSTOM_FIELDS_BLOCK_LEN    4096

typedef struct _CUSTOM_FIELDS_BLOCK {
  char     cfb_eyec[4];              @* eye catcher = CFB           *@
  void   * next_block;               @* -> next block               *@
  short    used_fields_count;        @* # of used entries           *@
  RACF_CUSTOM_FIELD_rec_typ fields[0]; @* area for custom fields.   *@
 } RACF_CUSTOM_FIELDS_block_typ, *RACF_CUSTOM_FIELDS_block_ptr;


@*****************************************************************
 *
 *   Dynamic KDT types
 *
 *   When changed, RACF_dynkdt_req in CTSRKDT should be changed
 *   accordingly.
 *
 *****************************************************************@
typedef enum {                         @* Dynamic LDT types         *@
   RACF_INSERT_USER = 1,
   RACF_UPDATE_USER,
   RACF_INSERT_GROUP,
   RACF_UPDATE_GROUP
} RACF_DYNKDT_typ;

typedef struct _DYNAMIC_KDT_REQ {
  ADDINFO_KDTE_rec_typ  *stat_kdt;      @* -> static kdt. *@
  int                    stat_kdt_entnum; @* number of entries in kdt *@
  int                    negative;      @* fill negative (T / F)    *@
 } RACF_DYNKDT_REQ_typ, *RACF_DYNKDT_REQ_ptr;

typedef struct _DYNAMIC_KDT {
  ADDINFO_KDTE_rec_ptr p_kdt;        @* -> kdt.                     *@
  int                  dynalloc;     @* TRUE / FALSE.               *@
 } RACF_DYNAMIC_KDT_info_typ, *RACF_DYNAMIC_KDT_info_ptr;

 @*  Custom Fields Support (IS10076) - End                          *@

@* ------------------- IS10100 - start -------------------------- *@

@*  IDs for features supported by CTSA custom fields  *@
typedef enum {
  NONE,
  LOCKED_ACCOUNT
} RACF_CTSACF_FEATURE_typ;

@*  Names of features supported by CTSA custom fields             *@
@*                                                                *@
@*  The order of the names should match the order of the          *@
@*  features in RACF_CTSACF_FEATURE_typ.                          *@
@*                                                                *@
@*  Name length should not exceed 30 characters.                  *@
static char   * RACF_CTSACF_feature_names[] = {
     "NONE",
     "LOCKED ACCOUNT"
   };

@*  CTSACF active values.                            *@
typedef enum {
  ACT_NO,
  ACT_YES,
  ACT_ERROR
} RACF_CTSACF_ACT_typ;

@*  CTSA custom field attributes entry structure   *@
typedef  struct _CTSACF_ATTR{
  char CF_def_name[9];                   @* Custom Field name       *@
  char rssparm_name[RSSPRM_NAME_LEN+1];  @* custom field name parm  *@
  RACF_CTSACF_FEATURE_typ CF_Feature;    @* feature id              *@
  char CF_type;                          @* custom field type.      *@
  int  CF_maxlen;                        @* maximum length allowed. *@
  int  CF_minval;                        @* minimum value allowed.  *@
  int  CF_maxval;                        @* maximum value allowed.  *@
  char CF_first;                         @* first char restriction. *@
  char CF_other;                         @* other chars restriction.*@
  int  CF_mixed;                         @* lowercase allowed?      *@
} RACF_CTSACF_ATTR_rec_typ, *RACF_CTSACF_ATTR_rec_ptr;

@*  CTSA custom field - structure of attribute block descriptor *@
typedef  struct _CTSACF_ATTR_BLK{
  char   type[6];                        @* USER / GROUP             *@
  short  attr_num;                       @* Number of CTSACF fields  *@
  RACF_CTSACF_ATTR_rec_typ *attr_tab;    @* -> CTSACF attributes blk *@
} RACF_CTSACF_ATTR_blk_typ, *RACF_CTSACF_ATTR_blk_ptr;

@*  List of CTSA internal user custom fields and their attributes *@
static RACF_CTSACF_ATTR_rec_typ RACF_u_ctsacfs_attr[] = {
  "CTSLKACT", LOCKED_ACCOUNT_CF, LOCKED_ACCOUNT, 'C', 1, 0, 0,
                                                         3, 3, FALSE
  };

@*  List of CTSA internal custom fields atrtibute tables.          *@
static RACF_CTSACF_ATTR_blk_typ RACF_ctsacf_attr_blks[] = {
  "USER",
   (sizeof(RACF_u_ctsacfs_attr) / sizeof(RACF_CTSACF_ATTR_rec_typ)),
     RACF_u_ctsacfs_attr,
  "", 0, NULL                    @* end-of-table                 *@
  };

@*  CTSA internal custom field status entry structure  *@
typedef  struct _CTSACF{
  char CF_name[9];                     @* Custom Field name         *@
  RACF_CTSACF_FEATURE_typ CF_Feature;  @* Feature id                *@
  RACF_CTSACF_ACT_typ act;       @* active ? Y(es) / N(o) / E(rror) *@
} RACF_CTSACF_rec_typ, *RACF_CTSACF_rec_ptr;

@*  CTSA internal custom field block structure *@
typedef struct _CTSACF_BLOCK {
  char     ctsacf_eyec[8];            @* eye catcher = CTSACFB       *@
  short    num_of_entries;            @* # of entries                *@
  RACF_CTSACF_rec_typ entries[0];     @* area for ctsacf entries.    *@
 } RACF_CTSACF_block_typ, *RACF_CTSACF_block_ptr;

@*  CTSA custom field processing routine prototype     *@
typedef ESA_RC RACF_CTSACF_PROC_typ(ESA_DIAG_COMP_typ    comp,
                                  char                  *cf_name,
                                  ONE_FIELD_rec_ptr      lpair,
                                  ADDINFO_rec_ptr        addinfo,
                                  ADDINFO_rec_ptr        ctsacf_addinfo,
                                  void                  *entity_parms,
                                  ADMIN_PARAMS_rec_typ  *admin_params,
                                  int                    args_num,
                                  va_list                args_list);

typedef RACF_CTSACF_PROC_typ *RACF_CTSACF_PROC_ptr;

@*  CTSA custom field processing routines table structure   *@
typedef  struct _CTSACF_PROC{
  RACF_CTSACF_FEATURE_typ CF_Feature;    @* feature id               *@
  RACF_CTSACF_PROC_ptr     proc_rtn;     @* -> processing routine    *@
} RACF_CTSACF_PROC_rec_typ, *RACF_CTSACF_PROC_rec_ptr;


@* ------------------- IS10100 - end  -------------------------- *@

@* ------------------------ IS10002 ---------------------------- *
 * RACF USA-API Parameters                                       *
 * ------------------------------------------------------------- *
 * The following structure is initialized during CTSAPIInit call *
 * and contains static data of the USA-API such are loaded       *
 * RSSPARM information.                                          *
 *                                                               *
 * apiinit_handle is pointing to this structure.                 *
 *                                                               *
 * Note that changes in this type might require rebuilt of ALL   *
 * the USA-API  modules.                                         *
 * ------------------------------------------------------------- *
 *    !!! Minimize changes and usage of this record !!!          *
 * ------------------------------------------------------------- *@

typedef  struct _RACF_PARAMS{
  char rssparm_MSG_CONNGRP_ERR[4]; @* MSG_CONNGRP_ERR value *@
  char rssparm_VERIFY_PASS_LOGIN[2]; @*VERIFY_PASSWORD_BY_LOGIN val *@
  char rssparm_RES_CLR_REV[2];       @*RESUME clears REVOKE_DATE    *@
  char rssparm_REV_CLR_RES[2];       @*REVOKE clears RESUME_DATE    *@
  char rssparm_LOGIN_INTERCEPT[2];   @*LOGIN_INTERCEPT   WS10013    *@

 @*  Custom Fields Support (IS10076) - Start                        *@

  char rssparm_CUSTOM_FIELDS_SUPPORT[2]; @*Custom fields support (y/n)*@
  RACF_CUSTOM_FIELDS_block_ptr p_user_CFields; @* -> User CFields     *@
  RACF_CUSTOM_FIELDS_block_ptr p_group_CFields; @* -> Group CFields   *@
  RACF_DYNAMIC_KDT_info_typ iu_kdt;   @* -> KDT for insert user      *@
  RACF_DYNAMIC_KDT_info_typ uu_kdt;   @* -> KDT for update user      *@
  RACF_DYNAMIC_KDT_info_typ ig_kdt;   @* -> KDT for insert group     *@
  RACF_DYNAMIC_KDT_info_typ ug_kdt;   @* -> KDT for update group     *@

 @*  Custom Fields Support (IS10076) - end                          *@

 @*  IS10100 - start                                                *@

  RACF_CTSACF_block_ptr p_u_ctsacfs;  @* -> user CTSA cfields       *@
  RACF_CTSACF_block_ptr p_g_ctsacfs;  @* -> group CTSA cfields      *@

 @*  IS10100 - end                                                  *@

  char ffu[934];    @* for future use, please update in change *@
  COMMON_PARAMS_rec_typ * common_params; @* ptr to common    WS10054 *@
                          @* usaapi parameters area (look at WS10054 *@
                          @* CTS.CMAC(CTSCSRV) for details)  WS10054 *@
} RACF_PARAMS_rec_typ, *RACF_PARAMS_rec_ptr;
@* ------------------- end of IS10002 -------------------------- *@

@*
 *    Typedef for Prototype of build_param functions
 *@

typedef ESA_RC BUILD_PARAM_FUNC_typ(RACF_OPTIONS_rec_typ      *WrkArea,
                                    CTSAMSG_DEST_TABLE_rec_ptr dest,
                                    CTSAMSG_HANDLE_rec_ptr     msgs) ;

typedef BUILD_PARAM_FUNC_typ  *BUILD_PARAM_FUNC_ptr;

typedef struct {
  char                 *addinfo_kwd;
  char                 *racf_option_kwd;
  BUILD_PARAM_FUNC_ptr  build_param_prog;
 } RACF_OPTIONS_DESC_TABLE_rec_typ, RACF_OPTIONS_DESC_TABLE_rec_ptr;

typedef int (*RACF_ACCESS_PROG) (char                     *option,
                                 char                     *entry_type,
                                 char                     *entry,
                                 int                      *flags,
                                 RACF_OUTPUT_LINE_rec_ptr  racfl,
                                 int                      *debug,
                                 int                      *rc,
                                 int                      *reason,
                                 char                     *sep);

                                                    @*BS10008*@
typedef int (*RACF_ACCESS_PROG_E) (char                     *option,
                                 char                     *entry_type,
                                 char                     *entry,
                                 int                      *flags,
                                 RACF_OUTPUT_LINE_rec_ptr  racfl,
                                 int                      *debug,
                                 int                      *rc,
                                 int                      *reason,
                                 char                     *sep,
       @* BS10008 *@             int                      max_connect);

@* ps0231 *@
typedef int (*RACF_RES_ACCESS_PROG) (char                 * option,
                                 char                     * entry_type,
                                 char                     * entry,
                                 int                      * flags,
                                 char                     * volume,
                                 char                     * unit,
                                 RACF_OUTPUT_LINE_rec_ptr   racfl,
                                 int                      * debug,
                                 int                      * rc,
                                 int                      * reason,
                                 char                     * sep);

@* ps0231 *@
typedef int (*RACF_ACE_ACCESS_PROG) (char                 * option,
                                 char                     * entry_type,
                                 char                     * entry,
                                 int                      * flags,
                                 char                     * volume,
                                 char                     * unit,
                                 int                      * acl_amount,
                                 RACF_ACE_AREA_rec_typ    * output,
                                 int                      * debug,
                                 int                      * rc,
                                 int                      * reason,
                                 char                     * sep);

typedef struct S_ONE_RACF_FIELD_rec_typ
       {
          short length ;
          char  value[RCF_ADDINFO_VAL_LEN+1] ;
       } ONE_RACF_FIELD_rec_typ, *ONE_RACF_FIELD_rec_ptr;


@*
 *      Header for connection structure handle
 *@

 #define CNN_EYECATCHER         "CNN "

 typedef struct S_CONNECT_HEADER {
     char   eyecatcher[4];                      @*eyecatcher 'CNN'*@
     struct s_entry {
       char f_entry_name[RCF_USER_NAME_LEN];    @* first entry  *@
       char s_entry_name[RCF_UG_NAME_LEN];      @* second entry *@
       char t_entry_name[RCF_UG_NAME_LEN];      @* buffer       *@
     } entry ;
     int  max_connect_number  ;
     int  done_request_number ;
     char func[80];                             @* func name    *@
     ADDINFO_rec_ptr   addinfo_in ;             @* local addinfo*@
     RACF_CNN_OUTPUT_LINE_rec_ptr  current_ptr;
     RACF_CNN_OUTPUT_LINE_rec_ptr  start_ptr  ;
  } RACF_CONNECT_HEADER_typ ;

 typedef struct S_hCONNECT_AREA {
           RACF_CONNECT_HEADER_typ header ;
           RACF_CNN_OUTPUT_LINE_rec_ptr  output_lines_ptr;
  } RACF_CONNECT_AREA_typ , *RACF_CONNECT_AREA_ptr ;

@**************************************************************
*                                                             *
* PROCEDURE NAME : racfcmd                                    *
*                                                             *
* DESCRIPTION    : check, build and issue a racf command      *
*                                                             *
* INPUT          :                                            *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@
EXT ESA_RC racfcmd (char                       * func,
                    char                       * ddn,
                    USER_typ                     admin_uname,
                    char                       * cmd,
    @* ps0320 *@    CMD_RC_typ                   max_ok_rc,
                    short                        numaddi,
                    ADDINFO_rec_ptr            * addinfo,
                    ADDINFO_MKTE_rec_typ       * mkt,
                    ADDINFO_KDTE_rec_typ       * kdt,
                    CTSAMSG_DEST_TABLE_rec_typ * dest,
                    CTSAMSG_HANDLE_rec_typ     * msgs,
    @* ps320 *@     CMDOUT_typ                   gout_flag,
                    RACF_FLAG_typ                ignore_no_addinfo);

@* IS10100A - start  *@
@**************************************************************
*                                                             *
* Subroutine name : RACF_do_AddAlt_cmd                        *
*                                                             *
* DESCRIPTION     : Prepare and issue the commands.           *
*                   If the addinfo contains more than 85      *
*                   CSDATA parameters (limited by RACF),      *
*                   aplit the addinfo and issue multiple      *
*                   commands.                                 *
*                   - If ALT command is requested, issue      *
*                     multiple commands using the addinfos    *
*                     created by the split.                   *
*                   - If ADD command is requested, issue      *
*                     and ADD command with the primary        *
*                     attributes and ALT commands for all     *
*                     other addinfos created by the aplit.    *
*                                                             *
* INPUT           : type        - U(ser) / G(roup)            *
*                   id          - user/group id               *
*                   cmd         - ADD / ALT                   *
*                   addinfo     - addinfo                     *
*                   laddinfo    - local addinfo.              *
*                   mkt, etc    - parameters required for     *
*                                 racfcmd.                    *
*                                                             *
* OUTPUT          : none                                      *
*                                                             *
* RETURN VALUE    : ESA_RC                                    *
*                                                             *
* WHEN TO USE:      This function should be called to prepare *
*                   and issue the command when the addinfo    *
*                   may contain CSDATA informtion. This is    *
*                   required becasue RACF command is limited  *
*                   to 68 CSDATA pramateres so if the         *
*                   addinfo contains more than 68 CSDATA      *
*                   parameters we will issue multiple         *
*                   commands.                                 *
*                                                             *
**************************************************************@

ESA_RC RACF_do_AddAlt_cmd(ESA_DIAG_COMP_typ   comp,
                          char                   type,
                          char                  *id,
                          char                  *act,
                          ADDINFO_rec_ptr        addinfo,
                          ADDINFO_rec_ptr        laddinfo,
                          ADDINFO_MKTE_rec_ptr   mkt,
                          ADDINFO_KDTE_rec_ptr   kdt,
                          CMDOUT_typ             gout_flag,
                          RACF_FLAG_typ          ignore_no_addinfo,
                          CMD_RC_typ             cmd_max_ok_rc,
                          ADMIN_PARAMS_rec_typ  *admin_params);
@* IS10100A - end    *@

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_refresh_class                         *
*                                                             *
* DESCRIPTION    : SETROPTS qualifier(class) REFRESH command  *
*                                                             *
* INPUT          : resource  type                             *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@
EXT  ESA_RC RACF_refresh_class(
                          RESOURCE_typ                 restype,
                          char                       * qualifier,
                          ADMIN_PARAMS_rec_typ       * admin_params,
                          CTSAMSG_DEST_TABLE_rec_typ * dest,
                          CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_refresh                               *
*                                                             *
* DESCRIPTION    : Checking need/no refresh class             *
*                                                             *
* INPUT          : resource  type                             *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@

EXT  ESA_RC RACF_refresh(RESOURCE_typ                 res_name,
                         RESOURCE_typ                 res_class,
                         ADMIN_PARAMS_rec_typ       * admin_params,
                         CTSAMSG_DEST_TABLE_rec_typ * dest,
                         CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_is_raclisted_class                    *
*                                                             *
* DESCRIPTION    : Check if class is raclisted                *
*                                                             *
* INPUT          : resource  type                             *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@

EXT  ESA_RC RACF_is_raclisted_class(
                          RESOURCE_typ                 restype,
                          CTSAMSG_DEST_TABLE_rec_typ * dest,
                          CTSAMSG_HANDLE_rec_typ     * msgs);


@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_is_global_class                       *
*                                                             *
* DESCRIPTION    : Check if class is global                   *
*                                                             *
* INPUT          : resource  type                             *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@

EXT  ESA_RC RACF_is_global_class( RESOURCE_typ                 restype,
                                  CTSAMSG_DEST_TABLE_rec_typ * dest,
                                  CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_is_generic_class                      *
*                                                             *
* DESCRIPTION    : Check if class is generic                  *
*                                                             *
* INPUT          : resource  type                             *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@

EXT  ESA_RC RACF_is_generic_class( RESOURCE_typ                 restype,
                                   CTSAMSG_DEST_TABLE_rec_typ * dest,
                                   CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_is_active_class                       *
*                                                             *
* DESCRIPTION    : Check if class is active                   *
*                                                             *
* INPUT          : resource  type                             *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@
EXT ESA_RC RACF_is_active_class(RESOURCE_typ             restype,
                            CTSAMSG_DEST_TABLE_rec_typ * dest,
                            CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_extract                               *
*                                                             *
* DESCRIPTION    : get information from racf data base        *
*                                                             *
* INPUT          : racfprg   - access program ( assembler )   *
*                  entry     - entry name                     *
*                  entry_type- racf profile user , group , ...*
*                  debug     - debug level                    *
*                  locate_err_flag  - send/no err message     *
*                  generic_flag - 1 - generic , 0 - no        *
*                              valid only for dataset,        *
*                              general resource profile       *
*                  locate_err_flag - send/no err msg          *
*                  option    - locate / next / nextc          *
*                  dest      - Vector of error msg dests      *
*                  msgs      - Error messages buffer          *
*                                                             *
* OUTPUT         : output    - output area                    *
*                                                             *
* CALLED BY      : getugs, gtug2uc, gtusers                   *
*                                                             *
* CALLS TO       :  func getugs - ctsfrlg                     *
*                   func gtusers- ctsfrlu                     *
*                   func gtug2uc- ctsfrcn                     *
*                                 ctsfrnm                     *
*                                 ctsfruc                     *
*                                 ctsfrgc                     *
*                  get_list_count                             *
*                  OS_DIAG_printf                             *
*                  OS_DIAG_fnc_enter                          *
*                  OS_DIAG_fnc_exit                           *
*                  CTSAMSG_print                              *
*                                                             *
* RETURN VALUE    : ESA_ERR   - end of database / not found   *
*                   ESA_FATAL - serious error                 *
*                                                             *
*                : from assemblers access program :           *
*                  16  -  overflow output area                *
*                  20  -  invalid option                      *
*                  24  -  invalid entry-typ                   *
*                  28  -  invalid map code                    *
*                  32  -  invalid freemain oparetion          *
*                  36  -  invalid connection request          *
*                                                             *
*                  when return code = 0  - RACF extractor OK  *
*                  ------------------------------------------ *
*                  racf_rc     - racf return code             *
*                  racf_reason - racf reason code             *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_extract(RACF_ACCESS_PROG_E  racfprg,      @*BS10008*@
               char                         *option,
               char                         *entry_type,
               char                         *entry,
               int                          generic_flag,
               char                         *func,
               RACF_OUTPUT_LINE_rec_ptr     output,
               int                          debug,
               RACF_FLAG_typ                locate_err_flag,
               CTSAMSG_DEST_TABLE_rec_ptr   dest,
               CTSAMSG_HANDLE_rec_ptr       msgs,
               int                          max_connect); @*BS10008*@

@* ps0231 *@
@***************************************************************
*                                                              *
* PROCEDURE NAME : RACF_extract_res                            *
*                                                              *
* DESCRIPTION    : Extract information from racf data base     *
*                  (resources)                                 *
* INPUT          : racfprg   - access program ( assembler )    *
*                  entry     - profile name                    *
*                  entry_type- profile type (DS, GEN)          *
*                  debug     - debug level                     *
*                  generic_flag - 1 - generic , 0 - no         *
*                              ( only for resource profiles )  *
*                  locate_err_flag - send/no err msg           *
*                  option    - locate / next / nextc           *
*                  dest      - Vector of error msg dests       *
*                  msgs      - Error messages buffer           *
*                                                              *
* OUTPUT         : output    - output area                     *
*                                                              *
* RETURN VALUE   : ESA_ERR   - End of database/Profile n/a     *
*                  ESA_FATAL - Fatal extractor error           *
*                                                              *
*                  Rc from assembler access programs :         *
*                                                              *
*                   16  -  overflow of output area             *
*                   20  -  invalid option                      *
*                   24  -  invalid entry-type                  *
*                   28  -  invalid map code                    *
*                   32  -  invalid freemain operation          *
*                   36  -  invalid connection request          *
*                   40  -  invalid load extractor (for connect)*
*                   42  -  invalid del  extractor (for connect)*
*                                                              *
*                   When return code = 0  - RACF extractor OK  *
*                   ------------------------------------------ *
*                                                              *
*                   racf_rc     - racf return code             *
*                   racf_reason - racf reason code             *
*                                                              *
***************************************************************@

EXT ESA_RC RACF_extract_res(RACF_RES_ACCESS_PROG         racfprg,
                    char                               * option,
                    char                               * entry_type,
                    char                               * entry,
                    int                                  generic_flag,
                    char                               * volume,
                    char                               * unit,
                    char                               * func,
                    RACF_OUTPUT_LINE_rec_typ           * output,
                    int                                  debug,
                    RACF_FLAG_typ                       locate_err_flag,
                    CTSAMSG_DEST_TABLE_rec_typ         * dest,
                    CTSAMSG_HANDLE_rec_typ             * msgs);

@* ps0231 *@
@***************************************************************
*                                                              *
* PROCEDURE NAME : RACF_extract_ace                            *
*                                                              *
* DESCRIPTION    : Extract information from racf data base     *
*                  (ace's)                                     *
* INPUT          : racfprg   - access program ( assembler )    *
*                  entry     - profile name                    *
*                  entry_type- profile type (DS, GEN)          *
*                  debug     - debug level                     *
*                  generic_flag - 1 - generic , 0 - no         *
*                              ( only for resource profiles )  *
*                  locate_err_flag - send/no err msg           *
*                  option    - locate / next / nextc           *
*                  dest      - Vector of error msg dests       *
*                  msgs      - Error messages buffer           *
*                                                              *
* OUTPUT         : output    - output area                     *
*                                                              *
* RETURN VALUE   : ESA_ERR   - End of database/Profile n/a     *
*                  ESA_FATAL - Fatal extractor error           *
*                                                              *
*                  Rc from assembler access programs :         *
*                                                              *
*                   16  -  overflow of output area             *
*                   20  -  invalid option                      *
*                   24  -  invalid entry-type                  *
*                   28  -  invalid map code                    *
*                   32  -  invalid freemain operation          *
*                   36  -  invalid connection request          *
*                   40  -  invalid load extractor (for connect)*
*                   42  -  invalid del  extractor (for connect)*
*                                                              *
*                   When return code = 0  - RACF extractor OK  *
*                   ------------------------------------------ *
*                                                              *
*                   racf_rc     - racf return code             *
*                   racf_reason - racf reason code             *
*                                                              *
***************************************************************@
EXT ESA_RC RACF_extract_ace(RACF_ACE_ACCESS_PROG         racfprg,
                    char                               * option,
                    char                               * entry_type,
                    char                               * entry,
                    int                                  generic_flag,
                    char                               * volume,
                    char                               * unit,
                    char                               * func,
                    int                                * acl_amount,
                    RACF_ACE_AREA_rec_typ              * output,
                    int                                  debug,
                    RACF_FLAG_typ                       locate_err_flag,
                    CTSAMSG_DEST_TABLE_rec_typ         * dest,
                    CTSAMSG_HANDLE_rec_typ             * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_output_to_addinfo                     *
*                                                             *
* DESCRIPTION    : separate information reutrned from RACF    *
*                  and insert it to addinfo buffer            *
*                                                             *
* INPUT          : output    - output area                    *
*                  addinfo_in- special addinfo for            *
*                              special keywords               *
*                  keywords  - Special keywords names         *
*                  admin_params                               *
*                                                             *
* INPUT/OUTPUT   : addinfo   - addinfo ptr                    *
*                                                             *
* CALLED BY      : getugs, gtusers, gtug2uc                   *
*                                                             *
* CALLS TO       : RACF_get_next_parameter                    *
*                  ADDINFO_search                             *
*                  ADDINFO_insert                             *
*                  OS_DIAG_printf                             *
*                  OS_DIAG_fnc_enter                          *
*                  OS_DIAG_fnc_exit                           *
*                  CTSAMSG_print                              *
*                                                             *
* RETURN VALUE   : ESA_OK                                     *
*                                                             *
**************************************************************@
EXT ESA_RC RACF_output_to_addinfo(RACF_OUTPUT_LINE_rec_ptr output,
               ADDINFO_rec_ptr                addinfo,
               ADDINFO_rec_ptr                addinfo_in,
               char                           *keywords[],
               RACF_CUSTOM_FIELDS_block_ptr   p_cfb,         @*IS10076*@
   @* IS10076  CTSAMSG_DEST_TABLE_rec_ptr     dest,           *@
   @* IS10076  CTSAMSG_HANDLE_rec_ptr         msgs);          *@
               RACF_CTSACF_block_ptr          p_ctsacf_blk,  @*IS10100*@
               ADMIN_PARAMS_rec_typ          *admin_params); @*IS10076*@

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_get_parameter                         *
*                                                             *
* DESCRIPTION    : Return value for a requested field name    *
*                                                             *
* INPUT          : output     - output area (racf information)*
*                  field_name -                               *
*                                                             *
* OUTPUT         : length     - length of value               *
*                  value                                      *
*                  dest       - Vector of error msg dests     *
*                  msgs       - Error messages buffer         *
*                                                             *
* CALLED BY     : get_list_count                              *
*                                                             *
* CALLS TO      : OS_DIAG_printf                              *
*                 OS_DIAG_fnc_enter                           *
*                 OS_DIAG_fnc_exit                            *
*                 CTSAMSG_print                               *
*                 RACF_get_next_parameter                     *
*                                                             *
* RETURN VALUE   : ESA_OK  - field found                      *
*                  ESA_ERR - field not found                  *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_get_parameter(RACF_OUTPUT_LINE_rec_ptr output,
               char                     *field_name,
               int                      *length,
               char                     *value,
               CTSAMSG_DEST_TABLE_rec_ptr dest,
               CTSAMSG_HANDLE_rec_ptr   msgs);

@**************************************************************
*                                                             *
* Procedure Name: RACF_get_field                              *
*                                                             *
* Description   : Get current fields from output line         *
*                                                             *
* Input         : 1. curr_char                                *
*                    ptr to current information from racf     *
*                    output line                              *
*                                                             *
* Output        : none                                        *
*                                                             *
* CALLED BY     : RACF_get_next_parameter                     *
*                                                             *
* CALLS TO      : OS_DIAG_printf                              *
*                 OS_DIAG_fnc_enter                           *
*                 OS_DIAG_fnc_exit                            *
*                 CTSAMSG_print                               *
*                                                             *
* Return Value  : output_char_ptr                             *
*                 ptr to new position in output line          *
*                                                             *
**************************************************************@
EXT char * RACF_get_field (char *curr_char,
                           ONE_RACF_FIELD_rec_ptr racf_field,
                           int  update_len_flag);  @* ps0292 *@

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_get_next_parameter                    *
*                                                             *
* DESCRIPTION    : field separator    from racf output area   *
*                                                             *
* INPUT          : 1) ptr to output area ( racf information ) *
*                                                             *
* OUTPUT         : 1) structure : keyword(length,value)       *
*                  2) structure : data_value(length,value)    *
*                  3) structure : text_after(length,value)    *
*                                                             *
* CALLED BY     : RACF_output_to_addinfo                      *
*                 RACF_get_parameter                          *
*                                                             *
* CALLS TO      : OS_DIAG_printf                              *
*                 OS_DIAG_fnc_enter                           *
*                 OS_DIAG_fnc_exit                            *
*                 CTSAMSG_print                               *
*                 RACF_get_field                              *
*                                                             *
* RETURN VALUE   : ptr to next field                          *
*                                                             *
**************************************************************@

EXT char * RACF_get_next_parameter (char *curr_ptr,
                       ONE_RACF_FIELD_rec_ptr racf_keyword,
                       ONE_RACF_FIELD_rec_ptr racf_value,
                       ONE_RACF_FIELD_rec_ptr racf_pass) ;

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_get_list_count                        *
*                                                             *
* DESCRIPTION    : Return count of connections                *
*                                                             *
* INPUT          : racfprg    - racf program (assembler)      *
*                  output     - output line                   *
*                  entry      -                               *
*                  opcode     - locate/group                  *
*                               next/group                    *
*                  field_name -                               *
*                                                             *
* OUTPUT         : field_value-                               *
*                  dest       - Vector of error msg dests     *
*                  msgs       - Error messages buffer         *
*                                                             *
* CALLED BY     : gtug2uc                                     *
*                                                             *
* CALLS TO      : OS_DIAG_printf                              *
*                 OS_DIAG_fnc_enter                           *
*                 OS_DIAG_fnc_exit                            *
*                 CTSAMSG_print                               *
*                 RACF_extrcat                                *
*                 RACF_get_parameter                          *
*                                                             *
* RETURN VALUE   : ESA_OK  - field found, value returned      *
*                  ESA_ERR - field not found ,value not return*
*                                                             *
**************************************************************@

EXT ESA_RC RACF_get_list_count (RACF_ACCESS_PROG  racfprg,
                       char                      *opcode,
                       char                      *entry_type,
                       char                      *entry,
                       int                        generic_flag,
                       char                      *func,
                       RACF_OUTPUT_LINE_rec_ptr   output,
                       char                      *field,
                       int                       *field_value,
                       int                       debug,
                       CTSAMSG_DEST_TABLE_rec_ptr dest,
                       CTSAMSG_HANDLE_rec_ptr     msgs);
@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_alloc_output_lines                    *
*                                                             *
* DESCRIPTION    : Return ptr to connect_area                 *
*                                                             *
* INPUT          : 1) connect counter                         *
*                                                             *
* OUTPUT         : 1) ptr to allocated connect-area           *
*                                                             *
* CALLED BY      : gtug2uc                                    *
*                                                             *
* CALLS TO       : OS_DIAG_printf                             *
*                  OS_DIAG_fnc_enter                          *
*                  OS_DIAG_fnc_exit                           *
*                  CTSAMSG_print                              *
*                                                             *
* RETURN VALUE   : ESA_OK   -     O.K                         *
*                  ESA_FATAL-     func malloc failed          *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_alloc_output_lines (void             ** handle,
                             RACF_CONNECT_AREA_ptr    * grp_us,
                             char                     * entry,
                             char                     * func,
                             int                        cnt,
                             int                        addinfo_cnt,
                             CTSAMSG_DEST_TABLE_rec_ptr dest,
                             CTSAMSG_HANDLE_rec_ptr     msgs);

@***************************************************************
*                                                              *
* PROCEDURE NAME : RACF_extract_field                          *
*                                                              *
* DESCRIPTION    : Extract a field from a RACF user/group      *
*                  profile usinf RACXTRCT                      *
*                                                              *
* INPUT          : class     - user/group                      *
*                  id        - userid/groupid                  *
*                  segment   - segment name                    *
*                  fldname   - field name                      *
*                  dest      - Vector of error msg dests       *
*                  msgs      - Error messages buffer           *
*                                                              *
* OUTPUT         : value     - value area                      *
*                                                              *
* INPUT/OUTPUT   : valuel    - length of value area            *
*                                                              *
* RETURN VALUE   : ESA_OK    - extract successful              *
*                  ESA_ERR   - extract failure                 *
*                                                              *
***************************************************************@

EXT ESA_RC RACF_extract_field(char                       * class,
                              char                       * id,
                              char                       * segment,
                              char                       * field,
                              char                       * value,
                              int                        * valuel,
                              CTSAMSG_DEST_TABLE_rec_typ * dest,
                              CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
* SAS2IBMT This routine does not exist!                       *
*                                                             *
* PROCEDURE NAME : RACF_get_classes                           *
*                                                             *
* DESCRIPTION    : Return ptr to classes array                *
*                                                             *
* INPUT          : None                                       *
*                                                             *
* OUTPUT         : None                                       *
*                                                             *
* RETURN VALUE   : Pointer to array of classes names          *
*                  Each class is 8 characters long.           *
*                  Last Class name is a NULL STRING           *
*                                                             *
***************************************************************
EXT char  *RACF_get_classes(void);                                   *@

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_revoke_date                           *
*                                                             *
* DESCRIPTION    : Conevrts esa date format to RACF           *
*                  revoke/resume date format                  *
*                                                             *
* INPUT          : esa_date                                   *
*                  dest                                       *
*                  msgs                                       *
*                                                             *
* OUTPUT         : racf_date                                  *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_revoke_date( char                   * esa_date,
                         char                       * racf_date,
                         CTSAMSG_DEST_TABLE_rec_typ * dest,
                         CTSAMSG_HANDLE_rec_typ     * msgs    );

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_extract_defug                         *
*                                                             *
* DESCRIPTION    : Retrive a RACF user default group          *
*                                                             *
* INPUT          : user                                       *
*                  dest                                       *
*                  msgs                                       *
*                                                             *
* OUTPUT         : group                                      *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_extract_defug ( USER_typ                     user,
                                UG_typ                       group,
                                CTSAMSG_DEST_TABLE_rec_typ * dest,
                                CTSAMSG_HANDLE_rec_typ     * msgs );


@* WSAR1001 *@
@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_get_option                            *
*                                                             *
* DESCRIPTION    : Retrive a RACF option                      *
*                                                             *
* INPUT          : addinfo, field ptr                        *
*                                                             *
* OUTPUT         : option value                               *
*                                                             *
* RETURN VALUE   : ESA_OK,  ESA_FATAL                         *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_get_option( ONE_FIELD_rec_ptr            f_ptr,
                            RACF_OPTIONS_rec_typ       * work_area,
                            CTSAMSG_DEST_TABLE_rec_typ * dest,
                            CTSAMSG_HANDLE_rec_typ     * msgs );


@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_get_option_value                      *
*                                                             *
* DESCRIPTION    : Get SETROPTS single parameter value        *
*                                                             *
* INPUT          : addinfo keyword                            *
*                                                             *
*                                                             *
* OUTPUT         : result                                     *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_get_option_value(char                       * kwd,
                                 ADDINFO_typ                  add_typ,
                                 char                       * result,
                                 CTSAMSG_DEST_TABLE_rec_typ * dest,
                                 CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_version_get                           *
*                                                             *
* DESCRIPTION    : Retrive a RACF version/release             *
*                                                             *
* INPUT          : none                                      *
*                                                             *
* OUTPUT         : version                                    *
*                                                             *
* RETURN VALUE   : ESA_OK,  ESA_FATAL                         *
*                                                             *
**************************************************************@
@* is0263 - add 2nd parm to function                        *@
ESA_RC RACF_version_get( char       *rss_type_used, @*is0263*@
                         char       *version);
@* is0263 EXT ESA_RC RACF_version_get( char   * version); *@

@* following prototype added by ps0491 *@
EXT ESA_RC RACF_version_at_least(char *version,RACF_FLAG_typ *at_least);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_version_check                         *
*                                                             *
* DESCRIPTION    : Check   a RACF version/release             *
*                                                             *
* INPUT          : rss_type                                   *
*                  dest                                       *
*                  msgs                                       *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_OK,  ESA_FATAL                         *
*                                                             *
**************************************************************@

EXT ESA_RC RACF_version_check(RSS_typ                      rss_type,
                              CTSAMSG_DEST_TABLE_rec_typ * dest,
                              CTSAMSG_HANDLE_rec_typ     * msgs);

@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_chk_user                              *
*                                                             *
* DESCRIPTION    : Check RACF user   existance                *
*                                                             *
* INPUT          : user     - User name                       *
*                  test     - Check type - EXIST, NOEXIST     *
*                  msg_flag - Issue existance error message   *
*                  dest     - Vector of error msg dests       *
*                  msgs     - Error messages buffer           *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
*                  ESA_OK   - Test OK                         *
*                  ESA_ERR  - Test FAILED                     *
*                  ESA_FATAL- Internal Error                  *
*                                                             *
**************************************************************@
ESA_RC RACF_chk_user(char                       * user,
                     OBJ_EXISTS_typ               test,
                     int                          msg_flag,
                     CTSAMSG_DEST_TABLE_rec_ptr   dest,
                     CTSAMSG_HANDLE_rec_ptr       msgs);

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_pro_user  added by PS0460             *
 *                                                             *
 * DESCRIPTION    : Check RACF user PROTECTED attribute        *
 *                                                             *
 * INPUT          : user     - User name                       *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : protest  - RACF_NO -Not Protected          *
 *                           - RACF_YES- No Password=Protected *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED no USER             *
 *                  ESA_FATAL- Test FAILED Internal Error      *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_pro_user(char                       * user,
                      RACF_FLAG_typ           * protest,
                      int                      msg_flag,
                      CTSAMSG_DEST_TABLE_rec_ptr   dest,
                      CTSAMSG_HANDLE_rec_ptr       msgs);


 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_uni_group added by ws2427             *
 *                                                             *
 * DESCRIPTION    : Check RACF group Universal attribute       *
 *                                                             *
 * INPUT          : group    - Group name                      *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : unitest  - RACF_NO - Not Universal         *
 *                           - RACF_YES- Universal             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED no GROUP            *
 *                  ESA_FATAL- Test FAILED Internal Error      *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_uni_group(char                      * group,
                       RACF_FLAG_typ           * unitest,
                       int                      msg_flag,
                       CTSAMSG_DEST_TABLE_rec_ptr   dest,
                       CTSAMSG_HANDLE_rec_ptr      msgs);

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_chk_group                             *
 *                                                             *
 * DESCRIPTION    : Check RACF group  existance                *
 *                                                             *
 * INPUT          : group    - User name                       *
 *                  test     - Check type - EXIST, NOEXIST     *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED                     *
 *                  ESA_FATAL- Intrenal Error                  *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_chk_group(char                       * group,
                       OBJ_EXISTS_typ               test,
                       int                          msg_flag,
                       CTSAMSG_DEST_TABLE_rec_ptr   dest,
                       CTSAMSG_HANDLE_rec_ptr       msgs);

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_chk_conn                              *
 *                                                             *
 * DESCRIPTION    : Check RACF connection existance            *
 *                                                             *
 * INPUT          : group    - User name                       *
 *                  test     - Check type - EXIST, NOEXIST     *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                  admin_params -                             *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED                     *
 *                  ESA_FATAL- Intrenal Error                  *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_chk_conn(char                       * user_name,
                      char                       * group_name,
                      OBJ_EXISTS_typ               test,
                      int                          msg_flag,
                      CTSAMSG_DEST_TABLE_rec_ptr   dest,
                      CTSAMSG_HANDLE_rec_ptr       msgs,
                      ADMIN_PARAMS_rec_typ * admin_params);@* WS2549 *@

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_chk_res                               *
 *                                                             *
 * DESCRIPTION    : Check RACF resource existance              *
 *                                                             *
 * INPUT          : res_class   - class                        *
 *                  res_profile - profile                      *
 *                  res_volume  - volume                       *
 *                  generic_flag- generic flag                 *
 *                  test     - Check type - EXIST, NOEXIST     *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED                     *
 *                  ESA_FATAL- Intrenal Error                  *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_chk_res(char                       * res_class,
                     char                       * res_profile,
                     char                       * res_volume,
                     int                          generic_flag,
                     OBJ_EXISTS_typ               test,
                     int                          msg_flag,
                     CTSAMSG_DEST_TABLE_rec_ptr   dest,
                     CTSAMSG_HANDLE_rec_ptr       msgs);

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_chk_hilvds                            *
 *                                                             *
 * DESCRIPTION    : Check RACF HILVDS resource existance       *
 *                                                             *
 * INPUT          : res_object - object ID                     *
 *                  test     - Check type - EXIST, NOEXIST     *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED                     *
 *                  ESA_FATAL- Intrenal Error                  *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_chk_hilvds(char                     *res_object,
                        OBJ_EXISTS_typ               test,
                        int                          msg_flag,
                        CTSAMSG_DEST_TABLE_rec_ptr   dest,
                        CTSAMSG_HANDLE_rec_ptr       msgs);

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_chk_ace                               *
 *                                                             *
 * DESCRIPTION    : Check RACF resource ace existance          *
 *                                                             *
 * INPUT          : res_class   - class                        *
 *                  res_profile - profile                      *
 *                  res_volume  - volume                       *
 *                  generic_flag- generic flag                 *
 *                  res_object  - user, group                  *
 *                  res_access  - access ( may be NULL )       *
 *                  test     - Check type - EXIST, NOEXIST     *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED                     *
 *                  ESA_FATAL- Intrenal Error                  *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_chk_ace(char                       * res_class,
                     char                       * res_profile,
                     char                       * res_volume,
                     int                          generic_flag,
                     char                       * res_object,
                     char                       * res_access,
                     OBJ_EXISTS_typ               test,
                     int                          msg_flag,
                     CTSAMSG_DEST_TABLE_rec_ptr   dest,
                     CTSAMSG_HANDLE_rec_ptr       msgs);

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_chk_cond_ace                          *
 *                                                             *
 * DESCRIPTION    : Check RACF resource ace existance          *
 *                                                             *
 * INPUT          : res_class   - class                        *
 *                  res_profile - profile                      *
 *                  res_volume  - volume                       *
 *                  generic_flag- generic flag                 *
 *                  res_object  - user, group                  *
 *                  res_access  - access ( may be NULL )       *
 *                  res_wtype   - cond qulifier                *
 *                  res_wname   - cond name                    *
 *                  test     - Check type - EXIST, NOEXIST     *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED                     *
 *                  ESA_FATAL- Intrenal Error                  *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_chk_cond_ace(char                       * res_class,
                          char                       * res_profile,
                          char                       * res_volume,
                          int                          generic_flag,
                          char                       * res_object,
                          char                       * res_access,
                          char                       * res_wtype,
                          char                       * res_wname,
                          OBJ_EXISTS_typ               test,
                          int                          msg_flag,
                          CTSAMSG_DEST_TABLE_rec_ptr   dest,
                          CTSAMSG_HANDLE_rec_ptr       msgs);

 @**************************************************************
 *                                                             *
 * PROCEDURE NAME : RACF_chk_res_class                         *
 *                                                             *
 * DESCRIPTION    : Check RACF resource class exictance        *
 *                                                             *
 * INPUT          : rcls     - Resource class                  *
 *                  test     - Check type - EXIST, NOEXIST     *
 *                  msg_flag - Issue existance error message   *
 *                  dest     - Vector of error msg dests       *
 *                  msgs     - Error messages buffer           *
 *                                                             *
 * OUTPUT         : none                                       *
 *                                                             *
 * RETURN VALUE   : ESA_RC                                     *
 *                                                             *
 *                  ESA_OK   - Test OK                         *
 *                  ESA_ERR  - Test FAILED                     *
 *                  ESA_FATAL- Intrenal Error                  *
 *                                                             *
 **************************************************************@
 ESA_RC RACF_chk_res_class(char                  * rscls,
                      OBJ_EXISTS_typ               test,
                      int                          msg_flag,
                      CTSAMSG_DEST_TABLE_rec_ptr   dest,
                      CTSAMSG_HANDLE_rec_ptr       msgs);

@* ps0292 *@
@**************************************************************
*                                                             *
* PROCEDURE NAME : RACF_getdig_info                           *
*                                                             *
* DESCRIPTION    : Get digital certificate info               *
*                                                             *
* INPUT          : asn1buf          -  asn1 info buffer       *
*                  length           -  data length            *
*                                                             *
* OUTPUT         : serial_number    - serial number           *
*                  issuer_name      - issuer name             *
*                  valid_date       - valid_date              *
*                  expired_date     - expired_date            *
*                  subject_name     - subject_name            *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@
ESA_RC RACF_getdig_info(char                      *asn1buf,
                        int                        length,
                        char                      *serial_number,
                        char                      *issuer_name,
                        char                      *valid_date,
                        char                      *expired_date,
                        char                      *subject_name,
                        CTSAMSG_DEST_TABLE_rec_ptr p_dest,
                        CTSAMSG_HANDLE_rec_ptr     p_msgs);

 @* ps0292 *@
 @**************************************************************
 *                                                             *
 * Subroutine name : RACF_Dig_cert_add                         *
 *                                                             *
 * DESCRIPTION     : Add cerificate                            *
 *                                                             *
 * INPUT           : 1.addinfo       - ptr to addinfo          *
 *                   2.user_params   - user_params struct      *
 *                                                             *
 * OUTPUT          : none                                      *
 *                                                             *
 * RETURN VALUE    : none                                      *
 *                                                             *
 **************************************************************@

 ESA_RC RACF_Dig_cert_add(ESA_DIAG_COMP_typ           comp,
                          USER_PARAMS_rec_typ       * user_params,
                          ADDINFO_rec_ptr             addinfo,
                          ADDINFO_rec_ptr             laddinfo,
                          ADMIN_PARAMS_rec_typ      * admin_params,
                          CTSAMSG_DEST_TABLE_rec_ptr  dest,
                          CTSAMSG_HANDLE_rec_ptr      msgs);

 @* ps0292 *@
 @**************************************************************
 *                                                             *
 * Subroutine name : RACF_Dig_cert_upd                         *
 *                                                             *
 * DESCRIPTION     : Upd cerificate                            *
 *                                                             *
 * INPUT           : 1.addinfo       - ptr to addinfo          *
 *                   2.user_params   - user_params struct      *
 *                                                             *
 * OUTPUT          : none                                      *
 *                                                             *
 * RETURN VALUE    : none                                      *
 *                                                             *
 **************************************************************@

 ESA_RC RACF_Dig_cert_upd(ESA_DIAG_COMP_typ           comp,
                          RACF_ACCESS_PROG            racfprg,
                          USER_PARAMS_rec_typ       * user_params,
                          ADDINFO_rec_ptr             addinfo,
                          ADDINFO_rec_ptr             laddinfo,
                          ADMIN_PARAMS_rec_typ      * admin_params,
                          CTSAMSG_DEST_TABLE_rec_ptr  dest,
                          CTSAMSG_HANDLE_rec_ptr      msgs);

@* ps0292 *@
@**************************************************************
*                                                             *
* Subroutine name : RACF_strtok                               *
*                                                             *
* DESCRIPTION     : strtok                                    *
*                                                             *
* INPUT           : sptr        - ptr to string               *
*                   charset     - ptr to char set             *
*                   handle      - ptr to handle               *
*                                                             *
* OUTPUT          : none                                      *
*                                                             *
* RETURN VALUE    : ptr to current token                      *
*                                                             *
**************************************************************@

char * RACF_strtok(char       *  sptr,
                   char         *  charset,
                   void         ** handle,
                   char         *  result);

@* ps0292 *@
@**************************************************************
*                                                             *
* Subroutine name : RACF_Dig_cert_get_info                    *
*                                                             *
* DESCRIPTION     : Get Dig Cert Info                         *
*                                                             *
* INPUT           : serial      - pending serial number       *
*                   laddinfo    - local addinfo               *
*                   user        - user                        *
*                                                             *
* OUTPUT          : none                                      *
*                                                             *
* RETURN VALUE    : ESA_RC                                    *
*                                                             *
**************************************************************@

ESA_RC RACF_Dig_cert_get_info(ESA_DIAG_COMP_typ           comp,
                              RACF_ACCESS_PROG            racfprg,
                              int                       * serial,
                              char                      * user,
                              ADDINFO_rec_ptr             laddinfo,
                              ADDINFO_rec_ptr           * dgt_laddinfo,
              @* IS10076      CTSAMSG_DEST_TABLE_rec_ptr  dest,    *@
              @* IS10076      CTSAMSG_HANDLE_rec_ptr      msgs);   *@
              @* IS10076 *@   ADMIN_PARAMS_rec_typ      * admin_params);

@* IS10076 - start   *@

@* IS10100
 ********************************************************************
 *
 * Procedure Name : RACF_Create_CField_Blocks
 *
 * Description    : Retrieve Custom Fields information and create
 *                  the user and group custom field blocks.
 *
 * Input          : admin
 *
 * Output         : p_user_CFields -> User Custom Fields block
 *                : p_group_CFields -> roup Custom Fields block
 *
 * Input/Output   :
 *
 * Return Value   : ESA_RC
 *
 * Side Effects   :
 *
 * Comments       : Added by IS10076
 *
 ********************************************************************

ESA_RC RACF_Create_CField_Blocks(ESA_DIAG_COMP_typ    comp,
                        RACF_CUSTOM_FIELDS_block_ptr *p_user_CFields,
                        RACF_CUSTOM_FIELDS_block_ptr *p_group_CFields,
                        int                          *msg_issued,
                        ADMIN_PARAMS_rec_typ         *admin_params );
 *@

@********************************************************************
 *
 * Procedure Name : RACF_Free_CField_Blocks
 *
 * Description    : Free all CField blocks starting from the first
 *                  block received as parameter.
 *
 * Input          : first block address.
 *
 * Output         : None.
 *
 * Input/Output   :
 *
 * Return Value   : None.
 *
 * Side Effects   :
 *
 * Comments       : Added by IS10076
 *
 ********************************************************************@

void RACF_Free_CField_Blocks(ESA_DIAG_COMP_typ            comp,
                             RACF_CUSTOM_FIELDS_block_ptr first_block,
                             ADMIN_PARAMS_rec_typ        *admin_params);

@***************************************************************
*                                                              *
* PROCEDURE NAME : RACF_Init_Dynamic_KDTs                      *
*                                                              *
* DESCRIPTION    : Init dynamic KDTs for the requested types.  *
*                                                              *
* INPUT          : num_types    - number of type entries.      *
*                  types        - array containing required    *
*                                 types.                       *
*                  params_ptr   - -> apiinit handle.           *
*                  admin_params                                *
*                                                              *
* OUTPUT         : kdts        - array where pointers to the   *
*                                created KDTs are returned.    *
*                                the order of the pointers is  *
*                                the order of the types.       *
*                                                              *
* RETURN VALUE   : ESA_OK    - ok.                             *
*                : ESA_FATAL - Fatal error.                    *
*                                                              *
***************************************************************@

ESA_RC RACF_Init_Dynamic_KDTs(ESA_DIAG_COMP_typ           comp,
                              RACF_DYNKDT_typ             types[1],
                              int                         num_types,
                              RACF_PARAMS_rec_typ        *params_ptr,
                              RACF_DYNAMIC_KDT_info_typ   kdts_info[1],
                              ADMIN_PARAMS_rec_typ       *admin_params);

@***************************************************************
*                                                              *
* PROCEDURE NAME : RACF_Free_Dynamic_KDT                       *
*                                                              *
* DESCRIPTION    : Free a dynamic KDT.                         *
*                                                              *
* INPUT          : kdt_info    - KDT address and dynalloc      *
*                                indication.                   *
*                  admin_params                                *
*                                                              *
* OUTPUT         : kdt_info    - KDT address and dynalloc      *
*                                indication.                   *
*                                                              *
* RETURN VALUE   : ESA_OK    - ok.                             *
*                : ESA_FATAL - Fatal error.                    *
*                                                              *
***************************************************************@

void   RACF_Free_Dynamic_KDT(ESA_DIAG_COMP_typ         comp,
                           RACF_DYNAMIC_KDT_info_typ  *kdt_info,
                           ADMIN_PARAMS_rec_typ       *admin_params);

@********************************************************************
 *
 * Procedure Name : RACF_Search_CField_Block
 *
 * Description    : Look for a specific field in the CFields block.
 *
 * Input          : first_block - -> first CFB.
 *                : cf_name- Custom field name to look for.
 *
 * Output         : None.
 *
 * Input/Output   :
 *
 * Return Value   : CField entry in CFB or NULL if not found.
 *
 * Side Effects   :
 *
 * Comments       :
 *
 ********************************************************************@

RACF_CUSTOM_FIELD_rec_typ * RACF_Search_CField_Block(
                             ESA_DIAG_COMP_typ            comp,
                             RACF_CUSTOM_FIELDS_block_ptr first_block,
                             char                        *cf_name,
                             ADMIN_PARAMS_rec_typ        *admin_params);
@* IS10076 - end     *@

@* IS10100 - start   *@
@********************************************************************
 *
 * Procedure Name : RACF_Init_Custom_Fields
 *
 * Description    : Initialize the site custom field blocks and/or
 *                  CTSA custom field blocks.
 *
 * Input          : admin
 *
 * Output         : msg_issued - on error, contains an indicatiom if
 *                               an error message was issued or not.
 *
 * Input/Output   : p_user_CFields - area for address of user Custom
 *                                   Fields block.
 *                : p_group_CFields - area for address of group Custom
 *                                    Fields block.
 *                : p_user_ctsacfs - area for address of user CTSA
 *                                   custom field blocks.
 *                : p_group_CFields - area for address of user CTSAtom
 *                                    custom field blocks.
 *                  Note: NULL in any of the above parameters indicates
 *                        this table is not required.
 *
 * Return Value   : ESA_RC
 *
 * Side Effects   :
 *
 * Comments       : Added by IS10100
 *
 ********************************************************************@

 ESA_RC RACF_Init_Custom_Fields(ESA_DIAG_COMP_typ      comp,
                        RACF_CUSTOM_FIELDS_block_ptr *p_user_CFields,
                        RACF_CUSTOM_FIELDS_block_ptr *p_group_CFields,
                        RACF_CTSACF_block_ptr        *p_user_ctsacfs,
                        RACF_CTSACF_block_ptr        *p_group_ctsacfs,
                        int                          *msg_issued,
                        ADMIN_PARAMS_rec_typ         *admin_params);

@********************************************************************
 *
 * Procedure Name : RACF_Free_CTSACF_Block
 *
 * Description    : Free the CTSA custom field block.
 *
 * Input          : Block address.
 *
 * Output         : None.
 *
 * Input/Output   :
 *
 * Return Value   : None.
 *
 * Side Effects   :
 *
 * Comments       :
 *
 ********************************************************************@

 void  RACF_Free_CTSACF_Block(ESA_DIAG_COMP_typ        comp,
                          RACF_CTSACF_block_ptr        p_block,
                          ADMIN_PARAMS_rec_typ        *admin_params);

@********************************************************************
 *
 * Procedure Name : RACF_IsAct_CTSACF_Feature
 *
 * Description    : Check if a CTSACF-feature is active.
 *
 * Input          : type - U / G
 *                : feature - feature number to look for.
 *
 * Output         : is_act -  active, inactive or in-error.
 *
 * Input/Output   :
 *
 * Return Value   : ESA_OK - found.
 *                : ESA_ERR - not found.
 *
 * Side Effects   :
 *
 * Comments       :
 *
 ********************************************************************@

 ESA_RC RACF_IsAct_CTSACF_Feature(ESA_DIAG_COMP_typ    comp,
                              char                     type,
                              RACF_CTSACF_FEATURE_typ  feature,
                              RACF_CTSACF_ACT_typ     *is_act,
                              char                    *cf_name,
                              ADMIN_PARAMS_rec_typ    *admin_params);

@********************************************************************
 *
 * Procedure Name : RACF_IsIt_CTSACF_cf
 *
 * Description    : Check if the custom field is internal (CTSACF)
 *
 *                  The caller can provide the CTSACF block address
 *                  or the type.  If block address is provided, it
 *                  be used. otherwise, this routine will find the
 *                  block according to the type.
 *
 *                  The caller can provide all or part of the output
 *                  parameters and get data accordingly. If a
 *                  parameter is omitted, NULL shuld be passed.
 *
 * Input          : type - U / G / NULL
 *                : ctsacf_block - block address / NULL
 *
 * Output         : p_entry - address of entry on CTSACF block.
 *                : feature - feature number.
 *                : act_type -  active, inactive or in-error.
 *
 * Input/Output   :
 *
 * Return Value   : ESA_OK - found.
 *                : ESA_ERR - not found.
 *
 * Side Effects   :
 *
 * Comments       :
 *
 ********************************************************************@

 ESA_RC RACF_IsIt_CTSACF_cf(ESA_DIAG_COMP_typ        comp,
                            char                     type,
                            RACF_CTSACF_block_ptr    ctsacf_block,
                            char                    *cf_name,
                            RACF_CTSACF_rec_ptr     *p_entry,
                            RACF_CTSACF_FEATURE_typ *feature,
                            RACF_CTSACF_ACT_typ     *is_act,
                            ADMIN_PARAMS_rec_typ    *admin_params);

@********************************************************************
 *
 * Procedure Name : RACF_Update_Revoke_Reason
 *
 * Description    : Set or remove the revoke reason and issue the
 *                  appropriate message.
 *
 * Input          : admin
 *
 * Input/Output   : user - userid
 *                : action - U(pdate) / R(evoke)
 *                : action_type - V(reVoke) / S(dreSume)
 *                : revoke_reason - revoke reason
 *                : cf_name - name of the custom field used to
 *                            hold the revoke reason.
 *
 * Return Value   : ESA_RC:
 *                  - ESA_OK - OK
 *                  - ESA_FATAL - error.
 *
 *
 * Side Effects   :
 *
 ********************************************************************@

 ESA_RC RACF_Update_Revoke_Reason(ESA_DIAG_COMP_typ      comp,
                                  char                  *user,
                                  char                   action,
                                  char                   update_type,
                                  char                   revoke_reason,
                                  char                  *cf_name,
                                  ADMIN_PARAMS_rec_typ  *admin_params);

@********************************************************************
 *
 * Procedure Name : RACF_Handle_CTSACF_Features
 *
 * Description    : Call the feature handling routines received from
 *                  the caller when the feature is actuve and the
 *                  local addinfo contains data for the relevant
 *                  field.
 *
 * Input          : type - U(ser) / G(roup)
 *                : rtn_table - routines, table.
 *                : addinfo - general addinfo.
 *                : ctsacf_addinfo - addinfo containing CTSACF fields.
 *                : entity_params - user_params, groyp_params, etc,
 *                                  depending on the caller.
 *                : args_num - number of arguments in variable parameter
 *                             list (...)
 *
 * Output         :
 *
 * Input/Output   :
 *
 * Return Value   : highest return code from the called routines.
 *
 * Side Effects   :
 *
 * Comments       :
 *
 ********************************************************************@

 ESA_RC RACF_Handle_CTSACF_Features(ESA_DIAG_COMP_typ   comp,
                              char                      type,
                              RACF_CTSACF_PROC_rec_typ *rtn_tab,
                              ADDINFO_rec_ptr           addinfo,
                              ADDINFO_rec_ptr           ctsacf_addinfo,
                              void                     *entity_params,
                              ADMIN_PARAMS_rec_typ     *admin_params,
                              int                       args_num,
                              ...                                   );
@* IS10100 - end  *@

@* WS10043 - start *@
@********************************************************************
 *
 * Procedure Name : RACF_Revoke_Reason_Code2Name
 *
 * Description    : Get the revoke reason name according to the
 *                  revoke reason code.
 *
 * Input          : code - 1 character revoke reason code.
 *
 * Return Value   : revoke reason name. Length: up to 20 characters.
 *
 * Side Effects   :
 *
 ********************************************************************@
 char *  RACF_Revoke_Reason_Code2Name(char     code);
@* WS10043 - end *@
#endif  @* RACF_H_included *@
*/
