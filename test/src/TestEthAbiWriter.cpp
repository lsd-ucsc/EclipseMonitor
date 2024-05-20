// Copyright (c) 2022 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <cstdint>

#include <vector>

#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/AbiWriter.hpp>

#include "Common.hpp"


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;
using namespace EclipseMonitor::Eth;


GTEST_TEST(TestEthAbiWriter, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}


template<typename _WriterImpl, typename _ValType>
static void TestWriterImpl(
	const _WriterImpl& writerImpl,
	const std::vector<uint8_t>& expOutput,
	const _ValType& inVal
)
{
	std::vector<uint8_t> output;
	auto destIt = writerImpl.Write(std::back_inserter(output), inVal);

	EXPECT_EQ(output, expOutput);

	// ensures that the destination iterator is at the last position
	*destIt++ = 0x56U;
	EXPECT_EQ(output.size(), expOutput.size() + 1);
	EXPECT_EQ(output[output.size() - 1], 0x56U);
}


template<typename _WriterImpl, typename _SrcIt>
static void TestWriterImpl(
	const _WriterImpl& writerImpl,
	const std::vector<uint8_t>& expOutput,
	_SrcIt begin,
	_SrcIt end
)
{
	std::vector<uint8_t> output;
	auto destIt = writerImpl.Write(std::back_inserter(output), begin, end);

	EXPECT_EQ(output, expOutput);

	// ensures that the destination iterator is at the last position
	*destIt++ = 0x56U;
	EXPECT_EQ(output.size(), expOutput.size() + 1);
	EXPECT_EQ(output[output.size() - 1], 0x56U);
}


template<typename _Writer>
static void TestObjWriter(
	const _Writer& writer,
	const SimpleObjects::BaseObj& obj,
	const std::vector<uint8_t>& expHeadOutput,
	const std::vector<uint8_t>& expTailOutput,
	size_t expDataOffsetIncr = 0
)
{
	std::vector<uint8_t> head;
	std::vector<uint8_t> tail;

	size_t inDataOffset = 0x80U;
	size_t resDataOffset = 0;

	auto headDestIt = SimpleObjects::ToOutIt<uint8_t>(std::back_inserter(head));
	std::tie(headDestIt, resDataOffset) =
		writer.WriteHead(headDestIt, obj, inDataOffset);

	EXPECT_EQ(head, expHeadOutput);
	EXPECT_EQ(resDataOffset, inDataOffset + expDataOffsetIncr);

	auto tailDestIt = SimpleObjects::ToOutIt<uint8_t>(std::back_inserter(tail));
	tailDestIt = writer.WriteTail(tailDestIt, obj);

	EXPECT_EQ(tail, expTailOutput);

	// ensures that the destination iterator is at the last position
	*headDestIt++ = 0x56U;
	EXPECT_EQ(head.size(), expHeadOutput.size() + 1);
	EXPECT_EQ(head[head.size() - 1], 0x56U);

	*tailDestIt++ = 0x78U;
	EXPECT_EQ(tail.size(), expTailOutput.size() + 1);
	EXPECT_EQ(tail[tail.size() - 1], 0x78U);
}


GTEST_TEST(TestEthAbiWriter, WriteIntegerImpl)
{
	using AbiWriterImplUInt64 = EthInternal::AbiWriterImpl<
		SimpleObjects::ObjCategory::Integer,
		AbiUInt64
	>;
	using AbiWriterUInt64 = AbiWriter<
		SimpleObjects::ObjCategory::Integer,
		AbiUInt64
	>;

	EXPECT_EQ(AbiWriterImplUInt64().IsDynamicType(), false);
	EXPECT_EQ(AbiWriterImplUInt64().GetNumHeadChunks(), 1);
	EXPECT_EQ(AbiWriterImplUInt64().GetNumTailChunks(), 0);

	EXPECT_EQ(AbiWriterUInt64().IsDynamicType(), false);
	EXPECT_EQ(AbiWriterUInt64().GetNumHeadChunks(), 1);
	EXPECT_EQ(AbiWriterUInt64().GetNumTailChunks(SimpleObjects::UInt64()), 0);

	{
		std::vector<uint8_t> expOutput = {
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x12U, 0x34U, 0x56U, 0x78U, 0x90U, 0xABU, 0xCDU, 0xEFU,
		};
		uint64_t inVal(0x1234567890ABCDEFULL);
		SimpleObjects::UInt64 obj(inVal);

		TestWriterImpl(AbiWriterImplUInt64(), expOutput, inVal);
		TestObjWriter(AbiWriterUInt64(), obj, expOutput, {});
	}
}


