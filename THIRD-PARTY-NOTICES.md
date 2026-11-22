# Third-party notices

macOS-RTL-WIFI is licensed under GPL-2.0 (see `LICENSE-GPL-2.0.txt`). It
includes, or is built from, the third-party components below. Each keeps its
own license; this file reproduces the notices that those licenses require to
accompany a distribution. The per-file copyright headers in the source tree are
retained and remain authoritative.

## Source components

| Component | Where | License |
|---|---|---|
| OpenBSD `net80211`, `rtwn` and crypto code | `Stack/itl80211/openbsd/`, `Stack/rtwn/` | BSD / ISC, per-file notices retained |
| OpenIntelWireless `itlwm` (net80211 macOS integration) | `Stack/itl80211/` | GPL-2.0 (`Stack/itl80211/LICENSE.itlwm`) |
| wireless-regdb (regulatory data) | `Stack/wireless-regdb/` | ISC (`Stack/wireless-regdb/LICENSE`) |
| Linux, FreeBSD and lwfinger drivers (register tables, algorithms, references) | `Core/`, `Driver/` | GPL-2.0 or BSD, see `CREDITS.md`, `SOURCES.md` and the `*-LICENSE.txt` files in `Core/` |

The following small crypto files in `Stack/itl80211/openbsd/crypto/` carry no
header in this tree: `sha1.cpp`, `poly1305.cpp`, `rijndael.cpp`, `cast.cpp`.
Their upstream OpenBSD versions are public-domain or BSD-licensed
implementations (SHA-1 by Steve Reid, Poly1305 by Andrew Moon, Rijndael by
Vincent Rijmen, Antoon Bosselaers and Paulo Barreto, CAST by the OpenBSD
authors).

## Firmware images (Realtek)

The firmware images embedded in `Driver/*Firmware.inc` are **separately
licensed binary data written by Realtek**. They are not covered by the GPL-2.0
license of this project, they are not modified, and they are redistributed
under the terms below. Neither license permits reverse engineering,
decompiling or disassembling the firmware. Each `.inc` file states its source
and SHA256.

### A. Realtek `rtlwifi` firmware license

Applies to: RTL8192DU, RTL8192FU, RTL8710BU, RTL8723BU, RTL8812AU, RTL8814AU,
RTL8821AU, RTL8821CU, RTL8822BU and RTL8822CU firmware.

```
Copyright (c) 2010, Realtek Semiconductor Corporation 
All rights reserved.

Redistribution.  Redistribution and use in binary form, without 
modification, are permitted provided that the following conditions are 
met:

* Redistributions must reproduce the above copyright notice and the 
  following disclaimer in the documentation and/or other materials 
  provided with the distribution. 
* Neither the name of Realtek Semiconductor Corporation nor the names of its
  suppliers may be used to endorse or promote products derived from this
  software without specific prior written permission. 
* No reverse engineering, decompilation, or disassembly of this software 
  is permitted.

Limited patent license. Realtek Semiconductor Corporation grants a world-wide, 
royalty-free, non-exclusive license under patents it now or hereafter 
owns or controls to make, have made, use, import, offer to sell and 
sell ("Utilize") this software, but solely to the extent that any 
such patent is necessary to Utilize the software alone, or in 
combination with an operating system licensed under an approved Open 
Source license as listed by the Open Source Initiative at 
http://opensource.org/licenses.  The patent license shall not apply to 
any other combinations which include this software.  No hardware per 
se is licensed hereunder.

DISCLAIMER.  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
DAMAGE.
```

### B. `urtwn` firmware license

Applies to: RTL8188EU, RTL8192CU/CT and RTL8192EU firmware (from the OpenBSD
`urtwn` microcode, pinned in `Firmware/legacy-firmware-provenance.json`).

```
* Copyright (C) 2009-2021 Realtek Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Realtek Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * 4. No reverse engineering, decompilation, or disassembly of this software
 *    is permitted.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.

This license applies to the firmware loaded for the urtwn(4) driver.
```

## Trademarks

Realtek is a trademark of Realtek Semiconductor Corp. This project is
independent and is not affiliated with or endorsed by Realtek.
