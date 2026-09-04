/*****************************************************************************
 *
 * @brief Portation of whetstone test for Bear.
 *
 * @author vitaly.gaiduk@cloudbear.ru
 *
 * Copyright (c) 2017, 2020 CloudBEAR LLC, all rights reserved.
 *
 * This file contains confidential, proprietary information and trade
 * secrets of CloudBEAR LLC. The information contained in this file
 * may only be used by a person authorised under and to the extent
 * permitted by a subsisting license agreement or design service
 * agreement from CloudBEAR LLC.
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person
 * is permitted to do so under the terms of a subsisting license
 * agreement or design service agreement from CloudBEAR LLC.
 *
 *****************************************************************************/
/* File: whetstone.h */

#ifndef _WHETSTONE_H
#define _WHETSTONE_H

#include <stdarg.h>
#include <time.h>

#define RVTEST_FP_ENABLE


#define SPDP float
#define Precision "Single"


/*****************************************************/
/* Various timer routines.                           */
/* Al Aburto, aburto@nosc.mil, 08 Oct 1996           */
/*                                                   */
/* t = dtime() outputs the current time in seconds.  */
/* Use CAUTION as some of these routines will mess   */
/* up when timing across the hour mark!!!            */
/*                                                   */
/* For timing I use the 'user' time whenever         */
/* possible. Using 'user+sys' time is a separate     */
/* issue.                                            */
/*                                                   */
/* Example Usage:                                    */
/* [timer options added here]                        */
/* main()                                            */
/* {                                                 */
/*  double starttime,benchtime,dtime();              */
/*                                                   */
/*  starttime = dtime();                             */
/*  [routine to time]                                */
/*  benchtime = dtime() - starttime;                 */
/* }                                                 */
/*                                                   */
/* [timer code below added here]                     */
/*****************************************************/

/*********************************/
/* Timer code.                   */
/*********************************/
/*******************/
/*  Amiga dtime()  */
/*******************/
#ifdef Amiga
#include <ctype.h>
#define HZ 50

SPDP dtime()
{
  SPDP q;

  struct tt
  {
    long  days;
    long  minutes;
    long  ticks;
  } tt;

  DateStamp(&tt);

  q = ((SPDP)(tt.ticks + (tt.minutes * 60L * 50L))) / (SPDP)HZ;

  return q;
}
#endif

/*****************************************************/
/*  UNIX dtime(). This is the preferred UNIX timer.  */
/*  Provided by: Markku Kolkka, mk59200@cc.tut.fi    */
/*  HP-UX Addition by: Bo Thide', bt@irfu.se         */
/*****************************************************/
#ifdef UNIX
#include <sys/time.h>
#include <sys/resource.h>

#ifdef hpux
#include <sys/syscall.h>
#define getrusage(a,b) syscall(SYS_getrusage,a,b)
#endif

struct rusage rusage;

SPDP dtime()
{
  SPDP q;

  getrusage(RUSAGE_SELF, &rusage);

  q = (SPDP)(rusage.ru_utime.tv_sec);
  q = q + (SPDP)(rusage.ru_utime.tv_usec) * 1.0e-06;

  return q;
}
#endif

/***************************************************/
/*  UNIX_Old dtime(). This is the old UNIX timer.  */
/*  Use only if absolutely necessary as HZ may be  */
/*  ill defined on your system.                    */
/***************************************************/
#ifdef UNIX_Old
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>

#ifndef HZ
#define HZ 60
#endif

struct tms tms;

SPDP dtime()
{
  SPDP q;

  times(&tms);

  q = (SPDP)(tms.tms_utime) / (SPDP)HZ;

  return q;
}
#endif

/*********************************************************/
/*  VMS dtime() for VMS systems.                         */
/*  Provided by: RAMO@uvphys.phys.UVic.CA                */
/*  Some people have run into problems with this timer.  */
/*********************************************************/
#ifdef VMS
#include time

#ifndef HZ
#define HZ 100
#endif

struct tbuffer_t
{
  int proc_user_time;
  int proc_system_time;
  int child_user_time;
  int child_system_time;
};
struct tbuffer_t tms;

SPDP dtime()
{
  SPDP q;

  times(&tms);

  q = (SPDP)(tms.proc_user_time) / (SPDP)HZ;

  return q;
}
#endif

/******************************/
/*  BORLAND C dtime() for DOS */
/******************************/
#ifdef BORLAND_C
#include <ctype.h>
#include <dos.h>
#include <time.h>

#define HZ 100
struct time tnow;

SPDP dtime()
{
  SPDP q;

  gettime(&tnow);

  q = 60.0 * (SPDP)(tnow.ti_min);
  q = q + (SPDP)(tnow.ti_sec);
  q = q + (SPDP)(tnow.ti_hund) / (SPDP)HZ;

  return q;
}
#endif

/***************************************/
/*  Microsoft C (MSC) dtime() for DOS  */
/*  Also suitable for Watcom C/C++ and */
/*  some other PC compilers            */
/***************************************/
#ifdef MSC
#include <time.h>
#include <ctype.h>

#define HZ CLOCKS_PER_SEC
clock_t tnow;

SPDP dtime()
{
  SPDP q;

  tnow = clock();
  q = (SPDP)tnow / (SPDP)HZ;
  return q;
}
#endif

/*************************************/
/*  Macintosh (MAC) Think C dtime()  */
/*************************************/
#ifdef MAC
#include <time.h>

#define HZ 60

SPDP dtime()
{
  SPDP q;

  q = (SPDP)clock() / (SPDP)HZ;

  return q;
}
#endif

/************************************************************/
/*  iPSC/860 (IPSC) dtime() for i860.                       */
/*  Provided by: Dan Yergeau, yergeau@gloworm.Stanford.EDU  */
/************************************************************/
#ifdef IPSC
extern double dclock();

