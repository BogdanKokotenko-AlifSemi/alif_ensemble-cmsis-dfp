/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
/******************************************************************************
 * @file     system_utils.h
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @brief    System Header File for Utility functions
 * @version  V1.0.0
 * @date     13. May 2021
 * @bug      None
 * @Note     None
 ******************************************************************************/
#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include <stdbool.h>

#if defined (M55_HP)
  #include "M55_HP.h"
#elif defined (M55_HE)
  #include "M55_HE.h"
#else
  #error device not specified!
#endif


#ifdef __cplusplus
extern "C" {
#endif

// common helper macros

/* for Unused Arguments. */
#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)                     ((void)arg)
#endif

#ifndef ENABLE
#define ENABLE                              (0x1)
#endif

#ifndef DISABLE
#define DISABLE                             (0x0)
#endif

#ifndef MASK
/**
 * @Note : 1st arg should be higher bit & 2nd arg should be lower i.e. (h > l)
 */
#define MASK(h,l)           (((~(0U)) << (l)) &  (~(0U) >> (32 - 1 - (h))))
#define MASK64(h,l)         (((~(0UL)) << (l)) &  (~(0UL) >> (32 - 1 - (h))))
#endif

#define BIT(nr)                             (1UL << (nr))
#define SET_BIT(REG, BIT_Msk)               ((REG) |= (BIT_Msk))
#define CLEAR_BIT(REG, BIT_Msk)             ((REG) &= ~(BIT_Msk))
#define READ_BIT(REG, BIT_Msk)              ((REG) & (BIT_Msk))
#define CLEAR_REG(REG)                      ((REG) = (0x0))
#define WRITE_REG(REG, VAL)                 ((REG) = (VAL))
#define READ_REG(REG)                       ((REG))

/*Below Macros are helpful for bitwise operator (input argument 1 means bit 0)*/
#define BIT64(n)                            ((1ULL) << n)
#define REG64_SET_ONE_BIT(src, n)           (src |=  (BIT64(n)))
#define REG64_CLR_ONE_BIT(src, n)           (src &= ~(BIT64(n)))
#define REG32_SET_ONE_BIT(src, n)           (src |=  (  BIT(n)))
#define REG32_CLR_ONE_BIT(src, n)           (src &= ~(  BIT(n)))

#define REG64_SET_M_AND_N_BIT(src, m, n)    (src |=  ( (BIT64(m) |  BIT64(n)) ))
#define REG64_CLR_M_AND_N_BIT(src, m, n)    (src &=  (~(BIT64(m) |  BIT64(n)) ))
#define REG32_SET_M_AND_N_BIT(src, m, n)    (src |=  ( (  BIT(m) |    BIT(n)) ))
#define REG32_CLR_M_AND_N_BIT(src, m, n)    (src &=  (~(  BIT(m) |    BIT(n)) ))

#define REG64_SET_M_TO_N_BIT(src, h, l)     (src |=  MASK64(h, l))
#define REG64_CLR_M_TO_N_BIT(src, h, l)     (src &= ~MASK64(h, l))
#define REG32_SET_M_TO_N_BIT(src, h, l)     (src |=    MASK(h, l))
#define REG32_CLR_M_TO_N_BIT(src, h, l)     (src &=   ~MASK(h, l))

#define REG32_TOGGLE_BIT(src, m)            (src ^=   BIT(m))
#define REG64_TOGGLE_BIT(src, m)            (src ^= BIT64(m))

#define RTSS_FORCE_GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE (128*1024)

// Function documentation

/**
  \fn          bool RTSS_Is_TCM_Addr(const volatile void *local_addr)
  \brief       Return true if the local_addr is in TCM
  \param[in]   local_addr  local address
  \return      bool True: if local_addr resides in TCM
*/
__STATIC_FORCEINLINE
bool RTSS_Is_TCM_Addr(const volatile void *local_addr)
{
    uint32_t addr = (uint32_t)local_addr;

    return ((addr < (ITCM_BASE + ITCM_SIZE)) || \
            ((addr > DTCM_BASE) && (addr < (DTCM_BASE + DTCM_SIZE))));
}

/**
  \fn          uint32_t LocalToGlobal(const volatile void *local_addr)
  \brief       Return the corresponding global address
  \param[in]   local_addr  local address to convert
  \return      uint32_t global address
*/
__STATIC_INLINE
uint32_t LocalToGlobal(const volatile void *local_addr)
{
    /* Only for local TCM address, we need to map it to global address space, rest
     * for all other memories like SRAM0/1, MRAM, OctalSPI etc we can pass the address
     * as-is as those are accessed using global address
     */
    uint32_t addr = (uint32_t)local_addr;

    if((addr >= DTCM_BASE) && (addr < (DTCM_BASE + DTCM_SIZE)))
        return (addr - DTCM_BASE + DTCM_GLOBAL_BASE);
    else if((addr < (ITCM_BASE + ITCM_SIZE)))
        return (addr - ITCM_BASE + ITCM_GLOBAL_BASE);
    else
        return (addr);
}

/**
  \fn          void* GlobalToLocal(uint32_t global_addr)
  \brief       Return the corresponding local address
  \param[in]   global_addr  address to convert
  \return      void* local address
*/
__STATIC_INLINE
void* GlobalToLocal(uint32_t global_addr)
{
    /* Only for local TCM address, we need to map it to local address space, rest
     * for all other memories like SRAM0/1, MRAM, OctalSPI etc we can pass the address
     * as-is as it is global.
     */
    uint32_t addr = global_addr;

    if((addr >= DTCM_GLOBAL_BASE) && (addr < (DTCM_GLOBAL_BASE + DTCM_SIZE)))
        return (void*)(addr - DTCM_GLOBAL_BASE + DTCM_BASE);
    else if((addr >= ITCM_GLOBAL_BASE) && (addr < (ITCM_GLOBAL_BASE+ ITCM_SIZE)))
        return (void*)(addr - ITCM_GLOBAL_BASE + ITCM_BASE);
    else
        return ((void*)addr);
}

/**
  \fn          void PMU_delay_loop_us(unsigned int delay_us)
  \brief       Using PMU cycle counter for delay. User need to
               take care of disabling the preemption before
               calling this PMU_delay_loop_us function. Maximum
               delay supported (2^32/(SystemCoreClock/1000000))
               micro seconds.
  \param[in]   delay_us delay in micro seconds.
*/
void PMU_delay_loop_us(unsigned int delay_us);

/**
  \fn          void RTSS_IsCacheInvalidate_Required_by_Addr (volatile void *addr, int32_t size)
  \brief       Return True if Cache Invalidate operation is required for the provided
               address region else return False.
  \param[in]   addr    address
  \param[in]   size   size of memory block (in number of bytes)
  return       True : If CacheOperation Required, else False
*/
bool RTSS_IsCacheInvalidate_Required_by_Addr (volatile void *addr, int32_t size);

/**
  \fn          void RTSS_IsCacheClean_Required_by_Addr (volatile void *addr, int32_t size)
  \brief       Return True if Cache Clean operation is required for the provided
               address region else return False.
  \param[in]   addr    address
  \param[in]   size   size of memory block (in number of bytes)
  return       True : If CacheOperation Required, else False
*/
bool RTSS_IsCacheClean_Required_by_Addr (volatile void *addr, int32_t size);

/**
  \fn          void RTSS_InvalidateICache_by_Addr (volatile void *addr, int32_t isize)
  \brief       Add a wrapper on the InvalidateIcache APIs so that
               TCM regions are ignored.
  \param[in]   addr    address
  \param[in]   isize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE
void RTSS_InvalidateICache_by_Addr (volatile void *addr, int32_t isize)
{
    if(RTSS_IsCacheInvalidate_Required_by_Addr (addr, isize))
    {
        SCB_InvalidateICache_by_Addr (addr, isize);
    }
    else
    {
        __DSB();
        __ISB();
    }
}

/**
  \fn          void RTSS_InvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
  \brief       Add a wrapper on the InvalidateDcache APIs so that
               TCM regions are ignored.
  \param[in]   addr    address
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE
void RTSS_InvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
{
    if(RTSS_IsCacheInvalidate_Required_by_Addr (addr, dsize))
    {
        /*
         * Considering the time required to Invalidate by address for
         * more than 128K size, it is better to do global clean and Invalidate.
         *
         * Perform the check for threshold size and decide.
         *
         */
        if (dsize < RTSS_FORCE_GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE)
            SCB_InvalidateDCache_by_Addr (addr, dsize);
        else
            SCB_CleanInvalidateDCache ();
    }
    else
    {
        __DSB();
        __ISB();
    }
}

