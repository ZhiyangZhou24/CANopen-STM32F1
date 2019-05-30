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
File        : GUI_PNG.c
Purpose     : Implementation of GUI_PNG... functions
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "png.h"
#include "pngstruct.h"

#include "GUI_Private.h"
#include "GUI_PNG_Private.h"

#if (GUI_WINSUPPORT)
  #include "WM.h"
#endif

#if (GUI_VERSION <= 41800)
  int GUI_PNG_Draw      (const void * pFileData, int DataSize, int x0, int y0);
  int GUI_PNG_DrawEx    (GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0);
  int GUI_PNG_GetXSize  (const void * pFileData, int FileSize);
  int GUI_PNG_GetXSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p);
  int GUI_PNG_GetYSize  (const void * pFileData, int FileSize);
  int GUI_PNG_GetYSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p);
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define _LOCK() \
  GUI_LOCK(); \
  GUI_ALLOC_Lock()

#define _UNLOCK() \
  GUI_UNLOCK(); \
  GUI_ALLOC_Unlock()

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_PNG__GetData
*
* Parameters:
*   p           - Pointer to application defined data.
*   NumBytesReq - Number of bytes requested.
*   ppData      - Pointer to data pointer. This pointer should be set to
*                 a valid location.
*
* Return value:
*   Number of data bytes available.
*/
static int GUI_PNG__GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  GUI_PNG_PARAM * pParam;
  U8 * pData;

  pData = (U8 *)*ppData;
  pParam = (GUI_PNG_PARAM *)p;
  GUI_MEMCPY(pData, (const void *)(pParam->pFileData + Off), NumBytesReq);
  return NumBytesReq;
}

/*********************************************************************
*
*       _png_cexcept_error
*
* Purpose:
*   Called by PNG library if an error occurs.
*
* Parameter:
*   png_ptr - Context pointer
*   msg     - Error message
*/
static void _png_cexcept_error(png_structp png_ptr, png_const_charp msg) {
  GUI_USE_PARA(png_ptr);
  GUI_USE_PARA(msg);
  GUI_DEBUG_ERROROUT("GUI_PNG.c:\nError in _png_cexcept_error().");
}

/*********************************************************************
*
*       _png_read_data
*/
static void PNGAPI _png_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
  GUI_PNG_CONTEXT * pContext;
  pContext = (GUI_PNG_CONTEXT *)png_ptr->io_ptr;
  if ((png_size_t)pContext->pfGetData(pContext->pParam, (const U8 **)&data, length, pContext->Off) != length) {
    _png_cexcept_error(png_ptr, "Error reading data!");
  }
  pContext->Off += length;
}

/*********************************************************************
*
*       _malloc_fn
*
* Purpose:
*   Uses the emWin memory manager for allocating memory
*/
static png_voidp _malloc_fn(png_structp png_ptr, png_size_t size) {
  #if 1
    void * p;
    GUI_HMEM hMem;

    GUI_USE_PARA(png_ptr);
    hMem = GUI_ALLOC_AllocNoInit(size);
    p = (void *)GUI_LOCK_H(hMem);
    return p;
  #else
    GUI_USE_PARA(png_ptr);
    return malloc(size);
  #endif
}

/*********************************************************************
*
*       _free_fn
*
* Purpose:
*   Uses the emWin memory manager for freeing memory
*/
static void _free_fn(png_structp png_ptr, png_voidp ptr) {
  #if 1
    GUI_HMEM hMem;

    GUI_USE_PARA(png_ptr);
    hMem = GUI_ALLOC_p2h(ptr);
    GUI_UNLOCK_H(ptr);
    GUI_ALLOC_Free(hMem);
  #else
    GUI_USE_PARA(png_ptr);
    free(ptr);
  #endif
}

/*********************************************************************
*
*       _GetImageHeader
*/
static int _GetImageHeader(png_structp * ppng_ptr, png_infop * pinfo_ptr, GUI_PNG_CONTEXT * pContext, U32 * pWidth, U32 * pHeight, int * pBitDepth, int * pColorType) {
  U8 acHeader[8];
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;

  png_ptr  = *ppng_ptr;
  info_ptr = *pinfo_ptr;
  //
  // Read-struct creation
  //
  png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)_png_cexcept_error, (png_error_ptr)NULL, NULL, _malloc_fn, _free_fn);
  if (png_ptr == NULL) {
    return 1;
  }
  //
  // Info-struct creation
  //
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 1;
  }
  //
  // Set read function
  //
  png_set_read_fn(png_ptr, (png_voidp)pContext, _png_read_data);
  //
  // Check file header
  //
  _png_read_data(png_ptr, acHeader, 8);
  if (png_sig_cmp(acHeader, 0, 8) != 0) {
    return 1;
  }
  png_set_sig_bytes(png_ptr, 8);
  //
  // Read all PNG info up to image data
  //
  png_read_info(png_ptr, info_ptr);
  //
  // Get width, height, bit-depth and color-type
  //
  png_get_IHDR(png_ptr, info_ptr, pWidth, pHeight, pBitDepth, pColorType, NULL, NULL, NULL);
  *ppng_ptr  = png_ptr;
  *pinfo_ptr = info_ptr;
  return 0;
}

