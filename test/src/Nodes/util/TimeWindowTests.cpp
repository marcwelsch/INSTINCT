// This file is part of INSTINCT, the INS Toolkit for Integrated
// Navigation Concepts and Training by the Institute of Navigation of
// the University of Stuttgart, Germany.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

/// @file TimeWindowTests.cpp
/// @brief Time Window Tests
/// @author M. Maier (marcel.maier@ins.uni-stuttgart.de)
/// @date 2022-11-07

#include <catch2/catch_test_macros.hpp>

#include "FlowTester.hpp"

#include "internal/NodeManager.hpp"
namespace nm = NAV::NodeManager;

#include "Logger.hpp"

// This is a small hack, which lets us change private/protected parameters
#pragma GCC diagnostic push
#if defined(__clang__)
    #pragma GCC diagnostic ignored "-Wkeyword-macro"
    #pragma GCC diagnostic ignored "-Wmacro-redefined"
#endif
#define protected public
#define private public
#include "Nodes/Utility/TimeWindow.hpp"
#undef protected
#undef private
#pragma GCC diagnostic pop

namespace NAV::TESTS::TimeWindowTests
{

TEST_CASE("[TimeWindow][flow] Simulate IMU and cut off start and end time", "[TimeWindow][flow]")
{
    auto logger = initializeTestLogger();

    // ###########################################################################################################
    //                                              TimeWindow.flow
    // ###########################################################################################################
    //
    //  ImuSimulator (6)             TimeWindow (3)                           Plot (13)
    //     (4) ImuObs |>  ---(7)-->  |> Input (1)  (2) Output |>  ---(14)-->  |> Pin 1 (8)
    //  (5) PosVelAtt |>
    //
    // ###########################################################################################################

    size_t messageCounterInput = 0;
    nm::RegisterWatcherCallbackToInputPin(1, [&messageCounterInput](const Node* /* node */, const InputPin::NodeDataQueue& /* queue */, size_t /* pinIdx */) {
        messageCounterInput++;
    });

    size_t messageCounterOutput = 0;
    nm::RegisterWatcherCallbackToInputPin(8, [&messageCounterOutput](const Node* /* node */, const InputPin::NodeDataQueue& queue, size_t /* pinIdx */) {
        REQUIRE(queue.front()->insTime >= InsTime(2000, 1, 1, 0, 0, 1.5)); // Start time
        REQUIRE(queue.front()->insTime <= InsTime(2000, 1, 1, 0, 0, 8.5)); // End time

        messageCounterOutput++;
    });

    REQUIRE(testFlow("test/flow/Nodes/util/TimeWindow.flow"));

    REQUIRE(messageCounterInput == 101);
    REQUIRE(messageCounterOutput == 71);
}

TEST_CASE("[TimeWindow][flow] Simulate IMU and cut off middle part", "[TimeWindow][flow]")
{
    auto logger = initializeTestLogger();

    // ###########################################################################################################
    //                                              TimeWindow.flow
    // ###########################################################################################################
    //
    //  ImuSimulator (6)             TimeWindow (3)                           Plot (13)
    //     (4) ImuObs |>  ---(7)-->  |> Input (1)  (2) Output |>  ---(14)-->  |> Pin 1 (8)
    //  (5) PosVelAtt |>
    //
    // ###########################################################################################################

    nm::RegisterPreInitCallback([&]() {
        dynamic_cast<TimeWindow*>(nm::FindNode(3))->_inverseWindow = true;
    });

    size_t messageCounterInput = 0;
    nm::RegisterWatcherCallbackToInputPin(1, [&messageCounterInput](const Node* /* node */, const InputPin::NodeDataQueue& /* queue */, size_t /* pinIdx */) {
        messageCounterInput++;
    });

    size_t messageCounterOutput = 0;
    nm::RegisterWatcherCallbackToInputPin(8, [&messageCounterOutput](const Node* /* node */, const InputPin::NodeDataQueue& queue, size_t /* pinIdx */) {
        CAPTURE(messageCounterOutput);
        if (messageCounterOutput < 15)
        {
            REQUIRE(queue.front()->insTime < InsTime(2000, 1, 1, 0, 0, 1.5)); // Start time
        }
        else
        {
            REQUIRE(queue.front()->insTime > InsTime(2000, 1, 1, 0, 0, 8.5)); // End time
        }

        REQUIRE((queue.front()->insTime < InsTime(2000, 1, 1, 0, 0, 1.5)       // Start time
                 || queue.front()->insTime > InsTime(2000, 1, 1, 0, 0, 8.5))); // End time
        messageCounterOutput++;
    });

    REQUIRE(testFlow("test/flow/Nodes/util/TimeWindow.flow"));

    REQUIRE(messageCounterInput == 101);
    REQUIRE(messageCounterOutput == 30);
}

} // namespace NAV::TESTS::TimeWindowTests
