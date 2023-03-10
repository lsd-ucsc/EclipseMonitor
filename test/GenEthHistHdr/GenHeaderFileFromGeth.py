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

from typing import List


def GetHeaderRlpHex(blkNum: int, gethAddr: str, gethPort: int) -> List[str]:

	#  curl "http://127.0.0.1:8545/" -X POST
	#      -H "Content-Type: application/json"
	#      --data '{
	#                   "method":"debug_getHeaderRlp",
	#                   "params":[15209997],
	#                   "id":1,
	#                   "jsonrpc":"2.0"
	#              }'

	url = 'http://{}:{}'.format(gethAddr, gethPort)
	payload = {
		'method': 'debug_getHeaderRlp',
		'params': [blkNum],
		'id': 1,
		'jsonrpc': '2.0'
	}
	headers = {'Content-Type': 'application/json'}
	r = requests.post(url, json=payload, headers=headers)
	if r.status_code != 200:
		raise RuntimeError('Failed to get header from Geth Client')
	rJson = r.json()
	if 'result' not in rJson:
		raise RuntimeError('Failed to get header from Geth Client')

	return rJson['result']


def UnhexlifyHeader(header: str) -> bytes:
	# need to remove the first two characters '0x'
	return binascii.unhexlify(header[2:])


def main():
	argsParser = argparse.ArgumentParser()
	argsParser.add_argument(
		'--blk-num', type=int, required=True,
		help='block number'
	)
	argsParser.add_argument(
		'--geth-addr', type=str, required=False, default='localhost',
		help='host address to Geth Client'
	)
	argsParser.add_argument(
		'--geth-port', type=int, required=False, default=8545,
		help='port number to Geth Client'
	)
	argsParser.add_argument(
		'--output', type=str, required=True,
		help='output file'
	)
	args = argsParser.parse_args()

	headerRlp = GetHeaderRlpHex(args.blk_num, args.geth_addr, args.geth_port)
	headerRlp = UnhexlifyHeader(headerRlp)

	with open(args.output, 'wb') as f:
		f.write(headerRlp)


if __name__ == '__main__':
	main()
