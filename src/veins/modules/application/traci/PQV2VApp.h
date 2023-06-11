//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "veins/veins.h"
#include <vector>
#include <algorithm>

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/messages/ecdsa_full_spdu_m.h"
#include "veins/modules/messages/ecdsa_digest_spdu_m.h"
#include "veins/modules/messages/ecdsa_spdu_m.h"
#include "veins/modules/messages/P2PCDLearningRequest_m.h"
#include "veins/modules/messages/LearningResponseSPDU_m.h"

using namespace omnetpp;

namespace veins {

/**
 * @brief
 * This is a stub for a typical Veins application layer.
 * Most common functions are overloaded.
 * See PQV2VApp.cc for hints
 *
 * @author David Eckhoff
 *
 */

class VEINS_API PQV2VApp : public DemoBaseApplLayer {
public:

    static int vehicle_id_counter;

    static const int ECDSA_FULL_SPDU_SIZE_BITS = 330 * 8;
    static const int ECDSA_DIGEST_SPDU_SIZE_BITS = 200 * 8;

    void initialize(int stage) override;
    void finish() override;


protected:
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onRealBSM(J2735_bsm* bsm) override;
    void populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type rcvId = LAddress::L2BROADCAST(), int serial = 0) override;

    void onECDSA_SPDU(ECDSA_SPDU* spdu);

    void handleLowerMsg(cMessage* msg) override;
    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;

    void learn_certificate(uint8_t vehicle_id);
    bool is_known_certificate(uint8_t vehicle_id);

    uint32_t beaconCount = 0;
    uint8_t vehicle_id;
    uint32_t transmissionCounter = 0;
    std::vector<uint8_t> known_certificates;
    std::vector<uint8_t> certificatesToLearn;
    std::vector<uint8_t> certificatesToShare;
    bool sendLearningRequest = false;
    bool learningResponseQueued = false;

    cMessage* sendLearningResponseEvt;

};

} // namespace veins