/*********************************************************************
*
*       _Draw
*/
static int _Draw(int x0, int y0, GUI_PNG_CONTEXT * pContext) {
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;
  U32 Width, Height;
  int BitDepth, ColorType;
  png_color bkgColor = {127, 127, 127};

  U32 RowBytes;
  U32 Channels;
  double Gamma;
  U8 * pImageData;
  U8 * pImageDataOld;
  U8 ** ppRowPointers = NULL;

  int BitsPerPixel, BytesPerPixel, HasAlpha, HasTrans;
  unsigned i, x, y;
  int BkPixelIndex;
  LCD_PIXELINDEX * pBkGnd;
  U32 * pColor;
  U32 * pWrite;
  tLCDDEV_Index2Color * pfIndex2Color;
  tLCDDEV_Color2Index * pfColor2Index;
  GUI_HMEM hBkGnd;
  GUI_HMEM hColor;
  int xSize, xPos;
  LCD_PIXELINDEX * p;
  U8 r, g, b, a, Alpha;
  U32 Color;
  U32 BkColor, DataColor;

#if (GUI_WINSUPPORT)
  GUI_RECT Rect;
  GUI_RECT ClipRect;
#endif

  //
  // Get image header
  //
  if (_GetImageHeader(&png_ptr, &info_ptr, pContext, &Width, &Height, &BitDepth, &ColorType)) {
    return 1;
  }
  //
  // Expand images of all color-type and bit-depth to 3x8 bit RGB images, and
  // let the library process things like alpha, transparency, background
  //
  if (BitDepth == 16) {
    png_set_strip_16(png_ptr);
  }
  if (ColorType == PNG_COLOR_TYPE_PALETTE) {
    png_set_expand(png_ptr);
  }
  if (BitDepth < 8) {
    png_set_expand(png_ptr);
  }
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_expand(png_ptr);
  }
  if (ColorType == PNG_COLOR_TYPE_GRAY || ColorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png_ptr);
  }
  //
  // Set the background color to draw transparent and alpha images over.
  //
  // IMPORTANT: Not all programs consider this information. Mozilla Firefox, IrfanView
  //            and Adobe Photoshop Elements for example do not use background
  //            information for drawing PNGs in all cases. If there are differences 
  //            in showing the background this could be the cause. If the same behavior
  //            of this library is desired please comment out the following lines.
  //
#if 1 // Please set to 0 for ignoring the background information
  {
    png_color_16 * pBackground;

    if (png_get_bKGD(png_ptr, info_ptr, &pBackground)) {
      png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    }
  }
