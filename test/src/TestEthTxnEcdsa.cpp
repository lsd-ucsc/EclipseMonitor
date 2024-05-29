// Copyright (c) 2024 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#include <gtest/gtest.h>

#include <EclipseMonitor/Eth/Transaction/Ecdsa.hpp>
#include <mbedTLScpp/EcKey.hpp>

#include "Common.hpp"


namespace EclipseMonitor_Test
{
	extern size_t g_numOfTestFile;
}

using namespace EclipseMonitor_Test;

using namespace EclipseMonitor::Eth;


GTEST_TEST(TestEthTxnEcdsa, CountTestFile)
{
	static auto tmp = ++EclipseMonitor_Test::g_numOfTestFile;
	(void)tmp;
}


static const mbedTLScpp::EcGroup<> sk_secp256k1(mbedTLScpp::EcType::SECP256K1);


GTEST_TEST(TestEthTxnEcdsa, JacobianInv)
{
	{
		mbedTLScpp::BigNum a(0);
		mbedTLScpp::BigNum n(1);

		mbedTLScpp::BigNum expOut(0);

		mbedTLScpp::BigNum out = Transaction::JacobianInv(a, n);

		EXPECT_EQ(out, expOut);
	}

	{
		mbedTLScpp::BigNum a("99894079990873359630376909829698352524809824171660155898784064965080081534450");
		mbedTLScpp::BigNum n("115792089237316195423570985008687907852837564279074904382605163141518161494337");

		mbedTLScpp::BigNum expOut("48205471765048878769112722906746240055246381653081666392697376400160573977909");

		mbedTLScpp::BigNum out = Transaction::JacobianInv(a, n);

		EXPECT_EQ(out, expOut);
	}

	{
		mbedTLScpp::BigNum a("4755893456204814032994661987161704419906856408563089114860023487200879314920");
		mbedTLScpp::BigNum n("115792089237316195423570985008687907852837564279074904382605163141518161494337");

		mbedTLScpp::BigNum expOut("8366362605996906969732918924814779820613675455983702332239682356052933443685");

		mbedTLScpp::BigNum out = Transaction::JacobianInv(a, n);

		EXPECT_EQ(out, expOut);
	}
}


