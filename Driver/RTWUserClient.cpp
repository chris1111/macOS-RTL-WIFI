/* SPDX-License-Identifier: GPL-2.0-only */
#include "RTWUserClient.hpp"
#include "RTWUSBController.hpp"
#include "RtWlanUCompat.h"
#include <kern/task.h>
#include <string.h>

#define super IOUserClient
OSDefineMetaClassAndStructors(RTWUserClient, IOUserClient)

namespace RtwWlanUCompat {
IOReturn handleExternalMethod(RTWUSBController* controller, RTWUserClient* client,
                             uint32_t selector, IOExternalMethodArguments* arguments);
}

namespace {
void wipe(void* memory, size_t size) {
    volatile uint8_t* bytes = static_cast<volatile uint8_t*>(memory);
    while (size--) *bytes++ = 0;
}
}

bool RTWUserClient::initWithTask(task_t owningTask, void* securityToken,
                                UInt32 type, OSDictionary* properties) {
    /* Opening is allowed for any task: the stock Realtek utility's GUI runs as
     * the logged-in user and must be able to connect for its read-only
     * scan/status path. Privilege is re-checked per call below - the fixed
     * RTWControl ABI and every state-changing Realtek OID remain
     * administrator-only, so an opened connection grants no capability by
     * itself. The SDK defines Administrator as "root". */
    if (!owningTask || type != RTW_CONTROL_USER_CLIENT_TYPE) return false;
    return super::initWithTask(owningTask, securityToken, type, properties);
}

bool RTWUserClient::start(IOService* provider) {
    RTWUSBController* controller = OSDynamicCast(RTWUSBController, provider);
    if (!controller || !super::start(provider)) return false;
    controller->retain();
    controller_ = controller;
    /* Build 68: unambiguous build stamp in every capture. */
    IOLog("RtWlanUCompat: user client start, driver " RTWLANU_DRIVER_VERSION "\n");
    return true;
}

void RTWUserClient::stop(IOService* provider) {
    super::stop(provider);
}

void RTWUserClient::free() {
    if (controller_) {
        controller_->release();
        controller_ = nullptr;
    }
    super::free();
}

IOReturn RTWUserClient::clientClose() {
    (void)terminate();
    return kIOReturnSuccess;
}

IOReturn RTWUserClient::externalMethod(uint32_t selector,
    IOExternalMethodArguments* arguments, IOExternalMethodDispatch* dispatch,
    OSObject* target, void* reference) {
    (void)dispatch; (void)target; (void)reference;
    if (isInactive() || !controller_ || controller_->isInactive())
        return kIOReturnNotAttached;

    /* Realtek utility compatibility protocol (selectors 0/4/6/7/9/10 in the
     * utility's call shapes). It enforces its own privilege model - read-only
     * queries and the scan trigger for any caller, association OIDs for the
     * administrator helper only - and answers kIOReturnUnsupported for any
     * selector/shape outside its protocol so the control path below still
     * applies. */
    IOReturn compat = RtwWlanUCompat::handleExternalMethod(controller_, this,
                                                           selector, arguments);
    if (compat != kIOReturnUnsupported) return compat;

    /* Recheck the caller so transferring an opened connection does not grant
     * a nonadministrator access to the driver's control interface. */
    if (clientHasPrivilege(current_task(), kIOClientPrivilegeAdministrator) != kIOReturnSuccess)
        return kIOReturnNotPrivileged;
    if (selector >= RTW_CONTROL_SELECTOR_COUNT) return kIOReturnUnsupported;
    const size_t expectedOutput = rtw_control_output_size(selector);
    if (!arguments || arguments->version != kIOExternalMethodArgumentsCurrentVersion ||
        arguments->scalarInputCount || arguments->scalarOutputCount ||
        arguments->asyncWakePort != MACH_PORT_NULL || arguments->asyncReferenceCount ||
        arguments->structureInputDescriptor || arguments->structureOutputDescriptor ||
        arguments->structureVariableOutputData ||
        !arguments->structureOutput || arguments->structureOutputSize != expectedOutput ||
        rtw_control_validate_request(selector, arguments->structureInput,
            arguments->structureInputSize) != RTW_CONTROL_VALID)
        return kIOReturnBadArgument;

    union Request {
        RTWControlRequest request;
        RTWControlJoin join;
    } request{};
    const size_t inputLength = rtw_control_input_size(selector);
    memcpy(&request, arguments->structureInput, inputLength);
    memset(arguments->structureOutput, 0, expectedOutput);
    size_t outputLength = expectedOutput;
    IOReturn result = controller_->controlRequest(selector, &request, inputLength,
        arguments->structureOutput, &outputLength);
    wipe(&request, sizeof(request));
    if (result == kIOReturnSuccess && outputLength != expectedOutput)
        result = kIOReturnInternalError;
    if (result != kIOReturnSuccess) {
        memset(arguments->structureOutput, 0, expectedOutput);
        outputLength = 0;
    }
    arguments->structureOutputSize = static_cast<uint32_t>(outputLength);
    return result;
}
