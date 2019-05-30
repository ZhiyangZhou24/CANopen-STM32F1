/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.32 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_PNG_Private.h
Purpose     : Private header
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUI_PNG_PRIVATE_H
#define GUI_PNG_PRIVATE_H

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
/* Default parameter structure for reading data from memory */
typedef struct {
  const U8 * pFileData;
  U32   FileSize;
} GUI_PNG_PARAM;

/* Context structure for getting stdio input */
typedef struct {
  GUI_GET_DATA_FUNC * pfGetData; /* Function pointer */
  U32 Off;                       /* Data pointer */
  void * pParam;                 /* Parameter pointer passed to function */
} GUI_PNG_CONTEXT;

#endif /* GUI_PNG_PRIVATE_H */
