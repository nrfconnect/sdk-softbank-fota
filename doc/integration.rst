.. _sb_fota_integration:

Application Integration and Configuration
#########################################

Overview
========

Softbank specifies that your certified product must be able to update its modem firmware, regardless other tasks and operations your main application is running.
The Softbank FOTA library is an independent library that is intended to runs together with your application when included in your project. See the below image.

.. figure:: images/softbank_fota_summary.svg
   :alt: Interaction between the Softbank FOTA library and the user application

   Interaction between the Softbank FOTA library and the user application

OS layer
========

The Softbank FOTA library has an OS abstraction layer.
This abstraction layer makes the softbank library independent of some |NCS| modules and underlying implementation of primitives such as timers, non-volatile storage, and heap allocation.
For more information, see the :file:`sb_fota_os.h` file or the :ref:`sb_fota_os` section for available APIs. It provides an abstraction of the following modules:

* |NCS| modules:

  * `AT Monitor`_
  * `Download client`_
  * `PDN`_
  * `DFU target`_
  * `nRF Cloud`_

* Zephyr modules:

  * `Kernel`_
  * `NVS`_

Following modules are used direclty by the library.

  * `HTTP Client`_
  * `Settings subsystem <Settings_>`_
  * `Modem JWT`_
  * `Modem Information`_
  * `LTE Link Control`_
  * `nrfxlib_nrf_modem`_

When the Softbank FOTA library is enabled in your application, it includes the file :file:`sb_fota/lib/bin/sb_fota/os/sb_fota.c`.
This automatically runs the library's main function (:c:func:`sb_fota_main`).

.. _sb_fota_configuration:

Configuration
*************

Enable the library in your application by setting the ``CONFIG_SB_FOTA`` Kconfig option to ``y``.

The :ref:`sb_fota_sample` project configuration (:file:`sb_fota/samples/cellular/sb_fota/prj.conf`) contains all the configurations that are needed by the Softbank FOTA library.

By default, sec_tag 50 is used for JWT signing and TLS. This can be changed with ``TLS SB_FOTA_JWT_SECURITY_TAG`` and ``SB_FOTA_TLS_SECURITY_TAG``

Usage
*****

To use this library, the application must set the Kconfig option ``CONFIG_SB_FOTA`` to ``y`` and then register a callback for the library using :c:func:`sb_fota_init` function.
This is demonstrated in the following example and it is based on the assumption that you are initializing the `nrfxlib_nrf_modem`_ and LTE connection when the device boots:

.. code-block:: c

    static void modem_fota_callback(enum sb_fota_event e)
    {
       switch(e) {
       case FOTA_EVENT_DOWNLOADING:
          printk("FOTA_EVENT_DOWNLOADING\n");
          break;
       case FOTA_EVENT_IDLE:
          printk("FOTA_EVENT_IDLE\n");
          break;
       case FOTA_EVENT_MODEM_SHUTDOWN:
          printk("FOTA_EVENT_MODEM_SHUTDOWN\n");
          break;
       case FOTA_EVENT_REBOOT_PENDING:
          printk("FOTA_EVENT_REBOOT_PENDING\n");
          sys_reboot(SYS_REBOOT_COLD);
          break;
       }
    }

    void main(void)
    {
       if (sb_fota_init(&modem_fota_callback) != 0) {
          printk("Failed to initialize modem FOTA\n");
          return;
       }
       ...
    }

Requirements and limitations
============================

The application can control the modem usage normally, like any application based on the |NCS|, but with the following limitations:

* Occasionally, the library might need to connect to `nRF Cloud`_ to check and possibly download a new modem firmware image.
  It issues the :c:enum:`FOTA_EVENT_DOWNLOADING` event when it starts the download.
  At that time, application must not use any TLS sockets that are using offloaded TLS stack from the modem.
  Also, it is recommended to stop all network operations until the :c:enum:`FOTA_EVENT_IDLE` event is received, as there might be mandatory operation mode switches between NB-IoT and LTE-M networks.

* When the modem is updated with a new firmware, it gets disconnected from the network and shuts down.
  This is indicated by the :c:enum:`FOTA_EVENT_MODEM_SHUTDOWN` event.
  The modem update can take a few minutes.
  After the modem is updated, it requires the device to boot itself, so that it can return all modem libraries into a known state.
  This request is indicated by the :c:enum:`FOTA_EVENT_REBOOT_PENDING` event.
  However, if no event handler is specified, the device reboots automatically.

To prevent any automatic reboots, it is recommended to install an event handler with :c:func:`sb_fota_init` API.

Configuration options
*********************

* ``CONFIG_SB_FOTA`` - Enables the Softbank FOTA library
* ``CONFIG_SB_FOTA_AUTOINIT`` - Initializes the library automatically
* ``CONFIG_SB_FOTA_TLS_SECURITY_TAG`` - Security tag (``sec_tag``) for nRF Cloud TLS connection
* ``CONFIG_SB_FOTA_JWT_SECURITY_TAG`` - ``sec_tag`` for authentication with the cloud.