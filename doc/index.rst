.. _libsb_fota_index:

|addon| for the |NCS|
#####################

.. contents::
   :local:
   :depth: 2

Welcome to the SoftBank FOTA library NCS Add-on.
The SoftBank FOTA library is a binary library that will periodically check `nRF Connect for Cloud`_ for modem firmware updates.
If there is an applicable update, the SoftBank FOTA library will download and install it.

In addition to the library itself, this NCS add-on contains a :ref:`sample <sb_fota_sample>` for testing, and instructions for correctly :ref:`provisioning with nRF Cloud <libsb_fota_provisioning>`.


.. note::
   This library is not intended to be a general purpose FOTA library.


.. important::
   To conform with the SoftBank certification, this library **must** be integrated with your application, unless otherwise agreed with your local Nordic or SoftBank representatives.

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
