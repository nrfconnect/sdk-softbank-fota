.. _libsb_fota_provisioning:

Provisioning
############

.. contents::
   :local:
   :depth: 2

Provisioning
************

Before using Softbank FOTA library with `nRF Cloud`_, you must provision the device.

The modem allows a device to issue `JSON Web Tokens (JWT) <JSON Web Token (JWT)_>`_ at run time that are signed with a certain public and private key pair.
This key pair must be generated beforehand, and the public key must be provisioned to the nRF Cloud.

Provisioning by certificate
===========================

Hence you must provision a certificate using `ProvisionDevices API`_.

For  provisioning the certificates, first generate a Certificate Signing Request (CSR) with the modem.
For more information, see the documentation on ``AT%KEYGEN set command``.
In the following example, ``sec_tag 51`` is used, so the ``CONFIG_SB_FOTA_JWT_SECURITY_TAG`` must be set accordingly:

Have NCS and installed in ~/ncs
Have cloud utils installed in ~/ncs

Make sure the python requirements files are installed

.. code-block::

  pip -r ncs/nRFcloudrequirements.txt




cd utils/python/modem-firmware-1.3+/
pip3 install -r requirements.txt
Have an account at nRF Cloud and copy the API_Key from the User Account page

.. code-block::

  export API_KEY="aabbccddeeff00112233445566778899aabbccdd"

Create certificates
Only do this once! (or remove the old old certificates if you create new ones.)

.. code-block::

  python3 create_ca_cert.py -c NO -o "My Company" -p ./my_ca -f my_company-

Provision each device

.. code-block::

  cd ~/ncs/nrf/samples/cellular/at_client/
  west build -b nrf9160dk_nrf9160_ns
  west flash
  cd -
  python3 device_credentials_installer.py -d --sectag 50 --ca $(ls my_ca/*_ca.pem) --ca_key $(ls my_ca/*_prv.pem) --append --verify


Upload the list of devices to nRF Cloud

.. code-block::

  python3 nrf_cloud_onboard.py --apikey $API_KEY --csv onboard.csv

You can now proceed to test that the provisioning was successfull by using the :ref:`sb_fota_sample`


.. note::
   As the device uses only JWT and a keypair to authenticate, you need not provision that certificate back to the device.
   Similarly, cloud will extract the public key from a certificate when it verifies the JWT.

