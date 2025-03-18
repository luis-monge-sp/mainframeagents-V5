/* IS10174

    This member is obsolete

*/

/**************************************************************
*                                                             *
* Title            : ACF2                                     *
*                                                             *
* File Name        : ctsbacf.h                                *
*                                                             *
* Author           : Yishay Yovel                             *
*                                                             *
* Creation Date    : 22/06/1997                               *
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
* BS2306   Guy    27/10/1997 Added ACF2_MAX_GRP_UIDMASKS      *
*                            and updated some function        *
*                            prototypes                       *
* BS2307   Guy    03/11/1997 Added ACF2_MAX_LID_PARTS         *
* PS0234   Guy    13/01/1998 Added definitions for resources  *
* PS0278   Guy    15/06/1998 Added Rule_Backup                *
* WS2333   Guy    28/07/1998 Changes required for improved    *
*                            ruleset filtering routine        *
*                            + NEXTKEYs processing            *
*                            + deleted Get_Dsn_Resource       *
* WS2337   Guy    07/10/1998 Added Multi Valued Fields support*
* PS0320   Guy    22/10/1998 Deleted ACF2_CMDOUT_typ and      *
*                            ACF2_CMD_RC_typ                  *
* PS0321   Guy    25/10/1998 Changed structure of ACFFDR field*
*                            and added UIDX field             *
* PS0325   Guy    01/11/1998 Support new GSO fields+records   *
* BS2367   Guy    05/11/1998 Exits Names not nullified        *
* WS2338   Guy    10/11/1998 Support multivalued fields in    *
*                            groups processing                *
* PS0338   Guy    25/11/1998 Fix URID length                  *
* PS0339   Guy    26/11/1998 Return connections of all user   *
*                            UIDs if MULTI VALUED             *
* PS0345   Guy    02/12/1998 Add MULTIVALUE limits            *
* PS0347   Guy    03/12/1998 Determine MAXRULE usage with parm*
* PS0354   Guy    09/12/1998 Check Disconnect from Multivalued*
*                            field in Update User             *
* PS0364   Guy    24/12/1998 Remove EMULATE stuff             *
* WS2358   Guy    12/10/1999 Support for the virtual resource *
* PS0398   Guy    02/12/1999 ARI fails for qualified resrules *
* WS2370   RamiK  01/01/2002 add support for specific user    *
*                            profiles                         *
* BS2509   RamiK  03/03/2002 correct problem in online intrcptr*
* BS2513   RamiK  17/03/2002 add check of user existance      *
*                            before user profile update       *
* BS2537   AvnerL 17/07/2002 change switches to Y/N (ws2370)  *
* BS2556   AvnerL 14/10/02 send acf2 original error msg       *
* BS2593   AvnerL 07/04/03 trunc blanks from res-name         *
* BS2607   AvnerL 31/05/03 support user defined date fields   *
* IS0272   Michael 10/06/04  Update C31B libs with old fixes  *
* WS2565   Eli MY  15/10/06 ACF2 V3202: Update new password   *
*                           keywords.                         *
* IS10002  AvnerL 31/01/07 keep VERIFY_PASSWORD_BY_LOGIN value*
* IS10005  AvnerL 08/05/07 Support RU_LOCKED & RU_SUSPENDED.  *
* WS10005  AvnerL 29/05/07 Add OMVS kwds & more user profiles.*
* ws10006  AvnerL 01/08/07 Add PASSWORD user profile.         *
* WS10007  AvnerL 22/08/07 Insert calls CTSUpdPassword.       *
* BS10028  Nurity 23/02/12 Create Account fails after ugrade  *
*                          to 3.2.02.                         *
* BS10029  Nurity 12/03/12 Copy from 3.2.02 in order to       *
*                          apply BS10028.                     *
* IS10088  NuritY 03/06/13 S0C4 when UID len < 24.  (BS10030) *
* IS10119  AvnerL 17/03/14 Support ACCREL# as release source. *
* IS10122  NuritY 16/06/14 Ignore ROLESET rules.              *
* BS10126  AvnerL 28/09/14 Allow correlated UIDs for IIQ.     *
* WS10054  SeligT 26/07/16 Add ENQ/DEQ SYSTEMS RNL Parameter  *
* IS10139  SeligT 28/08/16 Update OS Functions for Agents     *
* sasc     NuritY 25/08/16 Adapt to CC changes.               *
* SAS2IBMT SeligT 17/07/16 SAS/C to IBM C Conversion Project  *
* SAS2IBMA AvnerL 17/07/16 SAS/C to IBM C Conversion Project  *
* IS10147A SeligT 18/06/17 Add support for password phrase    *
* IS10147B AvnerL 18/06/17 Set common_prm_ptr first.          *
* BS10076  SeligT 11/12/17 Organize Connector Locking         *
*                          Functions and Structures           *
* IS10174  NuritY 25/01/18 Make this member obsolete          *
**************************************************************/

