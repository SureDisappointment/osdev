# sureOS
x86-64 operating system written in Intel Assembly and C

## Prerequisites
### Required Software
- [Docker](https://www.docker.com)
- [qemu](https://www.qemu.org)
#### Windows users:
- [MinGW](https://sourceforge.net/projects/mingw)
- [Chocolately](https://chocolatey.org/install)

### Windows Setup:
- Install the mingw-gcc-g++ package
- Install GNU make using Chocolately: ```choco install make```

### Linux Setup:
Install the gcc and make package for your ditribution

## Building the kernel image
Navigate to the root directory of the repository.

Enter the docker container: ```make docker-run```

Inside the docker container: ```make build-x86_64```

## Running the image using qemu
Outside the docker container: ```make run-x86_64```
