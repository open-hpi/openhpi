/*
 * Copyright (c) 2003, Service Availability Forum
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or 
 * without modification, are permitted provided that the following 
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Service Availalability Forum nor the names 
 * of its contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */



/*******************************************************************************
********************************************************************************
********** 						  	      **********
********** Watchdogs 						      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_WATCHDOG_H
#define HPI_WATCHDOG_H

/*
** This section defines all of the data types associated with watchdog timers.
*/

/* Watchdog Number - Identifier for a watchdog timer. */
typedef SaHpiUint8T SaHpiWatchdogNumT;
#define SAHPI_DEFAULT_WATCHDOG_NUM (SaHpiWatchdogT)0x00

/*
** Watchdog Timer Action
**
** These enumerations represent the possible actions to be taken upon watchdog
** timer timeout and the events that are generated for watchdog actions.
*/
typedef enum {
	SAHPI_WA_NO_ACTION = 0,
	SAHPI_WA_RESET,
	SAHPI_WA_POWER_DOWN,
	SAHPI_WA_POWER_CYCLE
} SaHpiWatchdogActionT;

typedef enum {
	SAHPI_WAE_NO_ACTION = 0,
	SAHPI_WAE_RESET,
	SAHPI_WAE_POWER_DOWN,
	SAHPI_WAE_POWER_CYCLE,
	SAHPI_WAE_TIMER_INT=0x08 /* Used if Timer Preinterrupt only */
} SaHpiWatchdogActionEventT;

/*
** Watchdog Pre-timer Interrupt
**
** These enumerations represent the possible types of interrupts that may be
** triggered by a watchdog pre-timer event. The actual meaning of these
** operations may differ depending on the hardware architecture.
*/
typedef enum {
	SAHPI_WPI_NONE = 0,
	SAHPI_WPI_SMI,
	SAHPI_WPI_NMI,
	SAHPI_WPI_MESSAGE_INTERRUPT,
	SAHPI_WPI_OEM = 0x0F
} SaHpiWatchdogPretimerInterruptT;

/*
** Watchdog Timer Use
**
** These enumerations represent the possible watchdog users that may have caused
** the watchdog to expire. For instance, if watchdog is being used during power
** on self test (POST), and it expires, the SAHPI_WTU_BIOS_POST expiration type
** will be set. Most specific uses for Watchdog timer by users of HPI should
** indicate SAHPI_WTU_SMS_OS if the use is to provide an OS-healthy heartbeat,
** or SAHPI_WTU_OEM if it is used for some other purpose.
*/
typedef enum {
	SAHPI_WTU_NONE = 0,
	SAHPI_WTU_BIOS_FRB2,
	SAHPI_WTU_BIOS_POST,
	SAHPI_WTU_OS_LOAD,
	SAHPI_WTU_SMS_OS, /* System Management System providing
			   * heartbeat for OS */
	SAHPI_WTU_OEM,
	SAHPI_WTU_UNSPECIFIED = 0x0F
} SaHpiWatchdogTimerUseT;

