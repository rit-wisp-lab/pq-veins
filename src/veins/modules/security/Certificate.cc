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

#include <veins/modules/security/Certificate.h>

using namespace veins;

Certificate::Certificate() {
    // TODO Auto-generated constructor stub

}

Certificate::~Certificate() {
    // TODO Auto-generated destructor stub
}

Certificate::Certificate(const Certificate &c) {
    this->vehicle_id = c.vehicle_id;
}

uint8_t Certificate::getVehicle_id() {
    return vehicle_id;
}

void Certificate::setVehicle_id(uint8_t vehicle_id) {
    this->vehicle_id = vehicle_id;
}