/**
  \fn          void RTSS_CleanDCache_by_Addr (volatile void *addr, int32_t dsize)
  \brief       Add a wrapper on the CleanDcache APIs so that
               TCM regions are ignored.
  \param[in]   addr    address
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE
void RTSS_CleanDCache_by_Addr (volatile void *addr, int32_t dsize)
{
    if(RTSS_IsCacheClean_Required_by_Addr (addr, dsize))
    {
        /*
         * Considering the time required to Clean by address for more
         * than 128K size, it is better to do global clean.
         *
         * Perform the check for threshold size and decide.
         *
         */
        if (dsize < RTSS_FORCE_GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE)
            SCB_CleanDCache_by_Addr (addr, dsize);
        else
            SCB_CleanDCache ();
    }
    else
    {
        __DSB();
        __ISB();
    }
}

/**
  \fn          void RTSS_CleanInvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
  \brief       Add a wrapper on the CleanInvalidateDcache APIs so that
               TCM regions are ignored.
  \param[in]   addr    address (aligned to 32-byte boundary)
  \param[in]   dsize   size of memory block (in number of bytes)
*/
__STATIC_FORCEINLINE
void RTSS_CleanInvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
{
    bool clean_req = true;
    bool invalidate_req =  true;

    clean_req = RTSS_IsCacheClean_Required_by_Addr (addr, dsize);
    invalidate_req = RTSS_IsCacheInvalidate_Required_by_Addr (addr, dsize);

    if(clean_req & invalidate_req)
    {
        /*
         * Considering the time required to CleanInvalidate by address
         * for more than 128K size, it is better to do global clean & Invalidate.
         *
         * Perform the check for threshold size and decide.
         *
         */
        if (dsize < RTSS_FORCE_GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE)
            SCB_CleanInvalidateDCache_by_Addr (addr, dsize);
        else
            SCB_CleanInvalidateDCache ();
    }
    else if(clean_req)
    {
        if (dsize < RTSS_FORCE_GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE)
            SCB_CleanDCache_by_Addr (addr, dsize);
        else
            SCB_CleanDCache ();
    }
    else if(invalidate_req)
    {
        if (dsize < RTSS_FORCE_GLOBAL_CLEAN_INVALIDATE_THRESHOLD_SIZE)
            SCB_InvalidateDCache_by_Addr (addr, dsize);
        else
            SCB_CleanInvalidateDCache ();
    }
    else
    {
        __DSB();
        __ISB();
    }
}


#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_UTILS_H */
