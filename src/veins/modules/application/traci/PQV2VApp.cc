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

#include "veins/modules/application/traci/PQV2VApp.h"

using namespace veins;

Define_Module(veins::PQV2VApp);

int PQV2VApp::vehicle_id_counter;

void PQV2VApp::initialize(int stage)
{
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        EV << "Initializing " << par("appName").stringValue() << std::endl;

        // Set vehicle ID to a unique number
        this->vehicle_id = this->vehicle_id_counter++;

    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void PQV2VApp::finish()
{
    DemoBaseApplLayer::finish();
    // statistics recording goes here
}

void PQV2VApp::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
    EV << "Received BSM\n";
}

void PQV2VApp::onRealBSM(J2735_bsm* bsm)
{
    EV << "Received J2735 BSM:";
    EV << "\n\tVehicle ID: " << bsm->getVehicle_id();
    EV << "\n\tPSID: " << bsm->getPsid() << "\n";
    this->beaconCount++;
    EV << "\n\tI have received a total of " << this->beaconCount << " BSMs";
}

void PQV2VApp::onECDSA_SPDU(ECDSA_SPDU* spdu) {
    if(spdu->getContains_full_certificate()) {
        if(!is_known_certificate(spdu->getVehicle_id())) {
            EV << "Certificate for vehicle " << std::to_string(spdu->getVehicle_id()) << "is unknown, recording\n";
            learn_certificate(spdu->getVehicle_id());
        }
        else
            EV << "Certificate for vehicle " << std::to_string(spdu->getVehicle_id()) << "is already known, ignoring\n";
    }
    else {
        if(!is_known_certificate(spdu->getVehicle_id())) {
            EV << "Digest for vehicle " << std::to_string(spdu->getVehicle_id()) << "is unknown, learning\n";
            // Trigger
        }
        else
            EV << "Digest for vehicle " << std::to_string(spdu->getVehicle_id()) << "is already known, ignoring\n";
    }
}

void PQV2VApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* wsm = dynamic_cast<BaseFrame1609_4*>(msg);
    ASSERT(wsm);
//    if(ECDSA_FULL_SPDU* spdu = dynamic_cast<ECDSA_FULL_SPDU*>(wsm)) {
//        receivedBSMs++;
//        EV << "Received ECDSA_FULL_SPDU from " << std::to_string(spdu->getVehicle_id()) << "\n";
//    }
//    else if(ECDSA_DIGEST_SPDU* spdu = dynamic_cast<ECDSA_DIGEST_SPDU*>(wsm)) {
//        receivedBSMs++;
//        EV << "Received ECDSA_DIGEST_SPDU from " << std::to_string(spdu->getVehicle_id()) << "\n";
//    }
    if(ECDSA_SPDU* spdu = dynamic_cast<ECDSA_SPDU*>(msg)) {
        receivedBSMs++;
        EV << "Received ECDSA_SPDU from vehicle " << std::to_string(spdu->getVehicle_id()) << "\n";
        onECDSA_SPDU(spdu);
    }
    else {
        DemoBaseApplLayer::handleLowerMsg(msg);
    }
}

void PQV2VApp::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples

}

void PQV2VApp::populateWSM(BaseFrame1609_4* wsm, LAddress::L2Type rcvId, int serial)
{
    wsm->setRecipientAddress(rcvId);
    wsm->setBitLength(headerLength);

    if (ECDSA_SPDU* spdu = dynamic_cast<ECDSA_SPDU*>(wsm)) {
            spdu->setVehicle_id(this->vehicle_id);
            spdu->setPsid(32);
            spdu->setChannelNumber(static_cast<int>(Channel::cch));
            if(this->transmissionCounter % 5 == 0) {
                spdu->setContains_full_certificate(true);
                spdu->addBitLength(ECDSA_FULL_SPDU_SIZE_BITS - spdu->getBitLength());
            }
            else {
                spdu->setContains_full_certificate(false);
                spdu->addBitLength(ECDSA_DIGEST_SPDU_SIZE_BITS - spdu->getBitLength());
            }
            spdu->setUserPriority(beaconUserPriority);
        }
    else {
        DemoBaseApplLayer::populateWSM(wsm);
    }
}

void PQV2VApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void PQV2VApp::handleSelfMsg(cMessage* msg)
{
    //DemoBaseApplLayer::handleSelfMsg(msg);

    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    switch (msg->getKind()) {

    case SEND_BEACON_EVT: {
//        if(transmissionCounter % 5 == 0) {
//            ECDSA_FULL_SPDU* spdu = new ECDSA_FULL_SPDU();
//            populateWSM(spdu);
//            sendDown(spdu);
//        }
//        else {
//           ECDSA_DIGEST_SPDU* spdu = new ECDSA_DIGEST_SPDU();
//           populateWSM(spdu);
//           sendDown(spdu);
//        }
        ECDSA_SPDU* spdu = new ECDSA_SPDU();
        populateWSM(spdu);
        sendDown(spdu);
        transmissionCounter++;
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        break;
    }
    default: {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }

    }
}

void PQV2VApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class
}

void PQV2VApp::learn_certificate(uint8_t vehicle_id) {
    known_certificates.push_back(vehicle_id);
}
bool PQV2VApp::is_known_certificate(uint8_t vehicle_id) {
    return std::find(known_certificates.begin(), known_certificates.end(), vehicle_id) != known_certificates.end();
}


//void PQV2VApp::learn_certificate(Certificate* certificate) {
//    this->known_certificates.push_back(*certificate);
//}
//
//bool PQV2VApp::is_known_certificate(Certificate* certificate) {
//    auto idx = std::find_if(this->known_certificates.begin(),
//                            this->known_certificates.end(),
//                            [&certificate](Certificate& obj) {
//                            return obj.getVehicle_id() == certificate->getVehicle_id();
//                            });
//
//    return idx != this->known_certificates.end();
//}
