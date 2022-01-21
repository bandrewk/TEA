# TEA (TEsting tool for Android)[![GitHub license](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://github.com/bandrewk/TEA/blob/main/LICENSE)
![alt text](https://github.com/bandrewk/TEA/blob/main/screenshot.png?raw=true)

This tool was originally designed to help in QA with Samsung phones back in 2015. 

TEA is using ADB to communicate with the connected devices.

**Functions:**
- List all connected devices 
- View phone identification (Model, SW Version AP, OS Version, Max App SDK and CSC)
- Install an APK (Drag and Drop)
- Take a screenshot 
- Video screen recording

## Developing

### Built With

QT version >= 6.0.1

### Prerequisites

[Android SDK Platform Tools](https://developer.android.com/studio/releases/platform-tools) are needed in order to run the ADB commands.

TEA is expecting the platform tools to be in the PATH variable on windows.

## Usage

Upon starting, TEA will immediately start querying for new devices. You can refresh TEA with `CRTL+R` or by using the Menu `Device -> Refresh`.

> Note: The device must have USB-Debugging enabled.

### Taking a screenshot

Press the `Screenshot` button, a screenshot (.png) of the device will be taken and is moved to the users desktop. 

All leftovers on the device will be removed.


### Screen recording

Press the `START Video` button, a new popup will open. When done press OK to stop the recording. The file (.mp4) is moved to the users desktop. 

All leftovers on the device will be removed.

## Troubleshooting

1. **Empty or no entry**

Please double-check that USB-Debugging is enabled. If verified, try using another USB cable. 

ADB is very error prone to faulty USB cables.

*(This means, the device is offline but connected to the pc. You might even be able to transfer files but ADB won't work)*

2. **Not all functions are working with my non Samsung phone**

TEA has been designed to be used with Samsung phones, hence other phone brands might not offer the same features.

3. **Not all functions are working with my Samsung phone**

Verify that theres enough free storage on phone AND your PC.
