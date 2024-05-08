#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Copyright (c) 2023 Haofan Zheng
# Use of this source code is governed by an MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT.
###


import argparse
import rlp

from typing import Tuple, Union


def ReadAndParseRlpFile(rlpFile: str) -> Tuple[bytes, Union[list, bytes]]:
	with open(rlpFile, 'rb') as f:
		rawRlp = f.read()
		rlpObj = rlp.decode(rawRlp)
		return rawRlp, rlpObj


def RlpToStrCppStyle(
	rlpObj: Union[list, bytes],
	indent: int = 0,
	indentFactor: int = 0,
	parentIndex: int = 0,
) -> str:
	currIndentStr = ' ' * indentFactor * indent
	nextIndentStr = ' ' * (indentFactor + 1) * indent

	if parentIndex > 0:
		parentIndexStr = '{:02}.'.format(parentIndex)
	else:
		parentIndexStr = ''

	s = ''

	if isinstance(rlpObj, list):
		itemIndex = 1

		s += currIndentStr + '{\n'
		for x in rlpObj:
			s += '{}// {}{:02}.\n'.format(nextIndentStr, parentIndexStr, itemIndex)
			s += RlpToStrCppStyle(x, indent, indentFactor + 1, itemIndex)
			itemIndex += 1
		s += currIndentStr + '},\n'

		return s
	else:
		i = 0

		s += currIndentStr + '{\n'

		if len(rlpObj) > 0:
			s += nextIndentStr

		for x in rlpObj:
			s += '{:#04X}U, '.format(x)
			if i % 16 == 15 and i != len(rlpObj) - 1:
				s += '\n' + nextIndentStr
			i += 1

		if len(rlpObj) > 0:
			s += '\n'

		s += currIndentStr + '},\n'

		return s


def main():
	argParser = argparse.ArgumentParser()
	argParser.add_argument(
		'--rlp-file',
		type=str,
		required=True,
		help='The path to the RLP file to be parsed'
	)
	argParser.add_argument(
		'--raw',
		action='store_true',
		help='If set, the raw data will also be printed'
	)
	args = argParser.parse_args()

	rawRlp, rlpObj = ReadAndParseRlpFile(args.rlp_file)

	if args.raw:
		print('Raw RLP data:')
		print(RlpToStrCppStyle(rawRlp, 4, 0))

	print('Parsed RLP data:')
	print(RlpToStrCppStyle(rlpObj, 4))


if __name__ == '__main__':
	main()
