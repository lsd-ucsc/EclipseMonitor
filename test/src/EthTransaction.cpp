// Copyright 2022 Tuan Tran
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "EthTransaction.hpp"

const SimpleObjects::Bytes& EclipseMonitor_Test::LegacyTxn_15415840()
{
	static const SimpleObjects::Bytes inst({
		// Block 15415840 Transaction Index 2
		// RLP List
		0xf8U, 0x6cU, 
		// Nonce
		0x2bU, 
		// GasPrice
		0x85U, 0x06U, 0x1cU, 0x06U, 0xa0U, 0x00U, 
		// Gas
		0x82U, 0x52U, 0x08U, 
		// To
		0x94U, 0x29U, 0x2fU, 0x04U, 0xa4U, 0x45U, 0x06U, 0xc2U, 
		0xfdU, 0x49U, 0xbaU, 0xc0U, 0x32U, 0xe1U, 0xcaU, 0x14U, 
		0x8cU, 0x35U, 0xa4U, 0x78U, 0xc8U, 
		// Value
		0x88U, 0x01U, 0xa9U, 0x51U, 0xf7U, 0xd6U, 0x87U, 0x6cU, 
		0x00U, 
		// Data
		0x80U, 
		// V
		0x26U, 
		// R
		0xa0U, 0xc4U, 0x2dU, 0x81U, 0x7bU, 0x5cU, 0x33U, 0x3dU, 
		0x6eU, 0x96U, 0x91U, 0x7bU, 0x3cU, 0xe0U, 0x67U, 0xceU, 
		0x60U, 0x2aU, 0x17U, 0xe8U, 0x22U, 0x2bU, 0xe8U, 0x46U, 
		0x32U, 0x14U, 0xc7U, 0xd8U, 0x56U, 0x52U, 0xfaU, 0x57U, 
		0xd8U, 
		// S
		0xa0U, 0x7cU, 0x98U, 0x61U, 0x1aU, 0xb7U, 0x44U, 0x02U, 
		0xceU, 0x31U, 0xa1U, 0xe1U, 0x03U, 0x12U, 0x02U, 0x6dU, 
		0xd4U, 0x1cU, 0x35U, 0x07U, 0x42U, 0xb2U, 0x5aU, 0x3fU, 
		0x5fU, 0x9aU, 0xc6U, 0x2eU, 0x5bU, 0xa3U, 0x8bU, 0x0fU, 
		0xe2U,
	});

	return inst;
}

const SimpleObjects::Bytes& EclipseMonitor_Test::AccessListTxn_15415840()
{
	static const SimpleObjects::Bytes inst({
		// Block 15415840 Transaction Index 1
		// Transaction Type
		0x01U,
		// RLP List
		0xf9U, 0x01U, 0x31U,
		// ChainId
		0x01U, 
		// Nonce
		0x83U, 0x01U, 0x3cU, 0x8cU, 
		// GasPrice
		0x85U, 0x04U, 0x78U, 0x04U, 0x44U, 0x7bU, 
		// Gas
		0x83U, 0x06U, 0xe8U, 0xf6U, 
		// To
		0x94U, 0xa1U, 0x00U, 0x6dU, 0x00U, 0x51U, 0xa3U, 0x5bU, 
		0x00U, 0x00U, 0xf9U, 0x61U, 0xa8U, 0x00U, 0x00U, 0x00U, 
		0x00U, 0x9eU, 0xa8U, 0xd2U, 0xdbU, 
		// Value
		0x83U, 0xebU, 0x3aU, 0x20U, 
		// Data
		0xb8U, 0xc3U, 0x01U, 0x00U, 0x14U, 0x96U, 0x1cU, 0xb0U, 
		0xb3U, 0x9aU, 0xf5U, 0x01U, 0x09U, 0x29U, 0x0aU, 0x6aU, 
		0x74U, 0x60U, 0xb3U, 0x08U, 0xeeU, 0x3fU, 0x19U, 0x02U, 
		0x3dU, 0x2dU, 0x00U, 0xdeU, 0x60U, 0x4bU, 0xcfU, 0x5bU, 
		0x42U, 0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 
		0x38U, 0xcaU, 0x28U, 0x12U, 0xbeU, 0x66U, 0xffU, 0x53U, 
		0xa5U, 0x04U, 0x00U, 0x05U, 0x05U, 0x06U, 0x00U, 0x00U, 
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x07U, 0x19U, 0xf9U, 
		0x2dU, 0xf6U, 0x7bU, 0x41U, 0x00U, 0x74U, 0xc9U, 0x9fU, 
		0x3fU, 0x53U, 0x31U, 0x67U, 0x6fU, 0x6aU, 0xecU, 0x27U, 
		0x56U, 0xe1U, 0xf3U, 0x9bU, 0x4fU, 0xc0U, 0x29U, 0xa8U, 
		0x3eU, 0x01U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x33U, 0x73U, 0x6eU, 
		0x6aU, 0x98U, 0xc2U, 0xb0U, 0x68U, 0x1dU, 0x8bU, 0xf0U, 
		0x77U, 0x67U, 0x82U, 0x6eU, 0xa8U, 0xbdU, 0x3bU, 0x11U, 
		0xb0U, 0xcaU, 0x42U, 0x16U, 0x31U, 0x00U, 0x00U, 0x00U, 
		0x00U, 0x00U, 0x34U, 0xf5U, 0x71U, 0xf5U, 0x8aU, 0x66U, 
		0xd6U, 0x36U, 0xfeU, 0x2dU, 0x55U, 0x25U, 0x6aU, 0xd5U, 
		0x81U, 0x56U, 0x55U, 0x76U, 0x05U, 0x27U, 0x4bU, 0x33U, 
		0x92U, 0xf8U, 0xbfU, 0xe0U, 0x5eU, 0x1eU, 0x4fU, 0xcaU, 
		0x8aU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 
		0x38U, 0xcaU, 0x28U, 0x12U, 0xbeU, 0x66U, 0xffU, 0x53U, 
		0xa5U, 0x29U, 0x0aU, 0x6aU, 0x74U, 0x60U, 0xb3U, 0x08U, 
		0xeeU, 0x3fU, 0x19U, 0x02U, 0x3dU, 0x2dU, 0x00U, 0xdeU, 
		0x60U, 0x4bU, 0xcfU, 0x5bU, 0x42U, 
		// AccessList
		0xc0U, 
		// V
		0x01U, 
		// R
		0xa0U, 0xefU, 0xafU, 0x1eU, 0x99U, 0x68U, 0x33U, 0x82U, 
		0x68U, 0x88U, 0x0eU, 0x14U, 0xd9U, 0x7aU, 0x71U, 0x6eU, 
		0x1aU, 0x9fU, 0x0fU, 0x15U, 0x55U, 0xaaU, 0x22U, 0xb5U, 
		0x08U, 0x91U, 0x98U, 0x4bU, 0x7bU, 0x48U, 0x73U, 0xefU, 
		0xb7U, 
		// S
		0xa0U, 0x38U, 0x68U, 0x73U, 0xceU, 0x3fU, 0xfcU, 0xe5U, 
		0xb7U, 0xbaU, 0x86U, 0x94U, 0x46U, 0xb5U, 0xabU, 0x10U, 
		0x68U, 0xb5U, 0xc3U, 0x9fU, 0xbbU, 0xc9U, 0xc8U, 0x3eU, 
		0x5eU, 0x1eU, 0x0dU, 0x50U, 0x54U, 0x52U, 0x74U, 0x74U, 
		0x44U,
	});

	return inst;
}


