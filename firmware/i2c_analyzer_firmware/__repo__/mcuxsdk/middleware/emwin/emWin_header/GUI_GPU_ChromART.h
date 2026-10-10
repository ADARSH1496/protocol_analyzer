/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2023  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V6.52 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  NXP Semiconductors USA, Inc.  whose
registered  office  is  situated  at 411 E. Plumeria Drive, San  Jose,
CA 95134, USA  solely for  the  purposes  of  creating  libraries  for
NXPs M0, M3/M4 and  ARM7/9 processor-based  devices,  sublicensed  and
distributed under the terms and conditions of the NXP End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              NXP Semiconductors, 1109 McKay Dr, M/S 76, San Jose, CA 95131, USA
Licensed SEGGER software: emWin
License number:           GUI-00186
License model:            emWin License Agreement, dated August 20th 2011 and Amendment No. 1, dated October 17th 2017 and Amendment No. 2, dated December 18th 2018
Licensed platform:        NXP's ARM 7/9, Cortex-M0, M3, M4, M7, A7, M33
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2011-08-19 - 2025-09-02
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : GUI_GPU_ChromART.h
Purpose     : Header of ST ChromART GPU driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_GPU_CHROMART_H
#define GUI_GPU_CHROMART_H

#include "GUI_GPU.h"

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GPU_CHROMART_CONFIG
* 
*  Description
*    Configuration structure used for ChromART driver (GUI_GPU_CHROMART)
*    during creation with GUI_GPU_Create().
*/
typedef struct {
  void              * pDMA2D;                  // ChromART peripheral base address.
  GUI_VOID_FUNCTION * pfPreExecHook;           // Hook to be called before the execution of one action.
  GUI_VOID_FUNCTION * pfPostExecHook;          // Hook to be called after the execution of one action.
  GUI_VOID_FUNCTION * pfTransferCompleteHook;  // Hook to be called from within the IRQ handler after
                                               // the transfer has been completed.
  U8                  FillAlpha : 1;           // Some ChromART devices are able to fill rectangles with
                                               // alpha blending while other devices are not able to do this.
                                               // If this flag is true, the routine for filling rectangles
                                               // will make use of this feature.
} GUI_GPU_CHROMART_CONFIG;

#endif /* GUI_GPU_CHROMART_H */

/*************************** End of file ****************************/
