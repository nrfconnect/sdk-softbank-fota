.. _sb_fota_sample:

SoftBank FOTA sample
####################

.. contents::
   :local:
   :depth: 2

The SoftBank FOTA sample demonstrates how to run the :ref:`libsb_fota_index` library in an application in order to periodically check for and download modem firmware updates.

Requirements
************

The sample supports the following development kits:

.. list-table::
   :widths: 25 25 25 25
   :header-rows: 1

   * - Hardware platforms
     - PCA
     - Board name
     - Board target
   * - nRF9160 DK
     - PCA10090
     - nrf9160dk
     - nrf9160dk/nrf9160/ns
   * - nRF9151 DK
     - PCA10171
     - nrf9151dk
     - nrf9151dk/nrf9151/ns

Overview
********

Before running this sample, make sure you have provisioned your device to nRF Cloud :ref:`libsb_fota_provisioning`.

The sample connects to the LTE network, and will handle DFU events triggered by the SoftBank FOTA library.
Using the modem time, the SoftBank FOTA library will calculate when the next check for the modem firmware update will happen.

Configuration
*************

|config|

Configuration options
=====================

During development, it is recommended to use the verbose version of the library (``CONFIG_SB_FOTA_LOG=y``) in order to be able to see when the next update is scheduled.
For production, it is recommended to use the non-verbose version of the library (``CONFIG_SB_FOTA_LOG=n``), in order to save memory.

Configuration files
===================

The sample provides predefined configuration files for typical use cases.

The following files are available:

* :file:`prj.conf` - Standard default configuration file.

To configure modem trace output, see `Sending traces over UART on an nRF91 Series DK`_.

Building and running
********************

This sample can be found under the ``sample`` folder in the |addon| structure.

You can build this sample as a firmware image for a board target with the \*/ns variant (see the Requirements section above).
In such cases, the sample has Cortex-M Security Extensions (CMSE) enabled and separates the firmware between Non-Secure Processing Environment (NSPE) and Secure Processing Environment (SPE).
Because of this, it automatically includes Trusted Firmware-M (TF-M).

To build the sample, follow the instructions in `Building an application`_ for your preferred building environment. See also `Programming an application`_ for programming steps and `Testing and optimization`_ for general information about testing and debugging in the nRF Connect SDK.

Testing
=======

After programming the sample and all prerequisites to the development kit, test it by performing the following steps:

1. Connect the USB cable and power on or reset your nRF91 Series DK.
#. Use a terminal emulator, like the `Serial Terminal app`_, to connect to the serial port.
   See `Testing and optimization`_ for the required settings and steps.
#. Observe that the kit prints the following information::

    SoftBank FOTA client started

#. Deploy an `nRF Cloud FOTA`_.
#. In the shell, observe when the next FOTA check is.
   For example::

     <inf> sb_fota: Next update check in 14 days, 09:20:10

#. Use ``sb_fota_clock_set()`` to set the modem time to trick the update to happen (Or simply wait ~14 days).

   For example, if ``AT+CCLK`` returns ``25/03/04,01:30:40+04``, then use the ``app clock`` shell command to set the clock::

    uart:~$ app clock 25/03/18,10:50:40+04

#. Verify that the download starts::

    SB_FOTA_EVENT_DOWNLOADING

#. When the download has completed, verify that the modem shuts down to apply the update::

    SB_FOTA_EVENT_MODEM_SHUTDOWN

#. After the modem has reinitialized, the library will connect to nRF Cloud to update the status of the update::

    <inf> sb_fota: Modem firmware was updated, updating job

#. Finally, verify that the library schedules a new check in 14 days and goes idle::

    <inf> sb_fota: Next update check in 14 days, 11:05:29
    <inf> sb_fota: SB_FOTA_EVENT_IDLE

Troubleshooting
===============

If you do not see any output logs such as "... Next update check in..." and only see "SoftBank FOTA client started", check that you have linked the verbose library ``CONFIG_SB_FOTA_LOG = y``.
By default, the library does not use LOG.

Dependencies
************

This sample uses the following `sdk-nrfxlib`_ library:

* `Modem library <nrfxlib_nrf_modem_>`_

It uses the following |addon|.

* SoftBank FOTA library

In addition, it uses the following secure firmware component:

* `Trusted Firmware-M`_
