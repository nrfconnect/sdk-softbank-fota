.. _libsb_fota_index:

|addon| for the |NCS|
#####################

.. contents::
   :local:
   :depth: 2

Welcome to the SoftBank FOTA library NCS Add-on.
The Softbank FOTA library is a small binary library that will periodically the `nRF Connect for Cloud`_ for modem firmware updates.
If there is an applicable update, the Softbank FOTA library will download and install it.

In addition to the library itself, this NCS add-on contains a :ref:`sample <sb_fota_sample>` for testing, and instructions for correctly :ref:`provisioning with nRF Cloud <libsb_fota_provisioning>`.


.. note::
   This library is not intended to be a general purpose FOTA library.


.. important::
   As part of Nordics certification with SoftBank Mobile, this Softbank FOTA library **must** be integrated with your application, unless otherwise agreed with you local Nordic or Softbank representatives.

.. toctree::
   :maxdepth: 1
   :caption: Subpages:

   provisioning
   integration
   sample
   API_documentation
   CHANGELOG

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
