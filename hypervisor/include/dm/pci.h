/*-
* Copyright (c) 2011 NetApp, Inc.
* Copyright (c) 2018 Intel Corporation
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY NETAPP, INC ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL NETAPP, INC OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*
* $FreeBSD$
*/

#ifndef PCI_H_
#define PCI_H_

#define PCIM_BAR_MEM_BASE     0xFFFFFFF0U

#define PCI_BUSMAX            0xFFU
#define PCI_SLOTMAX           0x1FU
#define PCI_FUNCMAX           0x7U

#define PCIR_VENDOR           0x00U
#define PCIR_DEVICE           0x02U
#define PCIR_COMMAND          0x04U
#define PCIR_REVID            0x08U
#define PCIR_SUBCLASS         0x0AU
#define PCIR_CLASS            0x0BU
#define PCIR_HDRTYPE          0x0EU
#define PCIM_HDRTYPE_NORMAL   0x00U
#define PCIM_MFDEV            0x80U

#define PCIC_BRIDGE           0x06U
#define PCIS_BRIDGE_HOST      0x00U

#define PCI_CONFIG_ADDR       0xCF8U
#define PCI_CONFIG_DATA       0xCFCU

#define PCI_CFG_ENABLE        0x80000000U

static inline uint32_t pci_bar_offset(uint32_t idx)
{
	return 0x10U + (idx << 2U);
}

static inline int pci_bar_access(uint32_t offset)
{
	if ((offset >= pci_bar_offset(0U))
		&& (offset < pci_bar_offset(PCI_BAR_COUNT))) {
		return 1;
	} else {
		return 0;
	}
}

#endif /* PCI_H_ */