#endif
  //
  // If required set gamma conversion
  //
  if (png_get_gAMA(png_ptr, info_ptr, &Gamma)) {
    png_set_gamma(png_ptr, (double)2.2, Gamma);
  }
  //
  // After the transformations have been registered update info_ptr data
  //
  png_read_update_info(png_ptr, info_ptr);
  //
  // Get again width, height and the new bit-depth and color-type
  //
  png_get_IHDR(png_ptr, info_ptr, &Width, &Height, &BitDepth, &ColorType, NULL, NULL, NULL);
  //
  // Row_bytes is the width x number of channels
  //
  RowBytes = png_get_rowbytes(png_ptr, info_ptr);
  Channels = png_get_channels(png_ptr, info_ptr);
  //
  // Now we can allocate memory to store the image
  //
  if ((pImageData = (png_byte *)_malloc_fn(NULL, RowBytes * Height * sizeof(png_byte))) == NULL) {
    png_error(png_ptr, "Out of memory");
    return 1;
  }
  //
  // And allocate memory for an array of row-pointers
  //
  if ((ppRowPointers = (png_bytepp)_malloc_fn(NULL, Height * sizeof(png_bytep))) == NULL) {
    png_error(png_ptr, "Out of memory");
    _free_fn(NULL, pImageData);
    return 1;
  }
  //
  // Set the individual row-pointers to point at the correct offsets
  //
  for (i = 0; i < Height; i++) {
    ppRowPointers[i] = pImageData + i * RowBytes;
  }
  //
  // Read the whole image
  //
  png_read_image(png_ptr, ppRowPointers);
  //
  // Cleanup memory except image data
  //
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  _free_fn(NULL, ppRowPointers);
  pImageDataOld = pImageData;
  {
    //
    // Allocate line buffer(s)
    //
    hColor = GUI_ALLOC_AllocZero(Width * sizeof(U32));
    hBkGnd = GUI_ALLOC_AllocZero(Width * sizeof(LCD_PIXELINDEX));
    if (hColor && hBkGnd) {
      pColor = (U32 *)GUI_LOCK_H(hColor);
      pBkGnd = (LCD_PIXELINDEX *)GUI_LOCK_H(hBkGnd);
      BitsPerPixel = GUI_GetBitsPerPixelEx(GUI_pContext->SelLayer);
      if (BitsPerPixel <= 8) {
        BytesPerPixel = 1;
      } else if (BitsPerPixel <= 16) {
        BytesPerPixel = 2;
      } else {
        BytesPerPixel = 4;
      }
      //
      // Get function pointer(s)
      //
      pfIndex2Color = GUI_GetpfIndex2ColorEx(GUI_pContext->SelLayer);
      pfColor2Index = GUI_GetpfColor2IndexEx(GUI_pContext->SelLayer);
      //
      // Iterate over window manager rectangles
      //
      #if (GUI_WINSUPPORT)
        WM_ADDORG(x0,y0);
        Rect.x1 = (Rect.x0 = x0) + Width - 1;
        Rect.y1 = (Rect.y0 = y0) + Height - 1;
        WM_ITERATE_START(&Rect) {
      #endif
      //
      // Iterate over all lines
      //
      for (y = 0; y < Height; y++) {
        pWrite = pColor;
        HasAlpha = HasTrans = 0;
        //
        // Read one line of pixel data
        //
        for (x = 0; x < Width; x++) {
          r = *pImageData++;
          g = *pImageData++;
          b = *pImageData++;
          if (Channels == 4) { // If alpha channel exist...
            a = 255 - *pImageData++;
            if (a < 255) {
              HasAlpha = 1;
            } else if (a == 255) {
              HasTrans = 1;
            }
#if (GUI_USE_ARGB)
            Color = b + ((U16)g << 8) + ((U32)r << 16) + ((U32)(255 - a) << 24);
#else
            Color = r + ((U16)g << 8) + ((U32)b << 16) + ((U32)a << 24);
#endif
          } else {
#if (GUI_USE_ARGB)
            Color = b + ((U16)g << 8) + ((U32)r << 16);
#else
            Color = r + ((U16)g << 8) + ((U32)b << 16);
#endif
          }
          *pWrite++ = Color;
        }
        //
        // Read background if transparency or alpha exist
        //
        #if (GUI_WINSUPPORT)
          ClipRect = GUI_pContext->ClipRect;
          WM_Deactivate();
        #endif
        if (HasAlpha || HasTrans) {
          if (x0 < 0) {
            p     = pBkGnd - x0;
            xSize = Width + x0;
            xPos  = 0;
          } else {
            p     = pBkGnd;
            xSize = Width;
            xPos  = x0;
          }
          if (xSize > 0) {
            GUI_ReadRectEx(xPos, y0 + y, xPos + xSize - 1, y0 + y, p, GUI__apDevice[GUI_pContext->SelLayer]);
            GUI__ExpandPixelIndices(p, xSize, GUI_GetBitsPerPixelEx(GUI_pContext->SelLayer));
          }
        }
        #if (GUI_WINSUPPORT)
          WM_Activate();
          GUI_pContext->ClipRect = ClipRect;
        #endif
        if (HasAlpha) {
          //
          // Mix with background
          //
          for (x = 0; x < Width; x++) {
            DataColor = *(pColor + x);
            Alpha = DataColor >> 24;
#if (GUI_USE_ARGB)
            if (Alpha < 255) {
#else
            if (Alpha) {
#endif
              BkPixelIndex = *(pBkGnd + x);
              BkColor = pfIndex2Color(BkPixelIndex);
#if (GUI_USE_ARGB)
              Color = GUI__MixColors(DataColor, BkColor, Alpha);
#else
              Color = GUI__MixColors(DataColor, BkColor, 255 - Alpha);
#endif
              *(pBkGnd + x) = pfColor2Index(Color);
            } else {
              *(pBkGnd + x) = pfColor2Index(DataColor);
            }
          }
        } else {
          //
          // Store data
          //
          for (x = 0; x < Width; x++) {
            Alpha = *(pColor + x) >> 24;
#if (GUI_USE_ARGB)
            if (Alpha == 255) {
#else
            if (Alpha == 0) {
#endif
              Color = *(pColor + x);
              *(pBkGnd + x) = pfColor2Index(Color);
            }
          }
        }
        //
        // Draw line of pixels
        //
        GUI__CompactPixelIndices(pBkGnd, Width, BitsPerPixel);
        LCD_DrawBitmap(x0, y0 + y, Width, 1, 1, 1, BytesPerPixel * 8, 0, (U8 *)pBkGnd, NULL);
      }
      pImageData = pImageDataOld;
      #if (GUI_WINSUPPORT)
        } WM_ITERATE_END();
      #endif
      //
      // Unlock pointers
      //
      GUI_UNLOCK_H(pColor);
      GUI_UNLOCK_H(pBkGnd);
    }
    GUI_ALLOC_Free(hColor);
    GUI_ALLOC_Free(hBkGnd);
  }
  //
  // Cleanup image data
  //
  _free_fn(NULL, pImageData);
  return 0;
}

/*********************************************************************
*
*       _GetSize
*/
static int _GetSize(GUI_GET_DATA_FUNC * pfGetData, void * p, U32 * pxSize, U32 * pySize) {
  png_structp png_ptr  = NULL;
  png_infop   info_ptr = NULL;
  int BitDepth, ColorType;
  GUI_PNG_CONTEXT Context = {0};

  Context.pfGetData = pfGetData;
  Context.pParam    = p;
  //
  // Get image header
  //
  if (_GetImageHeader(&png_ptr, &info_ptr, &Context, pxSize, pySize, &BitDepth, &ColorType)) {
    return 1;
  }
  //
  // Cleanup memory
  //
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_PNG_GetXSizeEx
*/
int GUI_PNG_GetXSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p) {
  U32 Width, Height;
  int r;

  _LOCK();
  if (_GetSize(pfGetData, p, &Width, &Height)) {
    r = 0;
  } else {
    r = (int)Width;
  }
  _UNLOCK();
  return r;
}

/*********************************************************************
*
*       GUI_PNG_GetXSize
*/
int GUI_PNG_GetXSize(const void * pFileData, int FileSize) {
  GUI_PNG_PARAM Param = {0};
  Param.pFileData = (const U8 *)pFileData;
  Param.FileSize  = FileSize;
  return GUI_PNG_GetXSizeEx(GUI_PNG__GetData, &Param);
}

/*********************************************************************
*
*       GUI_PNG_GetYSizeEx
*/
int GUI_PNG_GetYSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p) {
  U32 Width, Height;
  int r;

  _LOCK();
  if (_GetSize(pfGetData, p, &Width, &Height)) {
    r = 0;
  } else {
    r = (int)Height;
  }
  _UNLOCK();
  return r;
}

