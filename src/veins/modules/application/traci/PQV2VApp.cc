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

        sendLearningResponseEvt = new cMessage("send learn response", SEND_LEARNING_RESPONSE_EVT);

    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void PQV2VApp::finish()
{
    recordScalar("learnRequestsReceived", learnRequestsReceived);
    recordScalar("learnRequestsSent", learnRequestsSent);
    recordScalar("learnResponsesReceived", learnResponsesReceived);
    recordScalar("learnResponsesSent", learnResponsesSent);
    DemoBaseApplLayer::finish();
    // statistics recording goes here

}

void PQV2VApp::onBSM(DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
//    printIDText(); EV << "Received BSM\n";
}

void PQV2VApp::onRealBSM(J2735_bsm* bsm)
{
//    EV << "Received J2735 BSM:";
//    EV << "\n\tVehicle ID: " << bsm->getVehicle_id();
//    EV << "\n\tPSID: " << bsm->getPsid() << "\n";
//    this->beaconCount++;
//    EV << "\n\tI have received a total of " << this->beaconCount << " BSMs";
}

void PQV2VApp::onECDSA_SPDU(ECDSA_SPDU* spdu) {
    printIDText(); EV << "Processing SPDU from vehicle " << std::to_string(spdu->getVehicle_id()) << "\n";
    displayKnownCertificates();
    // Deal with certificate
    if(spdu->getContains_full_certificate()) {
        if(!is_known_certificate(spdu->getVehicle_id())) {
            printIDText(); EV << "Certificate for vehicle " << std::to_string(spdu->getVehicle_id()) << " is unknown, recording\n";
            learn_certificate(spdu->getVehicle_id());
            displayKnownCertificates();
        }
        else {}
//            printIDText(); EV << "Certificate for vehicle " << std::to_string(spdu->getVehicle_id()) << " is already known, ignoring\n";
//        }
    }
    else {
        if(!is_known_certificate(spdu->getVehicle_id())) {
            printIDText(); EV << "Digest for vehicle " << std::to_string(spdu->getVehicle_id()) << " is unknown, triggering learning request\n";
            this->sendLearningRequest = true;
            this->certificatesToLearn.push_back(spdu->getVehicle_id());
            displayCertificatesToLearn();
        }
        else {}
//            printIDText(); EV << "Digest for vehicle " << std::to_string(spdu->getVehicle_id()) << " is already known, ignoring\n";
//        }
    }

    // Handle learning request (if present)
    if(spdu->getContains_learning_request()) {
        learnRequestsReceived++;
        printIDText(); EV << "Received learning request for certificates: ";
        for (auto i : spdu->getLearningRequest().get_certIDs())
            EV << std::to_string(i) << " ";
        EV << "\n";

        for(auto i : spdu->getLearningRequest().get_certIDs()) {
            if(std::find(known_certificates.begin(), known_certificates.end(), i) != known_certificates.end() &&
                    std::find(certificatesToShare.begin(), certificatesToShare.end(), i) == certificatesToShare.end())
                this->certificatesToShare.push_back(i);
        }

        if(!sendLearningResponseEvt->isScheduled()) {
            if(!certificatesToShare.empty()) {
                printIDText(); EV << "Learning response being scheduled now for vehicles";
                for(auto i: certificatesToShare) {
                    EV<< std::to_string(i) << " ";
                }
                EV << "\n";

                float delay = (std::rand() % 250) / (float)1000;
                printIDText(); EV << "========> Will transmit learning request in " << std::to_string(delay) << "\n";
                scheduleAt(simTime() + delay, sendLearningResponseEvt);
//                scheduleAt(simTime() + 0.2, sendLearningResponseEvt);
                for(uint8_t i : this->certificatesToShare) {
                    this->learningResponseTracker[i] = 0;
                }
            }
            else {
                displayKnownCertificates();
                printIDText(); EV << "I don't know any of the requested certificates, no response will be scheduled.\n";
            }
        }
        else {
            printIDText(); EV << "Learning response triggered but already scheduled\n";
        }
    }
}

void PQV2VApp::onLearningResponse(LEARNING_RESPONSE_SPDU* spdu) {
    std::vector<uint8_t> certIDs(spdu->getLearningResponse().get_certIDs());

    printIDText(); EV << "Received learning response with certificates for ";
    for(auto i : certIDs) {
        EV << std::to_string(i) << " ";
    }
    EV << "\n";

    for(auto i : certIDs) {
        if(std::find(this->known_certificates.begin(), this->known_certificates.end(), i) == this->known_certificates.end()) {
            printIDText(); EV << "Learned certificate for vehicle " << std::to_string(i) << "\n";
            this->known_certificates.push_back(i);
        }
        if(sendLearningResponseEvt->isScheduled()) {
            printIDText(); EV << "Observed and recorded learning response for certificate " << std::to_string(i) << "\n";
            if(this->learningResponseTracker.find(i) != this->learningResponseTracker.end()) {
                this->learningResponseTracker[i] = 1;
            }
            else {
                this->learningResponseTracker[i] = this->learningResponseTracker[i] + 1;
            }
        }
    }
}

