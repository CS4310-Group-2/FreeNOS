/*
 * Copyright (C) 2015 Niek Linnenbank
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include "USBDescriptor.h"
#include "SynopsisController.h"
#include "SynopsisChannel.h"

SynopsisController::SynopsisController(const char *path)
    : USBController(path)
{
    // Allocate channels
    for (Size i = 0; i < ChannelCount; i++)
        m_channels.insert(i, *(new SynopsisChannel(i, &m_io)));
}

Error SynopsisController::initialize()
{
    Error r = USBController::initialize();

    if (r != ESUCCESS)
        return r;

    // Map USB host controller registers
    if (m_io.map(IO_BASE + Base, PAGESIZE*2,
                 Memory::User|Memory::Readable|Memory::Writable|Memory::Device) != IO::Success)
    {
        ERROR("failed to map I/O registers");
        return EIO;
    }

    // Check device ID
    if (m_io.read(VendorId) != DefaultVendorId)
    {
        ERROR("incompatible vendorId: " << m_io.read(VendorId) << " != " << DefaultVendorId);
        return EIO;
    }

    DEBUG("UserId: " << m_io.read(UserId) << " VendorId: " << m_io.read(VendorId));
    NOTICE("Synopsis Design Ware USB-on-the-go Host Controller found");

    // Initialize power manager
    if (m_power.initialize() != BroadcomPower::Success)
    {
        ERROR("failed to initialize power manager");
        return EIO;
    }
    // Power on the USB subsystem
    if (m_power.enable(BroadcomPower::USB) != BroadcomPower::Success)
    {
        ERROR("failed to power on the USB subsystem");
        return EIO;
    }
    DEBUG("powered on");

    // Soft-Reset
    softReset();
    //DEBUG("software reset done");
    NOTICE("software reset done");

    // Setup DMA
    m_io.write(RxFIFOSize, 1024);
    m_io.write(TxFIFOSize, (1024 << 16) | 1024);
    m_io.write(PeriodTxFIFOSize, (1024 << 16) | 2048);
    m_io.set(AHBConfig, DMAEnable | AXIWait);

    // Enable the USB controller interrupt on the ARM's interrupt controller
    addIRQHandler(InterruptNumber, (IRQHandlerFunction) &SynopsisController::interruptHandler);
    addIRQHandler(0, (IRQHandlerFunction) &SynopsisController::interruptHandler); // TODO: ARM does not have IRQ_REG() yet
    ProcessCtl(SELF, WatchIRQ, InterruptNumber);
    ProcessCtl(SELF, EnableIRQ, InterruptNumber);
    //DEBUG("interrupt handler installed");
    NOTICE("interrupt handler installed");

    // Clear all pending core interrupts
    m_io.write(CoreIntMask, 0);
    m_io.write(CoreInterrupt, 0xffffffff);

    // Enable core host channel and port interrupts
    m_io.write(CoreIntMask, CoreIntChannel | CoreIntPort);

    // Enable interrupts globally on the USB host controller.
    m_io.set(AHBConfig, InterruptEnable);
    //DEBUG("interrupts enabled");
    NOTICE("interrupts enabled");
    sleep(60);

    // Power-on host port (virtual root hub)
    u32 val = m_io.read(HostPortControl);
    val &= ~(HostPortEnable | HostPortEnableChanged |
             HostPortConnectChanged | HostPortCurrentChanged);
    val |= HostPortPower;
    m_io.write(HostPortControl, val);
    //DEBUG("poweron host port");
    //NOTICE("poweron host port");
    sleep(60);

    // Begin host port reset (raise the reset signal)
    val = m_io.read(HostPortControl);
    val &= ~(HostPortEnable | HostPortEnableChanged |
             HostPortConnectChanged | HostPortCurrentChanged);
    val |=  HostPortReset;
    m_io.write(HostPortControl, val);

#warning TODO: implement real sleep() now. We need it here.
    sleep(60);

    // Finish host port reset (lower the reset signal)
    val = m_io.read(HostPortControl);
    val &= ~(HostPortReset | HostPortEnable | HostPortEnableChanged |
             HostPortConnectChanged | HostPortCurrentChanged);
    m_io.write(HostPortControl, val);

    sleep(60);
    DEBUG("host port reset done. root hub enabled");

    hostPortChanged();

    // Initialize channels
    for (Size i = 0; i < ChannelCount; i++)
        m_channels[i].initialize();

    // Done.
    return ESUCCESS;
}

void SynopsisController::interruptHandler(InterruptMessage *msg)
{
    DEBUG("coreint =" << m_io.read(CoreInterrupt));

    u32 coreInt    = m_io.read(CoreInterrupt);

    // Host port interrupt occurred
    if (coreInt & CoreIntPort)
    {
        hostPortChanged();
    }

    // Channel interrupt occurred
    if (coreInt & CoreIntChannel)
    {
        u32 channelInt = m_io.read(HostChannelInt);

        // Post-process channel interrupt for each channel
        // which has the interrupt flag set high.
        for (Size i = 0; i < ChannelCount; i++)
        {
            if (channelInt & (1 << i))
                m_channels[i].interrupt();
        }
    }
    return DeviceServer::interruptHandler(msg);
}

void SynopsisController::hostPortChanged()
{
    u32 portStatus = m_io.read(HostPortControl);

    NOTICE("connected = " << (int)(portStatus & HostPortConnect) <<
          "connectedChanged = " << (int)(portStatus & HostPortConnectChanged) <<
          "enabled = " << (int)(portStatus & HostPortEnable) <<
          "enabledChanged = " << (int)(portStatus & HostPortEnableChanged) <<
          "overcurrent = " << (int)(portStatus & HostPortCurrent) <<
          "overcurrentChanged = " << (int)(portStatus & HostPortCurrentChanged) <<
          "reset = " << (int)(portStatus & HostPortReset) << 
          "power = " << (int)(portStatus & HostPortPower) <<
          "speed = " << (int)(portStatus & HostPortSpeed));

    // Clear host port interrupt flags
    portStatus &= ~(HostPortEnable | HostPortReset);
    portStatus |= HostPortEnableChanged | HostPortConnectChanged | HostPortCurrentChanged;
    m_io.write(HostPortControl, portStatus);
}

void SynopsisController::softReset()
{
    DEBUG("");

    // Raise reset flag
    m_io.write(CoreReset, 1);

    // Wait until cleared
    while (m_io.read(CoreReset) & 1);
}

SynopsisChannel * SynopsisController::getChannel(const FileSystemMessage *msg,
                                                 USBMessage *usb)
{
    // Check if the message is already in process inside a channel.
    for (Size i = 0; i < ChannelCount; i++)
    {
        SynopsisChannel *ch = &m_channels[i];

        if (ch->getState() != SynopsisChannel::Idle &&
            ch->getMessage() == msg)
            return ch;
    }
    // Message is not in process. Try to find unused channel.
    for (Size i = 0; i < ChannelCount; i++)
    {
        SynopsisChannel *ch = &m_channels[i];

        if (ch->getState() == SynopsisChannel::Idle)
            return ch;
    }
    // All channels busy
    return ZERO;
}

Error SynopsisController::transfer(const FileSystemMessage *msg,
                                   USBMessage *usb)
{
    DEBUG("");

    SynopsisChannel *ch = getChannel(msg, usb);
    if (!ch)
        return EAGAIN;

    switch (usb->state)
    {
        case USBMessage::Setup:
        case USBMessage::Data:
        case USBMessage::Status:

            switch (ch->transfer(msg, usb))
            {
                case SynopsisChannel::TransferStarted: return EAGAIN;
                case SynopsisChannel::Success:         return ESUCCESS;
                default:                               return EIO;
            }
            break;

        case USBMessage::Success:
            return ESUCCESS;

        case USBMessage::Failure:
            return EIO;

        default:
            ERROR("unhandled USBMessage.state = " << (int) usb->state);
            break;
    }
    return ENOTSUP;
}