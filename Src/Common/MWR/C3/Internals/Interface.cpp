#include "StdAfx.h"
#include "Interface.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MWR::C3::AbstractPeripheral::OnReceive()
{
	if (auto bridge = GetBridge(); bridge)
		if (auto command = OnReceiveFromPeripheral(); !command.empty())
			bridge->PostCommandToConnector(command);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MWR::C3::AbstractChannel::OnReceive()
{
	if (auto bridge = GetBridge(); bridge)
		for (auto&& packet : OnReceiveFromChannelInternal())
			bridge->PassNetworkPacket(packet);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MWR::C3::Device::SetUpdateDelay(std::chrono::milliseconds minUpdateDelayInMs, std::chrono::milliseconds maxUpdateDelayInMs)
{
	// Sanity checks.
	if (minUpdateDelayInMs > maxUpdateDelayInMs)
		throw std::invalid_argument{ OBF("maxUpdateDelay must be greater or equal to minUpdateDelay.") };
	if (minUpdateDelayInMs < 30ms)
		throw std::invalid_argument{ OBF("minUpdateDelay must be greater or equal to 30ms.") };

	std::lock_guard<std::mutex> guard(m_UpdateDelayMutex);
	m_MinUpdateDelay = minUpdateDelayInMs;
	m_MaxUpdateDelay = maxUpdateDelayInMs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MWR::C3::Device::SetUpdateDelay(std::chrono::milliseconds frequencyInMs)
{
	std::lock_guard<std::mutex> guard(m_UpdateDelayMutex);
	m_MinUpdateDelay = frequencyInMs;
	m_MaxUpdateDelay = m_MinUpdateDelay;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::chrono::milliseconds MWR::C3::Device::GetUpdateDelay() const
{
	std::lock_guard<std::mutex> guard(m_UpdateDelayMutex);
	return m_MinUpdateDelay != m_MaxUpdateDelay ? MWR::Utils::GenerateRandomValue(m_MinUpdateDelay, m_MaxUpdateDelay) : m_MinUpdateDelay;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MWR::ByteVector MWR::C3::Device::OnRunCommand(ByteView command)
{
	switch (command.Read<uint16_t>())
	{
		case static_cast<uint16_t>(MWR::C3::Command::Close):
		return Close(), ByteVector{};
	case static_cast<uint16_t>(MWR::C3::Command::UpdateJitter) :
	{
		auto [minVal, maxVal] = command.Read<float, float>();
		return SetUpdateDelay(MWR::Utils::ToMilliseconds(minVal), MWR::Utils::ToMilliseconds(maxVal)), ByteVector{};
	}
	default:
		throw std::runtime_error(OBF("Device received an unknown command"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MWR::ByteVector MWR::C3::AbstractConnector::OnRunCommand(ByteView command)
{
	switch (command.Read<uint16_t>())
	{
		case static_cast<uint16_t>(-1) :
			return TurnOff(), ByteVector{};
		default:
			throw std::runtime_error(OBF("AbstractConnector received an unknown command"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MWR::C3::Device::Close()
{
	GetBridge()->Close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MWR::C3::AbstractConnector::TurnOff()
{
	GetBridge()->TurnOff();
}
