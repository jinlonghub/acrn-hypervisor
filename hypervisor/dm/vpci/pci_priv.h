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

#ifndef PCI_PRIV_H_
#define PCI_PRIV_H_

#include <pci.h>

static inline uint8_t
pci_vdev_read_cfg_u8(struct pci_vdev *vdev, uint32_t offset)
{
	return (*(uint8_t *)(&vdev->cfgdata[0] + offset));
}

static inline uint16_t pci_vdev_read_cfg_u16(struct pci_vdev *vdev,
	uint32_t offset)
{
	return (*(uint16_t *)(&vdev->cfgdata[0] + offset));
}

static inline uint32_t pci_vdev_read_cfg_u32(struct pci_vdev *vdev,
	uint32_t offset)
{
	return (*(uint32_t *)(&vdev->cfgdata[0] + offset));
}

static inline void
pci_vdev_write_cfg_u8(struct pci_vdev *vdev, uint32_t offset, uint8_t val)
{
	*(uint8_t *)(vdev->cfgdata + offset) = val;
}

static inline void
pci_vdev_write_cfg_u16(struct pci_vdev *vdev, uint32_t offset, uint16_t val)
{
	*(uint16_t *)(vdev->cfgdata + offset) = val;
}

static inline void
pci_vdev_write_cfg_u32(struct pci_vdev *vdev, uint32_t offset, uint32_t val)
{
	*(uint32_t *)(vdev->cfgdata + offset) = val;
}

uint32_t pci_vdev_read_cfg(struct pci_vdev *vdev, uint32_t offset, uint32_t bytes);
void pci_vdev_write_cfg(struct pci_vdev *vdev, uint32_t offset, uint32_t bytes, uint32_t val);

void pci_vdev_cfg_handler(struct vpci *vpci, uint32_t in, union pci_bdf vbdf, uint32_t offset,
	uint32_t bytes, uint32_t *val);
uint32_t pci_pdev_read_cfg(struct pci_pdev *pdev, uint32_t offset, uint32_t bytes);
void pci_pdev_write_cfg(struct pci_pdev *pdev, uint32_t offset, uint32_t bytes, uint32_t val);

#endif /* PCI_PRIV_H_ */
