#pragma once

#ifdef OS_WIN
	// Include image lib
	#include <lpng/png.h>
	#include <lpng/pnginfo.h>
	#include <lpng/pngconf.h>
	#define png_infopp_NULL (png_infopp)NULL
	#define int_p_NULL (int*)NULL

	// Include font lib
	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_OUTLINE_H
	#include FT_GLYPH_H
	#include FT_BITMAP_H

	// Include render engine lib
#endif

#ifdef OS_QNX

	#ifdef CHIPSET_JACINTO4 
		// Include image lib
		#include <img/img.h>

		// Include font lib
		#include <ft2build.h>
		#include FT_FREETYPE_H
		#include FT_OUTLINE_H
		#include FT_GLYPH_H
		#include FT_BITMAP_H

		// Include render engine lib
		#include <screen/screen.h>
	#endif

	#ifdef CHIPSET_JACINTO5 
		// Include image lib
		#include <img/img.h>

		// Include font lib
		#include <ft2build.h>
		#include FT_FREETYPE_H
		#include FT_OUTLINE_H
		#include FT_GLYPH_H
		#include FT_BITMAP_H

		// Include render engine lib
		#include <screen/screen.h>
	#endif
	
	#ifdef CHIPSET_IMX6SOLO
		// Include image lib
		#include <img/img.h>

		// Include font lib
		#include <ft2build.h>
		#include FT_FREETYPE_H
		#include FT_OUTLINE_H
		#include FT_GLYPH_H
		#include FT_BITMAP_H

		// Include render engine lib
		#include <screen/screen.h>
	#endif

#endif

#ifdef OS_LINUX

	#ifdef CHIPSET_JACINTO4
		// Include image lib
		#include <libpng12/png.h>
		#include <libpng12/pnginfo.h>
		#include <libpng12/pngconf.h>

		// Include font lib
		#include <ft2build.h>
		#include FT_FREETYPE_H
		#include FT_OUTLINE_H
		#include FT_GLYPH_H
		#include FT_BITMAP_H

		// Include render engine lib
		#include <linux/fb.h>  
		#include <sys/mman.h>  
		#include <sys/ioctl.h>  

		#include "ti81xxfb.h"

	#endif

	#ifdef CHIPSET_IMX6SOLO
		// Include image lib
		#include <libpng12/png.h>
		#include <libpng12/pnginfo.h>
		#include <libpng12/pngconf.h>

		// Include font lib
		#include <ft2build.h>
		#include FT_FREETYPE_H
		#include FT_OUTLINE_H
		#include FT_GLYPH_H
		#include FT_BITMAP_H

		// Include render engine lib
		#include <linux/fb.h>
		#include <sys/mman.h>
		#include <sys/ioctl.h> 
	#endif

	#ifdef CHIPSET_NBTEVO
		// Include image lib
		#include <libpng12/png.h>
		#include <libpng12/pnginfo.h>
		#include <libpng12/pngconf.h>

		// Include font lib
		#include <ft2build.h>
		#include FT_FREETYPE_H
		#include FT_OUTLINE_H
		#include FT_GLYPH_H
		#include FT_BITMAP_H

		// Include render engine lib
		#include <X11/Xlib.h>
		#include <X11/Xutil.h>
	#endif

	#ifdef CHIPSET_PCIBM
		// Include image lib
		#include <libpng12/png.h>
		#include <libpng12/pnginfo.h>
		#include <libpng12/pngconf.h>

		// Include font lib
		#include <ft2build.h>
		#include FT_FREETYPE_H
		#include FT_OUTLINE_H
		#include FT_GLYPH_H
		#include FT_BITMAP_H

		// Include render engine lib
		#include <X11/Xlib.h>
		#include <X11/Xutil.h>
	#endif

#endif