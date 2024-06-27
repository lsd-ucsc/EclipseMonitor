#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Copyright (c) 2024 Haofan Zheng
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.
###


# This python code helps to generate the input/output and raw transaction data
# for unit tests in the EclipseMonitor::Eth::Transaction module and
# EclipseMonitor::Eth ABI writer classes.
# Many of the sample input data are taken from
# https://github.com/ethereum/eth-account/blob/8478a86a8d235acba0a33fcae5804887473c72de/eth_account/account.py#L655


import web3

from eth_utils import (
	keccak,
)


TEST_ABI_1 = [
	# ('uint64','(uint64,uint64)','bytes5')
	{
		"name": "foo",
		"type": "function",
		"inputs": [
			{ "name": "val1", "type": "uint64" },
			{
				"name": "val2", "type": "tuple",
				"components": [
					{ "name": "val21", "type": "uint64" },
					{ "name": "val22", "type": "uint64", }
				]
			},
			{ "name": "val3", "type": "bytes5" }
		],
		"outputs": [
			{ "name": "ret1", "type": "uint64" },
			{ "name": "ret2", "type": "bytes5"  }
		]
	},
	# ('uint64','(uint64,(uint64,bytes))','bytes5')
	{
		"name": "bar",
		"type": "function",
		"inputs": [
			{ "name": "val1", "type": "uint64" },
			{
				"name": "val2", "type": "tuple",
				"components": [
					{ "name": "val21", "type": "uint64" },
					{
						"name": "val22",
						"type": "tuple",
						"components": [
							{ "name": "val221", "type": "uint64" },
							{ "name": "val222", "type": "bytes"  }
						]
					}
				]
			},
			{ "name": "val3", "type": "bytes5" }
		],
		"outputs": [
			{ "name": "ret1", "type": "uint64" },
			{ "name": "ret2", "type": "bytes"  }
		]
	}
]
TEST_CONTRACT_ADDR_1 = '0x09616C3d61b3331fc4109a9E41a8BDB7d9776609'
TEST_PRIVATE_KEY_1 = '0x''4c0883a69102937d6231471b5dbb6204fe5129617082792ae468d01a3f362318'
TEST_PRIVATE_KEY_2 = '0x''8b03d7fdc28059800b252d475164a581d94563c9a39f435dc9b130acb5ab93bb'


# import sys
# print()
# for byte in serialized:
# 	sys.stdout.write('{}U, '.format(hex(byte)))
# print()


w3 = web3.Web3()


# transaction1 = {
# 	"type": 2,  # optional - can be implicitly determined based on max fee params
# 	"gas": 100000,
# 	"maxFeePerGas": 2000000000,
# 	"maxPriorityFeePerGas": 2000000000,
# 	"data": "0x616263646566",
# 	"nonce": 34,
# 	"to": "0x09616C3d61b3331fc4109a9E41a8BDB7d9776609",
# 	"value": "0x5af3107a4000",
# 	"accessList": (  # optional
# 		{
# 			"address": "0x0000000000000000000000000000000000000001",
# 			"storageKeys": (
# 				"0x0100000000000000000000000000000000000000000000000000000000000000",
# 			)
# 		},
# 	),
# 	"chainId": 1900,
# }
# transaction1Serialized = 'f86f82076c2284773594008477359400830186a09409616c3d61b3331fc4109a9e41a8bdb7d9776609865af3107a400086616263646566f838f7940000000000000000000000000000000000000001e1a00100000000000000000000000000000000000000000000000000000000000000'
# transaction1Hash = 'd385a3379c2fbd2ccdda2cb84fa1202cfd0635ba0e422a1921ccf8361b24465c'

# assert keccak(b'\x02' + bytes.fromhex(transaction1Serialized)) == bytes.fromhex(transaction1Hash)

# signedTransaction1 = w3.eth.account.sign_transaction(transaction1, TEST_PRIVATE_KEY_1)





# transaction1 = {
# 	"type": 2,  # optional - can be implicitly determined based on max fee params
# 	"gas": 123456,
# 	"maxFeePerGas": 987654321,
# 	"maxPriorityFeePerGas": 98765432154321,
# 	"data": "0x9879ab123d274ef5",
# 	"nonce": 814370,
# 	"to": "0x09616C3d61b3331fc4109a9E41a8BDB7d9776609",
# 	"value": 28957,
# 	"accessList": (  # optional
# 		{
# 			"address": "0x0000000000000000000000000000000000000001",
# 			"storageKeys": tuple()
# 		},
# 	),
# 	"chainId": 634,
# }
# transaction1Serialized = 'f85082027a830c6d228659d39e7fe8d1843ade68b18301e2409409616c3d61b3331fc4109a9e41a8bdb7d977660982711d889879ab123d274ef5d7d6940000000000000000000000000000000000000001c0'
# transaction1Hash = 'ac8785206b5b48c55ec9ccd796282dad8d8669cb91dde26e4ce424ccbdcbd0e0'

