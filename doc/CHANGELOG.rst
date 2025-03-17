.. _sb_fota_changelog:

Changelog
#########

FOTA library and sample application for SoftBank.
All notable changes to this project are documented in this file.

For a full list of SoftBank FOTA add-on releases, related nRF Connect SDK, modem firmware versions, and supported REST API host, view the following table:

+-------------------+------------------+--------------------------+------------------------------------+
| |addon| version   | |NCS| version    | Modem firmware version   | REST API host                      |
+===================+==================+==========================+====================================+
| 2.8.0             | 2.8.0            | * mfw_nrf9160_1.3.7      |                                    |
|                   |                  | * mfw_nrf9160_2.0.2      |                                    |
|                   |                  | * mfw_nrf9160_2.0.3      |                                    |
+-------------------+------------------+--------------------------+------------------------------------+
| 2.7.0             | 2.7.0            | * mfw_nrf9160_1.3.5      |                                    |
|                   |                  | * mfw_nrf9160_1.3.6      |                                    |
+-------------------+------------------+--------------------------+------------------------------------+
| 2.6.0             | 2.6.0            | * mfw_nrf9160_1.3.5      |                                    |
|                   |                  | * mfw_nrf9160_1.3.6      |                                    |
+-------------------+------------------+--------------------------+------------------------------------+
| 2.0.0             | 2.0.0            | * mfw_nrf9160_1.3.1      |                                    |
+-------------------+------------------+--------------------------+------------------------------------+
| 1.1.0             | 1.4.0            | * mfw_nrf9160_1.2.2      | * https://api.nrfcloud.com/v1      |
|                   |                  | * mfw_nrf9160_1.2.3      |                                    |
+-------------------+------------------+--------------------------+------------------------------------+
| 1.0.0             | 1.3.0            | * mfw_nrf9160_1.2.2      | * https://api.beta.nrfcloud.com/v1 |
+-------------------+------------------+--------------------------+------------------------------------+

Version 2.8.0
*************

Changes
=======

* SDK: ported to |NCS| v2.8.0
* There are now two versions of the binary: default (``CONFIG_SB_FOTA``) and verbose (``CONFIG_SB_FOTA_LOG``).

  * Logs are no longer compiled into the default library.
    This saves some memory.
  * The verbose library can be used during the development phase to ensure that the library is integrated correctly into your application.

* More consistent use of ``SoftBank FOTA`` (as opposed to modem FOTA).
  Renamed events such as ``FOTA_EVENT_DOWNLOADING`` to ``SB_FOTA_EVENT_DOWNLOADING``
* The library now schedules work on its internal queue instead of the syswork queue (this avoids potential blocking of the syswork queue).
* Removed ``SB_FOTA_EVENT_REBOOT_PENDING``.
  Rebooting is not needed to apply the modem update.
* Removed the dependency on the `LTE Link Control`_ library.
  This makes it simpler to integrate the SoftBank FOTA library into applications that do not use the Link Controller library by default, such as the `Serial LTE modem`_ application.

Version 2.7.0
*************

Changes
=======

* SDK: ported to |NCS| 2.7.0.

Version 2.6.0
*************

Changes
=======

* Allow updating modem firmware without reboot.
* Fix potential segfault on settings handling.
* Fix Kconfig menu name.
* Fix CME error when reading disabled APNs.

Version 2.6-pre-release
***********************

Changes
=======

* SDK: ported to |NCS| 2.6.0.

Version 2.0-pre-release
***********************

Changes
=======

* SDK: ported to |NCS| v2.0.0.
* Limit FOTA to devices attached to the SoftBank home network only.
* Add a glue layer to SDK porting.
* Allow library to auto-init.
* Apply the modem update before rebooting.
* Rename ``MODEM_FOTA`` to ``SB_FOTA``.
* Refactor application events.
* Cloud: Update nRF Cloud endpoint to current REST API.
* Cloud: Use JWT tokens to authenticate.
  Requires provisioning.
  Requires modem firmware v1.3.

Version 1.1
***********

Changes
=======

* SDK: Ported to |NCS| v1.4.0.
* Cloud: Adapted to commercial FOTA server in nRF Connect for Cloud.
* FOTA server is assigned with an IP address instead of an FQDN hostname.

APN configuration
=================

Same as v1.0

Version 1.0
***********

Changes
=======

* First release that is used to complete SBM IOT certification test.
