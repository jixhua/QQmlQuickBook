﻿/*
Copyright Benjamin Worpitz 2018
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef BOOST_PREDEF_LANGUAGE_CUDA_H
#define BOOST_PREDEF_LANGUAGE_CUDA_H

#include <sstd/boost/predef/version_number.h>
#include <sstd/boost/predef/make.h>

/*`
[heading `BOOST_LANG_CUDA`]

[@https://en.wikipedia.org/wiki/CUDA CUDA C/C++] language.
If available, the version is detected as VV.RR.P.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CUDACC__`] [__predef_detection__]]
    [[`__CUDA__`] [__predef_detection__]]

    [[`CUDA_VERSION`] [VV.RR.P]]
    ]
 */

#define BOOST_LANG_CUDA BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__CUDACC__) || defined(__CUDA__)
#   undef BOOST_LANG_CUDA
#   include <cuda.h>
#   if defined(CUDA_VERSION)
#       define BOOST_LANG_CUDA BOOST_PREDEF_MAKE_10_VVRRP(CUDA_VERSION)
#   else
#       define BOOST_LANG_CUDA BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if BOOST_LANG_CUDA
#   define BOOST_LANG_CUDA_AVAILABLE
#endif

#define BOOST_LANG_CUDA_NAME "CUDA C/C++"


#endif

#include <sstd/boost/predef/detail/test.h>
BOOST_PREDEF_DECLARE_TEST(BOOST_LANG_CUDA,BOOST_LANG_CUDA_NAME)