/*
** Timer Use Expiration Flags
** These values are used for the Watchdog Timer Use Expiration flags in the
** SaHpiWatchdogT structure.
*/
typedef SaHpiUint8T SaHpiWatchdogExpFlagsT;
#define SAHPI_WATCHDOG_EXP_BIOS_FRB2 (SaHpiWatchdogExpFlagsT)0x02
#define SAHPI_WATCHDOG_EXP_BIOS_POST (SaHpiWatchdogExpFlagsT)0x04
#define SAHPI_WATCHDOG_EXP_OS_LOAD (SaHpiWatchdogExpFlagsT)0x08
#define SAHPI_WATCHDOG_EXP_SMS_OS (SaHpiWatchdogExpFlagsT)0x10
#define SAHPI_WATCHDOG_EXP_OEM (SaHpiWatchdogExpFlagsT)0x20
/*
** Watchdog Structure
**
** This structure is used by the saHpiWatchdogTimerGet() and
** saHpiWatchdogTimerSet() functions. The use of the structure varies slightly by
** each function.
**
** For saHpiWatchdogTimerGet() :
**
** Log - indicates whether or not the Watchdog is configured to
** issue an event when it next times out. TRUE=event will
** be issued on timeout.
** Running - indicates whether or not the Watchdog is currently
** running or stopped. TRUE=Watchdog is running.
** TimerUse - indicates the current use of the timer; one of five
** preset uses which was included on the last
** saHpiWatchdogTimerSet() function call, or through some
** other implementation-dependent means to start the
** Watchdog timer.
** TimerAction - indicates what action will be taken when the Watchdog
** times out.
** PretimerInterrupt - indicates which action will be taken
** "PreTimeoutInterval" seconds prior to Watchdog timer
** expiration.
** PreTimeoutInterval - indicates how many milliseconds prior to timer time
** out the PretimerInterrupt action will be taken. If
** "PreTimeoutInterval" = 0, the PretimerInterrupt action
** will occur concurrently with "TimerAction." HPI
** implementations may not be able to support millisecond
** resolution and may have a maximum value restriction.
** These restrictions should be documented by the
** provider of the HPI interface.
** TimerUseExpFlags - set of five bit flags which indicate that a Watchdog
** timer timeout has occurred while the corresponding
** TimerUse value was set. Once set, these flags stay
** set until specifically cleared with a
** saHpiWatchdogTimerSet() call, or by some other
** implementation-dependent means.
** InitialCount - The time, in milliseconds, before the timer will time
** out after a saHpiWatchdogTimerReset() function call is
** made, or some other implementation-dependent strobe is
** sent to the Watchdog. HPI implementations may not be
** able to support millisecond resolution and may have a
** maximum value restriction. These restrictions should
** be documented by the provider of the HPI interface.
** PresentCount - The remaining time in milliseconds before the timer
** will time out unless a saHpiWatchdogTimerReset()
** function call is made, or some other implementation-
** dependent strobe is sent to the Watchdog.
** HPI implementations may not be able to support
** millisecond resolution on watchdog timers, but will
** return the number of clock ticks remaining times the
** number of milliseconds between each tick.
**
** For saHpiWatchdogTimerSet():
**
** Log - indicates whether or not the Watchdog should issue
** an event when it next times out. TRUE=event will be
** issued on timeout.
** Running - indicates whether or not the Watchdog should be
** stopped before updating.
** TRUE = Watchdog is not stopped. If it is already
** stopped, it will remain stopped, but if it is
** running, it will continue to run, with the
** countdown timer reset to the new InitialCount.
** Note that there is a race condition possible
** with this setting, so it should be used with
** care.
** FALSE = Watchdog is stopped. After
** saHpiWatchdogTimerSet() is called, a subsequent
** call to saHpiWatchdogTimerReset() is required to
** start the timer.
** TimerUse - indicates the current use of the timer. Will control
** which TimerUseExpFlag is set if the timer expires.
** TimerAction - indicates what action will be taken when the Watchdog
** times out.
** PretimerInterrupt - indicates which action will be taken
** "PreTimeoutInterval" seconds prior to Watchdog timer
** expiration.
** PreTimeoutInterval - indicates how many milliseconds prior to timer time
** out the PretimerInterrupt action will be taken. If
** "PreTimeoutInterval" = 0, the PretimerInterrupt action
** will occur concurrently with "TimerAction." HPI
** implementations may not be able to support millisecond
** resolution and may have a maximum value restriction.
** These restrictions should be documented by the
** provider of the HPI interface.
** TimerUseExpFlags - Set of five bit flags corresponding to the five
** TimerUse values. For each bit set, the corresponding
** Timer Use Expiration Flag will be CLEARED. Generally,
** a program should only clear the Timer Use Expiration
** Flag corresponding to its own TimerUse, so that other
** software, which may have used the timer for another
** purpose in the past can still read its TimerUseExpFlag
** to determine whether or not the timer expired during
** that use.
** InitialCount - The time, in milliseconds, before the timer will time
** out after a saHpiWatchdogTimerReset() function call is
** made, or some other implementation-dependent strobe is
** sent to the Watchdog. HPI implementations may not be
** able to support millisecond resolution and may have a
** maximum value restriction. These restrictions should
** be documented by the provider of the HPI interface.
** PresentCount - Not used on saHpiWatchdogTimerSet() function. Ignored.
**
*/
typedef struct {
	SaHpiBoolT Log;
	SaHpiBoolT Running;
	SaHpiWatchdogTimerUseT TimerUse;
	SaHpiWatchdogActionT TimerAction;
	SaHpiWatchdogPretimerInterruptT PretimerInterrupt;
	SaHpiUint32T PreTimeoutInterval;
	SaHpiWatchdogExpFlagsT TimerUseExpFlags;
	SaHpiUint32T InitialCount;
	SaHpiUint32T PresentCount;
} SaHpiWatchdogT;

#endif /* HPI_WATCHDOG_H */