# assert keccak(b'\x02' + bytes.fromhex(transaction1Serialized)) == bytes.fromhex(transaction1Hash)

# signedTransaction1 = w3.eth.account.sign_transaction(transaction1, TEST_PRIVATE_KEY_1)





# transaction1 = {
# 	"type": 2,  # optional - can be implicitly determined based on max fee params
# 	"gas": 2563498,
# 	"maxFeePerGas": 14067925928,
# 	"maxPriorityFeePerGas": 2956132109347,
# 	"data": "0x5789a7b3fe8d",
# 	"nonce": 25169,
# 	"to": "0x09616C3d61b3331fc4109a9E41a8BDB7d9776609",
# 	"value": 1532891978124,
# 	"chainId": 3948,
# }
# transaction1Serialized = 'f85082027a830c6d228659d39e7fe8d1843ade68b18301e2409409616c3d61b3331fc4109a9e41a8bdb7d977660982711d889879ab123d274ef5d7d6940000000000000000000000000000000000000001c0'
# transaction1Hash = 'ac8785206b5b48c55ec9ccd796282dad8d8669cb91dde26e4ce424ccbdcbd0e0'

# assert keccak(b'\x02' + bytes.fromhex(transaction1Serialized)) == bytes.fromhex(transaction1Hash)

# signedTransaction1 = w3.eth.account.sign_transaction(transaction1, TEST_PRIVATE_KEY_1)





def FormatBytes(data: bytes) -> str:
	s = ''
	i = 0
	for b in data:
		s += '0x{:02x}U, '.format(b)
		i += 1
		if i % 32 == 0:
			s += '\n'

	return s





paramTypes = ('bytes[2]',)
paramArgs  = ([b'\x01\x02\x03\x04\x05', b'\x09\x08\x07\x06\x05\x04\x03\x02\x01'],)
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()




paramTypes = ('uint64[]',)
paramArgs  = ([0x1234567890ABCDEF, 0xEFCDAB8967452301],)
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()




paramTypes = ('bytes[]',)
paramArgs  = ([b'\x01\x02\x03\x04\x05', b'\x09\x08\x07\x06\x05\x04\x03\x02\x01'],)
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()




paramTypes = ('bytes', 'bytes',)
paramArgs  = (b'\x01\x02\x03\x04\x05', b'\x09\x08\x07\x06\x05\x04\x03\x02\x01',)
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()




paramTypes = ('uint64', 'uint64[]', 'bytes5')
paramArgs  = (12345, [54321, 67890], b'\x01\x02\x03\x04\x05')
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()




paramTypes = ('uint64', '(uint64,uint64)', 'bytes5')
paramArgs  = (12345, (54321, 67890), b'\x01\x02\x03\x04\x05')
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()



paramTypes = ('uint64', '(uint64,bytes)', 'bytes5')
paramArgs  = (12345, (54321, b'\x01\x02\x03\x04\x05'), b'\x01\x02\x03\x04\x05')
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()



paramTypes = ('uint64', '(uint64,(uint64,bytes))', 'bytes5')
paramArgs  = (12345, (54321, (67890, b'\x01\x02\x03\x04\x05')), b'\x01\x02\x03\x04\x05')
encoded = w3.codec.encode(paramTypes, paramArgs)
print(paramTypes)
print(FormatBytes(encoded))
print()



testContract1 = w3.eth.contract(address=TEST_CONTRACT_ADDR_1, abi=TEST_ABI_1)
paramArgs  = [12345, (54321, 67890), b'\x01\x02\x03\x04\x05']
executable = testContract1.functions['foo'](*paramArgs)
msg = {
	"type": 2,
	"gas": 100000,
	"maxFeePerGas": 2000000000,
	"maxPriorityFeePerGas": 2000000000,
	"nonce": 34,
	"value": 0,
	"chainId": 1900,
}
tx = executable.build_transaction(msg)
signedTx = w3.eth.account.sign_transaction(tx, TEST_PRIVATE_KEY_1)
print('rawTransaction:')
print(FormatBytes(signedTx.rawTransaction))
print()
# w3.eth.send_raw_transaction(signedTx)



testContract1 = w3.eth.contract(address=TEST_CONTRACT_ADDR_1, abi=TEST_ABI_1)
paramArgs  = [12345, (54321, (67890, b'\x01\x02\x03\x04\x05')), b'\x01\x02\x03\x04\x05']
executable = testContract1.functions['bar'](*paramArgs)
msg = {
	"type": 2,
	"gas": 100000,
	"maxFeePerGas": 2000000000,
	"maxPriorityFeePerGas": 2000000000,
	"nonce": 34,
	"value": 1000,
	"chainId": 1900,
}
tx = executable.build_transaction(msg)
signedTx = w3.eth.account.sign_transaction(tx, TEST_PRIVATE_KEY_2)
print('rawTransaction:')
print(FormatBytes(signedTx.rawTransaction))
print()