SPDP dtime()
{
  SPDP q;

  q = dclock();

  return q;
}
#endif

/**************************************************/
/*  FORTRAN dtime() for Cray type systems.        */
/*  This is the preferred timer for Cray systems. */
/**************************************************/
#ifdef FORTRAN_SEC

fortran double second();

SPDP dtime()
{
  SPDP q;

  second(&q);

  return q;
}
#endif

/***********************************************************/
/*  UNICOS C dtime() for Cray UNICOS systems.  Don't use   */
/*  unless absolutely necessary as returned time includes  */
/*  'user+system' time.  Provided by: R. Mike Dority,      */
/*  dority@craysea.cray.com                                */
/***********************************************************/
#ifdef CTimer
#include <time.h>

SPDP dtime()
{
  SPDP q;
  clock_t   clock(void);

  q = (SPDP)clock() / (SPDP)CLOCKS_PER_SEC;

  return q;
}
#endif

/********************************************/
/* Another UNIX timer using gettimeofday(). */
/* However, getrusage() is preferred.       */
/********************************************/
#ifdef GTODay
#include <sys/time.h>

struct timeval tnow;

SPDP dtime()
{
  SPDP q;

  gettimeofday(&tnow, NULL);
  q = (SPDP)tnow.tv_sec + (SPDP)tnow.tv_usec * 1.0e-6;

  return q;
}
#endif

/*****************************************************/
/*  Fujitsu UXP/M timer.                             */
/*  Provided by: Mathew Lim, ANUSF, M.Lim@anu.edu.au */
/*****************************************************/
#ifdef UXPM
#include <sys/types.h>
#include <sys/timesu.h>
struct tmsu rusage;

SPDP dtime()
{
  SPDP q;

  timesu(&rusage);

  q = (SPDP)(rusage.tms_utime) * 1.0e-06;

  return q;
}
#endif

/**********************************************/
/*    Macintosh (MAC_TMgr) Think C dtime()    */
/*   requires Think C Language Extensions or  */
/*    #include <MacHeaders> in the prefix     */
/*  provided by Francis H Schiffer 3rd (fhs)  */
/*         skipschiffer@genie.geis.com        */
/**********************************************/
#ifdef MAC_TMgr
#include <Timer.h>
#include <stdlib.h>

static TMTask   mgrTimer;
static Boolean  mgrInited = false;
static SPDP     mgrClock;

#define RMV_TIMER RmvTime( (QElemPtr)&mgrTimer )
#define MAX_TIME  1800000000L
/* MAX_TIME limits time between calls to */
/* dtime( ) to no more than 30 minutes   */
/* this limitation could be removed by   */
/* creating a completion routine to sum  */
/* 30 minute segments (fhs 1994 feb 9)   */

static void Remove_timer()
{
  RMV_TIMER;
  mgrInited = false;
}

SPDP dtime()
{
  if (mgrInited) {
    RMV_TIMER;
    mgrClock += (MAX_TIME + mgrTimer.tmCount)*1.0e-6;
  }
  else {
    if (_atexit(&Remove_timer) == 0) mgrInited = true;
    mgrClock = 0.0;
  }
  if (mgrInited) {
    mgrTimer.tmAddr = NULL;
    mgrTimer.tmCount = 0;
    mgrTimer.tmWakeUp = 0;
    mgrTimer.tmReserved = 0;
    InsTime((QElemPtr)&mgrTimer);
    PrimeTime((QElemPtr)&mgrTimer, -MAX_TIME);
  }
  return(mgrClock);
}
#endif

/***********************************************************/
/*  Parsytec GCel timer.                                   */
/*  Provided by: Georg Wambach, gw@informatik.uni-koeln.de */
/***********************************************************/
#ifdef PARIX
#include <sys/time.h>

SPDP dtime()
{
  SPDP q;

  q = (SPDP)(TimeNowHigh()) / (SPDP)CLK_TCK_HIGH;

  return q;
}
#endif

/************************************************/
/*  Sun Solaris POSIX dtime() routine           */
/*  Provided by: Case Larsen, CTLarsen.lbl.gov  */
/************************************************/
#ifdef POSIX
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/rusage.h>

#ifdef __hpux
#include <sys/syscall.h>
#endif

struct rusage rusage;

SPDP dtime()
{
  SPDP q;

  getrusage(RUSAGE_SELF, &rusage);

  q = (SPDP)(rusage.ru_utime.tv_sec);
  q = q + (SPDP)(rusage.ru_utime.tv_nsec) * 1.0e-09;

  return q;
}
#endif

/****************************************************/
/*  Windows NT (32 bit) dtime() routine             */
/*  Provided by: Piers Haken, piersh@microsoft.com  */
/****************************************************/
#ifdef WIN32
#include <windows.h>

SPDP dtime(void)
{
  SPDP q;

  q = (SPDP)GetTickCount() * 1.0e-03;

  return q;
}
#endif

/***************************************************/
/*  RISCV dtime() routine  */
/*  Provided by: Vitaly Gaiduk, vitaly.gaiduk@cloudbear.ru  */
/***************************************************/
#if defined (__riscv)

#define HZ 1000000
#define CSR_TICKS 0xc01

uint32_t get_ticks (void) {
	  uint32_t result;

    __asm__ volatile ("csrr %0, %1"
                                    : "=r"(result)    // Output: register
                                    : "i"(CSR_TICKS)  // Input: constant immediate
                                    :);               // Clobbers: none
    return result;
}

SPDP dtime()
{
  return (SPDP)get_ticks() / (SPDP)HZ;
}

#endif

#define Start_Timer() Begin_Time = dtime()
#define Stop_Timer() End_Time = dtime()

#endif
