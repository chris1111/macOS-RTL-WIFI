#pragma once
/* RTL8192EU USB station controller. The imported net80211 stack owns RSN,
 * software CCMP, replay checks and controlled-port authorization. */
#include <IOKit/network/IOEthernetController.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/IOInterruptEventSource.h>
#include <IOKit/IOTimerEventSource.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <kern/thread_call.h>
#include <IOKit/usb/IOUSBHostInterface.h>
#include <IOKit/usb/IOUSBHostPipe.h>
#include "RTW8192EUHAL.hpp"
#include "RTWControl.h"
#include "rtw/USBDescriptors.h"
#include "rtw/RTL8192EUDataPath.h"
#include "rtw/USBPackets.h"
#include "rtw/USBRXCapacity.h"
#include "rtw/USBWiFiProfiles.h"
#include "rtw/FamilyBackend.h"
#include "rtw/SignalSample.h"
#include "rtw/DFSStation.h"
#include "rtw/ScanDiagnostics.hpp"
#include "rtw/USBWiFiFirmware.h"
#include "rtw/RTL8192CDataPath.h"
#include "rtw/RTL8192EU.h"
#include "rtw/RTL8814AU.h"
#include "rtw/RTL8814AUPHY.h"
#include "rtw/RTL8814AUDataPath.h"
#include "rtw/RTL8814AUStation.h"
#include "rtw/RTL8812AURuntime.h"
#include "rtw/RTL8821AUCoex.h"
#include "rtw/RTL8192FU.h"
#include "rtw/RTL8710BU.h"
#include "rtw/RTL8723BUDataPath.h"
#include "rtw/RTL8192DUStation.h"
#include "rtw/RTL8192DUUSB.h"
#include "rtw/RTL8192DUDataPath.h"
#include "rtw/RTW88PHYOperating.h"
#include "rtw/RTW88Coex.h"
#include "rtw/RTW88Station.h"
#include "rtw/RTW88RX.h"
#include "rtw/RTW88USBDataPath.h"
#include <net80211/ieee80211_amrr.h>

