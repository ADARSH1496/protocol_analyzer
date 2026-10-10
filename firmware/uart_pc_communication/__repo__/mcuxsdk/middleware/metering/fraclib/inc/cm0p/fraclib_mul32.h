/************************************************************************************************
*
* Copyright 2013-2016, Freescale Semiconductor, Inc.
* Copyright 2016-2021, 2024 NXP
*
* NXP Proprietary. This software is owned or controlled by NXP and may
* only be used strictly in accordance with the applicable license terms. 
* By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that
* you have read, and that you agree to comply with and are bound by,
* such license terms.  If you do not agree to be bound by the applicable
* license terms, then you may not retain, install, activate or otherwise
* use the software.
*
***************************************************************************//*!
* @file      FRACLIB_Mul32.h
* @author    R55013
* @version   1.1.0.0
* @date      Dec-15-2014
* @brief     Header file containing 32x32=64 multiplication function prototypes
*            coded in FRACLIB_Mul32.s assembler source file.
******************************************************************************/
#ifndef __FRACLIB_MUL32_H
#define __FRACLIB_MUL32_H

/****************************************************************************//*!
 * @brief   Multiply two 32-bit fractional values generating 64-bit fractional
 *          result.
 * @param   lsrc1   - Input 32-bit fractional value.
 * @param   lsrc2   - Input 32-bit fractional value.
 * @return  Function returns 64-bit value in fractional format.
 * @remarks Implementation in ASM-language.
 *******************************************************************************/
extern frac64 FRACLIB_FFMUL32 (register frac32 lsrc1, register frac32 lsrc2);

/****************************************************************************//*!
 * @brief   Multiply two 32-bit signed values generating 64-bit signed result.
 * @param   lsrc1   - Input 32-bit signed value.
 * @param   lsrc2   - Input 32-bit signed value.
 * @return  Function returns 64-bit value in signed integer format.
 * @remarks Implementation in ASM-language.
 *******************************************************************************/
extern int64 FRACLIB_SSMUL32 (register int32 lsrc1, register int32 lsrc2);

/****************************************************************************//*!
 * @brief   Multiply 32-bit unsigned value with 32-bit signed value generating
 *          64-bit signed result.
 * @param   lsrc1   - Input 32-bit unsigned value.
 * @param   lsrc2   - Input 32-bit signed value.
 * @return  Function returns 64-bit value in signed integer format.
 * @remarks Implementation in ASM-language.
 *******************************************************************************/
extern int64 FRACLIB_USMUL32 (register uint32 lsrc1, register int32 lsrc2);

/****************************************************************************//*!
 * @brief   Multiply 32-bit signed value with 32-bit unsigned value generating
 *          64-bit signed result.
 * @param   lsrc1   - Input 32-bit signed value.
 * @param   lsrc2   - Input 32-bit unsigned value.
 * @return  Function returns 64-bit value in signed integer format.
 * @remarks Implementation in ASM-language.
 *******************************************************************************/
extern int64 FRACLIB_SUMUL32 (register int32 lsrc1, register uint32 lsrc2);

/****************************************************************************//*!
 * @brief   Multiply two 32-bit unsigned values generating 64-bit unsigned result.
 * @param   lsrc1   - Input 32-bit unsigned value.
 * @param   lsrc2   - Input 32-bit unsigned value.
 * @return  Function returns 64-bit value in unsigned integer format.
 * @remarks Implementation in ASM-language.
 *******************************************************************************/
extern uint64 FRACLIB_UUMUL32 (register uint32 lsrc1, register uint32 lsrc2);

#endif /* __FRACLIB_MUL32_H */
