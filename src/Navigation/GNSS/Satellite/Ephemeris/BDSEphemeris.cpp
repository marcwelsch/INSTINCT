// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "BDSEphemeris.hpp"

#include "Navigation/Constants.hpp"
#include "Navigation/GNSS/Functions.hpp"

#include "util/Logger.hpp"

namespace NAV
{

BDSEphemeris::BDSEphemeris(/*const uint16_t& satNum, */ const InsTime& toc, const InsTime& toe,
                           const size_t& AODE, const size_t& AODC,
                           const std::array<double, 3>& a,
                           const double& sqrt_A, const double& e, const double& i_0, const double& Omega_0, const double& omega, const double& M_0,
                           const double& delta_n, const double& Omega_dot, const double& i_dot, const double& Cus, const double& Cuc,
                           const double& Cis, const double& Cic, const double& Crs, const double& Crc,
                           const double& svAccuracy, uint8_t satH1, double T_GD1, double T_GD2)
    : SatNavData(SatNavData::BeiDouEphemeris, toc),
      //   satNum(satNum),
      toc(toc),
      toe(toe),
      AODE(AODE),
      AODC(AODC),
      a(a),
      sqrt_A(sqrt_A),
      e(e),
      i_0(i_0),
      Omega_0(Omega_0),
      omega(omega),
      M_0(M_0),
      delta_n(delta_n),
      Omega_dot(Omega_dot),
      i_dot(i_dot),
      Cus(Cus),
      Cuc(Cuc),
      Cis(Cis),
      Cic(Cic),
      Crs(Crs),
      Crc(Crc),
      svAccuracy(svAccuracy),
      satH1(satH1),
      T_GD1(T_GD1),
      T_GD2(T_GD2)
{}

#ifdef TESTING

BDSEphemeris::BDSEphemeris(/*int32_t satNum, */ int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, double second, double svClockBias, double svClockDrift, double svClockDriftRate,
                           double AODE, double Crs, double delta_n, double M_0,
                           double Cuc, double e, double Cus, double sqrt_A,
                           double Toe, double Cic, double Omega_0, double Cis,
                           double i_0, double Crc, double omega, double Omega_dot,
                           double i_dot, double /* spare1 */, double BDTWeek, double /* spare2 */,
                           double svAccuracy, double satH1, double T_GD1, double T_GD2,
                           double /* TransmissionTimeOfMessage */, double AODC, double /* spare3 */, double /* spare4 */)
    : SatNavData(SatNavData::BeiDouEphemeris, InsTime(year, month, day, hour, minute, second, SatelliteSystem(BDS).getTimeSystem())),
      //   satNum(static_cast<uint16_t>(satNum)),
      toc(refTime),
      toe(InsTime(0, static_cast<int32_t>(BDTWeek) + InsTimeUtil::DIFF_BDT_WEEK_TO_GPST_WEEK, Toe, SatelliteSystem(BDS).getTimeSystem())),
      AODE(static_cast<size_t>(AODE)),
      AODC(static_cast<size_t>(AODC)),
      a({ svClockBias, svClockDrift, svClockDriftRate }),
      sqrt_A(sqrt_A),
      e(e),
      i_0(i_0),
      Omega_0(Omega_0),
      omega(omega),
      M_0(M_0),
      delta_n(delta_n),
      Omega_dot(Omega_dot),
      i_dot(i_dot),
      Cus(Cus),
      Cuc(Cuc),
      Cis(Cis),
      Cic(Cic),
      Crs(Crs),
      Crc(Crc),
      svAccuracy(svAccuracy),
      satH1(static_cast<uint8_t>(satH1)),
      T_GD1(T_GD1),
      T_GD2(T_GD2)
{}

#endif

Clock::Corrections BDSEphemeris::calcClockCorrections(const InsTime& recvTime, double dist, const Frequency& freq) const
{
    LOG_DATA("Calc Sat Clock corrections at receiver time {}", recvTime.toGPSweekTow(BDT));
    // Earth gravitational constant [m³/s²] (WGS 84 value of the earth's gravitational constant for GPS user)
    const auto mu = InsConst<>::BDS::MU;
    // Relativistic constant F for clock corrections [s/√m] (-2*√µ/c²)
    const auto F = InsConst<>::BDS::F;

    LOG_DATA("    toe {} (Time of ephemeris)", toe.toGPSweekTow(BDT));

    const auto A = sqrt_A * sqrt_A; // Semi-major axis [m]
    LOG_DATA("    A {} [m] (Semi-major axis)", A);
    auto n_0 = std::sqrt(mu / std::pow(A, 3)); // Computed mean motion [rad/s]
    LOG_DATA("    n_0 {} [rad/s] (Computed mean motion)", n_0);
    auto n = n_0 + delta_n; // Corrected mean motion [rad/s]
    LOG_DATA("    n {} [rad/s] (Corrected mean motion)", n);

    // Time at transmission
    InsTime transTime0 = recvTime - std::chrono::duration<double>(dist / InsConst<>::C);

    InsTime transTime = transTime0;
    LOG_DATA("    Iterating Time at transmission");
    double dt_sv = 0.0;
    double clkDrift = 0.0;

    for (size_t i = 0; i < 2; i++)
    {
        LOG_DATA("      transTime {} (Time at transmission)", transTime.toGPSweekTow(BDT));

        // [s]
        auto t_minus_toc = static_cast<double>((transTime - toc).count());
        LOG_DATA("      transTime - toc {} [s]", t_minus_toc);

        // Time difference from ephemeris reference epoch [s]
        double t_k = static_cast<double>((transTime - toe).count());
        LOG_DATA("      transTime - toe {} [s] (t_k = Time difference from ephemeris reference epoch)", t_k);

        // Mean anomaly [rad]
        auto M_k = M_0 + n * t_k;
        LOG_DATA("      M_k {} [s] (Mean anomaly)", M_k);

        // Eccentric anomaly [rad]
        double E_k = M_k;
        double E_k_old = 0.0;

        for (size_t i = 0; std::abs(E_k - E_k_old) > 1e-13 && i < 10; i++)
        {
            E_k_old = E_k; // Kepler’s equation ( Mk = E_k − e sin E_k ) may be solved for Eccentric anomaly (E_k) by iteration:
            E_k = M_k + e * sin(E_k);
        }

        // Relativistic correction term [s]
        double dt_r = F * e * sqrt_A * std::sin(E_k);
        LOG_DATA("      dt_r {} [s] (Relativistic correction term)", dt_r);

        // SV PRN code phase time offset [s]
        dt_sv = a[0] + a[1] * t_minus_toc + a[2] * std::pow(t_minus_toc, 2) + dt_r;

        // See BDS-SIS-ICD-2.1 BDS ICD, ch. 5.2.4.10, p.31
        dt_sv -= (freq == B02 ? T_GD1 : freq == B07 ? T_GD2 // TODO: check again
                                                    : 0);

        LOG_DATA("      dt_sv {} [s] (SV PRN code phase time offset)", dt_sv);

        // Groves ch. 9.3.1, eq. 9.78, p. 391
        clkDrift = a[1] + a[2] / 2.0 * t_minus_toc;

        // Correct transmit time for the satellite clock bias
        transTime = transTime0 - std::chrono::duration<double>(dt_sv);
    }

    return { .transmitTime = transTime, .bias = dt_sv, .drift = clkDrift };
}

Orbit::PosVelAccel BDSEphemeris::calcSatelliteData(const InsTime& transTime, Orbit::Calc calc) const
{
    Eigen::Vector3d e_pos = Eigen::Vector3d::Zero();
    Eigen::Vector3d e_vel = Eigen::Vector3d::Zero();
    Eigen::Vector3d e_accel = Eigen::Vector3d::Zero();

    LOG_DATA("Calc Sat Position at transmit time {}", transTime.toGPSweekTow(BDT));
    // Earth gravitational constant [m³/s²] (WGS 84 value of the earth's gravitational constant for GPS user)
    const auto mu = InsConst<>::BDS::MU;
    // Earth angular velocity [rad/s] (WGS 84 value of the earth's rotation rate)
    const auto Omega_e_dot = InsConst<>::BDS::omega_ie;

    LOG_DATA("    toe {} (Time of ephemeris)", toe.toGPSweekTow(BDT));

    const auto A = sqrt_A * sqrt_A; // Semi-major axis [m]
    LOG_DATA("    A {} [m] (Semi-major axis)", A);
    auto n_0 = std::sqrt(mu / std::pow(A, 3)); // Computed mean motion [rad/s]
    LOG_DATA("    n_0 {} [rad/s] (Computed mean motion)", n_0);
    auto n = n_0 + delta_n; // Corrected mean motion [rad/s]
    LOG_DATA("    n {} [rad/s] (Corrected mean motion)", n);

    // Eccentric anomaly [rad]
    double E_k = 0.0;

    // Computed time from ephemeris reference epoch [s]
    double t_k = static_cast<double>((transTime - toe).count());
    LOG_DATA("    t_k {} [s] (Time difference from ephemeris reference epoch)", t_k);

    // Computed Mean anomaly [rad]
    auto M_k = M_0 + n * t_k;
    LOG_DATA("    M_k {} [s] (Mean anomaly)", M_k);

    E_k = M_k; // Initial Value [rad]
    double E_k_old = 0.0;
    LOG_DATA("    Iterating E_k");
    LOG_DATA("      E_k {} [rad] (Eccentric anomaly)", E_k);
    for (size_t i = 0; std::abs(E_k - E_k_old) > 1e-13 && i < 10; i++)
    {
        E_k_old = E_k;                                                         // Kepler’s equation ( Mk = E_k − e sin E_k ) may be solved for Eccentric anomaly (E_k) by iteration:
        E_k = E_k + (M_k - E_k + e * std::sin(E_k)) / (1 - e * std::cos(E_k)); // – Refined Value, minimum of three iterations, (j=1,2,3)
        LOG_DATA("      E_k {} [rad] (Eccentric anomaly)", E_k);               // – Final Value (radians)
    }

    // auto v_k = std::atan2(std::sqrt(1 - e * e) * std::sin(E_k) / (1 - e * std::cos(E_k)), (std::cos(E_k) - e) / (1 - e * std::cos(E_k))); // True Anomaly [rad]
    auto v_k = std::atan2(std::sqrt(1 - e * e) * std::sin(E_k), (std::cos(E_k) - e)); // True Anomaly [rad] // simplified, since the denominators cancel out
    LOG_DATA("    v_k {} [rad] (True Anomaly (unambiguous quadrant))", v_k);

    auto Phi_k = v_k + omega; // Computed Argument of Latitude [rad]
    LOG_DATA("    Phi_k {} [rad] (Argument of Latitude)", Phi_k);

    // Second Harmonic Perturbations
    auto delta_u_k = Cus * std::sin(2 * Phi_k) + Cuc * std::cos(2 * Phi_k); // Argument of Latitude Correction [rad]
    LOG_DATA("    delta_u_k {} [rad] (Argument of Latitude Correction)", delta_u_k);
    auto delta_r_k = Crs * std::sin(2 * Phi_k) + Crc * std::cos(2 * Phi_k); // Radius Correction [m]
    LOG_DATA("    delta_r_k {} [m] (Radius Correction)", delta_r_k);
    auto delta_i_k = Cis * std::sin(2 * Phi_k) + Cic * std::cos(2 * Phi_k); // Inclination Correction [rad]
    LOG_DATA("    delta_i_k {} [rad] (Inclination Correction)", delta_i_k);

    auto u_k = Phi_k + delta_u_k; // Corrected Argument of Latitude [rad]
    LOG_DATA("    u_k {} [rad] (Corrected Argument of Latitude)", u_k);
    auto r_k = A * (1 - e * std::cos(E_k)) + delta_r_k; // Corrected Radius [m]
    LOG_DATA("    r_k {} [m] (Corrected Radius)", r_k);
    auto i_k = i_0 + delta_i_k + i_dot * t_k; // Corrected Inclination [rad]
    LOG_DATA("    i_k {} [rad] (Corrected Inclination)", i_k);

    auto x_k_op = r_k * std::cos(u_k); // Computed position in orbital plane [m]
    LOG_DATA("    x_k_op {} [m] (Position in orbital plane)", x_k_op);
    auto y_k_op = r_k * std::sin(u_k); // Computed position in orbital plane [m]
    LOG_DATA("    y_k_op {} [m] (Position in orbital plane)", y_k_op);

    double Omega_k = 0.0;
    double x_k = 0.0;
    double y_k = 0.0;
    double z_k = 0.0;

    // Satellite has a GEO orbit
    if (i_k < 30.0 * M_PI / 180) // TODO: Should be based on satNum: if (GeoSats.contains(satNum)){...}
    {
        // Corrected longitude of ascending node [rad]
        Omega_k = Omega_0 + Omega_dot * t_k - Omega_e_dot * static_cast<double>(toe.toGPSweekTow(BDT).tow);
        LOG_DATA("    Omega_k {} [rad] (Corrected longitude of ascending node)", Omega_k);

        Eigen::Vector3d X_GK{ 0, 0, 0 };

        X_GK(0) = x_k_op * std::cos(Omega_k) - y_k_op * std::cos(i_k) * std::sin(Omega_k);
        X_GK(1) = x_k_op * std::sin(Omega_k) + y_k_op * std::cos(i_k) * std::cos(Omega_k);
        X_GK(2) = y_k_op * std::sin(i_k);

        auto Rx = Eigen::AngleAxis((5.0) * M_PI / 180, Eigen::Vector3d::UnitX());
        auto Rz = Eigen::AngleAxis(-Omega_e_dot * t_k, Eigen::Vector3d::UnitZ());

        e_pos = Rz * Rx * X_GK;

        x_k = e_pos(0);
        y_k = e_pos(1);
        z_k = e_pos(2);
    }
    else // Satellite has a MEO or IGSO orbit
    {
        // Corrected longitude of ascending node [rad]
        Omega_k = Omega_0 + (Omega_dot - Omega_e_dot) * t_k - Omega_e_dot * static_cast<double>(toe.toGPSweekTow(BDT).tow);
        LOG_DATA("    Omega_k {} [rad] (Corrected longitude of ascending node)", Omega_k);

        // Earth-fixed x coordinates [m]
        x_k = x_k_op * std::cos(Omega_k) - y_k_op * std::cos(i_k) * std::sin(Omega_k);
        LOG_DATA("    x_k {} [m] (Earth-fixed x coordinates)", x_k);
        // Earth-fixed y coordinates [m]
        y_k = x_k_op * std::sin(Omega_k) + y_k_op * std::cos(i_k) * std::cos(Omega_k);
        LOG_DATA("    y_k {} [m] (Earth-fixed y coordinates)", y_k);
        // Earth-fixed z coordinates [m]
        z_k = y_k_op * std::sin(i_k);
        LOG_DATA("    z_k {} [m] (Earth-fixed z coordinates)", z_k);

        e_pos = Eigen::Vector3d{ x_k, y_k, z_k };
    }

    if (calc & Calc_Velocity || calc & Calc_Acceleration)
    {
        // Eccentric Anomaly Rate [rad/s]
        auto E_k_dot = n / (1 - e * std::cos(E_k));
        // True Anomaly Rate [rad/s]
        auto v_k_dot = E_k_dot * std::sqrt(1 - e * e) / (1 - e * std::cos(E_k));
        // Corrected Inclination Angle Rate [rad/s]
        auto i_k_dot = i_dot + 2 * v_k_dot * (Cis * std::cos(2 * Phi_k) - Cic * std::sin(2 * Phi_k));
        // Corrected Argument of Latitude Rate [rad/s]
        auto u_k_dot = v_k_dot + 2 * v_k_dot * (Cus * std::cos(2 * Phi_k) - Cuc * std::sin(2 * Phi_k));
        // Corrected Radius Rate [m/s]
        auto r_k_dot = e * A * E_k_dot * std::sin(E_k) + 2 * v_k_dot * (Crs * std::cos(2 * Phi_k) - Crc * std::sin(2 * Phi_k));
        // Longitude of Ascending Node Rate [rad/s]
        auto Omega_k_dot = Omega_dot - Omega_e_dot;
        // In-plane x velocity [m/s]
        auto vx_k_op = r_k_dot * std::cos(u_k) - r_k * u_k_dot * std::sin(u_k);
        // In-plane y velocity [m/s]
        auto vy_k_op = r_k_dot * std::sin(u_k) + r_k * u_k_dot * std::cos(u_k);
        // Earth-Fixed x velocity [m/s]
        auto vx_k = -x_k_op * Omega_k_dot * std::sin(Omega_k) + vx_k_op * std::cos(Omega_k) - vy_k_op * std::sin(Omega_k) * std::cos(i_k)
                    - y_k_op * (Omega_k_dot * std::cos(Omega_k) * std::cos(i_k) - i_k_dot * std::sin(Omega_k) * std::sin(i_k));
        // Earth-Fixed y velocity [m/s]
        auto vy_k = x_k_op * Omega_k_dot * std::cos(Omega_k) + vx_k_op * std::sin(Omega_k) + vy_k_op * std::cos(Omega_k) * std::cos(i_k)
                    - y_k_op * (Omega_k_dot * std::sin(Omega_k) * std::cos(i_k) + i_k_dot * std::cos(Omega_k) * std::sin(i_k));
        // Earth-Fixed z velocity [m/s]
        auto vz_k = vy_k_op * std::sin(i_k) + y_k_op * i_k_dot * std::cos(i_k);

        if (calc & Calc_Velocity)
        {
            e_vel = Eigen::Vector3d{ vx_k, vy_k, vz_k };
        }

        if (calc & Calc_Acceleration)
        {
            // Oblate Earth acceleration Factor [m/s^2]
            auto F = -(3.0 / 2.0) * InsConst<>::GPS::J2 * (mu / std::pow(r_k, 2)) * std::pow(InsConst<>::GPS::R_E / r_k, 2);
            // Earth-Fixed x acceleration [m/s^2]
            auto ax_k = -mu * (x_k / std::pow(r_k, 3)) + F * ((1.0 - 5.0 * std::pow(z_k / r_k, 2)) * (x_k / r_k))
                        + 2 * vy_k * Omega_e_dot + x_k * std::pow(Omega_e_dot, 2);
            // Earth-Fixed y acceleration [m/s^2]
            auto ay_k = -mu * (y_k / std::pow(r_k, 3)) + F * ((1.0 - 5.0 * std::pow(z_k / r_k, 2)) * (y_k / r_k))
                        + 2 * vx_k * Omega_e_dot + y_k * std::pow(Omega_e_dot, 2);
            // Earth-Fixed z acceleration [m/s^2]
            auto az_k = -mu * (z_k / std::pow(r_k, 3)) + F * ((3.0 - 5.0 * std::pow(z_k / r_k, 2)) * (z_k / r_k));

            e_accel = Eigen::Vector3d{ ax_k, ay_k, az_k };
        }
    }

    return { .e_pos = e_pos,
             .e_vel = e_vel,
             .e_accel = e_accel };
}

bool BDSEphemeris::isHealthy() const
{
    return satH1 == 0;
}

double BDSEphemeris::calcSatellitePositionVariance() const
{
    return std::pow(svAccuracy, 2);
}

} // namespace NAV