GTEST_TEST(TestEthTxnEcdsa, JacobianDouble)
{
	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> p = std::make_tuple(
			mbedTLScpp::BigNum(100),
			mbedTLScpp::BigNum(0),
			mbedTLScpp::BigNum(100)
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum(0),
			mbedTLScpp::BigNum(0),
			mbedTLScpp::BigNum(0)
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianDouble(
			p,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}

	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> p = std::make_tuple(
			mbedTLScpp::BigNum("36565028958672047985467739768252663549060054249238279599776773157982456747388"),
			mbedTLScpp::BigNum("26794385526983480225932345657229967401317403296218116044243872393481628346196"),
			mbedTLScpp::BigNum("87336916788907964076621474987505410415813069772338947919355744021760658771529")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("36036075634199074546129661232420548016519448468337978806663212807452641890103"),
			mbedTLScpp::BigNum("113492477844519581031850584107650655330363714502423581674624945975139107167263"),
			mbedTLScpp::BigNum("91707237065990426039921931734295629131137542276573823064360208304576683970847")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianDouble(
			p,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}

	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> p = std::make_tuple(
			mbedTLScpp::BigNum("14861663377811509140551858067315276845464282493407987296353092997784687167314"),
			mbedTLScpp::BigNum("39137420299569502647765350355455095302255521025456411443562043337254407045840"),
			mbedTLScpp::BigNum("40099571553575564002937172146104371577379850074239136598504885595987528294846")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("63575194886187257535938157704972707470722922421800391784521628407589249322769"),
			mbedTLScpp::BigNum("40161198225766027124270551863076939477305469833559547838035156868151083466459"),
			mbedTLScpp::BigNum("87891979409671700349269413343649637404628212098163418479237423976283077679673")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianDouble(
			p,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}
}


GTEST_TEST(TestEthTxnEcdsa, JacobianAdd)
{
	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> p = std::make_tuple(
			mbedTLScpp::BigNum("34832002147657580523623959555951687751971572118948814154019390413440244237266"),
			mbedTLScpp::BigNum("10575639833043288494724950414056322299517027438414803915230266464097535610696"),
			mbedTLScpp::BigNum("64471558224392339448117885746082193748654986342107253452511469024820282463724")
		);
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> q = std::make_tuple(
			mbedTLScpp::BigNum("55066263022277343669578718895168534326250603453777594175500187360389116729240"),
			mbedTLScpp::BigNum("32670510020758816978083085130507043184471273380659243275938904335757337482424"),
			mbedTLScpp::BigNum(1)
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("36565028958672047985467739768252663549060054249238279599776773157982456747388"),
			mbedTLScpp::BigNum("26794385526983480225932345657229967401317403296218116044243872393481628346196"),
			mbedTLScpp::BigNum("87336916788907964076621474987505410415813069772338947919355744021760658771529")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianAdd(
			p,
			q,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}

	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> p = std::make_tuple(
			mbedTLScpp::BigNum("81852845867168905301505054245430925093078919361432342426470976391340413601139"),
			mbedTLScpp::BigNum("44338445320234977458987797881500440713217305413439258886338627240901591305666"),
			mbedTLScpp::BigNum("47594349659260652877357148834108997596577843090446296597453723945805718117563")
		);
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> q = std::make_tuple(
			mbedTLScpp::BigNum("55066263022277343669578718895168534326250603453777594175500187360389116729240"),
			mbedTLScpp::BigNum("32670510020758816978083085130507043184471273380659243275938904335757337482424"),
			mbedTLScpp::BigNum(1)
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("40448055496287309044869980056118293952322637777137096262082128484131334177985"),
			mbedTLScpp::BigNum("103296625191508965494948037293515077505697237578720143244847055492794393537747"),
			mbedTLScpp::BigNum("63820715475438429361341171002329409693455152442280053906972735224332669284210")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianAdd(
			p,
			q,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}
}


GTEST_TEST(TestEthTxnEcdsa, JacobianMultiply)
{
	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> a = std::make_tuple(
			mbedTLScpp::BigNum("55066263022277343669578718895168534326250603453777594175500187360389116729240"),
			mbedTLScpp::BigNum("32670510020758816978083085130507043184471273380659243275938904335757337482424"),
			mbedTLScpp::BigNum(1)
		);

		mbedTLScpp::BigNum num("99894079990873359630376909829698352524809824171660155898784064965080081534450");

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("36036075634199074546129661232420548016519448468337978806663212807452641890103"),
			mbedTLScpp::BigNum("113492477844519581031850584107650655330363714502423581674624945975139107167263"),
			mbedTLScpp::BigNum("91707237065990426039921931734295629131137542276573823064360208304576683970847")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianMultiply(
			std::get<0>(a),
			std::get<1>(a),
			std::get<2>(a),
			num,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowN(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}

	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> a = std::make_tuple(
			mbedTLScpp::BigNum("55066263022277343669578718895168534326250603453777594175500187360389116729240"),
			mbedTLScpp::BigNum("32670510020758816978083085130507043184471273380659243275938904335757337482424"),
			mbedTLScpp::BigNum(1)
		);

		mbedTLScpp::BigNum num("4755893456204814032994661987161704419906856408563089114860023487200879314920");

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("63575194886187257535938157704972707470722922421800391784521628407589249322769"),
			mbedTLScpp::BigNum("40161198225766027124270551863076939477305469833559547838035156868151083466459"),
			mbedTLScpp::BigNum("87891979409671700349269413343649637404628212098163418479237423976283077679673")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianMultiply(
			std::get<0>(a),
			std::get<1>(a),
			std::get<2>(a),
			num,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowN(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}
}


GTEST_TEST(TestEthTxnEcdsa, JacobianFastMultiply)
{
	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> a = std::make_tuple(
			mbedTLScpp::BigNum("55066263022277343669578718895168534326250603453777594175500187360389116729240"),
			mbedTLScpp::BigNum("32670510020758816978083085130507043184471273380659243275938904335757337482424")
		);

		mbedTLScpp::BigNum num("99894079990873359630376909829698352524809824171660155898784064965080081534450");

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("55398850951096076018653516985588965820355649108029904532973662457952545528338"),
			mbedTLScpp::BigNum("1219865831688556121255156054523597135592503170307932332484054549583411582505")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianFastMultiply(
			std::get<0>(a),
			std::get<1>(a),
			num,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowN(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
	}

	{
		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> a = std::make_tuple(
			mbedTLScpp::BigNum("55066263022277343669578718895168534326250603453777594175500187360389116729240"),
			mbedTLScpp::BigNum("32670510020758816978083085130507043184471273380659243275938904335757337482424")
		);

		mbedTLScpp::BigNum num("4755893456204814032994661987161704419906856408563089114860023487200879314920");

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			mbedTLScpp::BigNum("57531915739949951657140584335392702729792194311398064976436476590597615371172"),
			mbedTLScpp::BigNum("74532461135409732490675357031953695959767481587610106665841628494200677277931")
		);

		std::tuple<
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::JacobianFastMultiply(
			std::get<0>(a),
			std::get<1>(a),
			num,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowN(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
	}
}


GTEST_TEST(TestEthTxnEcdsa, DeterministicGenerateK)
{
	std::vector<uint8_t> hash = {
		0x7b, 0xf0, 0x42, 0x15, 0xd5, 0xbc, 0x74, 0x80, 0x96, 0x40, 0xa0, 0xec, 0xd1, 0x11, 0xc5, 0x5e,
		0x7d, 0x07, 0xed, 0x14, 0xe4, 0xdb, 0x4c, 0x6e, 0x82, 0x03, 0x6d, 0x2c, 0xe8, 0x90, 0x54, 0x2e,
	};

	{
		mbedTLScpp::BigNum key("34390819888240390953029010971248455142986221947941601700148038000926730363672");
		auto keyBytes = key.Bytes</*littleEndian=*/false>();

		mbedTLScpp::BigNum expOut("99894079990873359630376909829698352524809824171660155898784064965080081534450");

		mbedTLScpp::BigNum out = Transaction::DeterministicGenerateK(
			mbedTLScpp::CtnFullR(hash),
			mbedTLScpp::CtnFullR(keyBytes)
		);

		EXPECT_EQ(out, expOut);
	}

	{
		mbedTLScpp::BigNum key("62878277211082814447969588726438965043493409034528277085854515572569382032315");
		auto keyBytes = key.Bytes</*littleEndian=*/false>();

		mbedTLScpp::BigNum expOut("4755893456204814032994661987161704419906856408563089114860023487200879314920");

		mbedTLScpp::BigNum out = Transaction::DeterministicGenerateK(
			mbedTLScpp::CtnFullR(hash),
			mbedTLScpp::CtnFullR(keyBytes)
		);

		EXPECT_EQ(out, expOut);
	}
}


GTEST_TEST(TestEthTxnEcdsa, EcdsaRawSign)
{
	std::vector<uint8_t> hash = {
		0x7b, 0xf0, 0x42, 0x15, 0xd5, 0xbc, 0x74, 0x80, 0x96, 0x40, 0xa0, 0xec, 0xd1, 0x11, 0xc5, 0x5e,
		0x7d, 0x07, 0xed, 0x14, 0xe4, 0xdb, 0x4c, 0x6e, 0x82, 0x03, 0x6d, 0x2c, 0xe8, 0x90, 0x54, 0x2e,
	};

	{
		mbedTLScpp::BigNum key("34390819888240390953029010971248455142986221947941601700148038000926730363672");

		std::tuple<
			uint8_t,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			1,
			mbedTLScpp::BigNum("55398850951096076018653516985588965820355649108029904532973662457952545528338"),
			mbedTLScpp::BigNum("42247689232515303603446946966523974649153981729397734462495890121129406260483")
		);

		std::tuple<
			uint8_t,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::EcdsaRawSign(
			mbedTLScpp::CtnFullR(hash),
			key,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowGx(),
			sk_secp256k1.BorrowGy(),
			sk_secp256k1.BorrowN(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}

	{
		mbedTLScpp::BigNum key("62878277211082814447969588726438965043493409034528277085854515572569382032315");

		std::tuple<
			uint8_t,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> expOut = std::make_tuple(
			0,
			mbedTLScpp::BigNum("57531915739949951657140584335392702729792194311398064976436476590597615371172"),
			mbedTLScpp::BigNum("52996200849355888973535696918874003680055380398586210701928882732133694725751")
		);

		std::tuple<
			uint8_t,
			mbedTLScpp::BigNum,
			mbedTLScpp::BigNum
		> out = Transaction::EcdsaRawSign(
			mbedTLScpp::CtnFullR(hash),
			key,
			sk_secp256k1.BorrowA(),
			sk_secp256k1.BorrowGx(),
			sk_secp256k1.BorrowGy(),
			sk_secp256k1.BorrowN(),
			sk_secp256k1.BorrowP()
		);

		EXPECT_EQ(std::get<0>(out), std::get<0>(expOut));
		EXPECT_EQ(std::get<1>(out), std::get<1>(expOut));
		EXPECT_EQ(std::get<2>(out), std::get<2>(expOut));
	}
}