class RTWUSBController : public IOEthernetController {
    OSDeclareDefaultStructors(RTWUSBController)
public:
    bool start(IOService *) override;
    void stop(IOService *) override;
    void free() override;
    IOWorkLoop *getWorkLoop() const override;
    IOReturn getHardwareAddress(IOEthernetAddress *) override;
    IOReturn enable(IONetworkInterface *) override;
    IOReturn disable(IONetworkInterface *) override;
    UInt32 outputPacket(mbuf_t, void *) override;
    UInt32 getFeatures() const override;
    IOReturn getPacketFilters(const OSSymbol *, UInt32 *) const override;
    IOReturn setMulticastMode(bool) override;
    IOReturn setMulticastList(IOEthernetAddress *, UInt32) override;
    IOReturn selectMedium(const IONetworkMedium *) override;
    bool setLinkStatus(UInt32, const IONetworkMedium * = nullptr,
                       UInt64 = 0, OSData * = nullptr) override;
    IOReturn message(UInt32, IOService *, void * = nullptr) override;
    IOReturn setPowerState(unsigned long, IOService *) override;
    IOReturn newUserClient(task_t, void *, UInt32, OSDictionary *, IOUserClient **) override;
    IOReturn controlRequest(uint32_t, const void *, size_t, void *, size_t *);
    /* ---- IONetworkController vtable ABI bridge (R28-monterey-vtable) ----
     * The inherited IONetworkController vtable differs across macOS releases:
     *   Monterey (Darwin 21) keeps the private-SPI layout - slots for
     *   outputStart / setInputPacketPollingEnable / pollInputPackets /
     *   networkInterfaceNotification followed by two _RESERVED pad slots.
     *   The SDK 26.x headers instead declare _RESERVEDIONetworkController2-5
     *   there and consume the pad slots 6/7 with allocatePacketNoWait(UInt32)
     *   and setHardwareAssists(UInt32, UInt32).
     * kxld refuses to load a kext whose inherited vtable entry lands on a
     * parent pad slot (kxld_vtable.c rule 4, kKxldLogParentOutOfDate), so a
     * kext built against one layout is rejected by the other OS outright -
     * this was the "works on Sequoia/Tahoe, never loads on Monterey" bug.
     * Overriding all six slots locally removes them from kxld's patching
     * entirely: one binary then loads on both layouts, and the bodies
     * reproduce the parent defaults exactly (kIOReturnUnsupported / no-op /
     * a real mbuf allocation) so runtime behaviour is unchanged everywhere.
     * NOTE: this bridge matches the SDK 26.x declarations used by mk.tool.
     * If the project ever builds against MacKernelSDK (legacy layout), the
     * four _RESERVEDIONetworkController2-5 overrides must be swapped for
     * outputStart/setInputPacketPollingEnable/pollInputPackets/
     * networkInterfaceNotification overrides and the two pad slots 6/7
     * bridged as _RESERVEDIONetworkController6/7 instead. */
    void _RESERVEDIONetworkController2() override;
    void _RESERVEDIONetworkController3() override;
    void _RESERVEDIONetworkController4() override;
    void _RESERVEDIONetworkController5() override;
    mbuf_t allocatePacketNoWait(UInt32) override;
    IOReturn setHardwareAssists(UInt32, UInt32) override;
protected:
    bool configureInterface(IONetworkInterface *) override;
    IOOutputQueue *createOutputQueue() override;
private:
    enum { RXSlots = 4, TXCapacity = 4352 };
    enum GateOperation { Initialize = 1, Shutdown, Enable, Disable, Output,
                         Control, Terminated, Sleep, Wake, TeardownReady,
                         /* 1.0.5/1.0.6: captures the device provider for the
                          * software re-plug while the gate is held; the
                          * re-enumeration itself runs off-gate. */
                         SoftwareReplug };
    struct Transfer {
        RTWUSBController *owner;
        IOBufferMemoryDescriptor *memory;
        IOUSBHostPipe *pipe;
        unsigned index;
        uint32_t submitted, actual, generation;
        IOReturn result;
        bool prepared;
    };
    struct OutputCall { mbuf_t packet; bool consumed; };
    struct ControlCall { uint32_t selector; const void *input; size_t length;
                         void *output; size_t *outputLength; };
    IOUSBHostInterface *usb_ = nullptr;
    IOUSBHostPipe *rxPipe_ = nullptr;
    IOUSBHostPipe *txPipes_[4]{};
    uint16_t txMPS_[4]{};
    rtw_usb_endpoints endpoints_{};
    IOWorkLoop *loop_ = nullptr;
    IOCommandGate *gate_ = nullptr;
    IOInterruptEventSource *completionEvent_ = nullptr;
    IOTimerEventSource *scanTimer_ = nullptr, *calibTimer_ = nullptr, *watchdogTimer_ = nullptr;
    IOSimpleLock *completionLock_ = nullptr;
    IOLock *lifecycleLock_ = nullptr;
    Transfer transfers_[RTW_TRANSFER_SLOTS]{};
    rtw_transfer_ledger ledger_{};
    /* Only the observed zero-byte RX transaction error is eligible, and only
     * on integrated station backends sharing this IOUSBHost RX transport.
     * All old endpoint reads must drain before a new RX epoch is submitted. */
    struct RXRecovery {
        enum Stage : uint32_t { Idle=0, Clear=1, RetryWait=2, Drain=3, Refill=4, Complete=5, Failed=6, Cancelled=7 };
        uint64_t errors = 0, attempts = 0, completed = 0, failures = 0, exhausted = 0;
        uint64_t clearAttempts = 0, clearFailures = 0, refillFailures = 0, transportRetries = 0, transportExhausted = 0;
        uint64_t startedMS = 0, nextAttemptMS = 0;
        uint32_t consecutive = 0, clearAttempt = 0, refillSlot = RXSlots;
        IOReturn originalError = kIOReturnSuccess, lastError = kIOReturnSuccess;
        IOReturn clearResult = kIOReturnSuccess, refillResult = kIOReturnSuccess;
        Stage stage = Idle, failureStage = Idle;
        uint8_t endpoint = 0;
        bool pending = false, cleared = false, refilling = false;
    } rxRecovery_{};
    rtwn_softc core_{};
    rtw_signal_sample signal_{};
    rtw::ScanDiagnostics scanDiag_{};
    uint64_t diagnosticPublicationGeneration_ = 0, diagnosticPublicationMonotonicMS_ = 0;
    const rtw_usb_wifi_profile *profile_ = nullptr;
    rtw_wifi_family family_ = RTW_WIFI_UNKNOWN;
#include "RTW8812BackendMembers.inc"
#include "RTW88BackendMembers.inc"
#include "RTW8192DUBackendMembers.inc"
#include "RTW8192FUBackendMembers.inc"
#include "RTW8710BUBackendMembers.inc"
#include "RTW8723BUBackendMembers.inc"
    rtw_8814au acCore_{};
    void *acPHYStorage_ = nullptr;
    size_t acPHYSize_ = 0;
    rtw_8814au_rate_config acRateConfig_{};
    bool acMediaConnected_ = false, acRateReady_ = false;
    uint8_t acRSSI_ = 0, acChannel_ = 0;
    device device_{};
    IOEthernetInterface *netif_ = nullptr;
    IONetworkStats *stats_ = nullptr;
    IOEthernetAddress mac_{};
    IOReturn usbError_ = kIOReturnSuccess;
    uint64_t txPackets_ = 0, rxPackets_ = 0, txErrors_ = 0, rxErrors_ = 0;
    uint64_t txQueueDrops_ = 0, txUSBTransferErrors_ = 0, txLinkDrops_ = 0;
    /* 1.0.20: DHCP TX verdict accounting - the 2026-11-10 field capture
     * proved the lease dies inside the TX builder (errno=22, sent=0), so
     * every builder failure exit names itself and DHCP frames are counted. */
    uint64_t txDhcpSeen_ = 0, txDhcpSent_ = 0, txDhcpDrops_ = 0, txOtherDrops_ = 0;
    uint64_t lastTxDhcpLogMs_ = 0, lastTxOtherDropLogMs_ = 0;
    /* R24: DHCP TX-report probe. During a dead lease the only frames that
     * carry an 88E CCX report request ARE the DHCP probes, so each verdict
     * names the firmware's own outcome for the DISCOVER: transmitted,
     * dropped internally, or never attempted. */
    uint64_t dhcpTxReports_ = 0, dhcpReportWaits_ = 0;
    void ccmpSelfTest(); char txProtHex_[3][224] = {}; unsigned txProtHexCount_ = 0;
    uint64_t txPipeFrames_[4] = {}, txMgmtFrames_ = 0, txDataFrames_ = 0, txEapolSent_ = 0, lastEapolPipe_ = 0, lastEapolQsel_ = 0, lastEapolRate_ = 0, lastEapolLen_ = 0, lastEapolQos_ = 0;
    uint32_t faultCode_ = 0, faultSite_ = 0, faultDetail_ = 0, faultIcState_ = 0, faultRunning_ = 0;
    uint64_t faultAfterAttachMS_ = 0, coexErrAfterAttachMS_ = 0;
    uint64_t joinBringUpRetries_ = 0, attachBringUpRetries_ = 0;
    uint32_t r88BootSub_ = 0, r88StartFailSub_ = 0, r88StartFailDetail_ = 0;
    uint32_t r88StartFailStage_ = 0, r88StartFailErr_ = 0, r88StartFails_ = 0;
    uint32_t coexErr_ = 0, coexErrSite_ = 0, coexErrSiteLatched_ = 0, coexTickFails_ = 0, coexTickErrors_ = 0;
    uint64_t joinStateMS_[5] = {}, disableIgnored_ = 0, txReportCalls_ = 0, txReportLastLen_ = 0, rxDataFrames_ = 0, rxDataToUs_ = 0, rxDataBcast_ = 0, rxMgmtFrames_ = 0;
    uint64_t eapolReportPending_ = 0, eapolReports_ = 0, eapolRepOK_ = 0, eapolRepRetryOver_ = 0, eapolRepLifetime_ = 0, eapolRepRetries_ = 0, eapolRepRate_ = 0;
    uint64_t dhcpRepOK_ = 0, dhcpRepRetryOver_ = 0, dhcpRepLifetime_ = 0, dhcpRepRetries_ = 0, dhcpRepRate_ = 0;
    unsigned dhcpReportPending_ = 0;
    uint64_t lastDhcpReportCheckMs_ = 0;
    struct RateControl {
        ieee80211_amrr policy{};
        ieee80211_amrr_node node{};
        uint64_t requests = 0, pendingReports = 0, reports = 0;
        uint64_t failures = 0, retried = 0, invalid = 0, ignored = 0;
        uint64_t repeatedSequences = 0, changes = 0, resets = 0, htSampleCounter = 0;
        uint8_t bssid[6]{}, lastSequence = 0, finalRate = 0;
        bool active = false, seenSequence = false, requestedSinceReset = false;
    } rateControl_{};
    bool rateTransportFresh_ = false;
    uint64_t txHTPackets_ = 0, txAMPDUPackets_ = 0, rxBARPackets_ = 0;
    static int ampduRXStart(ieee80211com *, ieee80211_node *, uint8_t);
    static int ampduTXStart(ieee80211com *, ieee80211_node *, uint8_t);
    static void ampduRXStop(ieee80211com *, ieee80211_node *, uint8_t);
    static void ampduTXStop(ieee80211com *, ieee80211_node *, uint8_t);
    bool usesFirmwareRate() const;
    bool usesLegacyFirmwareRate() const;
    bool uses8188FirmwareRate() const;
    rtw_packet_error decodeUSBFrame(rtw_rx_iterator *, rtw_8192eu_rx_frame *, uint8_t *) const;
    struct RXDiagnostics {
        uint64_t aggregates = 0, bytes = 0, aggregateErrors = 0, c2h = 0;
        /* Nonempty successful reads; error length refers to structural preflight. */
        uint64_t lastReadBytes = 0, maxReadBytes = 0, lastAggregateErrorReadBytes = 0;
        /* Reason of failed aggregate preflight; no packet data exposed. */
        uint64_t aggregateTruncated = 0, aggregateMalformed = 0;
        uint64_t aggregateTooMany = 0, aggregateBadArgument = 0;
        uint64_t lastAggregateErrorOffset = 0, lastAggregateErrorFrames = 0;
        /* Frames in a structurally valid prefix of a damaged transfer;
         * subject to later CRC, security and net80211 admission checks. */
        uint64_t partialAggregates = 0, partialPrefixFrames = 0;
        uint64_t partialPrefixBytes = 0;
        uint64_t crcDrops = 0, icvDrops = 0, invalidFrameDrops = 0;
        uint64_t hardwareDecryptDrops = 0, allocationDrops = 0;
        uint64_t copyDrops = 0, noNodeDrops = 0, protectedMPDUs = 0;
        uint64_t ethernetQueued = 0, handoffErrors = 0;
    } rxDiag_{};
    /* Observed associated downlink MPDUs before software decrypt/replay checks.
     * Cumulative raw MPDU counts/bytes are not delivered Ethernet throughput. */
    struct PerformanceDiagnostics {
        uint64_t rxRatePackets[12]{}, rxRateBytes[12]{};
        uint64_t rxMCSPackets[16]{}, rxMCSBytes[16]{};
        uint64_t rxVHTPackets[20]{},rxVHTBytes[20]{};
        uint64_t rxVHT20Packets=0,rxVHT40Packets=0,rxVHT80Packets=0,rxVHTUnknownBWPackets=0;
        uint64_t rxUnknownPackets = 0, rxUnknownBytes = 0;
        uint64_t rxHT20Packets = 0, rxHT40Packets = 0, rxHTUnknownBWPackets = 0;
        uint8_t rxLastCode = 255, rxLastRate500Kbps = 0;
    } perfDiag_{};
    uint64_t lastBeaconMs_ = 0;
    bool linkActivePosted_ = false;
    /* 1.0.15: the beacon-freshness guard proved blind to a dead IPv4 data
     * path: after an RF-off window the station re-associates on its own,
     * beacons keep flowing, but DHCP never completes again (field capture
     * 2026-11-09 06:11: en5 parked at 169.254.245.61 with RUN + fresh
     * beacons). Count the stack's DHCP DISCOVERs on the TX path instead:
     * configd only keeps re-sending DISCOVER when it has no lease, so a
     * repeating burst is unambiguous proof the data path is stuck. */
    uint64_t dhcpDiscovers_ = 0, lastDhcpDiscoverMs_ = 0, lastDhcpHealMs_ = 0;
    unsigned dhcpHeals_ = 0;
    /* 1.0.16: lease-silence watchdog state. episodeStartMs_ is stamped on
     * every carrier-UP edge (refreshLinkEvent); episodeDiscovers_ counts
     * the DISCOVERs of the current episode; everSawDhcp_ exempts hosts
     * that never used DHCP (static IP). */
    uint64_t episodeStartMs_ = 0, lastLeaseKickMs_ = 0, lastDhcpReplyMs_ = 0;
    unsigned episodeDiscovers_ = 0, leaseKicks_ = 0, episodeReplies_ = 0;
    bool everSawDhcp_ = false;
    /* 1.0.18: backoff ladder for failed rebuilds + broadcast RX probe */
    unsigned dhcpHealMisses_ = 0;
    unsigned rxBcast_ = 0;
    unsigned rxUcast_ = 0; /* R23: unicast side of the RX probe */
    /* R24 probe state lives with the DHCP TX counters above. */
    uint64_t lastBcastLogMs_ = 0;
    /* 1.0.17: shared rebuild budget + TX gate visibility */
    uint64_t lastHealMs_ = 0, lastTxGateLogMs_ = 0;
    unsigned heals_ = 0;
    bool dhcpStuck() const;
    void noteTxDhcpDiscover(mbuf_t);
    void noteRxDhcpReply(mbuf_t);
    void autoHealStuckDhcp();
    void autoHealSilentLease();
    rtw_dfs_station dfsStation_{};
    uint8_t dfsTunedChannel_ = 0, dfsBoundChannel_ = 0, dfsBoundBSSID_[6]{};
    bool dfsBound_ = false, dfsTXHold_ = false;
    uint8_t dfsAppliedReduction_ = 0;
    int32_t dfsAppliedLimitMBM_ = INT32_MAX;
    int prepareDFSPower(unsigned);
    int candidateDFSPower(uint8_t,int32_t *,uint8_t *) const;
    bool dfsTXBlocked() const;
    bool dfsStateAllowed(ieee80211_state,unsigned) const;
    void dfsStateChanged(ieee80211_state,unsigned,int);
    bool dfsFrameAllowed(mbuf_t,ieee80211_node *) const;
    void observeDFSFrame(const uint8_t *,size_t,unsigned);
    int pauseDFSStationTX();
    void advanceDFSRecovery();
    void resetDFSStation(bool releaseTX = false);
    void stopDFSStation();
    struct CalibrationDiagnostics {
        uint64_t calls = 0, generation = 0, lastStartMS = 0;
        uint64_t lastMS = 0, maxMS = 0, thermalChanges = 0, thermalIQs = 0;
        uint64_t lastH2CPolls = 0, maxH2CPolls = 0, totalH2CPolls = 0;
        uint64_t lastH2CReadMS = 0, maxH2CReadMS = 0;
        uint32_t thermalBefore = 0, thermalAfter = 0, thermalState = 0;
        bool active = false;
    } calibDiag_{};
    enum WPARecoveryState { WPARecoveryIdle, WPARecoveryHandshake,
                            WPARecoverySearching, WPARecoveryAuthorized,
                            WPARecoveryFailed };
    struct WPARecovery {
        uint64_t lastMS = 0, remainingMS = 0, timeouts = 0;
        uint32_t retries = 0, state = WPARecoveryIdle, error = 0;
        uint32_t lastSupplicantState = 0;
        bool active = false;
    } wpaRecovery_{};
    /* 1.0.7: a spent WPA handshake budget must NEVER freeze the station.
     * 1.0.6 latched WPARecoveryFailed and the state-machine gate then
     * rejected every AUTH/ASSOC/RUN with ETIMEDOUT until the kext
     * reloaded - a field shape of "join accepted, silence forever, only a
     * kext reload helps". wpaStandDown_ replaces the latch: one join
     * request gets four handshake windows; when they are spent the driver
     * stands down (scans continue, nothing is torn down) and the flag is
     * cleared by the NEXT join request, which buys a fresh budget. */
    bool wpaStandDown_ = false;
    bool usbOpened_ = false, present_ = false, runtimeLease_ = false;
    bool attached_ = false, running_ = false, enabled_ = false;
    bool radioReady_ = false, joinRequested_ = false, sleeping_ = false, everReachedAuth_ = false;
    bool tx88Auth_ = false, tx88Stall_ = false; void snapshotTx88(const char*tag);
    bool stopping_ = false, powerManaged_ = false, retainedForPending_ = false;
    bool macReady_ = false, closing_ = false;
    thread_call_t cleanupCall_ = nullptr;
    bool teardownReady_ = false, cleanupQueued_ = false;
    void markTeardownReady();
    void scheduleDeferredCleanup();
    static void deferredCleanup(thread_call_param_t, thread_call_param_t);
    /* 1.0.5 boot-wedge cure. Field log 2026-11-17 21:46: after a reboot the
     * kext can claim the dongle before the USB port is fully ready; the
     * controller registers (MAC/version OIDs answer) but every chip
     * bring-up fails and the watchdog exits early while !running_, so
     * nothing ever retries - only a physical replug fixed it. This is the
     * same cure in software: IOUSBHostDevice::reset() on the USB device
     * provider (Apple's replacement for the legacy IOUSBDevice::
     * ReEnumerateDevice, declared in every SDK from 10.11 through macOS 26)
     * re-runs port reset + enumeration exactly like replug. */
    thread_call_t reenumCall_ = nullptr;
    bool reenumQueued_ = false;
    uint32_t wedgedBringUps_ = 0, reenumFires_ = 0;
    bool bringUpEverSucceeded_ = false;
    void scheduleSoftwareReplug(const char *why, bool timeBased = false);
    static void reenumerateCall(thread_call_param_t, thread_call_param_t);
    /* 1.0.6 reboot-2 cure (field report 2026-11-18: "reboot 1 ok but not
     * work after reboot 2"). The failure ladder above needs its 4th counted
     * bring-up failure, but in the field wedge the FIRST failed bring-up
     * latches usbError_ (fault()), so every later enableStation() call
     * early-outs at the guards and nothing ever counts - the re-plug never
     * fired. rescueTimer_ ticks for the first minutes of every attach and
     * fires the same software re-plug on TIME instead: bring-up never
     * succeeded within 45 s of a real attempt, or a requested join never
     * authorized within 120 s while the station runs as a zombie. The
     * rescue is disarmed for good the moment the user first sees a working
     * connection (bootEverAuthorized_ in setLinkStatus) and outside the
     * boot window, so a healthy session is never replugged. */
    IOTimerEventSource *rescueTimer_ = nullptr;
    bool bootEverAuthorized_ = false, bootRescueArmed_ = false;
    bool rescueWhyLogged_ = false;
    uint64_t bootAttachCompleteMS_ = 0, firstJoinRequestedMS_ = 0;
    /* 1.0.7 field fix (RTLWiFiBar.log 2026-11-18 06:21-06:23): the zombie
     * clock was anchored at the FIRST join of the attach, so a join the
     * user submitted minutes later could be killed mid-association by a
     * re-plug armed from a dead first attempt. The rescue now judges the
     * CURRENT requested join: lastJoinRequestedMS_ is stamped on every
     * accepted join and the zombie rule fires only when THIS join has
     * been unauthorized for 120 s (and never while AUTH/ASSOC run). */
    uint64_t lastJoinRequestedMS_ = 0;
    uint64_t lastRunMs_ = 0; /* 1.0.11: last time ic_state reached RUN, for the flappy-association re-arm */
    /* 1.0.16: the 1.0.15 cold-attach gate members were removed together
     * with the gate itself (deadlock with the app's scan suppression). */
    IOReturn executeJoinTail(const RTWControlJoin *join);
    uint32_t bringUpAttempts_ = 0;
    static void bootRescueTick(OSObject *, IOTimerEventSource *);
    /* 1.0.9 session rescue (RTLWiFiBar.log 2026-11-18 07:42-07:48, driver
     * 1.0.8: the DHCP auto-heal's restart latched usbError_ mid-session and
     * the boot rescue refused to fire because bootEverAuthorized_ was true -
     * the dongle sat dead, 0xe00002d8 on every scan, for 6+ minutes until a
     * physical replug). The app's 1 Hz status poll lands in the NotReady
     * guard of handleControl while the transport is latched; once the fault
     * persists 20 s the driver re-plugs itself (max 2 per attach, the
     * boot rescue's global per-boot ceiling still applies). */
    uint64_t sessionFaultSinceMS_ = 0;
    uint32_t sessionReplugFires_ = 0;
    void considerSessionRescue();

