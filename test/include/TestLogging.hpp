// Copyright (c) 2023 Haofan Zheng
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#pragma once


#include <iostream>
#include <string>


namespace EclipseMonitor_Test
{

class TestingLogger
{
public:

	TestingLogger(const std::string& name) :
		m_name(name)
	{}

	~TestingLogger() = default;

	void Debug(const std::string& msg) const
	{
		return Log("DEBUG", msg);
	}

	void Info(const std::string& msg) const
	{
		return Log("INFO", msg);
	}

	void Warn(const std::string& msg) const
	{
		return Log("WARN", msg);
	}

	void Error(const std::string& msg) const
	{
		return Log("ERROR", msg);
	}

private:

	void Log(const std::string& level, const std::string& msg) const
	{
		std::cout << m_name << "(" << level << "): " << msg << std::endl;
	}

	std::string m_name;
}; // class TestingLogger


struct TestingLoggerFactory
{
	using LoggerType = TestingLogger;

	static LoggerType GetLogger(const std::string& name)
	{
		return LoggerType(name);
	}
}; // struct TestingLoggerFactory


} // namespace EclipseMonitor_Test


namespace EclipseMonitor
{

using LoggerFactory = ::EclipseMonitor_Test::TestingLoggerFactory;

} // namespace EclipseMonitor
