#pragma once

// From glfw3.h

/*! @brief The regular arrow cursor shape.
 *
 *  The regular arrow cursor shape.
 */
#define HZ_ARROW_CURSOR           0x00036001
 /*! @brief The text input I-beam cursor shape.
  *
  *  The text input I-beam cursor shape.
  */
#define HZ_IBEAM_CURSOR           0x00036002
  /*! @brief The crosshair cursor shape.
   *
   *  The crosshair cursor shape.
   */
#define HZ_CROSSHAIR_CURSOR       0x00036003
   /*! @brief The pointing hand cursor shape.
    *
    *  The pointing hand cursor shape.
    */
#define HZ_POINTING_HAND_CURSOR   0x00036004
    /*! @brief The horizontal resize/move arrow shape.
     *
     *  The horizontal resize/move arrow shape.  This is usually a horizontal
     *  double-headed arrow.
     */
#define HZ_RESIZE_EW_CURSOR       0x00036005
     /*! @brief The vertical resize/move arrow shape.
      *
      *  The vertical resize/move shape.  This is usually a vertical double-headed
      *  arrow.
      */
#define HZ_RESIZE_NS_CURSOR       0x00036006
      /*! @brief The top-left to bottom-right diagonal resize/move arrow shape.
       *
       *  The top-left to bottom-right diagonal resize/move shape.  This is usually
       *  a diagonal double-headed arrow.
       *
       *  @note @macos This shape is provided by a private system API and may fail
       *  with @ref HZ_CURSOR_UNAVAILABLE in the future.
       *
       *  @note @x11 This shape is provided by a newer standard not supported by all
       *  cursor themes.
       *
       *  @note @wayland This shape is provided by a newer standard not supported by
       *  all cursor themes.
       */
#define HZ_RESIZE_NWSE_CURSOR     0x00036007
       /*! @brief The top-right to bottom-left diagonal resize/move arrow shape.
        *
        *  The top-right to bottom-left diagonal resize/move shape.  This is usually
        *  a diagonal double-headed arrow.
        *
        *  @note @macos This shape is provided by a private system API and may fail
        *  with @ref HZ_CURSOR_UNAVAILABLE in the future.
        *
        *  @note @x11 This shape is provided by a newer standard not supported by all
        *  cursor themes.
        *
        *  @note @wayland This shape is provided by a newer standard not supported by
        *  all cursor themes.
        */
#define HZ_RESIZE_NESW_CURSOR     0x00036008
        /*! @brief The omni-directional resize/move cursor shape.
         *
         *  The omni-directional resize cursor/move shape.  This is usually either
         *  a combined horizontal and vertical double-headed arrow or a grabbing hand.
         */
#define HZ_RESIZE_ALL_CURSOR      0x00036009
         /*! @brief The operation-not-allowed shape.
          *
          *  The operation-not-allowed shape.  This is usually a circle with a diagonal
          *  line through it.
          *
          *  @note @x11 This shape is provided by a newer standard not supported by all
          *  cursor themes.
          *
          *  @note @wayland This shape is provided by a newer standard not supported by
          *  all cursor themes.
          */
#define HZ_NOT_ALLOWED_CURSOR     0x0003600A
          /*! @brief Legacy name for compatibility.
           *
           *  This is an alias for compatibility with earlier versions.
           */
#define HZ_HRESIZE_CURSOR         HZ_RESIZE_EW_CURSOR
           /*! @brief Legacy name for compatibility.
            *
            *  This is an alias for compatibility with earlier versions.
            */
#define HZ_VRESIZE_CURSOR         HZ_RESIZE_NS_CURSOR
            /*! @brief Legacy name for compatibility.
             *
             *  This is an alias for compatibility with earlier versions.
             */
#define HZ_HAND_CURSOR            HZ_POINTING_HAND_CURSOR
             /*! @} */