    static IOReturn gated(OSObject *, void *, void *, void *, void *);
    static void usbCompletion(void *, void *, IOReturn, uint32_t);
    static void completionAction(OSObject *, IOInterruptEventSource *, int);
    static void scanAction(OSObject *, IOTimerEventSource *);
    static void calibAction(OSObject *, IOTimerEventSource *);
    static void watchdogAction(OSObject *, IOTimerEventSource *);
    static RTWUSBController *owner(void *);
    static uint8_t read1(void *, uint16_t);
    static uint16_t read2(void *, uint16_t);
    static uint32_t read4(void *, uint16_t);
    static void write1(void *, uint16_t, uint8_t);
    static void write2(void *, uint16_t, uint16_t);
    static void write4(void *, uint16_t, uint32_t);
    static void delayUS(void *, uint32_t);
    static int writeRegion(void *, uint16_t, const uint8_t *, size_t);
    static int ioError(void *);
    static int busPower(void *), busDMA(void *), busFirmware(void *, int, uint8_t *, int);
    static int loadFirmware(void *, u_char **, size_t *);
    static void busMAC(void *), busBB(void *), busAggregation(void *);
    static int busAllocate(void *), busInitialize(void *), busBusy(void *);
    static void busStop(void *), nextScan(void *), cancelScan(void *);
    static void nextCalibration(void *), cancelCalibration(void *), waitAsync(void *);
    static int transmit(void *, mbuf_t, ieee80211_node *);
    static int stateTransition(ieee80211com *, ieee80211_state, int);
    void observeScanTransition(ieee80211_state,ieee80211_state,unsigned,int);
    static int acRead(void *, uint16_t, unsigned, uint32_t *);
    static int acWrite(void *, uint16_t, unsigned, uint32_t);
    static int acBulk(void *, uint8_t, const uint8_t *, size_t);
    rtw_8814au_speed acSpeed() const;
    int attach8814(), start8814(), set8814Channel(uint8_t);
    void stop8814();
    int state8814(ieee80211com *, ieee80211_state, int);
    static void update8814EDCA(ieee80211com *);
    static void watchdog8814(_ifnet *), initTask8814(void *);
    static int ioctl8814(_ifnet *, u_long, caddr_t);
    rtw_8814au_station_params station8814Params() const;
    bool uses8814FirmwareRate() const;
    void handle8814C2H(const uint8_t *, size_t);

