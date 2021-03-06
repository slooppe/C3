#include "stdafx.h"
#include "ArgumentParser.h"

namespace MWR::C3::Linter
{
	ArgumentParser::ArgumentParser(int argc, char** argv) : m_ArgParser()
	{
		ConfigureParser();
		m_ArgParser.parse(argc, argv);
		m_Config = CreateConfig();
	}

	void ArgumentParser::ConfigureParser()
	{
		m_ArgParser.addArgument("-n", "--name", 1, false);
		m_ArgParser.addArgument("-a", "--args", '*');
		m_ArgParser.addArgument("-c", "--complementary", '*');
		m_ArgParser.addArgument("-i", "--test-io");
		m_ArgParser.addArgument("-x", "--command", '+');
	}

	AppConfig const& ArgumentParser::GetConfig() const
	{
		return m_Config;
	}

	std::string ArgumentParser::GetUsage() const
	{
		return m_ArgParser.usage();
	}

	MWR::C3::Linter::AppConfig ArgumentParser::CreateConfig() const
	{
		AppConfig config;
		config.m_ChannelName = m_ArgParser.retrieve<std::string>("name");

		if (m_ArgParser.exists("args"))
			config.m_ChannelArguments = m_ArgParser.retrieve<std::vector<std::string>>("args");

		if (m_ArgParser.exists("complementary"))
			config.m_ComplementaryChannelArguments = m_ArgParser.retrieve<std::vector<std::string>>("complementary");

		config.m_TestChannelIO = m_ArgParser.exists("test-io");

		if (m_ArgParser.exists("command"))
			config.m_Command = m_ArgParser.retrieve<StringVector>("command");

		ValidateConfig();
		return config;
	}

	void ArgumentParser::ValidateConfig() const
	{
		if (m_Config.m_TestChannelIO && !m_Config.m_ChannelArguments)
			throw std::invalid_argument("Argument error: specified -i (--test-io) without -a (--args)");

		if (m_Config.m_Command && !m_Config.m_ChannelArguments)
			throw std::invalid_argument("Argument error: specified -x (--command) without -a (--args)");
	}

}
