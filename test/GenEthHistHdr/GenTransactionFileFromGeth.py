#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Copyright (c) 2023 Haofan Zheng
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.
###


import argparse
import binascii
import requests
import rlp

from typing import List, Union


def GetBlockRlpHex(blkNum: int, gethAddr: str, gethPort: int) -> List[str]:
	#  curl "http://127.0.0.1:8545/" -X POST
	#      -H "Content-Type: application/json"
	#      --data '{
	#                   "method":"debug_getRawBlock",
	#                   "params":[15209997],
	#                   "id":1,
	#                   "jsonrpc":"2.0"
	#              }'

	blkNumHex = hex(blkNum)

	url = 'http://{}:{}'.format(gethAddr, gethPort)
	payload = {
		'method': 'debug_getRawBlock',
		'params': [blkNumHex],
		'id': 1,
		'jsonrpc': '2.0'
	}
	headers = {'Content-Type': 'application/json'}
	r = requests.post(url, json=payload, headers=headers)
	if r.status_code != 200:
		raise RuntimeError('Failed to get block from Geth Client')
	rJson = r.json()
	if 'result' not in rJson:
		raise RuntimeError('Failed to get block from Geth Client')

	return rJson['result']


def GetTransactionsFromBlock(blockRlpHex: str) -> List[Union[bytes, list]]:
	blockRlp = binascii.unhexlify(blockRlpHex[2:])
	block = rlp.decode(blockRlp)

	# block[0] is the block header
	# block[1] is the list of transactions
	# block[2] is the list of uncles
	return block[1]


def AllTransactionsToBytes(txns: List[Union[bytes, list]]) -> List[bytes]:
	return [
		rlp.encode(txn) if not isinstance(txn, bytes) else txn
		for txn in txns
	]


def main():
	defaultGethAddr = 'localhost'
	defaultGethPort = 8545

	argsParser = argparse.ArgumentParser()
	argsParser.add_argument(
		'--blk-num', type=int, required=True,
		help='block number'
	)
	argsParser.add_argument(
		'--geth-addr', type=str, required=False, default=defaultGethAddr,
		help='host address to Geth Client'
	)
	argsParser.add_argument(
		'--geth-port', type=int, required=False, default=defaultGethPort,
		help='port number to Geth Client'
	)
	argsParser.add_argument(
		'--output', type=str, required=True,
		help='output file'
	)
	args = argsParser.parse_args()

	blockRlpHex = GetBlockRlpHex(args.blk_num, args.geth_addr, args.geth_port)
	# blockRlpHex = GetBlockRlpHex(15415840, defaultGethAddr, defaultGethPort)
	# blockRlpHex = GetBlockRlpHex(15209997, defaultGethAddr, defaultGethPort)

	txns = GetTransactionsFromBlock(blockRlpHex)
	txns = AllTransactionsToBytes(txns)
	txnsRlp = rlp.encode(txns)

	with open(args.output, 'wb') as f:
		f.write(txnsRlp)


if __name__ == '__main__':
	main()