    bool discoverUSB();
    void reportStartStage(const char *, IOReturn = kIOReturnSuccess);
    IOReturn initializePlatform();
    IOReturn enableStation();
    void quiesce(bool eraseProfile);
    void softLeave(); /* 1.1.2: leave the network, chip stays powered */
    void refreshLinkEvent();
    bool rebuildLink(const char *why);
    void shutdownPlatform();
    void closeUSBProvider();
    void releasePlatform();
    void eraseProfile();
    void fault(IOReturn error, uint32_t site = 0, uint32_t detail = 0);
    uint32_t readRegister(uint16_t, uint8_t);
    int writeRegister(uint16_t, uint8_t, uint32_t);
    int vendorRequest(bool, uint16_t, uint8_t *, size_t);
    int allocateTransfers();
    void freeTransfers();
    int submit(unsigned, IOUSBHostPipe *, uint32_t);
    bool beginRXRecovery(IOReturn);
    void advanceRXRecovery();
    void failRXRecovery(bool exhausted = false);
    void publishRXRecoveryDiagnostics();
    void recordCompletion(Transfer *, IOReturn, uint32_t, uint32_t);
    IOCommandGate *copyGate(IOWorkLoop **);
    static uint64_t nowMS();
    void publishCalibrationDiagnostics();
    bool associatedBeacon(const uint8_t *, size_t) const;
    void drainCompletions(bool);
    void serviceTXOutputQueue();
    void abortTransfers();
    void receive(const uint8_t *, size_t);
    void publishDiagnosticCounter(const char *, uint64_t);
    void publishDiagnostics();
    void publishRateControlDiagnostics();
    void publishScanDiagnostics();
    void publishWpaDiagnostics();
    void observeReceivedRate(const rtw_rx_packet &);
    void publishPerformanceDiagnostics();
    void resetWpaRecovery();
    void cancelWpaRecovery();
    void wpaStateChanged(ieee80211_state);
    void advanceWpaRecovery();
    void publishWpaRecoveryDiagnostics();
    int sendFrame(mbuf_t, ieee80211_node *);
    void resetRateControl();
    void handleTxReport(const uint8_t *, size_t);
    void chooseRateControl();
    bool hasFreeTX() const;
    bool authorized() const;
    uint32_t supported5GHzFlags() const;
    bool permitsDFSChannelByCountry(uint8_t) const;
    int16_t factoryDFSChannelLimitMBM(uint8_t) const;
    IOReturn handleControl(ControlCall *);
    void status(RTWControlStatus *);
    void scanResults(RTWControlScanResults *);
    void channelMap(RTWControlChannelMap *) const;
};
