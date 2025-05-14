# Zephyr application/module template repository

This repository is an example Zephyr application and module which can be used as a basis for custom Zephyr application and modules. The repository relies on the Docker image from [zenv](https://github.com/paradajz/zenv) repository. The specific version is pinned in `.devcontainer/Dockerfile` file. Technically, this repository can be used without Visual Studio Code, but such setup isn't supported. The setup is expected to work with any Linux distribution (x86-64 and ARM64 are supported architectures). Other operating systems might work but aren't supported.

The repository supports application and module development (also both at the same time). Zephyr module is a repository which can be added to a `west.yml` file and after which all the libraries and Kconfig options inside the `module` directory become available to the application using it. No special configuration for that is needed in this repository. Application source files are located in the `app` directory.

  - [Prerequisites](#prerequisites)
  - [Development](#development)

## Prerequisites

Before proceeding, ensure that you have Docker installed on your machine. Make sure that your Linux user account is in `docker` group. To do that, run the following command:

```bash
sudo usermod -aG docker ${USER}
```

This will ensure that root privileges aren't needed in order to execute Docker commands. Make sure to restart your PC after executing this command so that everything is reloaded correctly. Next, install [Visual Studio Code](https://code.visualstudio.com/). Alternatives such as VSCodium will not work and aren't supported. Once installed, open it and install the [Dev containers](https://marketplace.visualstudio.com/items/?itemName=ms-vscode-remote.remote-containers) extension.

**Note**: Visual Studio Code with the Dev Containers extension is the only officially supported way of running the repositories based on the [zenv](https://github.com/paradajz/zenv) repository, such as this one.

## Development

The easiest way to open this repository in Visual Studio Code is to type the following in terminal while in repository root directory:

```bash
code .
```

If for some reason a popup asking you whether to reopen repository in container doesn't appear, press `[CTRL+Shift+P]`, type "Remote" and select "Remote Containers: Reopen in Container". Instructions on how to use the development environment are located in [zenv](https://github.com/paradajz/zenv) repository.

## Debugging

Two VSCode debug configurations are available in `.vscode/launch.json` file: one for the tests and another for the application firmware. Test configuration allows debugging of individual tests, while firmware debug configuration debugs the firmware loaded onto the MCU.