GTEST_TEST(TestEthAbiWriter, WriteBoolImpl)
{
	using AbiWriterImplBool = EthInternal::AbiWriterImpl<
		SimpleObjects::ObjCategory::Bool
	>;
	using AbiWriterBool = AbiWriter<
		SimpleObjects::ObjCategory::Bool
	>;

	EXPECT_EQ(AbiWriterImplBool().IsDynamicType(), false);
	EXPECT_EQ(AbiWriterImplBool().GetNumHeadChunks(), 1);
	EXPECT_EQ(AbiWriterImplBool().GetNumTailChunks(), 0);

	EXPECT_EQ(AbiWriterBool().IsDynamicType(), false);
	EXPECT_EQ(AbiWriterBool().GetNumHeadChunks(), 1);
	EXPECT_EQ(AbiWriterBool().GetNumTailChunks(SimpleObjects::Bool()), 0);

	{
		std::vector<uint8_t> expOutput = {
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U,
		};
		bool inVal(true);
		SimpleObjects::Bool obj(inVal);

		TestWriterImpl(AbiWriterImplBool(), expOutput, inVal);
		TestObjWriter(AbiWriterBool(), obj, expOutput, {});
	}

	{
		std::vector<uint8_t> expOutput = {
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		};
		bool inVal(false);
		SimpleObjects::Bool obj(inVal);

		TestWriterImpl(AbiWriterImplBool(), expOutput, inVal);
		TestObjWriter(AbiWriterBool(), obj, expOutput, {});
	}
}


GTEST_TEST(TestEthAbiWriter, WriteStaticBytesImpl)
{
	using AbiWriterImplBytes = EthInternal::AbiWriterImpl<
		SimpleObjects::ObjCategory::Bytes,
		std::false_type
	>;
	using AbiWriterBytes = AbiWriter<
		SimpleObjects::ObjCategory::Bytes,
		std::false_type
	>;
	using AbiWriterBytes16 = AbiWriter<
		SimpleObjects::ObjCategory::Bytes,
		AbiSize<16>
	>;

	{
		AbiWriterImplBytes writerImpl(16);
		AbiWriterBytes16 writer;

		EXPECT_EQ(writerImpl.IsDynamicType(), false);
		EXPECT_EQ(writerImpl.GetNumHeadChunks(), 1);
		EXPECT_EQ(writerImpl.GetNumTailChunks(), 0);
		EXPECT_EQ(writerImpl.GetPadSize(), 16);

		EXPECT_EQ(writer.IsDynamicType(), false);
		EXPECT_EQ(writer.GetNumHeadChunks(), 1);
		EXPECT_EQ(writer.GetNumTailChunks(SimpleObjects::Bytes()), 0);

		std::vector<uint8_t> expOutput = {
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		};
		SimpleObjects::Bytes inVal = {
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
		};

		TestWriterImpl(writerImpl, expOutput, inVal.AsBytes());
		TestObjWriter(writer, inVal, expOutput, {});
	}

	{
		AbiWriterImplBytes writerImpl(32);
		AbiWriterBytes writer(32);

		EXPECT_EQ(writerImpl.IsDynamicType(), false);
		EXPECT_EQ(writerImpl.GetNumHeadChunks(), 1);
		EXPECT_EQ(writerImpl.GetNumTailChunks(), 0);
		EXPECT_EQ(writerImpl.GetPadSize(), 0);

		EXPECT_EQ(writer.IsDynamicType(), false);
		EXPECT_EQ(writer.GetNumHeadChunks(), 1);
		EXPECT_EQ(writer.GetNumTailChunks(SimpleObjects::Bytes()), 0);

		std::vector<uint8_t> expOutput = {
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
		};
		SimpleObjects::Bytes inVal = {
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
		};

		TestWriterImpl(writerImpl, expOutput, inVal.AsBytes());
		TestObjWriter(writer, inVal, expOutput, {});
	}

	// The given bytes length doesn't match the static length
	{
		AbiWriterImplBytes writerImpl(16);

		std::vector<uint8_t> output;
		SimpleObjects::Bytes inVal = {
			0x01U, 0x23U, 0x45U, 0x67U,
		};
		EXPECT_THROW_MSG(
			writerImpl.Write(std::back_inserter(output), inVal.AsBytes());,
			EclipseMonitor::Exception,
			"ABI writer - the given bytes should have exactly the same "
				"length as the static bytes type"
		);
	}

	// writer with static length longer than the chunk size
	{
		EXPECT_THROW(
			AbiWriterBytes writer(33),
			EclipseMonitor::Exception
		);
	}
}


