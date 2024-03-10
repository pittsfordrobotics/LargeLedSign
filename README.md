=== TBD: This needs to be filled in more ===

# Background

_Add some background and description of the overall Git project._

High level design and project breakdown
  - __Primary controller__: Contains the code that runs on the single primary controller.  This controller connects to and manages the individual sign elements.  It also exposes the main Bluetooth service that connects to the Android phone app.
  - __Secondary controller__: Contains the code that runs on the controllers that operate the LEDs for indidual signs.
  - __Legacy controller__: Contains the code that runs on the v1 sign, which has all 4 digits (3181) together in the same sign.
  - __Common lib__: Contains code shared by both the Primary and Secondary controllers.

# Very rough instructions for developing

Prerequisites:
  - [VSCode](https://code.visualstudio.com/Download)
  - [PlatformIO](https://docs.platformio.org/en/latest/integration/ide/vscode.html) plugin

Clone the repository

Steps to open the project
  - Using the Platform IO extension, select "Pick a folder"
    ![](./ReadmeImages/OpenPlatformProject.jpg)
  - Select the `LargeLedSign\PrimaryController`, `LargeLedSign\SecondaryController`, or `LargeLedSign\LegacyController` folder as appropriate

At this point, you'll be able to build and deploy the Arduino code.  If you intend to do development work, it's recommended that you also add the common libary folder and save the workspace so you have easy access to it in the future.

To add the common library and create a workspace:
  - Go to "File" / "Add Folder to Workspace..."
  - Select the `LargeLedSign\lib` folder
  - To save the workspace, go to "File" / "Save Worspace As..."
  - Save the workspace whereever you like.  Git has been told to ignore *.code-workspace files, so feel free to save the workspace directly under `LargeLedSign\PrimaryController` or `LargeLedSign\SecondaryController`.
  - Optional, but recommended: PlatformIO will successfully build the project, but VSCode's intellisense will be confused for files in the common `lib` folder.  To get VSCode to behave correctly, copy the file `LargeLedSign\PrimaryController\.vscode\c_cpp_properties.json` to `LargeLedSign\lib\.vscode\c_cpp_properties.json`.

# Sign configuration
When the code starts up, it will read a set of digital input pins to determine which type of sign it is and what order it should appear in if it is connected to a Controller.

Currently, the v1.0 sign has its own code and does not pay attention to the digital inputs.  This should change soon.

The digital inputs are set to `INPUT_PULLUP` and are considered active when pulled low.

Inputs have the following meaning:
- `D3` - `D7`: Indicates the sign's type.  `D3` is the most significant bit and `D7` is the least significant.  In general, this value corresonds to the digit number.  For example, the #3 sign has `D3`-`D7` set to 00011.
- `D8`-`D10`: Indicates the sign's position when connected to the controller. `D8` is the most significant bit and `D10` is the least significant. Lower postion values will be placed further to the left in the ordering. If more than one sign has the same position value, it will be indeterminate which of the duplicates will be placed first.  A value of 000 is used to indicate that the sign should not be included when a controller is scanning for signs to control.

For reference, the known sign types and input configurations are listed in the following table.  An "X" indicates the pin is pulled low by a jumper.  An "O" indicates the connection has been left open.

| Sign type | Digit |  D3  |  D4  |  D5  |  D6  |  D7  |  D8  |  D9  | D10  |
| :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: |
| v1 | - | X | O | O | O | O | O | O | N/A * |
| v2 | 3 | O | O | O | X | X | O | O | X |
| v2 | 1 | O | O | O | O | X | O | X | O |
| v2 | Logo | O | X | X | X | X | O | X | X |
| v2 | 8 | O | X | O | O | O | X | O | O |
| v2 | 1 | O | O | O | O | X | X | O | X |
| 6 inch | - | O * | X | O | X | O | O | O | O |

Comments:
- The v1 sign uses D10 as the data line to drive the TM1637 display.
- The 6 inch sign has a soft power button attached to D3. When not pressed, D3 will be pulled high acting as if it were an un-jumpered input.
