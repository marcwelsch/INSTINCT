// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file MultiImuFileTestsData.hpp
/// @brief
/// @author M. Maier (marcel.maier@ins.uni-stuttgart.de)
/// @date 2023-08-09

#pragma once

#include "NodeData/IMU/ImuObs.hpp"

namespace NAV::TESTS::MultiImuFileTests
{

/// @brief Returns the GPS second of the day
/// @param gpsSecond integer GPS second since start of the week
/// @param timeNumerator Numerator for the fraction of the integer GPS second
/// @param timeDenominator Denominator for the fraction of the integer GPS second
/// @return GPS second of the day
long double timestamp(double gpsSecond, double timeNumerator, double timeDenominator);

static inline InsTime IMU_STARTTIME = InsTime(2023, 8, 9, 9, 34, 35., UTC); // UTC time at start from 'GPZDA' NMEA msg

constexpr double SCALEFACTOR_ACCEL = 0.001;
constexpr double SCALEFACTOR_GYRO = 1. / 131.;

enum ImuRef : size_t
{
    SensorId,
    GpsSec, // timestamp from GPS, but with UTC as time base
    GpsNum,
    GpsDen,
    AccX,
    AccY,
    AccZ,
    GyroX,
    GyroY,
    GyroZ,
};

// IMU reference data - the first 20 lines of data
constexpr std::array<std::array<double, 12>, 100> IMU_REFERENCE_DATA{ {
    { 5., 34476., 1620., 992500., 685., 101., 8535., -377., 134., -121., 221., 40132. },
    { 1., 34476., 22724., 992500., 894., -137., 12134., -421., 153., -246., 273., 40060. },
    { 2., 34476., 23768., 992500., 1008., 497., 9644., 496., -1046., -90., 275., 40736. },
    { 4., 34476., 29900., 992500., 473., 92., 9349., -308., -56., 64., 277., 39976. },
    { 3., 34476., 30348., 992500., 638., 209., 10898., -419., 146., -89., 269., 40052. },
    { 5., 34476., 41760., 992500., 683., 108., 8520., -383., 137., -123., 221., 40140. },
    { 1., 34476., 62788., 992500., 876., -123., 12125., -423., 153., -249., 273., 40064. },
    { 2., 34476., 64496., 992500., 971., 486., 9649., 495., -1043., -80., 275., 40728. },
    { 4., 34476., 69888., 992500., 458., 85., 9347., -310., -53., 62., 277., 39988. },
    { 3., 34476., 70408., 992500., 634., 197., 10883., -420., 146., -97., 269., 40060. },
    { 5., 34476., 81896., 992500., 673., 117., 8505., -388., 136., -121., 221., 40136. },
    { 1., 34476., 102844., 992500., 880., -137., 12135., -423., 151., -247., 273., 40056. },
    { 2., 34476., 105224., 992500., 1001., 469., 9646., 496., -1038., -90., 275., 40728. },
    { 4., 34476., 109868., 992500., 441., 80., 9341., -308., -54., 62., 277., 39980. },
    { 3., 34476., 110452., 992500., 619., 189., 10880., -424., 152., -95., 269., 40044. },
    { 5., 34476., 122036., 992500., 655., 111., 8486., -387., 138., -124., 221., 40140. },
    { 1., 34476., 142900., 992500., 886., -141., 12140., -423., 155., -243., 273., 40056. },
    { 2., 34476., 145952., 992500., 1002., 487., 9651., 494., -1040., -91., 275., 40728. },
    { 4., 34476., 149852., 992500., 452., 69., 9341., -309., -59., 62., 277., 39984. },
    { 3., 34476., 150508., 992500., 635., 196., 10894., -425., 148., -91., 269., 40056. },
    { 5., 34476., 162176., 992500., 679., 108., 8518., -388., 138., -118., 221., 40140. },
    { 1., 34476., 182956., 992500., 905., -155., 12129., -420., 153., -238., 273., 40056. },
    { 2., 34476., 186680., 992500., 983., 468., 9655., 498., -1046., -85., 275., 40728. },
    { 4., 34476., 189828., 992500., 465., 73., 9360., -306., -59., 60., 277., 39976. },
    { 3., 34476., 190560., 992500., 639., 186., 10859., -424., 145., -101., 269., 40052. },
    { 5., 34476., 202312., 992500., 675., 91., 8480., -388., 133., -120., 221., 40136. },
    { 1., 34476., 223016., 992500., 904., -152., 12141., -424., 151., -243., 273., 40060. },
    { 2., 34476., 227408., 992500., 978., 464., 9649., 495., -1042., -87., 275., 40728. },
    { 4., 34476., 229808., 992500., 483., 73., 9362., -308., -55., 60., 277., 39980. },
    { 3., 34476., 230616., 992500., 649., 189., 10873., -420., 147., -91., 269., 40056. },
    { 5., 34476., 242444., 992500., 695., 97., 8476., -382., 136., -120., 221., 40132. },
    { 1., 34476., 263072., 992500., 904., -147., 12123., -421., 154., -249., 273., 40056. },
    { 2., 34476., 268136., 992500., 986., 474., 9644., 493., -1041., -88., 275., 40728. },
    { 4., 34476., 269792., 992500., 462., 70., 9367., -310., -60., 61., 277., 39984. },
    { 3., 34476., 270660., 992500., 647., 191., 10897., -424., 151., -94., 269., 40044. },
    { 5., 34476., 282572., 992500., 679., 99., 8520., -386., 136., -120., 221., 40128. },
    { 1., 34476., 303132., 992500., 891., -134., 12125., -425., 152., -246., 273., 40060. },
    { 2., 34476., 308860., 992500., 937., 481., 9670., 499., -1050., -88., 275., 40724. },
    { 4., 34476., 309776., 992500., 444., 91., 9338., -309., -59., 59., 277., 39984. },
    { 3., 34476., 310720., 992500., 629., 205., 10875., -421., 148., -95., 269., 40060. },
    { 5., 34476., 322696., 992500., 651., 97., 8488., -383., 137., -123., 221., 40124. },
    { 1., 34476., 343184., 992500., 879., -134., 12142., -423., 153., -247., 274., 40052. },
    { 2., 34476., 349592., 992500., 946., 469., 9663., 493., -1038., -84., 275., 40732. },
    { 4., 34476., 349756., 992500., 439., 71., 9368., -311., -59., 61., 277., 39980. },
    { 3., 34476., 350776., 992500., 623., 201., 10906., -421., 145., -93., 269., 40056. },
    { 5., 34476., 362824., 992500., 651., 104., 8513., -385., 133., -121., 221., 40128. },
    { 1., 34476., 383236., 992500., 883., -141., 12095., -424., 152., -250., 273., 40052. },
    { 4., 34476., 389740., 992500., 463., 50., 9360., -315., -53., 59., 277., 39984. },
    { 2., 34476., 390316., 992500., 960., 471., 9611., 496., -1036., -90., 275., 40724. },
    { 3., 34476., 390828., 992500., 623., 197., 10889., -420., 147., -91., 269., 40052. },
    { 5., 34476., 402948., 992500., 681., 111., 8510., -385., 132., -120., 221., 40124. },
    { 1., 34476., 423292., 992500., 897., -155., 12150., -426., 153., -243., 274., 40056. },
    { 4., 34476., 429728., 992500., 483., 70., 9336., -309., -60., 62., 277., 39988. },
    { 3., 34476., 430884., 992500., 627., 197., 10902., -420., 149., -92., 269., 40056. },
    { 2., 34476., 431036., 992500., 990., 471., 9662., 495., -1040., -90., 275., 40720. },
    { 5., 34476., 443076., 992500., 701., 99., 8524., -382., 137., -124., 221., 40128. },
    { 1., 34476., 463352., 992500., 900., -144., 12089., -424., 151., -246., 274., 40060. },
    { 4., 34476., 469712., 992500., 486., 61., 9332., -311., -60., 54., 277., 39984. },
    { 3., 34476., 470928., 992500., 635., 178., 10893., -423., 147., -98., 269., 40044. },
    { 2., 34476., 471760., 992500., 980., 480., 9653., 499., -1041., -90., 275., 40724. },
    { 5., 34476., 483204., 992500., 683., 112., 8486., -386., 138., -122., 221., 40128. },
    { 1., 34476., 503412., 992500., 893., -131., 12122., -422., 155., -244., 274., 40060. },
    { 4., 34476., 509696., 992500., 467., 67., 9356., -311., -58., 58., 277., 39984. },
    { 3., 34476., 510984., 992500., 646., 192., 10898., -425., 149., -95., 269., 40056. },
    { 2., 34476., 512488., 992500., 984., 480., 9657., 495., -1037., -86., 275., 40728. },
    { 5., 34476., 523332., 992500., 687., 113., 8499., -387., 138., -119., 221., 40128. },
    { 1., 34476., 543468., 992500., 901., -129., 12142., -425., 153., -238., 274., 40056. },
    { 4., 34476., 549680., 992500., 447., 70., 9352., -307., -60., 59., 277., 39984. },
    { 3., 34476., 551040., 992500., 616., 191., 10917., -419., 147., -92., 269., 40056. },
    { 2., 34476., 553212., 992500., 974., 475., 9667., 496., -1048., -89., 275., 40724. },
    { 5., 34476., 563468., 992500., 659., 105., 8502., -383., 138., -120., 221., 40136. },
    { 1., 34476., 583520., 992500., 869., -119., 12114., -428., 154., -245., 273., 40052. },
    { 4., 34476., 589660., 992500., 447., 71., 9353., -305., -58., 58., 277., 39980. },
    { 3., 34476., 591092., 992500., 622., 210., 10907., -421., 147., -91., 269., 40052. },
    { 2., 34476., 593944., 992500., 976., 497., 9640., 500., -1044., -87., 275., 40732. },
    { 5., 34476., 603600., 992500., 668., 118., 8517., -385., 135., -121., 221., 40132. },
    { 1., 34476., 623576., 992500., 877., -144., 12128., -426., 150., -243., 274., 40056. },
    { 4., 34476., 629644., 992500., 473., 65., 9367., -309., -52., 58., 277., 39984. },
    { 3., 34476., 631144., 992500., 616., 189., 10883., -421., 148., -90., 269., 40052. },
    { 2., 34476., 634672., 992500., 985., 471., 9669., 497., -1045., -92., 275., 40728. },
    { 5., 34476., 643732., 992500., 698., 95., 8513., -386., 135., -119., 221., 40132. },
    { 1., 34476., 663636., 992500., 894., -138., 12104., -421., 154., -244., 274., 40060. },
    { 4., 34476., 669628., 992500., 491., 69., 9336., -311., -61., 56., 277., 39984. },
    { 3., 34476., 671196., 992500., 665., 196., 10867., -419., 146., -96., 269., 40052. },
    { 2., 34476., 675400., 992500., 978., 461., 9639., 497., -1047., -90., 275., 40728. },
    { 5., 34476., 683860., 992500., 701., 97., 8501., -381., 135., -120., 221., 40128. },
    { 1., 34476., 703696., 992500., 911., -150., 12113., -421., 152., -244., 273., 40060. },
    { 4., 34476., 709616., 992500., 483., 91., 9338., -311., -56., 59., 277., 39988. },
    { 3., 34476., 711252., 992500., 641., 180., 10885., -421., 145., -87., 269., 40056. },
    { 2., 34476., 716132., 992500., 985., 476., 9668., 497., -1047., -88., 275., 40732. },
    { 5., 34476., 723988., 992500., 675., 113., 8504., -386., 137., -121., 221., 40128. },
    { 1., 34476., 743752., 992500., 879., -129., 12135., -427., 152., -244., 274., 40056. },
    { 4., 34476., 749600., 992500., 467., 71., 9337., -307., -58., 60., 277., 39984. },
    { 3., 34476., 751308., 992500., 644., 196., 10876., -424., 149., -89., 269., 40056. },
    { 2., 34476., 756860., 992500., 979., 476., 9670., 495., -1044., -87., 275., 40728. },
    { 5., 34476., 764116., 992500., 697., 128., 8477., -385., 137., -118., 221., 40128. },
    { 1., 34476., 783812., 992500., 865., -147., 12118., -423., 153., -241., 274., 40060. },
    { 4., 34476., 789580., 992500., 465., 67., 9366., -311., -54., 61., 277., 39980. },
    { 3., 34476., 791360., 992500., 637., 198., 10888., -425., 147., -93., 269., 40052. },
    { 2., 34476., 797584., 992500., 972., 482., 9613., 497., -1041., -87., 275., 40724. },
} };

} // namespace NAV::TESTS::MultiImuFileTests