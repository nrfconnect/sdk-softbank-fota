# Softbank FOTA add-on for nRF Connect SDK

<a href="https://github.com/nrfconnect/sdk-softbank-fota/actions/workflows/build-using-docker.yml?query=branch%3Amain">
  <img src="https://github.com/nrfconnect/sdk-softbank-fota/actions/workflows/build-using-docker.yml/badge.svg?event=push">
</a>
<a href="https://github.com/nrfconnect/ncs-example-application/actions/workflows/docs.yml?query=branch%3Amain">
  <img src="https://github.com/nrfconnect/ncs-example-application/actions/workflows/docs.yml/badge.svg?event=push">
</a>
<a href="https://nrfconnect.github.io/sdk-softbank-fota">
  <img alt="Documentation" src="https://img.shields.io/badge/documentation-3D578C?logo=sphinx&logoColor=white">
</a>
<a href="https://nrfconnect.github.io/sdk-softbank-fota/doxygen">
  <img alt="API Documentation" src="https://img.shields.io/badge/API-documentation-3D578C?logo=c&logoColor=white">
</a>

This repository contains the Softbank FOTA [add-on for NCS](https://nrfconnect.github.io/ncs-app-index/).

## Documentation

Pre-build documentation is available [here](https://nrfconnect.github.io/sdk-softbank-fota).

A minimal documentation setup is provided for Sphinx. To build the
documentation first change to the ``docs`` folder:

```shell
cd docs
```
To install Sphinx, make sure you have a Python installation in place and run:

```shell
pip install -r requirements.txt
```

The Sphinx documentation (HTML) can be built using the following command:

```shell
make html
```

The output will be stored in the ``_build_sphinx`` folder. You may check for
other output formats other than HTML by running ``make help``.
