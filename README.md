# Softbank FOTA add-on for nRF Connect SDK

This repository contains the Softbank FOTA [add-on for NCS] (https://nrfconnect.github.io/ncs-app-index/).

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
