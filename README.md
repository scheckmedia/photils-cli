# photils-cli
[![Build Status](https://travis-ci.org/scheckmedia/photils-cli.svg?branch=master)](https://travis-ci.org/scheckmedia/photils-cli)

Photils-cli is an application that passes an image through a neural network, classifies it, and extracts the suggested tags. Everything happens offline without the need that your data are sent over the internet.

The reason for this application is mainly the integration into [darktable](https://www.darktable.org/) as [plugin](https://github.com/scheckmedia/photils-dt).

## Installation
### Linux
 [![](https://img.shields.io/badge/Linux%40latest-Download-blue)](
https://github.com/scheckmedia/photils-cli/releases/latest/download/photils-cli-linux-x86_64.AppImage)

1. Download the latest release
2. Copy the photils-cli.AppImage at some safe pleace
3. Make the AppImage executable: https://youtu.be/nzZ6Ikc7juw

### MacOS
[![](https://img.shields.io/badge/MacOS%40latest-Download-blue)](
https://github.com/scheckmedia/photils-cli/releases/latest/download/photils-cli-osx.zip)

1. Download the latest release
2. Extract the zip and copy the photils-cli.app at some safe pleace

### Windows
*work in progress*


## Usage
#### Linux:
```bash
$ ./photils-cli.AppImage --image ~/Pictures/_DSC2437.jpg
```

#### MacOS:
```bash
$ ./photils-cli.app/Contents/MacOS/photils-cli --image ~/Pictures/_DSC2437.jpg
```

### Args
```bash
photils-cli [OPTION...]

-h, --help             print this help
-i, --image arg        Image to predict keywords
-o, --output_file arg  File where to write keywords. Optional (default: "")
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[GPLv3](https://choosealicense.com/licenses/gpl-3.0/)