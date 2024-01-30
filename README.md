# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## frdm-mcxa153-freemaster-heart-rate
This demo mainly demonstrates how to use FRDM-MCXA153 to collect ecg, heart rate and blood oxygen saturation data, also uses FreeMASTER as the UI for display. The MCU (MCXA153) adjusts intensity of LED's in sensor for reads better values of light intensity data from MAX30101 via the I2C bus, performs filtering, other signal processing operations and algorithms to obtain the final result (ecg ,heart rate and SPO2 with waveforms), and saves it in the on-chip RAM. The on-board debugger of FRDM-MCXA153 reads the ecg, heart rate and blood oxygen data from the MCU via the SWD, and displays the results on the FreeMASTER web page. The UI of FreeMASTER uses JS script for web-based waveform display.

[<img src="image/freemaster_1.png" width="800"/>](image/freemaster_1.png)

#### Boards: FRDM-MCXA153

#### Categories: Sensor
#### Peripherals: I2C
#### Toolchains: MDK, MCUXpresso IDE

## Table of Contents
1. [Software](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [FAQs](#step5) 
6. [Support](#step6)
7. [Release Notes](#step7)

## 1. Software<a name="step1"></a>
- Download and install [MCUXpresso IDE V11.9.0 or later](https://nxp.flexnetoperations.com/control/frse/download?agree=Accept&element=3595113).
- Download and install the latest version of [FreeMASTER](https://www.nxp.com/design/software/development-software/freemaster-run-time-debugging-tool:FREEMASTER)(3.2.2.2).
- MCUXpresso for Visual Studio Code: This example supports MCUXpresso for Visual Studio Code, for more information about how to use Visual Studio Code please refer [here](https://www.nxp.com/design/training/getting-started-with-mcuxpresso-for-visual-studio-code:TIP-GETTING-STARTED-WITH-MCUXPRESSO-FOR-VS-CODE).

## 2. Hardware<a name="step2"></a>
- FRDM-MCXA153   
[<img src="image/frdm_a153.png" width="200"/>](image/frdm_a153.png)
- [HEART RATE 4 CLICK module](https://www.mikroe.com/heart-rate-4-click)(MIKROE-5547)  
[<img src="image/hear_rate.png" width="100"/>](image/hear_rate.png)
- Personal Computer
- USB type-C cable

## 3. Setup<a name="step3"></a>
### Step 1 (Import Project)

1. Open MCUXpresso IDE, in the Quick Start Panel, choose **Import from Application Code Hub**    

	[<img src="image/import_project_1.png" width="300"/>](image/import_project_1.png)

2. Enter the demo name in the search bar.   

	[<img src="image/import_project_2.png" width="300"/>](image/import_project_2.png)

3. Click **Copy GitHub link**, MCUXpresso IDE will automatically retrieve project attributes, then click **Next>**.   

	[<img src="image/import_project_3.png" width="300"/>](image/import_project_3.png)

4. Select **main** branch and then click **Next>**, Select the MCUXpresso project, click **Finish** button to complete import.   

	[<img src="image/import_project_4.png" width="300"/>](image/import_project_4.png)


### Step 2 (Hardware Setup)

Connect FRDM-MCXA153 board with MIKROE HEART RATE 4 CLICK module via MIKEOE bus interface. Compile the project and use a TypeC-USB cable to connect to the FRDM board via MCU-LINK connector. Use MCU-LINK to download program into MCU.   

[<img src="image/1.png" width="200"/>](image/1.png)

### Step 3 (Running The Demo)

Compile the project and download program into MCU. Press SW1 on FRDM board to reset the board. Open  FreeMASTER project "heart_rate.pmpx" in the code package.

## 4. Results<a name="step4"></a>
Place the middle finger on the light-emitting part of the MAX30101 sensor and try to move the finger as little as possible. After sensor calibration about 5-10 seconds, you should see changes in the ecg, heart rate and blood oxygen waveform on FreeMASTER, indicating that the demo is running successfully.

[<img src="image/freemaster_1.png" width="800"/>](image/freemaster_1.png)

[<img src="image/finger.png" width="200"/>](image/finger.png)

## 5. FAQs<a name="step5"></a>
No FAQs have been identified for this project.

## 6. Support<a name="step6"></a>
#### Project Metadata
<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-FRDM&ndash;MCXA153-blue)](https://github.com/search?q=org%3Anxp-appcodehub+FRDM-MCXA153+in%3Areadme&type=Repositories)

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-SENSOR-yellowgreen)](https://github.com/search?q=org%3Anxp-appcodehub+sensor+in%3Areadme&type=Repositories)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-I2C-yellow)](https://github.com/search?q=org%3Anxp-appcodehub+i2c+in%3Areadme&type=Repositories)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-MDK-orange)](https://github.com/search?q=org%3Anxp-appcodehub+mdk+in%3Areadme&type=Repositories) [![Toolchain badge](https://img.shields.io/badge/Toolchain-MCUXPRESSO%20IDE-orange)](https://github.com/search?q=org%3Anxp-appcodehub+mcux+in%3Areadme&type=Repositories)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected funcionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/@NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/Twitter-Follow%20us%20on%20Twitter-white.svg)](https://twitter.com/NXP)

## 7. Release Notes<a name="step7"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | January 30<sup>th</sup> 2024 |

