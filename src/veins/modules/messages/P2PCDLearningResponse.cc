//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <veins/modules/messages/P2PCDLearningResponse.h>

P2PCDLearningResponse::P2PCDLearningResponse() {
    // TODO Auto-generated constructor stub

}

P2PCDLearningResponse::~P2PCDLearningResponse() {
    // TODO Auto-generated destructor stub
}

P2PCDLearningResponse::P2PCDLearningResponse(std::vector<uint8_t> &certIDs) {
    this->certIDs = std::vector<uint8_t>(certIDs);
}

std::vector<uint8_t> P2PCDLearningResponse::get_certIDs() {
    return std::vector<uint8_t>(this->certIDs);
}

void P2PCDLearningResponse::set_certIDs(std::vector<uint8_t> &certIDs) {
    this->certIDs = std::vector<uint8_t>(certIDs);
}
