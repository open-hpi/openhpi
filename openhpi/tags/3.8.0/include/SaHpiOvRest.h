#ifndef __SAHPIOVREST_H
#define __SAHPIOVREST_H

	/* UID control */
	#define OV_REST_UID_CNTRL                       (SaHpiCtrlNumT)   0x000
	/* Power control */
	#define OV_REST_PWR_CNTRL                       (SaHpiCtrlNumT)   0x001


#if 0

//Nandi: below are part of POWER SUB SYSTEM
// TODO : Need to check requirement, Enable required control as proceed

	/* Power Mode Control */
	#define OV_REST_PWR_MODE_CNTRL                  (SaHpiCtrlNumT)   0x003
	/* Dynamic Power Control */
	#define OV_REST_DYNAMIC_PWR_CNTRL               (SaHpiCtrlNumT)   0x004
	/* Power Limit Mode Control */
	#define OV_REST_PWR_LIMIT_MODE_CNTRL            (SaHpiCtrlNumT)   0x005
	/* Static Power Limit Control */
	#define OV_REST_STATIC_PWR_LIMIT_CNTRL          (SaHpiCtrlNumT)   0x006
	/* Dynamic Power Cap Control */
	#define OV_REST_DYNAMIC_PWR_CAP_CNTRL           (SaHpiCtrlNumT)   0x007

	/* Derated Circuit Cap Control */
	#define OV_REST_DERATED_CIRCUIT_CAP_CNTRL       (SaHpiCtrlNumT)   0x008
	/* Rated Circuit Cap Control */
	#define OV_REST_RATED_CIRCUIT_CAP_CNTRL         (SaHpiCtrlNumT)   0x009

	/* HP c7000 Power Modes */
	#define C7000_PWR_NON_REDUNDANT                 1
	#define C7000_PWR_AC_REDUNDANT                  2
	#define C7000_PWR_SUPPLY_REDUNDANT              3

	/* HP c7000 Power Limit Modes */
	#define C7000_PWR_LIMIT_NONE                    0
	#define C7000_PWR_LIMIT_STATIC                  1
	#define C7000_PWR_LIMIT_DYNAMIC_CAP             2

#endif 

/* Operational status sensor */
#define OV_REST_SEN_OPER_STATUS                 (SaHpiSensorNumT) 0x000

#endif  //__SAHPIOVREST_H



