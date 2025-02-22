// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "udpSend.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;
#include "internal/FlowManager.hpp"

#include "internal/gui/widgets/imgui_ex.hpp"
#include "internal/gui/widgets/HelpMarker.hpp"
#include "internal/gui/NodeEditorApplication.hpp"

#include "util/Logger.hpp"

NAV::UdpSend::UdpSend()
    : Node(typeStatic()), _socket(_io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0)), _resolver(_io_context)
{
    LOG_TRACE("{}: called", name);

    _hasConfig = true;
    _guiConfigDefaultWindowSize = { 202, 96 };

    nm::CreateInputPin(this, "PosVelAtt", Pin::Type::Flow, { NAV::PosVelAtt::type() }, &UdpSend::receivePosVelAtt);
}

NAV::UdpSend::~UdpSend()
{
    LOG_TRACE("{}: called", nameId());
}

std::string NAV::UdpSend::typeStatic()
{
    return "UdpSend";
}

std::string NAV::UdpSend::type() const
{
    return typeStatic();
}

std::string NAV::UdpSend::category()
{
    return "Data Link";
}

void NAV::UdpSend::guiConfig()
{
    ImGui::SetNextItemWidth(150 * gui::NodeEditorApplication::windowFontRatio());
    if (ImGui::InputInt4L(fmt::format("IPv4##{}", size_t(id)).c_str(), _ip.data(), IP_LIMITS[0], IP_LIMITS[1]))
    {
        flow::ApplyChanges();
    }
    ImGui::SetNextItemWidth(150 * gui::NodeEditorApplication::windowFontRatio());
    if (ImGui::InputIntL(fmt::format("Port##{}", size_t(id)).c_str(), &_port, PORT_LIMITS[0], PORT_LIMITS[1]))
    {
        flow::ApplyChanges();
    }
}

bool NAV::UdpSend::resetNode()
{
    return true;
}

json NAV::UdpSend::save() const
{
    LOG_TRACE("{}: called", nameId());

    json j;

    j["ip"] = _ip;
    j["port"] = _port;

    return j;
}

void NAV::UdpSend::restore(json const& j)
{
    LOG_TRACE("{}: called", nameId());
    if (j.contains("ip"))
    {
        j.at("ip").get_to(_ip);
    }
    if (j.contains("port"))
    {
        j.at("port").get_to(_port);
    }
}

bool NAV::UdpSend::initialize()
{
    LOG_TRACE("{}: called", nameId());

    std::string ipString{};
    for (size_t i = 0; i < 4; i++)
    {
        ipString.append(std::to_string(_ip.at(i)));
        i < 3 ? ipString.append(".") : ipString.append("");
    }

    _endpoints = _resolver.resolve(boost::asio::ip::udp::v4(), ipString, std::to_string(_port));

    return true;
}

void NAV::UdpSend::deinitialize()
{
    _io_context.stop();

    LOG_TRACE("{}: called", nameId());
}

void NAV::UdpSend::receivePosVelAtt(NAV::InputPin::NodeDataQueue& queue, size_t /* pinIdx */)
{
    auto posVelAtt = std::make_shared<PosVelAtt>(*std::static_pointer_cast<const PosVelAtt>(queue.extract_front()));

    Eigen::Vector3d posLLA = posVelAtt->lla_position();
    Eigen::Vector3d vel_n = posVelAtt->n_velocity();
    Eigen::Vector4d n_Quat_b = { posVelAtt->n_Quat_b().x(), posVelAtt->n_Quat_b().y(), posVelAtt->n_Quat_b().z(), posVelAtt->n_Quat_b().w() };
    auto timeStamp = posVelAtt->insTime.toGPSweekTow();
    auto gpsC = timeStamp.gpsCycle;
    auto gpsW = timeStamp.gpsWeek;
    auto gpsT = timeStamp.tow;

    std::vector<double> udp_posVelAtt{ posLLA(0), posLLA(1), posLLA(2), vel_n(0), vel_n(1), vel_n(2), n_Quat_b(0), n_Quat_b(1), n_Quat_b(2), n_Quat_b(3), static_cast<double>(gpsC), static_cast<double>(gpsW), static_cast<double>(gpsT) };

    _socket.send_to(boost::asio::buffer(udp_posVelAtt), *_endpoints.begin());
}