GTEST_TEST(TestEthAbiWriter, WriteDynamicBytesImpl)
{
	using AbiWriterImplBytes = EthInternal::AbiWriterImpl<
		SimpleObjects::ObjCategory::Bytes,
		std::true_type
	>;

	AbiWriterImplBytes writerImpl;

	EXPECT_EQ(writerImpl.IsDynamicType(), true);

	EXPECT_EQ(writerImpl.GetNumHeadChunks(), 1);

	EXPECT_EQ(writerImpl.GetNumDataChunks(0), 0);
	EXPECT_EQ(writerImpl.GetNumDataChunks(10), 1);
	EXPECT_EQ(writerImpl.GetNumDataChunks(16), 1);
	EXPECT_EQ(writerImpl.GetNumDataChunks(32), 1);
	EXPECT_EQ(writerImpl.GetNumDataChunks(33), 2);
	EXPECT_EQ(writerImpl.GetNumDataChunks(45), 2);
	EXPECT_EQ(writerImpl.GetNumDataChunks(64), 2);
	EXPECT_EQ(writerImpl.GetNumDataChunks(65), 3);

	EXPECT_EQ(writerImpl.GetNumTailChunks(0), 1);
	EXPECT_EQ(writerImpl.GetNumTailChunks(10), 2);
	EXPECT_EQ(writerImpl.GetNumTailChunks(16), 2);
	EXPECT_EQ(writerImpl.GetNumTailChunks(32), 2);
	EXPECT_EQ(writerImpl.GetNumTailChunks(33), 3);
	EXPECT_EQ(writerImpl.GetNumTailChunks(45), 3);
	EXPECT_EQ(writerImpl.GetNumTailChunks(64), 3);
	EXPECT_EQ(writerImpl.GetNumTailChunks(65), 4);

	EXPECT_EQ(writerImpl.GetPadSize(0), 0);
	EXPECT_EQ(writerImpl.GetPadSize(10), 22);
	EXPECT_EQ(writerImpl.GetPadSize(16), 16);
	EXPECT_EQ(writerImpl.GetPadSize(32), 0);
	EXPECT_EQ(writerImpl.GetPadSize(33), 31);
	EXPECT_EQ(writerImpl.GetPadSize(45), 19);
	EXPECT_EQ(writerImpl.GetPadSize(64), 0);
	EXPECT_EQ(writerImpl.GetPadSize(65), 31);

	{
		std::vector<uint8_t> expOutput = {
			// len
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x34U,
			// data
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			// data - continue
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x89U, 0xABU, 0xCDU, 0xEFU, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		};
		SimpleObjects::Bytes inVal = {
			// data
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			// data - continue
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x89U, 0xABU, 0xCDU, 0xEFU,
		};

		TestWriterImpl(writerImpl, expOutput, inVal.AsBytes());
		TestWriterImpl(
			writerImpl,
			expOutput,
			inVal.AsBytes().begin(),
			inVal.AsBytes().end()
		);
	}

	{
		std::vector<uint8_t> expOutput = {
			// len
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x40U,
			// data
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			// data - continue
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x89U, 0xABU, 0xCDU, 0xEFU, 0x01U, 0x23U, 0x45U, 0x67U,
			0x67U, 0x45U, 0x23U, 0x01U, 0xEFU, 0xCDU, 0xABU, 0x89U,
		};
		SimpleObjects::Bytes inVal = {
			// data
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			// data - continue
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x89U, 0xABU, 0xCDU, 0xEFU, 0x01U, 0x23U, 0x45U, 0x67U,
			0x67U, 0x45U, 0x23U, 0x01U, 0xEFU, 0xCDU, 0xABU, 0x89U,
		};

		TestWriterImpl(writerImpl, expOutput, inVal.AsBytes());
		TestWriterImpl(
			writerImpl,
			expOutput,
			inVal.AsBytes().begin(),
			inVal.AsBytes().end()
		);
	}

	// The given input iterator range is invalid
	{
		std::vector<uint8_t> output;
		SimpleObjects::Bytes inVal = {
			// data
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,
		};
		EXPECT_THROW_MSG(
			writerImpl.Write(
				std::back_inserter(output),
				inVal.AsBytes().end(),
				inVal.AsBytes().begin()
			);,
			EclipseMonitor::Exception,
			"ABI writer - invalid iterator range for input bytes"
		);
	}
}


