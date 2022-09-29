#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Copyright (c) 2022 Haofan Zheng
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.
###

import sys
sys.path.append('/home/ubuntu/codelab/GethDBReader')

from typing import List

from GethDBReader import GethDB


HEADER_RANGE = [0, 100]

OUTPUT_FILE = '../src/EthHistHdr_{}_{}.cpp'.format(
	HEADER_RANGE[0], HEADER_RANGE[1])

OUTPUT_FILE_PREFIX = '''
#include <cstdint>
#include <vector>

namespace EclipseMonitor_Test
{
'''

OUTPUT_FILE_SUFFIX = '''
} // namespace EclipseMonitor_Test
'''

VAR_NAME  = 'ethHistHdr_{}_{}'.format(HEADER_RANGE[0], HEADER_RANGE[1])
FUNC_NAME = 'GetEthHistHdr_{}_{}'.format(HEADER_RANGE[0], HEADER_RANGE[1])


def Save2File(lines: List[str]) -> None:
	with open(OUTPUT_FILE, 'w') as f:
		f.write(OUTPUT_FILE_PREFIX)
		for line in lines:
			f.write(line + '\n')
		f.write(OUTPUT_FILE_SUFFIX)


def GenHdrBinBlock(h: bytes) -> List[str]:
	MAX_ITEMS_PER_LINE = 16

	s = ["{:#04X}U".format(x) for x in h]
	i = 1
	line = ''
	res = []
	for x in s:
			line += (x + ', ')
			if i % MAX_ITEMS_PER_LINE == 0:
				res.append(line.strip())
				line = ''
			i += 1

	if line != '':
		res.append(line.strip())
	res.append('// {} bytes'.format(len(h)))

	return res


def AddBraceScope(lines: List[str]) -> List[str]:
	res = []
	res.append('{')
	for line in lines:
		res.append('\t' + line)
	res.append('}')
	return res


def SubVectorInit(lines: List[str]) -> List[str]:
	lines[0] = 'std::vector<uint8_t>(' + lines[0]
	lines[-1] = lines[-1] + '),'
	return lines


def RootVectorInit(lines: List[str]) -> List[str]:
	lines = AddBraceScope(lines)
	lines[0] = (
		'static ' +
		'const ' +
		'std::vector<std::vector<uint8_t> > {} = '.format(VAR_NAME) +
		lines[0])
	lines[-1] = lines[-1] + ';'
	return lines


def WrapInFunction(lines: List[str]) -> List[str]:
	res = []
	res.append(
		'const std::vector<std::vector<uint8_t> >& {}()'.format(FUNC_NAME))
	res += AddBraceScope(lines)
	res.append('\treturn {};'.format(VAR_NAME))
	res[-1], res[-2] = res[-2], res[-1]
	return res


db = GethDB.GethDB('', '/home/public/geth-ancient/')

lines = []
for i in range(HEADER_RANGE[0], HEADER_RANGE[1]):
	h = db.freezerdb.tables['headers'].Retrieve(i)
	blk = GenHdrBinBlock(h)
	blk = AddBraceScope(blk)
	blk = SubVectorInit(blk)
	blk = ['// Header {}'.format(i)] + blk
	lines += blk

lines = RootVectorInit(lines)
lines = WrapInFunction(lines)
Save2File(lines)