void PQV2VApp::handleLowerMsg(cMessage* msg)
{
    BaseFrame1609_4* wsm = dynamic_cast<BaseFrame1609_4*>(msg);
    ASSERT(wsm);

    if(ECDSA_SPDU* spdu = dynamic_cast<ECDSA_SPDU*>(msg)) {
        receivedBSMs++;
        printIDText(); EV << "Received ECDSA_SPDU from vehicle " << std::to_string(spdu->getVehicle_id()) << "\n";
        onECDSA_SPDU(spdu);
    }
    else if(LEARNING_RESPONSE_SPDU* spdu = dynamic_cast<LEARNING_RESPONSE_SPDU*>(msg)) {
        learnResponsesReceived++;
        onLearningResponse(spdu);
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
        if(this->transmissionCounter % 20 == 0) {
            spdu->setContains_full_certificate(true);
            spdu->addBitLength(ECDSA_FULL_SPDU_SIZE_BITS - spdu->getBitLength());
        }
        else {
            spdu->setContains_full_certificate(false);
            spdu->addBitLength(ECDSA_DIGEST_SPDU_SIZE_BITS - spdu->getBitLength());
        }
        spdu->setUserPriority(beaconUserPriority);

        // P2PCD requests
        if(this->sendLearningRequest) {
            learnRequestsSent++;
            spdu->setLearningRequest(P2PCDLearningRequest(this->certificatesToLearn));

            printIDText(); EV << "Sending learning request for vehicles ";
            for(auto i : spdu->getLearningRequest().get_certIDs())
                EV << std::to_string(i) << " ";
            EV << "\n";

            spdu->setContains_learning_request(true);
            spdu->addBitLength(this->certificatesToLearn.size() * 24); // each p2pcdLearningRequest under 1609.2-2022 is HashedDigest3 (24 bits)
            this->certificatesToLearn.clear();
            this->sendLearningRequest = false;
        }
    }
    else if(LEARNING_RESPONSE_SPDU* spdu = dynamic_cast<LEARNING_RESPONSE_SPDU*>(wsm)) {
        spdu->setPsid(32);
        spdu->setChannelNumber(static_cast<int>(Channel::cch));
        spdu->addBitLength((spdu->getLearningResponse().get_certIDs().size() * 162 * 8) - spdu->getBitLength());
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

void PQV2VApp::printIDText() {
    EV << "(V" << std::to_string(this->vehicle_id) << ") " << "";
}

void PQV2VApp::displayKnownCertificates() {
    printIDText(); EV << "Known certificates: ";
    for(auto i: this->known_certificates) {
        EV << std::to_string(i) << " ";
    }
    EV << "\n";
}

void PQV2VApp::displayCertificatesToLearn() {
    printIDText(); EV << "Certificates to learn: ";
    for(auto i: this->certificatesToLearn) {
        EV << std::to_string(i) << " ";
    }
    EV << "\n";
}

void PQV2VApp::handleSelfMsg(cMessage* msg)
{
    //DemoBaseApplLayer::handleSelfMsg(msg);

    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    switch (msg->getKind()) {

    case SEND_BEACON_EVT: {
        ECDSA_SPDU* spdu = new ECDSA_SPDU();
        populateWSM(spdu);
        sendDown(spdu);
        transmissionCounter++;
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        break;
    }
    case SEND_LEARNING_RESPONSE_EVT: {

        printIDText(); EV << "Intending to share certificates for vehicles ";
        for(auto i : certificatesToShare)
            EV << std::to_string(i) << " ";
        EV << "\n";

        printIDText(); EV << "So far have heard:\n";
        for(auto i : certificatesToShare) {
            if(learningResponseTracker.find(i) != learningResponseTracker.end()) {
                printIDText(); EV << "\t" << std::to_string(learningResponseTracker[i]) << " responses for cert of vehicle " << std::to_string(i) << "\n";
                if(learningResponseTracker[i] > 3) {
                    certificatesToShare.erase(std::find(certificatesToShare.begin(), certificatesToShare.end(), i));
                }
            }
        }

        printIDText(); EV << "Actually sending certificates for vehicles ";
        for(auto i : certificatesToShare)
            EV << std::to_string(i) << " ";
        EV << "\n";

        printIDText(); EV << "Learning response sent!\n";

        LEARNING_RESPONSE_SPDU* spdu = new LEARNING_RESPONSE_SPDU();
        spdu->setLearningResponse(P2PCDLearningResponse(this->certificatesToShare));

        populateWSM(spdu);
        sendDown(spdu);

        this->certificatesToShare.clear();
        this->learningResponseTracker.clear();

        learnResponsesSent++;

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
