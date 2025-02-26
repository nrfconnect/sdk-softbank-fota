.. _libsb_fota_changelog:

Changelog
#########

FOTA library and sample applications for SoftBank.
All notable changes to this project are documented in this file.

Version 2.8.0
*************

Changes
=======
* SDK: ported to |NCS| v2.8.0
* There are now two versions of the binary: default (``CONFIG_SB_FOTA``) and verbose (``CONFIG_SB_FOTA_LOG``).

  * Logs are no longer compiled into the default library. This saves some memory.
  * The verbose library can be used during development phase to ensure that the library is integrated correctly into your application.

* More consistent use of ``Softbank FOTA`` (as opposed to modem FOTA). Renamed events such as ``FOTA_EVENT_DOWNLOADING`` -> ``SB_FOTA_EVENT_DOWNLOADING``
* The library now schedules work on it's internal queue instead of the syswork queue (this avoids potential blocking of the syswork queue).
* Removed ``SB_FOTA_EVENT_REBOOT_PENDING``. Reboot is not needed to apply modem update.

Supported SDK version
=====================
* |NCS| v2.8.0

Supported modem firmware
************************
* mfw_nrf9160_1.3.7
* mfw_nrf9160_2.0.2
* mfw_nrf9160_2.0.3

Version 2.7.0
*************

Changes
=======
* SDK: ported to |NCS| 2.7.0

Supported SDK version
=====================
* nRF Connect SDK v2.7.0

Supported modem firmware
************************
* mfw_nrf9160_1.3.5
* mfw_nrf9160_1.3.6

Version 2.6.0
*************

Changes
=======
* Fix potential segfault on settings handling
* Allow updating modem FW without reboot
* Fix Kconfig menu name
* Fix CME error when reading disabled APNs

Version 2.6-pre-release
***********************

Changes
=======
* SDK: ported to |NCS| 2.6.0

Supported SDK version
*********************
* nRF Connect SDK v2.6.0

Supported modem firmware
========================
* mfw_nrf9160_1.3.5
* mfw_nrf9160_1.3.6

Version 2.0-pre-release
***********************

Changes
=======
* SDK: ported to nRF Connect SDK 2.0
* Limit FOTA to devices attached to SoftBank home network only
* Cloud: Update nRF Cloud endpoint to current REST API
* Cloud: Use JWT tokens to authenticate. Requires provisioning. Requires modem FW 1.3.
* SDK: Add glue layer to SDK porting
* Allow library to autoinit
* Apply the modem update before rebooting
* Rename MODEM_FOTA to SB_FOTA
* Refactor application events

Supported SDK version
=====================
* |NCS| v2.0.0

Supported modem firmware
========================
* mfw_nrf9160_1.3.1

Version 1.1
***********

Changes
=======
* SDK: Ported to |NCS| v1.4.0
* Cloud: Adapted to commercial FOTA server in nRF Connect for Cloud
* FOTA Server is assigned with IP address instead of FQDN hostname

APN configuration
=================
Same as 1.0

Supported SDK version
=====================
* |NCS| v1.4.0

Supported modem firmware
========================

* mfw_nrf9160_1.2.2
* mfw_nrf9160_1.2.3

Supported REST API Host
=======================
* https://api.nrfcloud.com/v1


Version 1.0
***********

Changes
=======
* First release that is used to complete SBM IOT certification test

Supported SDK version
=====================
* |NCS| v1.3.0

Supported modem firmware
========================
* mfw_nrf9160_1.2.2

Supported REST API Host
=======================
* https://api.beta.nrfcloud.com/v1
