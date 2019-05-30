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
File        : IMAGE_PNG.c
Purpose     : Implementation of image widget, 'PNG' related code
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"

#if (GUI_VERSION >= 51300) && GUI_WINSUPPORT

#if defined(__cplusplus)
  extern "C" {             // Make sure we have C-declarations in C++ programs
#endif

#include "IMAGE_Private.h"

#if defined(__cplusplus)
  }
#endif

/*********************************************************************
*
*       Static routines
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawImageAt
*/
static void _DrawImageAt(IMAGE_Handle hObj, int xPos, int yPos) {
  IMAGE_OBJ  * pObj;
  const void * pData;
  U32          FileSize;

  pObj = IMAGE_LOCK_H(hObj); {
    pData    = (GUI_BITMAP *)pObj->pData;
    FileSize = pObj->FileSize;
  } GUI_UNLOCK_H(pObj);
  GUI_PNG_Draw(pData, FileSize, xPos, yPos);
}

/*********************************************************************
*
*       _GetImageSize
*/
static void _GetImageSize(IMAGE_Handle hObj, int * pxSize, int * pySize) {
  IMAGE_OBJ   * pObj;
  const void  * pData;
  U32           FileSize;

  pObj = IMAGE_LOCK_H(hObj); {
    pData    = (GUI_BITMAP *)pObj->pData;
    FileSize = pObj->FileSize;
  } GUI_UNLOCK_H(pObj);
  *pxSize = GUI_PNG_GetXSize(pData, FileSize);
  *pySize = GUI_PNG_GetYSize(pData, FileSize);
}

/*********************************************************************
*
*       _DrawImageAtEx
*/
static void _DrawImageAtEx(IMAGE_Handle hObj, int xPos, int yPos) {
  IMAGE_OBJ         * pObj;
  void              * pVoid;
  GUI_GET_DATA_FUNC * pfGetData;

  pObj = IMAGE_LOCK_H(hObj); {
    pVoid     = pObj->pVoid;
    pfGetData = pObj->pfGetData;
  } GUI_UNLOCK_H(pObj);
  GUI_PNG_DrawEx(pfGetData, pVoid, xPos, yPos);
}

/*********************************************************************
*
*       _GetImageSizeEx
*/
static void _GetImageSizeEx(IMAGE_Handle hObj, int * pxSize, int * pySize) {
  IMAGE_OBJ         * pObj;
  void              * pVoid;
  GUI_GET_DATA_FUNC * pfGetData;

  pObj = IMAGE_LOCK_H(hObj); {
    pVoid     = pObj->pVoid;
    pfGetData = pObj->pfGetData;
  } GUI_UNLOCK_H(pObj);
  *pxSize = GUI_PNG_GetXSizeEx(pfGetData, pVoid);
  *pySize = GUI_PNG_GetYSizeEx(pfGetData, pVoid);
}

/*********************************************************************
*
*       Public routines
*
**********************************************************************
*/
/*********************************************************************
*
*       IMAGE_SetPNG
*/
void IMAGE_SetPNG(IMAGE_Handle hObj, const void * pData, U32 FileSize) {
  IMAGE_OBJ * pObj;

  if (hObj && pData && FileSize) {
    WM_LOCK();
    pObj = IMAGE_LOCK_H(hObj); {
      pObj->pData          = pData;
      pObj->FileSize       = FileSize;
      pObj->pfDrawImageAt  = _DrawImageAt;
      pObj->pfGetImageSize = _GetImageSize;
    } GUI_UNLOCK_H(pObj);
    IMAGE__FreeAttached(hObj, 0);
    IMAGE__SetWindowSize(hObj);
    WM_InvalidateWindow(hObj);
    WM_UNLOCK();
  }
}

/*********************************************************************
*
*       IMAGE_SetPNGEx
*/
void IMAGE_SetPNGEx(IMAGE_Handle hObj, GUI_GET_DATA_FUNC * pfGetData, void * pVoid) {
  IMAGE_OBJ * pObj;

  if (hObj && pfGetData) {
    WM_LOCK();
    pObj = IMAGE_LOCK_H(hObj); {
      pObj->pfGetData        = pfGetData;
      pObj->pVoid            = pVoid;
      pObj->pfDrawImageAt  = _DrawImageAtEx;
      pObj->pfGetImageSize = _GetImageSizeEx;
    } GUI_UNLOCK_H(pObj);
    IMAGE__FreeAttached(hObj, 0);
    IMAGE__SetWindowSize(hObj);
    WM_InvalidateWindow(hObj);
    WM_UNLOCK();
  }
}

#else /* avoid empty object files */

void IMAGE_PNG_C(void);
void IMAGE_PNG_C(void){}

#endif  /* #if GUI_WINSUPPORT */

/*************************** End of file ****************************/
