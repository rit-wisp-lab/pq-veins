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

#ifndef SRC_VEINS_MODULES_SECURITY_CERTIFICATE_H_
#define SRC_VEINS_MODULES_SECURITY_CERTIFICATE_H_

#include "veins/veins.h"

#include <stdint.h>

using namespace omnetpp;


class Certificate {
public:
    Certificate();
    Certificate(uint8_t vehicle_id);
    virtual ~Certificate();

    Certificate(const Certificate &c);

    uint8_t getVehicle_id();
    void setVehicle_id(uint8_t vehicle_id);

protected:
    uint8_t vehicle_id;
};

#endif /* SRC_VEINS_MODULES_SECURITY_CERTIFICATE_H_ */