/*********************************************************************
*
*       GUI_PNG_GetYSize
*/
int GUI_PNG_GetYSize(const void * pFileData, int FileSize) {
  GUI_PNG_PARAM Param = {0};
  Param.pFileData = (const U8 *)pFileData;
  Param.FileSize  = FileSize;
  return GUI_PNG_GetYSizeEx(GUI_PNG__GetData, &Param);
}

/*********************************************************************
*
*       GUI_PNG_DrawEx
*/
int  GUI_PNG_DrawEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0) {
  GUI_PNG_CONTEXT Context = {0};
  int r;

  _LOCK();
  Context.pfGetData = pfGetData;
  Context.pParam    = p;
  r = _Draw(x0, y0, &Context);
  _UNLOCK();
  return r;
}

/*********************************************************************
*
*       GUI_PNG_Draw
*/
int GUI_PNG_Draw(const void * pFileData, int FileSize, int x0, int y0) {
  GUI_PNG_PARAM Param = {0};
  int r;

  _LOCK();
  Param.pFileData = (const U8 *)pFileData;
  Param.FileSize  = FileSize;
  r = GUI_PNG_DrawEx(GUI_PNG__GetData, &Param, x0, y0);
  _UNLOCK();
  return r;
}

/*************************** End of file ****************************/