GTEST_TEST(TestEthAbiWriter, WriteStaticListImpl)
{
	using AbiWriterImplUInt64 = EthInternal::AbiWriterImpl<
		SimpleObjects::ObjCategory::Integer,
		AbiUInt64
	>;
	using AbiWriterImplUInt64List = EthInternal::AbiWriterImpl<
		SimpleObjects::ObjCategory::List,
		AbiWriterImplUInt64,
		std::false_type,
		std::false_type
	>;

	{
		AbiWriterImplUInt64List writerImpl(AbiWriterImplUInt64(), 5);

		EXPECT_EQ(writerImpl.IsDynamicType(), false);

		EXPECT_EQ(writerImpl.GetNumHeadChunks(), 5);
		EXPECT_EQ(writerImpl.GetNumTailChunks(), 0);

		std::vector<uint8_t> expOutput = {
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x12U, 0x34U, 0x56U, 0x78U, 0x90U, 0xABU, 0xCDU, 0xEFU,

			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0xEFU, 0xCDU, 0xABU, 0x89U, 0x67U, 0x45U, 0x23U, 0x01U,

			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x01U, 0x23U, 0x45U, 0x67U, 0x89U, 0xABU, 0xCDU, 0xEFU,

			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x11U, 0x22U, 0x33U, 0x44U, 0x55U, 0x66U, 0x77U, 0x88U,

			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x99U, 0x00U, 0xAAU, 0xBBU, 0xCCU, 0xDDU, 0xEEU, 0xFFU,
		};
		std::vector<uint64_t> inVal = {
			(0x1234567890ABCDEFULL),
			(0xEFCDAB8967452301ULL),
			(0x0123456789ABCDEFULL),
			(0x1122334455667788ULL),
			(0x9900AABBCCDDEEFFULL),
		};

		TestWriterImpl(writerImpl, expOutput, inVal);
	}

	{
		AbiWriterImplUInt64List writerImpl(AbiWriterImplUInt64(), 2);

		EXPECT_EQ(writerImpl.IsDynamicType(), false);

		EXPECT_EQ(writerImpl.GetNumHeadChunks(), 2);
		EXPECT_EQ(writerImpl.GetNumTailChunks(), 0);

		std::vector<uint8_t> expOutput = {
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x11U, 0x22U, 0x33U, 0x44U, 0x55U, 0x66U, 0x77U, 0x88U,

			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
			0x99U, 0x00U, 0xAAU, 0xBBU, 0xCCU, 0xDDU, 0xEEU, 0xFFU,
		};
		std::vector<uint64_t> inVal = {
			(0x1122334455667788ULL),
			(0x9900AABBCCDDEEFFULL),
		};

		TestWriterImpl(writerImpl, expOutput, inVal);
	}

}

