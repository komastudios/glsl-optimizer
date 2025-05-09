/*
 * Copyright 2010 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <stdlib.h>

#ifdef _GNU_SOURCE
#include <locale.h>
#ifdef __APPLE__
#include <xlocale.h>
#endif
#endif

#include "strtod.h"



/**
 * Wrapper around strtod which uses the "C" locale so the decimal
 * point is always '.'
 */
double
glslopt_glsl_strtod(const char *s, char **end)
{
#if defined(_GNU_SOURCE) && !defined(__CYGWIN__) && !defined(__FreeBSD__) && \
   !defined(__HAIKU__) && !defined(__UCLIBC__)
   static locale_t loc = NULL;
   if (!loc) {
      loc = newlocale(LC_CTYPE_MASK, "C", NULL);
   }
   return strtod_l(s, end, loc);
#else
   return strtod(s, end);
#endif
}


/**
 * Wrapper around strtof which uses the "C" locale so the decimal
 * point is always '.'
 */
float
glslopt_glsl_strtof(const char *s, char **end)
{
#if defined(_GNU_SOURCE) && !defined(__CYGWIN__) && !defined(__FreeBSD__) && \
   !defined(__HAIKU__) && !defined(__UCLIBC__)
   static locale_t loc = NULL;
   if (!loc) {
      loc = newlocale(LC_CTYPE_MASK, "C", NULL);
   }
   return strtof_l(s, end, loc);
#elif (defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE >= 600)) || (defined(_ISOC99_SOURCE) && _ISOC99_SOURCE)
   return strtof(s, end);
#else
   return (float) strtod(s, end);
#endif
}