const SimpleObjects::Bytes& EclipseMonitor_Test::DynamicFeeTxn_15415840()
{
	static const SimpleObjects::Bytes inst({
		// Block 15415840 Transaction Index 3
		// Transaction Type
		0x02U,
		// RLP List
		0xF8U, 0xB2U, 
		// ChainId
		0x01U, 
		// Nonce
		0x82U, 0x29U, 0x5bU, 
		// GasTipCap
		0x84U, 0x49U, 0x04U, 0x04U, 0x40U, 
		// GasFeeCap
		0x85U, 0x0bU, 0x61U, 0xadU, 0x75U, 0x9aU, 
		// Gas
		0x82U, 0xfeU, 0xa7U, 
		// To
		0x94U, 0x9fU, 0x8fU, 0x72U, 0xaaU, 0x93U, 0x04U, 0xc8U, 
		0xb5U, 0x93U, 0xd5U, 0x55U, 0xf1U, 0x2eU, 0xf6U, 0x58U, 
		0x9cU, 0xc3U, 0xa5U, 0x79U, 0xa2U, 
		// Value
		0x80U, 
		// Data
		0xb8U, 0x44U, 0xa9U, 0x05U, 0x9cU, 0xbbU, 0x00U, 0x00U, 
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 
		0x00U, 0x00U, 0xccU, 0x81U, 0x43U, 0x00U, 0x17U, 0x75U, 
		0x68U, 0xe1U, 0x74U, 0x3dU, 0x67U, 0xaaU, 0x49U, 0xc5U, 
		0x3eU, 0x9fU, 0x28U, 0xeeU, 0x56U, 0xd1U, 0x00U, 0x00U, 
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x37U, 0x3bU, 
		0xccU, 0xe9U, 0xbeU, 0x0eU, 0x00U, 0x00U, 
		// AccessList
		0xc0U, 
		// V
		0x01U, 
		// R
		0xa0U, 0x73U, 0x64U, 0x71U, 0xdfU, 0x42U, 0x1dU, 0x10U, 
		0x1eU, 0xcaU, 0x9cU, 0xefU, 0x73U, 0xceU, 0xc7U, 0x03U, 
		0x0dU, 0xddU, 0x44U, 0x9cU, 0xa5U, 0x12U, 0x67U, 0x62U, 
		0x51U, 0xa5U, 0xdeU, 0x74U, 0x4aU, 0xeeU, 0xc0U, 0xd7U, 
		0x1aU, 
		// S
		0xa0U, 0x3bU, 0x41U, 0x95U, 0x1eU, 0xa1U, 0xf8U, 0xf0U, 
		0x68U, 0x00U, 0x95U, 0xb2U, 0x03U, 0x13U, 0xb1U, 0x56U, 
		0xfcU, 0x16U, 0x2bU, 0xebU, 0x3bU, 0xf5U, 0xc4U, 0x7dU, 
		0xedU, 0xf1U, 0xe6U, 0xe2U, 0xe4U, 0xb6U, 0x35U, 0xfbU, 
		0xbfU
	});

	return inst;
}