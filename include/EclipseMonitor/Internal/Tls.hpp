// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <mbedTLScpp/Common.hpp>


namespace EclipseMonitor
{
namespace Internal
{


#ifndef MBEDTLSCPP_CUSTOMIZED_NAMESPACE
namespace Tls = ::mbedTLScpp;
#else
namespace Tls = ::MBEDTLSCPP_CUSTOMIZED_NAMESPACE;
#endif // !MBEDTLSCPP_CUSTOMIZED_NAMESPACE


} // namespace Internal
} // namespace EclipseMonitor

