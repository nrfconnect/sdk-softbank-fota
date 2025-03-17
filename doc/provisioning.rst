.. _libsb_fota_provisioning:

Provisioning
############

.. contents::
   :local:
   :depth: 2

Before using the SoftBank FOTA library with `nRF Cloud`_, you must provision the device.
The modem allows a device to issue `JSON Web Tokens (JWT) <JSON Web Token (JWT)_>`_ at runtime that are signed with a public and private key pair.
You must generate this key pair beforehand, and the public key must be provisioned to nRF Cloud.

Prerequisites
*************

Before you start provisioning, make sure that the following prerequisites are fulfilled:

* `nRF Connect SDK is installed <Installing the nRF Connect SDK>`_.
* You have an `nRF Cloud <nRF Connect for Cloud_>`_ account.

Provisioning by certificate
***************************

The following section refers to the APIs and procedures from the `nRF Cloud Utilities documentation`_.

1. Make sure that `nRF Cloud utils`_ and its required python packages are installed:

   .. code-block::

     west config manifest.path sdk-softbank-fota
     west update
     cd ~ncs/nrf_cloud_utils/python/modem-firmware-1.3+/
     pip install -r requirements.txt


#. Copy the API_Key from the nRF Cloud User Account page:

   .. code-block::

     export API_KEY="aabbccddeeff00112233445566778899aabbccdd"

#. Create certificates.

   .. note::
      Only do this once (or remove the old certificates if you create new ones).

   .. code-block::

     python3 create_ca_cert.py -c NO -o "My Company" -p ./my_ca -f my_company-

#. Provision each device.
   Here, the default value of the ``CONFIG_SB_FOTA_JWT_SECURITY_TAG`` Kconfig option ``50``, is used.

   .. code-block::

     cd ~/ncs/nrf/samples/cellular/at_client/
     west build -b nrf9160dk/nrf9160/ns
     west flash
     cd -
     python3 device_credentials_installer.py -d --sectag 50 --ca $(ls my_ca/*_ca.pem) --ca_key $(ls my_ca/*_prv.pem) --fwtypes "MODEM" --append --verify


#. Upload the list of provisioned devices to nRF Cloud.

   .. code-block::

     python3 nrf_cloud_onboard.py --apikey $API_KEY --csv onboard.csv

You can now proceed to test that the provisioning was successful by using the :ref:`sb_fota_sample`.

.. note::
   As the device uses only JWT and a keypair to authenticate, you do not need to provision that certificate back to the device.
   Similarly, nRF Cloud extracts the public key from a certificate when it verifies the JWT.
