.. _libsb_fota_provisioning:

Provisioning
############

.. contents::
   :local:
   :depth: 2

Provisioning
************

Before using Softbank FOTA library with `nRF Cloud`_, you must provision the device.
The modem allows a device to issue `JSON Web Tokens (JWT) <JSON Web Token (JWT)_>`_ at run time that are signed with a public and private key pair.
This key pair must be generated beforehand, and the public key must be provisioned to the nRF Cloud.

Provisioning by certificate
===========================

The following section refers to the APIs and procedures from the `nRF Cloud Utilities documentation`_.

* NCS is installed in ~/ncs
* nRF Cloud utils is installed in ~/ncs/nrf_cloud_utils
* You have an account at nRF Cloud.

1. Make sure the python requirements files are installed

   .. code-block::

     cd ~ncs/nrf_cloud_utils/python/modem-firmware-1.3+/
     pip install -r nRFcloudrequirements.txt


#. Copy API_Key from the User Account page

   .. code-block::

     export API_KEY="aabbccddeeff00112233445566778899aabbccdd"

#. Create certificates

   .. note::
      Only do this once (or remove the old old certificates if you create new ones).

   .. code-block::

     python3 create_ca_cert.py -c NO -o "My Company" -p ./my_ca -f my_company-

#. Provision each device. Here we use default value of ``CONFIG_SB_FOTA_JWT_SECURITY_TAG`` (50).

   .. code-block::

     cd ~/ncs/nrf/samples/cellular/at_client/
     west build -b nrf9160dk_nrf9160_ns
     west flash
     cd -
     python3 device_credentials_installer.py -d --sectag 50 --ca $(ls my_ca/*_ca.pem) --ca_key $(ls my_ca/*_prv.pem) --append --verify


#. Upload the list of provisioned devices to nRF Cloud.

   .. code-block::

     python3 nrf_cloud_onboard.py --apikey $API_KEY --csv onboard.csv


You can now proceed to test that the provisioning was successfull by using the :ref:`sb_fota_sample`

.. note::
   As the device uses only JWT and a keypair to authenticate, you need not provision that certificate back to the device.
   Similarly, cloud will extract the public key from a certificate when it verifies the JWT.

