.. _libsb_fota_index:

|addon| for the |NCS|
#####################

.. contents::
   :local:
   :depth: 2

The SoftBank FOTA library is a binary library that has been developed as an |NCS| add-on.
This library periodically checks `nRF Connect for Cloud`_ for modem firmware updates.
If there is an applicable update, the SoftBank FOTA library downloads and installs it.

In addition to the library itself, this |NCS| add-on contains a :ref:`sample <sb_fota_sample>` for testing and instructions for correctly :ref:`provisioning with nRF Cloud <libsb_fota_provisioning>`.

.. note::
   This library is not intended to be a general-purpose FOTA library.

.. important::
   To conform with the SoftBank certification, this library must be integrated with your application, unless agreed upon with your local Nordic or SoftBank representatives.

.. toctree::
   :maxdepth: 1
   :caption: Subpages:

   provisioning
   integration
   sample
   API_documentation
   CHANGELOG
   genindex