/* IS10174
#ifndef ACF2_H_included
#define ACF2_H_included



 @* sasc - not needed anymore because it is defined in GLOBS
#ifdef EXT
#undef EXT
#endif
#ifndef ACF2_owner
#define EXT extern
#else
#define EXT
#endif
*@

@*SAS2IBMA #include MVS_SRV                                * IS10139 *@
#include MVS_C_SRV                                        @* SAS2IBMA*@

#define ACF2_get_list_count                    acf2010
#define ACF2_alloc_output_lines                acf2011
#define Global_Get_Virtual_Res                 acf2012    @* WS2358 *@
#define ACF2_Get_Resource                      acf2013    @* PS0234 *@
#define Trunc_UIDMASKS                         acf2014    @* PS0234 *@
#define ACF2_output_to_addinfo                 acf2015
#define ACF2_get_field                         acf2016
#define Build_Ruleset                          acf2017    @* PS0234 *@
#define Compile_Ruleset                        acf2018    @* PS0234 *@
#define Delete_Ruleset                         acf2019    @* PS0234 *@
#define ACF2_revoke_date                       acf2020
#define Build_Rule_Entry                       acf2021    @* PS0234 *@
#define ACF2_version_get                       acf2022
#define ACF2_version_check                     acf2023
#define Trunc_One_UIDMASK                      acf2024    @* PS0234 *@
#define ACF2_Filter_ADDINFO                    acf2025    @* PS0234 *@
#define ACF2_Convert_Access_Levels             acf2026    @* PS0234 *@
#define convert_tod_to_YYYYMMDDHHMMSS          acf2027    @* PS0234 *@
#define Init_ACE_Handle                        acf2028    @* PS0234 *@
#define Fill_ACE_Keywords                      acf2029    @* PS0234 *@
#define Prepare_ACF2IN_File                    acf2030    @* PS0234 *@
#define ACES_Match                             acf2031    @* PS0234 *@
#define Fill_Res_Keywords                      acf2032    @* PS0234 *@
#define Is_UIDMASK_User                        acf2033    @* PS0234 *@
#define Build_UIDMASK_User                     acf2034    @* PS0234 *@
#define Check_ACE_UNTIL                        acf2035    @* PS0234 *@
#define Is_LID_In_UID                          acf2036    @* PS0234 *@
#define ACF2_Set_LID_In_UID                    acf2037    @* PS0234 *@
#define ACF2_Is_LID_In_UID                     acf2038    @* PS0234 *@
#define ACF2_Update_KDT                        acf2039
#define ACF2_build_KDT_from_ACFFDR             acf2040
#define ACF2_Get_User                          acf2041
#define ACF2_Set_Global_Data                   acf2042
#define Global_ACF2_Date_Format                acf2043
#define ACF2_Convert_Date_Fields               acf2044
#define ACF2_Get_GSO                           acf2045
#define ACF2_Match_Id_to_Mask                  acf2046
#define ACF2_Get_Group                         acf2047
#define Rule_Backup                            acf2048    @* PS0278 *@
#define ACF2_Match_User_Group                  acf2049
#define ACF2_Get_User_UID                      acf2050
#define ACF2_Get_Next_User                     acf2051
#define ACF2_Get_UID_Info                      acf2052
#define Initialize_UID_Definitions             acf2053
#define Generate_UID_Update                    acf2054
#define GDB_Set_Mode                           acf2055
#define GDB_Insert                             acf2056
#define GDB_Delete                             acf2057
#define GDB_Update                             acf2058
#define GDB_Locate                             acf2059
#define GDB_GetNext                            acf2060
#define UID_Contains_MULTI                     acf2061    @* PS0321 *@
#define Reset_Field                            acf2062
#define Convert_Multi_to_Vec                   acf2063
#define GDB_Convert_to_Record                  acf2064
#define GDB_Convert_from_Record                acf2065
#define Round_Number                           acf2066
#define ACF2_Is_Virtual_Resource               acf2067    @* WS2358 *@
@* BS10076 #define Global_Total_Wait_Lock_Time acf2068              *@
@* BS10076 #define Global_Retry_Wait_Lock_Time acf2069              *@
@* WS10054  #define Global_Get_QNAME           acf2070              *@
#define Calc_User_UID_String                   acf2071
#define Global_UDB_Active                      acf2072
#define Build_Multi_UIDs                       acf2073    @* PS0321 *@
#define ACF2_Is_Part_Of_Multi                  acf2074    @* PS0345 *@
#define ACF2_Automate                          acf2075    @* WS2358 *@
#define Get_First_Qualifier                    acf2076    @* WS2358 *@
#define Strip_First_Qualifier                  acf2077    @* WS2358 *@
#define Most_Specific_Match                    acf2078    @* WS2358 *@
#define Prepare_KEYs_List                      acf2079    @* WS2358 *@
#define ACF2_Set_Keyword_Value                 acf2080    @* WS2358 *@
#define ACF2_Get_Field_Information             acf2085
#define ACF2_Get_Group_UIDMASKS                acf2086
#define Convert_UID_Offset_to_Field_Name       acf2090
#define Check_Group_Correllation               acf2091
#define Check_User_Force_Disconnect_from_Group acf2092
#define Convert_Keyword_Chars                  acf2093
#define Get_ACFFDR_In_ESS_Format               acf2094
#define Get_UID_In_ESS_Format                  acf2095
#define Check_Group_UIDMASKS                   acf2096
#define ACF2_Refresh_User_UID                  acf2097
#define Check_Mult_Force_Disconnect_from_Group acf2098
#define UDB_Set_Mode                           acf2100
#define UDB_Insert                             acf2101
#define UDB_Delete                             acf2102
#define UDB_Update                             acf2103
#define UDB_Locate                             acf2104
#define UDB_GetNext                            acf2105
#define UDB_Get_Next_User                      acf2106    @* WS2338 *@
#define Trim                                   acf2110
#define ACF2_Get_User_Groups                   acf2120    @*ws2431  *@
#define ACF2_Get_specific_user_profile         acf2121    @*ws2370  *@
#define ACF2_Get_all_specific_user_profiles    acf2122    @*ws2370  *@
#define ACF2_UP_switch_is_activated            acf2123    @*ws2370  *@
#define ACF2_UP_exists                         acf2124    @*ws2370  *@
#define ACF2_Free_ACFFDR                       acf2125    @*BS2509  *@
#define ACF2_User_Exists                       acf2126    @*bs2513  *@
#define Trunc_Blanks                           acf2127    @*bs2593  *@

#define MAX_API_ST                   4096
#define ACF2_OUTPUT_SIZE            16384  @* for user/group   *@
                            @* r0003 increased to 16k from 4k  *@
#define ACF2_CNN_OUTPUT_SIZE          512  @* for connection   *@
                            @* r0003 increased to 512 from 500 *@
#define ACF2_END_OF_KDT        '\xff'

#define ACF2_EOF            "ACF02010"
#define ACF2_NOT_FOUND      "ACF0A005"
#define ACF2_ARULE_EXISTS   "ACF03017"
#define ACF2_RRULE_EXISTS   "ACF04012"
#define ACF2_ARULE_NOT_FND  "ACF03006"
#define ACF2_RESRULE_EOF    "ACF04005"
#define ACF2_RULE_NOT_FOUND "ACF03005"

#define ACF2_RETRIEVE_OP    "RET"
#define ACF2_RET_KEY_OP     "KEY"          @* WS2358 *@

#define ACF2_UID_SIZE          25
#define ACF2_LID_SIZE          9
#define ACF2_UDB_LID_SIZE      8           @* PS0338           *@
#define ACF2_URID_SIZE         5           @* PS0339           *@
#define ACF2_ACCRULE_KEY_SIZE  9           @* PS0234           *@
#define ACF2_RESRULE_KEY_SIZE  41          @* PS0234           *@
#define ACF2_MAX_LID_PARTS     24          @* BS2307           *@
#define ACF2_MAX_GRP_UIDMASKS  25          @* BS2306           *@
#define ACF2_FIELDNAME_SIZE    9
#define ACF2_QUALIFIER_SIZE    9           @* WS2358           *@
#define ACF2_MAX_QUALIFIERS    22          @* WS2358           *@
#define ACF2_VOLSER_SIZE       7           @* WS2358           *@
#define ACF2_MAX_NUM_OF_RULES  5000        @* WS2358           *@

#define ACF2_SYSID_SIZE        9
#define ACF2_RECID_SIZE       41

#define ACF2_ADDINFO               512
#define ACF2_RULE_BUFFER_LENGTH  32768     @* PS0234           *@
#define ACF2_RULE_SOURCE_LENGTH  128*1024  @* PS0234           *@
#define ACF2_RULE_SOURCE_INC     512*1024  @* PS0234           *@
#define ACF2_NEXTKEY_LIST_EXTENT 4096      @* WS2333           *@
#define ACF2_ARE_BUFFER_LENGTH   32760

#define ACF2_DATE_FORMAT             "OPTS.DATE"
#define ACF2_BYPASS_DISCONNECT_CHECK "@#!BYPASS!#@"
#define ACF2_ROLESET_IGNORED_MSG  "ROLESET rule ignored"  @* IS10122 *@

#define UP_SWITCH_CONST   "_SWITCH"  @* ws2370 *@
#define UP_SWITCH_ON      "Y"        @* ws2370 , bs2537 *@
#define UP_SWITCH_OFF     "N"        @* ws2370 , bs2537 *@
#define UP_SWITCH_CONST_LEN 7        @* ws2370 *@

typedef enum {
 CHECK_COLLISION,
 CHECK_CORRELLATION
} CORRELLATION_CHECK_MODE;

typedef enum {
 FLAG_KEYWORD,
 REG_KEYWORD,
 MULTI_KEYWORD,
 LIST_KEYWORD
} ACF2_Keyword_Type;

typedef enum {
 BEFORE_KEYWORD,
 IN_KEYWORD,
 IN_VALUE
} ACF2_Automat_States_Type;

typedef enum {
   ACF2_Return_Multi_UIDs,
   ACF2_No_Multi_UIDs
} ACF2_Multi_UIDs_Mode_Type;                              @* PS0321 *@

typedef enum {
   ACF2_Scan_Not_Started,
   ACF2_Scan_Found_Mask,
   ACF2_Scan_Found_Value
} ACF2_MVF_Scan_Status_Type;                              @* PS0345 *@

typedef enum {
   ACF2_Return_Ruleset,
   ACF2_Return_Rule_Entries
} ACF2_Resource_Data_Type;

typedef enum {
   ACF2_Replace_Rule,
   ACF2_Dont_Replace_Rule
} ACF2_Replace_Rule_Type;

typedef enum {
   ACF2_Object_is_Masked,
   ACF2_Object_is_Not_Masked
} ACF2_Object_Mask;

typedef enum {
   ACF2_NO, ACF2_YES
} ACF2_FLAG_typ;

@* PS0320 - Deleted
typedef enum {
   CMDOUT_YES, CMDOUT_NO, CMDOUT_ERR
} ACF2_CMDOUT_typ;

typedef enum {
   MAX_OK_RC_0 = 0, MAX_OK_RC_4 = 4
} ACF2_CMD_RC_typ;
*@

typedef char ONE_KEYWORD_typ[MAX_RSS_ADDINFO_KWD_LEN+1];

typedef struct S_KEYWORDS {
  ONE_KEYWORD_typ   keyword;
  ADDINFO_typ       type;
} KEYWORDS_LIST_typ;

@* WS2358 - moved from Get_Rule_Entry_Info (CTSBRES) *@
static KEYWORDS_LIST_typ access_keywords[] = {
                                              "READ",    TYPE_1A,
                                              "WRITE",   TYPE_1A,
                                              "EXEC",    TYPE_1A,
                                              "ALLOC",   TYPE_1A,
                                              "",        TYPE_1A };

@* WS2358 - list of unsupported keywords for ARI feature *@
static KEYWORDS_LIST_typ unsupp_keywords[] = {
                                              "VOL",     TYPE_1A,
                                              "SOURCE",  TYPE_1A,
                                              "SHIFT",   TYPE_1A,
                                              "LIB",     TYPE_1A,
                                              "PGM",     TYPE_1A,
                                              "DDN",     TYPE_1A,
                                              "UNTIL",   TYPE_1A,
                                              "ACTIVE",  TYPE_1A,
                                              "",        TYPE_1A };

typedef struct {
  int   max_len;                            @* max  length   *@
  int   used_len;                           @* used length   *@
  char  buffer[ACF2_OUTPUT_SIZE];           @* output buffer *@
  } ACF2_OUTPUT_LINE_rec_typ, *ACF2_OUTPUT_LINE_rec_ptr;

 @*
  *   Handle structure for Get Resource ACL processing
  *
  *   WS2333:
  *   Records_Buffer is the address of the first record which
  *   interests us inside Rule_Buffer. It is created by the
  *   Filter_Rule_Records function. Current_ACE_Offset is an
  *   offset WITHIN Records_Buffer and NOT Rule_Buffer !!!
  *   (Records_Buffer is considered as a "buffer within a buffer")
  *@

 typedef struct {
   char                        *Rule_Buffer;        @* Orig. buffer  *@
   int                          Rule_Buffer_Len;    @* Buffer length *@
   int                          Current_ACE_Pos;    @* ACE ID(0,1,..)*@
   int                          First_Call;         @* 1st call? Y/N *@
   char                        *Current_ACE_Offset; @* In filt buff! *@
   char                        *Records_Buffer;     @* WS2333        *@
   int                          Records_Buffer_Len; @* WS2333        *@
   } ACF2_Get_ResACL_Handle;

 @*
  *   Handle structure for Get Resource processing
  *@

 typedef struct {
   RESOURCE_typ                 Current_Key;
   int                          First_Call;
   char                        *Nextkeys_Chain;       @* WS2333 *@
   char                        *Nextkeys_List_Ptr;    @* WS2333 *@
   int                          Load_Nextkeys;        @* WS2333 *@
   int                          Nextkeys_Depth;       @* WS2333 *@
   int                          Num_Loaded_Nextkeys;  @* WS2333 *@
   } ACF2_Get_Res_Handle;

 @* WS2358 *@
 @*
  *   Structure representing one $KEY
  *   (used by Most_Specific_Match for ARI processing)
  *@
 typedef struct
 {
   char   Key_Cell_Val[ACF2_RESRULE_KEY_SIZE];     @* $KEY value    *@
   int    Key_Cell_Match;                          @* is matching ? *@
   int    Key_Cell_Rank;                           @* match rank    *@
 } ACF2_Key_Cell_Typ, *ACF2_Key_Cell_Ptr;

@* ------------------------ IS10002 ---------------------------- *
 * ACF2 USA-API Parameters                                       *
 * ------------------------------------------------------------- *
 * The following structure is initialized during CTSAPIInit call *
 * and contains static data of the USA-API such are loaded       *
 * RSSPARM information.                                          *
 *                                                               *
 * apiinit_handle is pointing to this structure.                 *
 *                                                               *
 * Note that changes in this type might require rebuilt of ALL   *
 * the USA-API  modules.                                         *
 * When adding fields, try to keep the total length 1,000 bytes. *
 * Which means if you add a 2 bytes fields, subtract 2 from ffu. *
 * ------------------------------------------------------------- *
 *    !!! Minimize changes and usage of this record !!!          *
 * ------------------------------------------------------------- *@

typedef  struct _ACF2_PARAMS{
  @* move "* common_params" to start of "ACF2 PARAMS".       IS10147B*@
  COMMON_PARAMS_rec_typ * common_params; @* ptr to common    WS10054 *@
                          @* usaapi parameters area (look at WS10054 *@
                          @* CTS.CMAC(CTSCSRV) for details)  WS10054 *@
  char rssparm_ffu1[4];              @* should be like RACF struct ! *@
  char rssparm_VERIFY_PASS_LOGIN[2]; @* VERIFY_PASSWORD_BY_LOGIN val *@
   char rssparm_SUSPEND_IS_LOCKED[2];@* SUSPEND_IS_LOCKED parm IS10005*@
   char rssparm_DISR_RU_SUSPENDED[2];@* DISREGARD_RU_SUSPENDED IS10005*@
   @* BS10028 - no insert calls to CTSUpdPassword
   char Insert_Calls_CTSUpdPassword[2];  * Insert calls        WS10007*
                                                                     *@
  short uidlen;                                             @*IS10088*@
  char rssparm_IIQACF2GROUPS[2];    @* IIQACF2GROUPS          IS10128*@
  int  rssparm_ACF2_LOCK;           @* copy of RSSPARM        BS10076
                                       ACF2_LOCK parameter    BS10076
                                       or the default value   BS10076*@
  int  rssparm_ACF2_LOCK_INTERVAL;  @* copy of RSSPARM        BS10076
                                       ACF2_LOCK_INTERVAL     BS10076
                                       parameter or the       BS10076
                                       default value          BS10076*@
  char ffu[970];          @* for future use, please update in change *@
} ACF2_PARAMS_rec_typ, *ACF2_PARAMS_rec_ptr;
@* ------------------- end of IS10002 -------------------------- *@

@**************************************************************
*                                                             *
* PROCEDURE NAME : ACF2cmd                                    *
*                                                             *
* DESCRIPTION    : check, build and issue a ACF2 command      *
*                                                             *
* INPUT          :                                            *
*                                                             *
* OUTPUT         : none                                       *
*                                                             *
* RETURN VALUE   : ESA_RC                                     *
*                                                             *
**************************************************************@
EXT ESA_RC ACF2cmd (char                       * func,
                    char                       * ddn,
                    USER_typ                     admin_uname,
                    char                       * cmd,
     @* ps0320 *@   CMD_RC_typ                   max_ok_rc,
                    short                        numaddi,
                    ADDINFO_rec_ptr            * addinfo,
                    ADDINFO_MKTE_rec_typ       * mkt,
                    ADDINFO_KDTE_rec_typ       * kdt,
                    CTSAMSG_DEST_TABLE_rec_typ * dest,
                    CTSAMSG_HANDLE_rec_typ     * msgs,
     @* ps0320 *@   CMDOUT_typ                   gout_flag,
                    ACF2_FLAG_typ                ignore_no_addinfo);

@**************************************************************
*                                                             *
* PROCEDURE NAME : ACF2_version_get                           *
*                                                             *
* DESCRIPTION    : Retrieve ACF2 version                      *
*                                                             *
* INPUT          : none                                       *
*                                                             *
* OUTPUT         : version                                    *
*                                                             *
* RETURN VALUE   : ESA_OK,  ESA_FATAL                         *
*                                                             *
**************************************************************@

EXT ESA_RC ACF2_version_get( char   * version,
         @*IS10119*@         ADMIN_PARAMS_rec_typ *admin_params);

@**************************************************************
*                                                             *
* PROCEDURE NAME : ACF2_version_check                         *
*                                                             *
* DESCRIPTION    : Check ACF2 version                         *
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

EXT ESA_RC ACF2_version_check(RSS_typ                      rss_type,
                              CTSAMSG_DEST_TABLE_rec_typ * dest,
                              CTSAMSG_HANDLE_rec_typ     * msgs);

 @**************************************************************
 * Global COnstants                                            *
 **************************************************************@
#define DEBUG_SHOW_IGNORE           0
#define DEBUG_SHOW_IMPORTANT        1
#define DEBUG_SHOW_REGULAR          10
#define DEBUG_SHOW_ALL              20

#define UDB_ACTIVE                  "Y"
@* BS10076 #define GLOBAL_TOTAL_WAIT_LOCK_TIME "WAIT_LOCK"           *@
@* BS10076 #define DEFAULT_TOTAL_WAIT_LOCK_TIME 600                  *@
@* BS10076 #define GLOBAL_RETRY_WAIT_LOCK_TIME "WAIT_QUEUE"          *@
@* BS10076 #define DEFAULT_RETRY_WAIT_LOCK_TIME 30                   *@
#define GLOBAL_UDB_ACTIVE           "UDB_ACTIVE"
#define DEFAULT_UDB_ACTIVE          "Y"

@* WS2358 *@
#define GLOBAL_VIRTUAL_RES_NAME     "VIRTUAL_RES_NAME"
#define DEFAULT_VIRTUAL_RES_NAME    "$$ADDACE$$"

#define GDB_KEY_LENGTH 64

#define GDB_RECORD_TYPE_GROUP  'G'
#define GDB_RECORD_TYPE_USER   'U'
#define GDB_RECORD_TYPE_IGNORE '*'

@* The following values are the default values for ACF2      BS10076
   file-related (GDB and UDB) locking. The ACF2_LOCK_TOTAL   BS10076
   field is in seconds and represents the total amount of    BS10076
   time we will wait for a lock to be released before we     BS10076
   give up. The ACF2_LOCK_INTERVAL_DFLT field is in          BS10076
   milliseconds and represents how often we attempt to       BS10076
   acquire the lock (if it was held on a previous attempt).  BS10076 *@
#define ACF2_LOCK_INTERVAL_DFLT 1000         @* milliseconds BS10076 *@
#define ACF2_LOCK_TOTAL          600         @* seconds      BS10076 *@

 @**************************************************************
 * UID Update modes                                            *
 **************************************************************@
typedef enum {
  Disconnect_Update,
  Connect_Update
} UID_Update_type;

 @**************************************************************
 * UIDMASK, UID , LID , URID                                   *
 **************************************************************@
typedef char UIDMASK[ACF2_UID_SIZE];
typedef char UID[ACF2_UID_SIZE];
typedef char LID[ACF2_LID_SIZE];
typedef char URID[ACF2_URID_SIZE];                        @* WS2338 *@
typedef char UDBLID[ACF2_UDB_LID_SIZE];                   @* PS0338 *@
typedef char UDBURID[ACF2_URID_SIZE-1];                   @* PS0339 *@

typedef char QUALIFIER[ACF2_QUALIFIER_SIZE];              @* WS2358 *@
typedef char VOLSER[ACF2_VOLSER_SIZE];                    @* WS2358 *@

 @**************************************************************
 * Group Database Locate  types                                *
 **************************************************************@
typedef enum {
 GDB_Exact_Locate,
 GDB_Generic_Locate,
 GDB_Next_Locate
} GDB_Locate_Mode;

 @**************************************************************
 * User  Database Locate  types                                *
 **************************************************************@
typedef enum {
 UDB_Exact_Locate,
 UDB_Generic_Locate,
 UDB_Next_Locate
} UDB_Locate_Mode;

 @**************************************************************
 * Group Database Codes                                        *
 **************************************************************@
typedef enum {
 GDB_OK,

 GDB_Wrong_Mode,

 GDB_Open_Failed,
 GDB_Close_Failed,
 GDB_Insert_Failed,
 GDB_Update_Failed,
 GDB_Delete_Failed,
 GDB_Locate_Failed,
 GDB_GetNext_Failed,

 GDB_Obtain_Lock_Failed,
 GDB_Release_Lock_Failed,
 GDB_Read_Lock_Failed,
 GDB_Update_Lock_Failed,

 GDB_EOF,
 GDB_Duplicate_Key,
 GDB_Not_Found,
 GDB_Database_Full,
 GDB_IO_Error
 } GDB_Codes;

 @**************************************************************
 * Group Database Open modes                                   *
 **************************************************************@
typedef enum {
 GDB_Closed  ,
 GDB_ReadOnly,
 GDB_ReadWrite
 } GDB_Modes;

 @**************************************************************
 * Group Database Lock types                                   *
 **************************************************************@
typedef enum {
 GDB_No_Lock,
 GDB_ReadOnly_Lock,
 GDB_ReadWrite_Lock,
 GDB_ALL_Locks
 } GDB_Locks;

 @**************************************************************
 * User  Database Codes                                        *
 **************************************************************@
typedef enum {
 UDB_OK,
 UDB_Wrong_Mode,
 UDB_Open_Failed,
 UDB_Close_Failed,
 UDB_Insert_Failed,
 UDB_Update_Failed,
 UDB_Delete_Failed,
 UDB_Locate_Failed,
 UDB_GetNext_Failed,
 UDB_Obtain_Lock_Failed,
 UDB_Release_Lock_Failed,
 UDB_Read_Lock_Failed,
 UDB_Update_Lock_Failed,
 UDB_EOF,
 UDB_Duplicate_Key,
 UDB_Not_Found,
 UDB_Database_Full,
 UDB_IO_Error
 } UDB_Codes;

 @**************************************************************
 * User  Database Open modes                                   *
 **************************************************************@
typedef enum {
 UDB_Closed  ,
 UDB_ReadOnly,
 UDB_ReadWrite
 } UDB_Modes;

 @**************************************************************
 * User  Database Lock types                                   *
 **************************************************************@
typedef enum {
 UDB_No_Lock,
 UDB_ReadOnly_Lock,
 UDB_ReadWrite_Lock,
 UDB_ALL_Locks
 } UDB_Locks;

 @**************************************************************
 * UID description entry                                       *
 **************************************************************@
typedef struct{
 char  UID_Part_Field_Name[16];
 short UID_Part_Offset;
 short UID_Field_Offset;
 short UID_Part_Length;
 char  UID_Part_Multi;                                    @* PS0321 *@
 } UID_Structure;

 typedef enum {
   ACF2_TRUE,
   ACF2_FALSE,
   ACF2_ERROR
   } ACF2_Flags;

 @**************************************************************
 * Match_Id_to_Mask object types                               *
 **************************************************************@
 typedef enum {
   LID_Object,
   UID_Object,
   Undefined_Object
  } ACF2_Object_Type;

 @**************************************************************
 * Get User with/without fields                                *
 **************************************************************@
 typedef enum
  {
    ACF2_GetUser_Get_Fields,
    ACF2_GetUser_No_Fields   } ACF2_GetUser_Fields;

 @**************************************************************
 * ACF2 API return codes                                       *
 **************************************************************@
 typedef enum
  {
    ACF2_OK ,
    ACF2_Not_Found,
    ACF2_ACF2_Not_Active,
    ACF2_Internal_Error } ACF2_API_Return_Codes;

 @**************************************************************
 * KDT and ACFFDR merge option                                 *
 **************************************************************@
 typedef enum
  { ACF2_KDT_Merge,
    ACF2_KDT_Replace } ACF2_KDT_Processing_Option;

 @**************************************************************
 * KDT from ACFFDR processing                                  *
 **************************************************************@
 typedef enum
  { ACF2_Get_ReadWrite_Fields,
    ACF2_Get_ReadWrite_Fields_and_NoFlag,
    ACF2_Get_ReadOnly_Fields,
    ACF2_Get_ReadOnly_Fields_and_NoFlag,
    ACF2_Get_All_Fields     ,
    ACF2_Get_All_Fields_and_NoFlag } ACF2_Get_Fields_Filter;

 @**************************************************************
 * ARE structure                                               *
 **************************************************************@
  typedef struct {
   short          ARE_Length;
   char           ARE_Last_Entry;
 #define ARE_LAST_ENTRY 0X80
   char           ARE_Field_Type;
 #define ARE_BINARY  0X80
 #define ARE_CHAR    0X40
 #define ARE_PACKED  0X20
 #define ARE_TIME    0X10
 #define ARE_BIT     0X08
 #define ARE_TOD     0X04
 #define ARE_HEX     0X02
 #define ARE_ENCR    0X01
   char           ARE_Field_Name[8];
   char           ARE_Type;
 #define          ARE_IS_XARE    0X10
 #define          ARE_IS_MULTI   0X01
 #define          ARE_IS_2BMULTI 0X05   @* RRK *@
   char           ARE_Auth;
   short          ARE_Value_Length;
   char           ARE_Value;
   } ARE_typ;

 @**************************************************************
 * XARE header                                                 *
 **************************************************************@
typedef struct {
 short          XARE_Fixed_Length;
 short          XARE_Value_Offset;
 } XARE_Header_typ;

 @**************************************************************
 * XARE value extension                                        *
 **************************************************************@
typedef struct {
 short          XARE_Value_Length;
 char           XARE_Value;
 } XARE_Value_typ;

 @**************************************************************
 * Infostorage Record retrieval strcuture                      *
 **************************************************************@
typedef struct {
 char  INF_sysid_masked;
 char  INF_recid_masked;
 short INF_sysid_length;
 short INF_recid_length;
 char  INF_class;
 char  INF_type[3];
 char  INF_key[41];
 } ACF2_INF_record;

 @**************************************************************
 * Infostorage GSO records matching mode                       *
 **************************************************************@
typedef enum {
 ACF2_INF_Get_Best_Match,
 ACF2_INF_Get_Match
 } ACF2_INF_Match;

 @**************************************************************
 * Snap Area in C                                              *
 **************************************************************@

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

 @**************************************************************
 * DATA AREAS for GLOBAL PARAMETERS Processing                 *
 **************************************************************@

#ifdef ACF2_GLOBAL_PARAMETERS_DEFINITIONS

static ADDINFO_RKTE_rec_typ rkt[] = {
   "PSWD.MINPSWD",         @* from RSS_PARAMS structure *@
   ""              @* a null string indicate end-of-rkt *@
 };

@*
 *     MKT for GSO
 *         (Mandatory Keywords Table)
 *@

static ADDINFO_MKTE_rec_typ mkt[] = {
       ""         @* a null string indicate end-of-mkt *@
 };

@*
 *     KDT for GSO
 *         (Keyword Decscription Table)
 *@

static ADDINFO_KDTE_rec_typ OPTS_kdt[] =      {

 @*                                                            *@
 @* keyword             type   negative       value delimiters *@
 @*                             value                          *@
 @* =================== ====== ============== ================ *@

  {"BLPLOG"             ,KDT_FLG ,"NOBLPLOG"           ,""  , "" },
  {"CACHE"              ,KDT_FLG ,"NOCACHE"            ,""  , "" },
  {"CMDREC"             ,KDT_FLG ,"NOCMDREC"           ,""  , "" },
  {"CONSOLE"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"CPF"                ,KDT_FLG ,"NOCPF"              ,""  , "" },
  {"CPUTIME"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"DATE"               ,KDT_KEY ,""                   ,"(" , ")"},
  {"DDB"                ,KDT_FLG ,"NODDB"              ,""  , "" },
  {"DFTLID"             ,KDT_KEY ,""                   ,"(" , ")"},
  {"DFTSTC"             ,KDT_KEY ,""                   ,"(" , ")"},
  {"DDB"                ,KDT_FLG ,"NODDB"              ,""  , "" },
  {"INFOLIST"           ,KDT_KEY ,""                   ,"(" , ")"},
  {"JOBCK"              ,KDT_FLG ,"NOJOBCK"            ,""  , "" },
  {"MODE"               ,KDT_KEY ,""                   ,"(" , ")"},
  {"MAXVIO"             ,KDT_KEY ,""                   ,"(" , ")"},
  {"NOTIFY"             ,KDT_FLG ,"NONOTIFY"           ,""  , "" },
  {"PRIMARY"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"SECONDARY"          ,KDT_KEY ,""                   ,"(" , ")"},
  {"RPTSCOPE"           ,KDT_FLG ,"NORPTSCOPE"         ,""  , "" },
  {"SHRDASD"            ,KDT_FLG ,"NOSHRDASD"          ,""  , "" },
  {"STAMPSMF"           ,KDT_FLG ,"NOSTAMPSMF"         ,""  , "" },
  {"STC"                ,KDT_FLG ,"NOSTC"              ,""  , "" },
  {"SYSPLEX"            ,KDT_FLG ,"NOSYSPLEX"          ,""  , "" },
  {"TAPEDSN"            ,KDT_FLG ,"NOTAPEDSN"          ,""  , "" },
  {"TEMPDSN"            ,KDT_FLG ,"NOTEMPDSN"          ,""  , "" },
  {"TNGMON"             ,KDT_FLG ,"NOTNGMON"           ,""  , "" },
  {"UADS"               ,KDT_FLG ,"NOUADS"             ,""  , "" },
  {"XBM"                ,KDT_FLG ,"NOXBM"              ,""  , "" },
  {"VTAMOPEN"           ,KDT_FLG ,"NOVTAMOPEN"         ,""  , "" },
  {""                   ,KDT_FLG ,""          ,""  ,""           }
  }; @* PS0325-Added CPF,SYSPLEX,TEMPDSN,TNGMON *@

static ADDINFO_KDTE_rec_typ PSWD_kdt[] =      {

 @*                                                            *@
 @* keyword             type   negative       value delimiters *@
 @*                             value                          *@
 @* =================== ====== ============== ================ *@

  {"MAXTRY"             ,KDT_KEY ,""                   ,"(" , ")"},
  {"MINPSWD"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"PASSLMT"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"PSWDALT"            ,KDT_FLG ,"NOPSWDALT"          ,""  , "" },
  {"PSWDFRC"            ,KDT_FLG ,"NOPSWDFRC"          ,""  , "" },
  {"PSWDHST"            ,KDT_FLG ,"NOPSWDHST"          ,""  , "" },
  {"PSWDJES"            ,KDT_FLG ,"NOPSWDJES"          ,""  , "" },
  {"PSWDLID"            ,KDT_FLG ,"NOPSWDLID"          ,""  , "" },
  {"PSWDNCH"            ,KDT_FLG ,"NOPSWDNCH"          ,""  , "" },
  {"PSWDNUM"            ,KDT_FLG ,"NOPSWDNUM"          ,""  , "" },
  {"PSWDREQ"            ,KDT_FLG ,"NOPSWDREQ"          ,""  , "" },
  {"PSWDRSV"            ,KDT_FLG ,"NOPSWDRSV"          ,""  , "" },
  {"PSWDXTR"            ,KDT_FLG ,"NOPSWDXTR"          ,""  , "" },
  {"WRNDAYS"            ,KDT_KEY ,""                   ,"(" , ")"},
  @* WS2565 : start here *@
  {"PSWDALPH"           ,KDT_FLG ,"NOPSWDALPH"         ,"" , ""},
  {"PSWDMAX"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"PSWDMAXL"           ,KDT_KEY ,""                   ,"(" , ")"},
  {"PSWDMIN"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"PSWDMIXD"           ,KDT_FLG ,"NOPSWDMIXD"         ,"" , ""},
  {"PSWDNMIC"           ,KDT_FLG ,"NOPSWDNMIC"         ,"" , ""},
  {"PSWDPAIR"           ,KDT_KEY ,""                   ,"(" , ")"},
  {"PSWDPLST"           ,KDT_KEY ,""                   ,"(" , ")"},
  {"PSWDSIM"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"PSWDSPLT"           ,KDT_FLG ,"NOPSWDSPLT"         ,"" , ""},
  {"PSWDVFY"            ,KDT_FLG ,"NOPSWDVFY"          ,"" , ""},
  {"PSWDVOWL"           ,KDT_FLG ,"NOPSWDVOWL"         ,"" , ""},
  {"PSWNAGE"            ,KDT_FLG ,"NOPSWNAGE"          ,"" , ""},
  {"PSWXHIST"           ,KDT_FLG ,"NOPSWXHIST"         ,"" , ""},
  {"PSWXHST@"           ,KDT_KEY ,"" @*ws10006*@       ,"(" , ")"},
  @* PSWXHST@ <==> PSWXHST# (# isn't supported in ESS) ws10006 *@
  @* PSWXHSTN is known as PSWXHST# (# isn't supported in ESS)  *@
  @* WS2565 : end here *@
  {""                   ,KDT_FLG ,""                   ,""  ,""   }
  }; @* PS0325-Added PSWDHST,PSWDLID,PSWDNCH,PSWDNUM,PSWDREQ,PSWDRSV *@

@*** password phrase KDT *** IS10147A start ***@
static ADDINFO_KDTE_rec_typ PWPHRASEG_kdt[] =  {
 @* keyword             type   negative val   value delimiters *@
 @* =================== ====== ============== ================ *@
  {"MINLEN"             ,KDT_KEY ,""                   ,"(" , ")"},
  {"HISTORY"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"REPCHAR"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"SPECLIST"           ,KDT_KEY ,""                   ,"(" , ")"},
  {""                   ,KDT_FLG ,""                   ,""  ,""   }
  };
@*** password phrase KDT *** IS10147A end   ***@

static ADDINFO_KDTE_rec_typ RULEOPTS_kdt[] =      {

 @*                                                            *@
 @* keyword             type   negative       value delimiters *@
 @*                             value                          *@
 @* =================== ====== ============== ================ *@

  {"ACCRULE"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"CENTRAL"            ,KDT_FLG ,"NOCENTRAL"          ,""  , "" },
  {"CHANGE"             ,KDT_FLG ,"NOCHANGE"           ,""  , "" },
  {"DECOMP"             ,KDT_KEY ,""                   ,"(" , ")"},
  {"$NOSORT"            ,KDT_FLG ,"NO$NOSORT"          ,""  , "" },
  {"PATHTRAN"           ,KDT_FLG ,"NOPATHTRAN"         ,""  , "" },
  {"RSCDIR"             ,KDT_KEY ,""                   ,"(" , ")"},
  {"RSCRULE"            ,KDT_KEY ,""                   ,"(" , ")"},
  {"RULELONG"           ,KDT_FLG ,"NORULELONG"         ,""  , "" },
  {"VOLRULE"            ,KDT_FLG ,"NOVOLRULE"          ,""  , "" },
  {""                   ,KDT_FLG ,""                   ,""  ,""  },
  }; @* PS0325-Added RULELONG *@

static ADDINFO_KDTE_rec_typ EXITS_kdt[] =      {

 @*                                                            *@
 @* keyword             type   negative       value delimiters *@
 @*                             value                          *@
 @* =================== ====== ============== ================ *@

  {"DSNGEN"             ,KDT_KEY ,"DSNGEN()"           ,"(" , ")"},
  {"DSNPOST"            ,KDT_KEY ,"DSNPOST()"          ,"(" , ")"},
  {"EXPPXIT"            ,KDT_KEY ,"EXPPXIT()"          ,"(" , ")"},
  {"INFOPRE"            ,KDT_KEY ,"INFOPRE()"          ,"(" , ")"},
  {"INFOPST"            ,KDT_KEY ,"INFOPST()"          ,"(" , ")"},
  {"LGNIXIT"            ,KDT_KEY ,"LGNIXIT()"          ,"(" , ")"},
  {"LGNPARM"            ,KDT_KEY ,"LGNPARM()"          ,"(" , ")"},
  {"LGNPXIT"            ,KDT_KEY ,"LGNPXIT()"          ,"(" , ")"},
  {"LGNTERM"            ,KDT_KEY ,"LGNTERM()"          ,"(" , ")"},
  {"LIDLOC"             ,KDT_KEY ,"LIDLOC()"           ,"(" , ")"},
  {"LIDMOD"             ,KDT_KEY ,"LIDMOD()"           ,"(" , ")"},
  {"LIDPOST"            ,KDT_KEY ,"LIDPOST()"          ,"(" , ")"},
  {"LIDPRE"             ,KDT_KEY ,"LIDPRE()"           ,"(" , ")"},
  {"NEWPXIT"            ,KDT_KEY ,"NEWPXIT()"          ,"(" , ")"},
  {"PGMOVRD"            ,KDT_KEY ,"PGMOVRD()"          ,"(" , ")"},
  {"RSCXIT1"            ,KDT_KEY ,"RSCXIT1()"          ,"(" , ")"},
  {"RSCXIT2"            ,KDT_KEY ,"RSCXIT2()"          ,"(" , ")"},
  {"RULEPRE"            ,KDT_KEY ,"RULEPRE()"          ,"(" , ")"},
  {"RULEPST"            ,KDT_KEY ,"RULEPST()"          ,"(" , ")"},
  {"SEVPOST"            ,KDT_KEY ,"SEVPOST()"          ,"(" , ")"},
  {"SEVPRE"             ,KDT_KEY ,"SEVPRE()"           ,"(" , ")"},
  {"SRCXIT"             ,KDT_KEY ,"SRCXIT()"           ,"(" , ")"},
  {"STCXIT"             ,KDT_KEY ,"STCXIT()"           ,"(" , ")"},
  {"SVCIXIT"            ,KDT_KEY ,"SVCIXIT()"          ,"(" , ")"},
  {"VIOEXIT"            ,KDT_KEY ,"VIOEXIT()"          ,"(" , ")"},
  {"VLDEXIT"            ,KDT_KEY ,"VLDEXIT()"          ,"(" , ")"},
  {""                   ,KDT_FLG ,""                   ,""  ,""  },
  }; @* BS2367 - Added negative values to all keywords *@

@* PS0325 - Added RESWORD record support *@
static ADDINFO_KDTE_rec_typ RESWORD_kdt[] =      {

 @*                                                            *@
 @* keyword             type   negative       value delimiters *@
 @*                             value                          *@
 @* =================== ====== ============== ================ *@

  {"PREFIXES"           ,KDT_KEY ,""                   ,"(" , ")"},
  {""                   ,KDT_FLG ,""                   ,""  ,""  },
  };

  typedef struct {
    char                  *gso_name;
    ADDINFO_KDTE_rec_typ  *gso_kdt ;
    } GSO_KDT;

  static GSO_KDT gso_kdts[] =      {

   @*                                                            *@
   @* RECORD NAME         KDT                                    *@
   @*                                                            *@
   @* =================== ======                                 *@

    {"OPTS"    ,(void *)   &OPTS_kdt                 @*1lalal*@     },
    {"PSWD"    ,(void *)   &PSWD_kdt                 @*SAS2IBMA*@   },
    {"PWPHRASE",(void *)   &PWPHRASEG_kdt            @*IS10147A*@   },
    {"RULEOPTS",(void *)   &RULEOPTS_kdt             @*SAS2IBMA*@   },
    {"EXITS"   ,(void *)   &EXITS_kdt                @*SAS2IBMA*@   },
    {"RESWORD" ,(void *)   &RESWORD_kdt              @*SAS2IBMA*@   },
    {""                   ,NULL                                     }
    } ; @* PS0325 - Added RESWORD record *@
 #endif

 @**************************************************************
 * WS2370 added KDT definitions for ACF2 User Profiles         *
 **************************************************************@
 @* CERTDATA is not supported - dropped by WS10005 *@
 static ADDINFO_KDTE_rec_typ CERTDATA_kdt[] =        {
    {"ACTIVE"          , KDT_KEY , "ACTIVE()"   ,"("    ,")"    },
    {"CERTID"          , KDT_KEY , "CERTID()" ,"(" ,")" },@*WS10005*@
    {"DSA"             , KDT_FLG , "NODSA"    ,"" ,"" },@*WS10005*@
    {"DSN"             , KDT_KEY , "DSN()"      ,"("    ,")"    },
    {"EXPIRE"          , KDT_KEY , "EXPIRE()"   ,"("    ,")"    },
    {"ICSF"            , KDT_FLG , "NOICSF"     ,"" ,"" },@*WS10005*@
    {"LABEL"           , KDT_KEY , "LABEL()"    ,"("    ,")"    },
    {"NEWLABEL"        , KDT_KEY , "NEWLABEL()" ,"("    ,")"    },
  @*{"ISSUERDN"        , KDT_KEY , "ISSUERDN()" ,"(",")"},@*WS10005*@
    {"PCICC"           , KDT_FLG , "NOPCICC"    ,"" ,"" },@*WS10005*@
    {"SUBJDN"          , KDT_KEY , "SUBJDN()"   ,"(",")"},@*WS10005*@
    {"TRUST"           , KDT_FLG , "NOTRUST"    ,""     ,""     },
    {"HITRUST"         , KDT_FLG , "NOTRUST"    ,"" ,"" },@*WS10005*@
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ CICS_kdt[] =        {
    {"FORCE"           , KDT_FLG , "NOFORCE"    ,""    ,""      },
    {"OPCLASS"         , KDT_KEY , "OPCLASS()"  ,"("    ,")"    },
    {"OPIDENT"         , KDT_KEY , "OPIDENT()"  ,"("    ,")"    },
    {"OPPRTY"          , KDT_KEY , "OPPRTY()"   ,"("    ,")"    },
    {"TIMEOUT"         , KDT_KEY , "TIMEOUT()"  ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ DCE_kdt[] =        {
    {"UUID"            , KDT_KEY , "UUID()"     ,"("    ,")"    },
    {"DCENAME"         , KDT_KEY , "DCENAME()"  ,"("    ,")"    },
    {"HOMEUUID"        , KDT_KEY , "HOMEUUID()" ,"("    ,")"    },
    {"HOMECELL"        , KDT_KEY , "HOMECELL()" ,"("    ,")"    },
    {"AUTOLOG"         , KDT_FLG , "NOAUTOLOG"  ,""    ,""      },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };
 @* KEYRING is not supported - dropped by WS10005 *@
 static ADDINFO_KDTE_rec_typ KEYRING_kdt[] =        {
    {"DEFAULT"         , KDT_KEY , "DEFAULT()"  ,"("    ,")"    },
    {"RINGNAME"        , KDT_KEY , "RINGNAME()" ,"("    ,")"    },
    @* no support for changing keyring from ESS       WS10005
    {"NEWNAME"         , KDT_KEY , "NEWNAME()"  ,"("    ,")"    },*@
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ KERB_kdt[] =        {
    {"KERBNAME"        , KDT_KEY , "KERBNAME()" ,"("    ,")"    },
    {"DES"             , KDT_FLG , "NODES"    ,"" ,"" },@*WS10005*@
    {"DES3"            , KDT_FLG , "NODES3"   ,"" ,"" },@*WS10005*@
    {"DESD"            , KDT_FLG , "NODESD"   ,"" ,"" },@*WS10005*@
    {"KERB-VIO"        , KDT_KEY , "KERB-VIO()" ,"("    ,")"    },
    {"MAXTKTLF"        , KDT_KEY , "MAXTKTLF()" ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ KERBLINK_kdt[] =  {        @*WS10005*@
    {"KBLKNAME"        , KDT_KEY , "KBLKNAME()" ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ LANGUAGE_kdt[] =        {
    {"PRIMARY"         , KDT_KEY , "PRIMARY()"  ,"("    ,")"    },
    {"SECONDRY"        , KDT_KEY , "SECONDRY()" ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ LINUX_kdt[] =         { @*WS10005*@
    {"LINUXGRP"        , KDT_KEY , "LINUXGRP()" ,"("    ,")"    },
    {"LINUXHOM"        , KDT_KEY , "LINUXHOM()" ,"("    ,")"    },
    {"LINUXNAM"        , KDT_KEY , "LINUXNAM()" ,"("    ,")"    },
    {"LINUXPGM"        , KDT_KEY , "LINUXPGM()" ,"("    ,")"    },
    {"LINUXUID"        , KDT_KEY , "LINUXUID()" ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ LNOTES_kdt[] =        {
    {"SNAME"           , KDT_KEY , "SNAME()"    ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ NDS_kdt[] =        {
    {"UNAME"           , KDT_KEY , "UNAME()"    ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ NETVIEW_kdt[] =        {
    {"IC"              , KDT_KEY , "IC()"       ,"("    ,")"    },
    {"CONSNAME"        , KDT_KEY , "CONSNAME()" ,"("    ,")"    },
    {"SECCTL"          , KDT_KEY , "SECCTL()"   ,"("    ,")"    },
    {"MSGRECVR"        , KDT_FLG , "NOMSGRECVR" ,"("    ,")"    },
    {"NGMFADMN"        , KDT_FLG , "NONGMFADMN" ,"("    ,")"    },
    {"NTVCLASS"        , KDT_KEY , "NTVCLASS()" ,"("    ,")"    },
    {"DOMAINS"         , KDT_KEY , "DOMAINS()"  ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ OMVS_kdt[] =        {
    {"UID"             , KDT_KEY , "UID()"      ,"("    ,")"    },
    {"HOME"            , KDT_KEY , "HOME()"     ,"("    ,")"    },
    {"OMVSPGM"         , KDT_KEY , "OMVSPGM()"  ,"("    ,")"    },
    {"CPUTIME"         , KDT_KEY , "CPUTIME()"  ,"("    ,")"    },
    {"ASSIZE"          , KDT_KEY , "ASSIZE()"   ,"("    ,")"    },
    {"FILEPROC"        , KDT_KEY , "FILEPROC()" ,"("    ,")"    },
    {"PROCUSER"        , KDT_KEY , "PROCUSER()" ,"("    ,")"    },
    {"THREADS"         , KDT_KEY , "THREADS()"  ,"("    ,")"    },
    {"MMAPAREA"        , KDT_KEY , "MMAPAREA()" ,"("    ,")"    },
 @* WS10005 fix - start *@
    {"MEMLIMIT"        , KDT_KEY , "MEMLIMIT()" ,"("    ,")"    },
    {"SHMEMMAX"        , KDT_KEY , "SHMEMMAX()" ,"("    ,")"    },
 @* WS10005 fix - end   *@
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ OPERPARM_kdt[] =        {
    {"STORAGE"         , KDT_KEY , "STORAGE()"  ,"("    ,")"    },
    {"AUTH"            , KDT_KEY , "AUTH()"     ,"("    ,")"    },
    {"AUTO"            , KDT_KEY , "AUTO()"     ,"("    ,")"    },
    {"MFORM"           , KDT_KEY , "MFORM()"    ,"("    ,")"    },
    {"MSGLEVEL"        , KDT_KEY , "MSGLEVEL()" ,"("    ,")"    },
    {"MONITOR"         , KDT_KEY , "MONITOR()"  ,"("    ,")"    },
    {"ROUTCODE"        , KDT_KEY , "ROUTCODE()" ,"("    ,")"    },
    {"LOGCMD"          , KDT_KEY , "LOGCMD()"   ,"("    ,")"    },
    {"MIGID"           , KDT_KEY , "MIGID()"    ,"("    ,")"    },
    {"DOM"             , KDT_KEY , "DOM()"      ,"("    ,")"    },
    {"UD"              , KDT_KEY , "UD()"       ,"("    ,")"    },
    {"KEY"             , KDT_KEY , "KEY()"      ,"("    ,")"    },
    {"CMDSYS"          , KDT_KEY , "CMDSYS()"   ,"("    ,")"    },
    {"ALTGROUP"        , KDT_KEY , "ALTGROUP()" ,"("    ,")"    },
    {"OPERMSCP"        , KDT_KEY , "OPERMSCP()" ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ PASSWORD_kdt[] = { @*ws10006*@
    {"#PSWDCNT"        , KDT_KEY , "#PSWDCNT()" ,"("    ,")"    },
    {"#PWD-TOD"        , KDT_KEY , "#PWD-TOD"   ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ PWPHRASEU_kdt[] = { @* IS10147A *@
    {"PWP-EXP"         , KDT_KEY , "NOPWP-EXP"  ,"("    ,")"    },
    {"PWP-HST"         , KDT_KEY , "PWP-HST()"  ,"("    ,")"    },
    {"PWP-TOD"         , KDT_KEY , "PWP-TOD()"  ,"("    ,")"    },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

 static ADDINFO_KDTE_rec_typ WORKATTR_kdt[] =        {
    {"WANAME"          , KDT_KEY , "WANAME()"   ,"("    ,")"    },
    {"WABLDG"          , KDT_KEY , "WABLDG()"   ,"("    ,")"    },
    {"WADEPT"          , KDT_KEY , "WADEPT()"   ,"("    ,")"    },
    {"WAROOM"          , KDT_KEY , "WAROOM()"   ,"("    ,")"    },
    {"WAADDR1"         , KDT_KEY , "WAADDR1()"  ,"("    ,")"    },
    {"WAADDR2"         , KDT_KEY , "WAADDR2()"  ,"("    ,")"    },
    {"WAADDR3"         , KDT_KEY , "WAADDR3()"  ,"("    ,")"    },
    {"WAADDR4"         , KDT_KEY , "WAADDR4()"  ,"("    ,")"    },
    {"WAACCNT"         , KDT_KEY , "WAACCNT()"  ,"("    ,")"    },
    {"REP"             , KDT_FLG , ""           ,""     ,""     },
    {""                , KDT_FLG , ""           ,""     ,""     }
    };

  typedef struct {
    char                  *UP_name;
    ADDINFO_KDTE_rec_typ  *UP_kdt ;
    } UP_KDT;

  static UP_KDT UP_kdts[] =      {

   @*                                                            *@
   @* User Profile Sysid  KDT                                    *@
   @*                                                            *@
   @* =================== ======                                 *@

    {"OMVS"               ,(void *) &OMVS_kdt      }, @* SAS2IBMT *@
    {"CICS"               ,(void *) &CICS_kdt      }, @* SAS2IBMT *@
    {"DCE"                ,(void *) &DCE_kdt       }, @* SAS2IBMT *@
    {"KERB"               ,(void *) &KERB_kdt      }, @* SAS2IBMT *@
    {"KERBLINK"           ,(void *) &KERBLINK_kdt  }, @* SAS2IBMT *@
    {"CERTDATA"           ,(void *) &CERTDATA_kdt  }, @* SAS2IBMT *@
    {"KEYRING"            ,(void *) &KEYRING_kdt   }, @* SAS2IBMT *@
    {"LANGUAGE"           ,(void *) &LANGUAGE_kdt  }, @* SAS2IBMT *@
    {"LINUX"              ,(void *) &LINUX_kdt     }, @* SAS2IBMT *@
    {"LNOTES"             ,(void *) &LNOTES_kdt    }, @* SAS2IBMT *@
    {"NDS"                ,(void *) &NDS_kdt       }, @* SAS2IBMT *@
    {"NETVIEW"            ,(void *) &NETVIEW_kdt   }, @* SAS2IBMT *@
    {"OPERPARM"           ,(void *) &OPERPARM_kdt  }, @* SAS2IBMT *@
    {"PASSWORD"           ,(void *) &PASSWORD_kdt  }, @* SAS2IBMT *@
    {"PWPHRASE"           ,(void *) &PWPHRASEU_kdt }, @* IS10147A *@
    {"WORKATTR"           ,(void *) &WORKATTR_kdt  }, @* SAS2IBMT *@
    {""                   ,NULL                              }
    } ;

 @**************************************************************
 * end ws2370                                                  *
 **************************************************************@

 @**************************************************************
 * KDT management functions                                    *
 **************************************************************@
 ADDINFO_KDTE_rec_ptr ACF2_Update_KDT (ADDINFO_KDTE_rec_ptr      ,
                                       ACF2_Get_Fields_Filter    ,
                                       ACF2_KDT_Processing_Option,
                                       ADMIN_PARAMS_rec_typ *    );


 ADDINFO_KDTE_rec_ptr  ACF2_build_KDT_from_ACFFDR(
                            ACF2_Get_Fields_Filter    ,
                            int                      *,
                            ADMIN_PARAMS_rec_typ *);    @* bs2607 *@

 @**************************************************************
 * ACF2 Low-Level API                                          *
 **************************************************************@
 ACF2_API_Return_Codes ACF2_Get_User(char *lidmask,
                                     ACF2_Object_Mask lid_masked,
                                     char  *uidmask,
                                     char  *current_lid,
                                     ACF2_GetUser_Fields Get_Fields,
                                     char  *Get_Field_Vec,
                                     ADDINFO_rec_ptr,
      @* PS0321 *@                   ACF2_Multi_UIDs_Mode_Type,
                                     char  *errmsg,
      @* bs2556 *@                   ADMIN_PARAMS_rec_typ *);

 ACF2_API_Return_Codes ACF2_User_Exists (char     *lid,  @* bs2513 *@
      @* bs2556 *@                       ADMIN_PARAMS_rec_typ *);


 @* PS0234 *@
 ESA_RC Init_ACE_Handle (ACF2_Get_ResACL_Handle **,
                         CTSAMSG_HANDLE_rec_typ *,
                         CTSAMSG_DEST_TABLE_rec_typ *);

 @* PS0234 *@
 ESA_RC Check_ACE_UNTIL (ADDINFO_rec_ptr);

 @* PS0234 *@
 void Fill_ACE_Keywords (ADDINFO_rec_ptr);

 @* PS0234 *@
 void Fill_Res_Keywords (ADDINFO_rec_ptr);

 @* WS2358 *@
 void ACF2_Set_Keyword_Value (char *,
                              char *,
                              ADDINFO_rec_ptr,
                              CTSAMSG_HANDLE_rec_typ *,
                              CTSAMSG_DEST_TABLE_rec_typ *);

 @* PS0234 *@
 int ACES_Match (ACE_rec_typ *,
                 ADDINFO_rec_typ *,
                 ACE_rec_typ *,
                 ADDINFO_rec_typ *);

 @* PS0278 *@
 ESA_RC Rule_Backup (char *,
                     char *,
                     char *,
                     ADMIN_PARAMS_rec_typ  *);

 @* PS0234 *@
 ESA_RC Prepare_ACF2IN_File (FILE **,
                             CTSAMSG_HANDLE_rec_typ *,
                             CTSAMSG_DEST_TABLE_rec_typ *);

 @* PS0234 *@
 ESA_RC Build_Ruleset (char *,                            @* WS2358 *@
                       char *,                            @* WS2358 *@
                       ADDINFO_rec_ptr,
                       FILE *,
                       CTSAMSG_HANDLE_rec_typ *,
                       CTSAMSG_DEST_TABLE_rec_typ *);

 @* PS0234 *@
 ESA_RC Build_Rule_Entry (ACE_rec_typ *,
                          ADDINFO_rec_ptr,
                          RESTYPE_typ,
                          FILE *,
                          CTSAMSG_HANDLE_rec_typ *,
                          CTSAMSG_DEST_TABLE_rec_typ *);

 @* PS0234 *@
 void ACF2_Filter_ADDINFO (char *,
                           ADDINFO_rec_ptr);
 @* PS0234 *@
 ESA_RC Compile_Ruleset (char *,
                         char *,
                         ACF2_Replace_Rule_Type,
                         int,
                         char *,
                         char *,
                         ADMIN_PARAMS_rec_typ *);         @* PS0347 *@

 @* PS0234 *@
 ESA_RC Delete_Ruleset (char *,
                        char *,
                        char *,
                        CTSAMSG_HANDLE_rec_typ *,
                        CTSAMSG_DEST_TABLE_rec_typ *);

 @* PS0234 *@
 ACF2_API_Return_Codes ACF2_Get_Resource(char *,
                                         ACF2_Object_Mask,
                                         char *,
                                         ACF2_Resource_Data_Type,
                                         ACF2_Get_ResACL_Handle *,
                                         ACF2_Get_Res_Handle *,
                                         char *,
                                         ACE_rec_typ *,
                                         ADDINFO_rec_ptr,
                                         char *,
                                         ADMIN_PARAMS_rec_typ *);

 @* WS2358 *@
 ESA_RC ACF2_Automate (char *,
                       char *,
                       ACE_rec_typ *,
                       ADDINFO_rec_ptr,
                       ACF2_Get_ResACL_Handle *,
                       ADDINFO_rec_ptr,
                       ADDINFO_rec_ptr,
                       FILE *,
                       int *,
                       char *,
                       ACF2_Key_Cell_Typ *,
                       int,
                       char *,
                       char *,
                       ADMIN_PARAMS_rec_typ *);

 @* WS2358 *@
 void Get_First_Qualifier (char *,
                           char *,
                           int);                          @* PS0398 *@

 @* WS2358 *@
 void Strip_First_Qualifier (char *,
                             char *);

 @* WS2358 *@
 ESA_RC Prepare_KEYs_List (char *,
                           ACF2_Key_Cell_Typ *,
                           int *,
                           ADMIN_PARAMS_rec_typ *);

 @* WS2358 *@
 int Most_Specific_Match (char *,
                          char *,
                          ACF2_Key_Cell_Typ *,
                          int,
                          ADMIN_PARAMS_rec_typ *);

 ACF2_API_Return_Codes ACF2_Get_GSO(char                 *recname,
                                    ADDINFO_rec_ptr      laddinfo,
                                    char                 *errmsg);

 ACF2_API_Return_Codes ACF2_Get_all_specific_user_profiles
                                 (char                       *recname,
                                  char                       *func,
                                  ADDINFO_rec_ptr            laddinfo,
                                  char                       *errmsg,
                                  ESA_DIAG_COMP_typ          comp,
                                  CTSAMSG_HANDLE_rec_typ     * msgs,
                                  CTSAMSG_DEST_TABLE_rec_typ * dest);

 ESA_RC ACF2_UP_switch_is_activated(char   *sysid,
                                    ADDINFO_rec_ptr       addinfo);

 ACF2_API_Return_Codes ACF2_UP_exists
                                 (char                       *recname,
                                  char                       *sysname,
                                  ESA_DIAG_COMP_typ          comp,
                                  CTSAMSG_HANDLE_rec_typ     * msgs,
                                  CTSAMSG_DEST_TABLE_rec_typ * dest);

 @**************************************************************
 * Global Data Area functions                                  *
 **************************************************************@
 ESA_RC ACF2_Set_Global_Data( ADMIN_PARAMS_rec_typ * admin_params,
                              ERR_STRUCT_rec_typ   * err);

 @* PS0234 *@
 void ACF2_Set_LID_In_UID(void);

 @* PS0234 *@
 int ACF2_Is_LID_In_UID(int *);

 @* WS2358 *@
 int ACF2_Is_Virtual_Resource(char *);

 void Global_ACF2_Date_Format(char *Date_Format);

 @* BS10076 int Global_Total_Wait_Lock_Time(void);                   *@

 @* BS10076 int Global_Retry_Wait_Lock_Time(void);                   *@

 int Global_UDB_Active(void);

@* WS10054  char *Global_Get_QNAME(void);  *@

 @* WS2358 *@
 void Global_Get_Virtual_Res(char *ResName);

 @**************************************************************
 * User Functions                                              *
 **************************************************************@
ACF2_API_Return_Codes  ACF2_Get_User_UID(char *lid,
                                         char *uid,
                                         char *current_lid,
                                         char *uidmask,
                                         char *uids_list, @* PS0339 *@
                                         ADDINFO_rec_ptr laddinfo,
      @*bs2556*@                 ADMIN_PARAMS_rec_typ * admin_params);

ACF2_API_Return_Codes  ACF2_Refresh_User_UID(char *lid,
                                             OBJ_EXISTS_typ lid_exist,
                                             ADDINFO_rec_ptr laddinfo);

 @**************************************************************
 * GDB Processing                                              *
 **************************************************************@
GDB_Codes GDB_Set_Mode(GDB_Modes,
                       ADMIN_PARAMS_rec_typ * admin_params,
                       ERR_STRUCT_rec_typ   * err);

GDB_Codes GDB_Locate(char Record_Type,char *key, GDB_Locate_Mode mode);

GDB_Codes GDB_GetNext(char Record_Type,char *key);

GDB_Codes GDB_Insert(void);

GDB_Codes GDB_Update(void);

GDB_Codes GDB_Delete(void);

ESA_RC GDB_Convert_to_Record(char Record_Type,
                             char *Key,
                             ADDINFO_rec_ptr   addinfo);

ESA_RC GDB_Convert_from_Record(char *Record_Type,
                               char *Key,
                               ADDINFO_rec_ptr   addinfo);

 @**************************************************************
 * UDB Processing                                              *
 **************************************************************@
 UDB_Codes UDB_Set_Mode(UDB_Modes,
                       ADMIN_PARAMS_rec_typ * admin_params,
                       ERR_STRUCT_rec_typ   * err);

 UDB_Codes UDB_Locate(URID urid, LID lid, UDB_Locate_Mode mode);
                                                          @* WS2338 *@

 UDB_Codes UDB_GetNext(URID urid, LID lid, UID uid);      @* WS2338 *@

 UDB_Codes UDB_Get_Next_User(URID urid, LID lid, UID uid); @* WS2338 *@

 UDB_Codes UDB_Insert(URID urid, LID lid,  UID uid);      @* WS2338 *@

 UDB_Codes UDB_Update(URID urid, LID lid,  UID uid);      @* WS2338 *@

 UDB_Codes UDB_Delete(void);

ESA_RC ACF2_Get_Field_Information(char *field_name,
                                  char *field_type,
                                  char *field_rw    ,
                                  int  *field_length,
                                  int  *field_offset,
                                  char *field_multi,      @* WS2337 *@
                                  int  *field_mvmax);     @* WS2337 *@

void Get_ACFFDR_In_ESS_Format(char *,     @*bs2607*@      @* PS0321 *@
                              ADMIN_PARAMS_rec_typ  *);   @* bs2607 *@

 @**************************************************************
 * ACFFDR                                                      *
 **************************************************************@
void ACF2_Free_ACFFDR(void);               @* BS2509 *@

 @**************************************************************
 * Group Processing functions                                  *
 **************************************************************@
 void Trunc_UIDMASKS(ADDINFO_rec_ptr);              @* PS0234 *@

 void Trunc_Blanks(char *);                         @* bs2593 *@

 void Trunc_One_UIDMASK(char *);                    @* PS0234 *@

 ACF2_API_Return_Codes ACF2_Get_Group(GDB_Locate_Mode mode,
                                      UG_typ Group,
                                      ADDINFO_rec_ptr laddinfo);

ESA_RC ACF2_Get_Group_UIDMASKS(char *group,
                               int  *Group_UID_Count,
                               UIDMASK Group_UID[1],
                               ADDINFO_rec_ptr laddinfo,
                               int  *reason_code,
                               CTSAMSG_DEST_TABLE_rec_typ  * dest,
                               CTSAMSG_HANDLE_rec_typ      * msgs);
                          @* BS2306 - added dest+msgs parms         *@

ESA_RC Check_Group_Correllation(UG_typ Group,
                                ADDINFO_rec_ptr addinfo,
                                UG_typ  cgroup,
                                ADDINFO_rec_ptr laddinfo,
                                CTSAMSG_DEST_TABLE_rec_typ  * dest,
                                CTSAMSG_HANDLE_rec_typ      * msgs,
                    @*IS10088*@ ADMIN_PARAMS_rec_typ * admin_params);
                          @* BS2306 - added dest+msgs parms         *@

int ACF2_Match_User_Group(USER_typ User,
                            UG_typ   Group);

ESA_RC Check_Group_UIDMASKS(UG_typ                      group,
                            ADDINFO_rec_ptr             addinfo,
                            ADMIN_PARAMS_rec_typ *); @*IS10128*@
 @* drop by admin_params    CTSAMSG_DEST_TABLE_rec_typ  * dest,
    IS10128                 CTSAMSG_HANDLE_rec_typ      * msgs); *@
 @**************************************************************
 * UID   Processing functions                                  *
 **************************************************************@
ESA_RC Generate_UID_Update(UID_Update_type Update_Type,
                           char *UID_User,
                           char *UID_Masks,
                           ADDINFO_rec_ptr uaddinfo,
                           ADDINFO_rec_ptr laddinfo);

 @* PS0234 *@
 int Is_UIDMASK_User (char *);

 @* PS0234 *@
 ESA_RC Build_UIDMASK_User (char *,
                            char *);

 @* PS0234 *@
 int Is_LID_In_UID (int *);

ESA_RC Calc_User_UID_String(USER_typ        user,         @* PS0354 *@
                            ADDINFO_rec_ptr addinfo,
                            UIDMASK UID,
                            char   *UIDs_List,            @* PS0354 *@
                            char    Init_UID_Char,
                            ADMIN_PARAMS_rec_typ *    );  @*bs2556*@

ESA_RC Convert_UID_Offset_to_Field_Name  (int   uid_offset,
                                          char  *field_name,
                                          int   *field_offset);

ESA_RC Check_User_Force_Disconnect_from_Group( UIDMASK old_uid,
                               UIDMASK new_uid,
                               UG_typ  cgroup,
                               ADDINFO_rec_ptr laddinfo,
                               CTSAMSG_DEST_TABLE_rec_typ  * dest,
                               CTSAMSG_HANDLE_rec_typ      * msgs);
                          @* BS2306 - added dest+msgs parms         *@

@* PS0354 *@
ESA_RC Check_Mult_Force_Disconnect_from_Group( char *old_uids,
                               char *new_uids,
                               UG_typ  cgroup,
                               ADDINFO_rec_ptr laddinfo,
                               CTSAMSG_DEST_TABLE_rec_typ  * dest,
                               CTSAMSG_HANDLE_rec_typ      * msgs);

 int ACF2_Is_Part_Of_Multi (int,
                            char *,
                            int *,
                            int *);                       @* PS0345 *@

 void ACF2_Get_UID_Info( char *                              ,
                         int *                               ,
                         UID_Structure *                     ,
                         char *,                          @* PS0321 *@
                         ADMIN_PARAMS_rec_typ *);         @* bs2607 *@

 int UID_Contains_MULTI (char *,
                         int *);                          @* PS0321 *@

 ESA_RC Build_Multi_UIDs (char *,
                          char *,
                          int,
                          ADDINFO_rec_ptr,                @* PS0354 *@
                          int *,                          @* PS0354 *@
                          char *,                         @* bs2556 *@
                          ADMIN_PARAMS_rec_typ *    );

 void Initialize_UID_Definitions(ADMIN_PARAMS_rec_typ  *); @*bs2607*@

 void Get_UID_In_ESS_Format(char *field);
 @**************************************************************
 * Service functions                                           *
 **************************************************************@
 ESA_RC Convert_Multi_to_Vec(char *, int *, char **, int, int *);
                          @*   BS2306 - added reason_code parm      *@

 void Convert_Keyword_Chars(ADDINFO_rec_ptr addinfo,
                           char from_char,
                           char to_char);
 int Round_Number(int, int);

 char *Trim(char *string);

 void Reset_Field(char *,int,char,char *);

 int ACF2_Match_Id_to_Mask(ACF2_Object_Type object_type,
                           char *object,
                           char *mask);

 ESA_RC ACF2_Convert_Date_Fields(ADDINFO_rec_ptr          addinfo,
                           CTSAMSG_DEST_TABLE_rec_ptr   dest,
                           CTSAMSG_HANDLE_rec_ptr       msgs);

 ESA_RC ACF2_Convert_Access_Levels(ADDINFO_rec_ptr          addinfo,
                           CTSAMSG_DEST_TABLE_rec_ptr   dest,
                           CTSAMSG_HANDLE_rec_ptr       msgs);

 void convert_tod_to_YYYYMMDDHHMMSS(char *,char *);

 @*ws2431 moved from CTSBGCN *@
 ACF2_API_Return_Codes ACF2_Get_User_Groups
              (GDB_Locate_Mode mode,
               char   *UID,
               UG_typ Group,
               ADDINFO_rec_ptr laddinfo,
               CTSAMSG_DEST_TABLE_rec_typ  * dest,
               CTSAMSG_HANDLE_rec_typ      * msgs);


#endif   * ACF2_H_included *
*/
