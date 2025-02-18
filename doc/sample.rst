.. _sb_fota_sample:

SoftBank FOTA sample
####################

.. contents::
   :local:
   :depth: 2

The Softbank FOTA sample demonstrates how to run the :ref:`libsb_fota_index` library in an application in order to periodically check / download modem firmware updates.

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

The sample connects to LTE network, and will handle dfu events triggered by the Softbank FOTA library.
Using the modem time, the Softbank FOTA library will calculate when the next check for modem firmware update will happen.

Configuration
*************

|config|

Configuration options
=====================

Check and configure the following configuration options for the sample:

(list relevant configs)

Configuration files
===================

The sample provides predefined configuration files for typical use cases.

The following files are available:

* :file:`prj.conf` - Standard default configuration file.

To configure modem trace output, see `Sending traces over UART on an nRF91 Series DK`_.

Building and running
********************

.. |sample path| replace:: :file:`sample`

.. include:: /includes/build_and_run_ns.txt

Building with overlay
=====================

Testing
=======

After programming the sample and all prerequisites to the development kit, test it by performing the following steps:

1. Connect the USB cable and power on or reset your nRF91 Series DK.
#. Use a terminal emulator, like the `Serial Terminal app`_, to connect to the serial port.
   See `Testing and optimization`_ for the required settings and steps.
#. Observe that the kit prints the following information::

        Softbank FOTA sample.
#. abc

Troubleshooting
===============

Dependencies
************

This sample uses the following |NCS| libraries:

* nRF Cloud
* (AT) Shell

It uses the following `sdk-nrfxlib`_ library:

* `nrfxlib_nrf_modem`_

It uses the following |addon|
* Softbank FOTA library