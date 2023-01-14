# sureOS
x86-64 operating system written in Intel Assembly and C

## Building the kernel image

### Required Software
- [Docker](https://www.docker.com)
#### Windows users:
- [MinGW](https://sourceforge.net/projects/mingw)
- [Chocolately](https://chocolatey.org/install)

### Windows Setup
- Install the mingw-gcc-g++ package
- Install GNU make: ```choco install make```

### Linux Setup
Install the gcc and make package for your distribution

### Build
Run all commands from the root directory of the repository.

Enter the docker container: ```make docker-run```

Inside the docker container: ```make build-x86_64```

## Running the kernel image using [qemu](https://www.qemu.org)

### Windows Setup
- Install [Chocolately](https://chocolatey.org/install)
- Install GNU make: ```choco install make```

### Linux Setup
Install the make package for your distribution

### Run qemu
```make run-x86_64```
