# photils-cli
![Build Status](https://github.com/scheckmedia/photils-cli/actions/workflows/build.yaml/badge.svg)

Photils-cli is an application that passes an image through a neural network, classifies it, and extracts the suggested tags. Everything happens offline without the need that your data are sent over the internet.

The reason for this application is mainly the integration into [darktable](https://www.darktable.org/) as [plugin](https://github.com/scheckmedia/photils-dt).

## Installation

[![](https://img.shields.io/badge/Releases%40latest-Download-blue)](https://github.com/scheckmedia/photils-cli/releases)

### Linux

1. Download the latest release `photils-cli-VERSION-linux.zip`
2. Copy the photils-cli.AppImage at some safe pleace
3. Make the AppImage executable: https://youtu.be/nzZ6Ikc7juw

### MacOS

1. Download the latest release `photils-cli-VERSION-osx.zip`
2. Extract the zip and copy the photils-cli.app at some safe place

### Windows

1. Download the latest release `photils-cli-VERSION-win64.zip`
2. Extract the zip and copy the content of the zip at some safe place

## Usage
#### Linux:
```bash
$ ./photils-cli.AppImage --image ~/Pictures/_DSC2437.jpg
```

#### MacOS:
```bash
$ ./photils-cli.app/Contents/MacOS/photils-cli --image ~/Pictures/_DSC2437.jpg
```

#### Windows:
```bash
$ .\photils-cli.exe --image C:\Documents\somepic.jpg
```

### Args
```bash
photils-cli [OPTION...]

  -h, --help             print this help
  -v, --version          print version
  -i, --image arg        Image to predict keywords
  -o, --output_file arg  File where to write keywords. Optional (default: "")
  -c, --with_confidence  If flag is used a confidence value for each
                         prediction is printed as well. Optional
```

### Tag translation
photils comes with a fixed list of ~3700 [available tags](https://github.com/scheckmedia/photils-cli/blob/master/res/labels.json). In some cases, the language or space-less tag style does not fit your workflow. For such cases, you have the option to override tags with your own style. For example, if you don't like the *blackbackground* tag and want *black background* instead, you can user the override file to fix this. You can also use it to translate the tags to the language of your choice.

All what you need is to place a file named **override_labels.json** in the folder:
| OS      | Path                                                           |
| ------- | -------------------------------------------------------------- |
| MacOS   | $HOME/Library/Application Support/photils/override_labels.json |
| Linux   | $HOME/.local/share/photils/override_labels.json                |
| Windows | %userprofile%\\.local\share\photils\override_labels.json        |

with a structure like:
```json
{
  "tag_name": "new_tag_name"
}
```
For example, a simple translation into German of 4 tags looks like this:

```json
{
    "art": "kunst",
    "shadow": "schatten",
    "light": "licht",
    "blackbackground": "schwarzer hintergrund"
}
```

*If you have done a translation of the available tags, please share it with us for the community. Thanks!*

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[GPLv3](https://choosealicense.com/licenses/gpl-3.0/)
