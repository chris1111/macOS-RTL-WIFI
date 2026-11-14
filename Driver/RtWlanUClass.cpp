/* SPDX-License-Identifier: GPL-2.0-only */
/* RtWlanU - the concrete controller class the Realtek utility discovers.
 *
 * The Realtek vendor utility/RtWlanHelper pair iterates
 * IOServiceMatching("IOEthernetController") results and accepts only services
 * whose registry IOClass reads "RtWlanU" or "RtWlanU1827" (see
 * REALTEK-APP-COMPAT-PROTOCOL.md). Publishing this subclass and pointing the
 * kext personalities' IOClass at it satisfies that filter without touching
 * the utility binaries. IOServiceMatching("RTWUSBController") and
 * IOObjectConformsTo(service, "RTWUSBController") both still match, so the
 * project's own privileged client keeps working unchanged.
 *
 * The class body is intentionally empty: all behaviour is inherited from
 * RTWUSBController, including the RTWControl v1 user client. The Realtek
 * protocol translation lives in RtWlanUCompat.cpp and is dispatched from
 * RTWUserClient::externalMethod.
 */

#include "RTWUSBController.hpp"

class RtWlanU : public RTWUSBController {
    OSDeclareDefaultStructors(RtWlanU)
};

OSDefineMetaClassAndStructors(RtWlanU, RTWUSBController)
