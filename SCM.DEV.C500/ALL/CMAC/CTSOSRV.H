/* IS10174

   This member is obsolete.

*/

/**************************************************************
*                                                             *
*  Note:                                    28.8.16 - Nurit   *
*  -----                                                      *
*                                                             *
*  This member is requierd because of the definition of       *
*  COMMON_PARAMS.  But, The funciotn defined in this member   *
*  are not used by any CTSOxxx program, therfore, the source  *
*  program was not copied as a CTSOSRV to the ALL.CSRC.       *
*  This source will have to be copied when a CTSOxxx program  *
*  needs to use its functions.                                *
*                                                             *
**************************************************************/
/**************************************************************
* Mod.Id   Who      When     Description                      *
* -------- -------- -------  -------------------------------- *
* WS10054  SeligT   04/08/16 Add ENQ/DEQ SYSTEMS RNL Parameter*
* IS10139  SeligT   28/08/16 Copied CTSCSRV WS10054 change    *
* IS10140  AvnerL   06/09/16 Fix mutiple includes.            *
* commons  AvnerL   18/06/17 Set common_prm_ptr first.        *
* IS10174  NuritY   25/01/18 Cmake thie member obsolete       *
**************************************************************/
/* IS10174
#ifndef OS_SRV_included                                    * IS10140 *
#define OS_SRV_included                                    * IS10140 *

#define init_global_admin_params csrv001
#define set_common_usaapi_parms  csrv002                  @* WS10054 *@
#define free_common_parms_area   csrv003                  @* WS10054 *@

extern
ESA_RC init_global_admin_params (ADMIN_PARAMS_rec_typ * admin_params);

@* * * * * * * * * * * Start of WS10054  * * * * * * * * * * * * * * *@
@* This modification is related to the ENQ / DEQ SYSTEMS RNL         *@
@* parameter.  Since it is common to all 3 RSSs, ACF2, RACF, and     *@
@* TSS, it was decided to create a common usaapi parameter area      *@
@* and to place the rssparm_ENQ_SYSTEMS_RNL parameter inside it.     *@
@* In the future, when there are additional common (to all 3 RSSs)   *@
@* parameters, they should be added to this common usaapi parameter  *@
@* area and then CTS.CSRC(CTSCSRV) must be re-compiled (because      *@
@* malloc is done there for the common area).                        *@
@*                                                                   *@
@* Note: Due to various problems, we did not place the ENQ / DEQ     *@
@*       SYSTEMS RNL parameter in the common parameter area.  We     *@
@*       placed it in CTSPARM instead.  However, we left the common  *@
@*       parameter area so that it can be used in the future.        *@
@*                                                                   *@
@* Here are the 2 routines which are related to this modification:   *@
@* 1)  set_common_usaapi_parms routine allocates this new common     *@
@*     usaapi parameter area and copies the ENQ_SYSTEMS_RNL field    *@
@*     into it (by calling the enq_systems_rnl routine). Eventually  *@
@*     additional common parameters will be copied to this common    *@
@*     usaapi parameter area.  This routine resides in CTSCSRV.      *@
@* 2)  free_common_parms_area routine frees this common area at      *@
@*     termination.  This routine resides in CTSCSRV.                *@
@*                                                                   *@
@* This modification required changes in these modules:              *@
@* CTSBINI, CTSRINI, CTSTINI - api initialization                    *@
@* CTSBTRM, CTSRTRM, CTSTTRM - api termination                       *@
@* CTSCOLK, CTSOOLK (common code) - contains the OS_CS_lock and      *@
@*                                  OS_CS_unlock routines which      *@
@*                                  call the CTSAENQ (Assembler)     *@
@*                                  routine with the new RNL parm    *@
@* CTSCSRV - common routine which contains both of the               *@
@*           aforementioned routines, set_common_usaapi_parms        *@
@*           and free_common_parms_area                              *@
@* CTSAENQ - Assembler routine which does the actual ENQ and DEQ     *@
@*                                                                   *@
@* Regarding the typedefs below, they are needed in order to be able *@
@* to access / address the pointer to the usaapi common parameter    *@
@* area and the actual fields in the usaapi common parameter area.   *@
@* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *@

@* In the future, when there are additional common (to all 3 RSSs)   *@
@* parameters, they should be added to the common usaapi parameter   *@
@* area (defined immediately below) and then CTS.CSRC(CTSCSRV)       *@
@* must be re-compiled (because malloc is done there for the common  *@
@* area).                                                            *@
typedef  struct _COMMON_PARAMS{
  char rssparm_FOR_FUTURE_USE[2]; @* FOR_FUTURE_USE          WS10054 *@
} COMMON_PARAMS_rec_typ, *COMMON_PARAMS_rec_ptr;          @* WS10054 *@

typedef  struct _DUMMY_PARAMS{ @* mv common_params to start* commons *@
  COMMON_PARAMS_rec_typ * common_params; @* ptr to common parameters *@
  char ffu[996];                               @* usaapi parameters. *@
} DUMMY_PARAMS_rec_typ, *DUMMY_PARAMS_rec_ptr;            @* WS10054 *@

extern
void   set_common_usaapi_parms (ADMIN_PARAMS_rec_typ * admin_params,
                                DUMMY_PARAMS_rec_typ * params_ptr);
                                                          @* WS10054 *@
extern
void   free_common_parms_area (ADMIN_PARAMS_rec_typ * admin_params,
                               DUMMY_PARAMS_rec_typ * params_ptr);
                                                          @* WS10054 *@
@* * * * * * * * * * * * End of WS10054  * * * * * * * * * * * * * * *@


#endif                                                     * IS10140 *
*/
