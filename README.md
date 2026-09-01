# PSOC&trade; Control MCU: MOTIF Hall sensor mode

This code example demonstrates the use of MOTIF IP in Hall sensor mode to control a BLDC motor with trapezoidal commutation.


[View this README on GitHub.](https://github.com/Infineon/mtb-example-ce240615-motif-3-hall-sensor-mode)

[Provide feedback on this code example.](https://yourvoice.infineon.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyNDA2MTUiLCJTcGVjIE51bWJlciI6IjAwMi00MDYxNSIsIkRvYyBUaXRsZSI6IlBTT0MmdHJhZGU7IENvbnRyb2wgTUNVOiBNT1RJRiBIYWxsIHNlbnNvciBtb2RlIiwicmlkIjoiYWJoaXNoZWsucHJhc2hhbnRAaW5maW5lb24uY29tIiwiRG9jIHZlcnNpb24iOiIxLjAuMCIsIkRvYyBMYW5ndWFnZSI6IkVuZ2xpc2giLCJEb2MgRGl2aXNpb24iOiJNQ0QiLCJEb2MgQlUiOiJJQ1ciLCJEb2MgRmFtaWx5IjoiUFNPQyJ9)


## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) v3.8 or later
- Board support package (BSP) minimum required version:
   - KIT_PSC3M5_CC2: v1.0.3
- Programming language: C
- Associated parts: All PSOC&trade; Control C3 MCUs


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v14.2.1 (`GCC_ARM`) – Default value of `TOOLCHAIN`
- Arm&reg; Compiler v6.22 (`ARM`)
- IAR C/C++ Compiler v9.70.4 (`IAR`)


## Supported kits (make variable 'TARGET')

- [PSOC&trade; Control C3M5 Motor Drive Control Card](https://www.infineon.com/evaluation-board/KIT-PSC3M5-CC2) (`KIT_PSC3M5_CC2`) – Default value of `TARGET` and validated firmware target
- [PSOC&trade; Control C3M5 Complete System Motor Control Kit](https://www.infineon.com/evaluation-board/KIT-PSC3M5-MC1) (`KIT_PSC3M5_MC1`) – Complete system setup reference; confirm the board-specific pin mapping before changing `TARGET`


## Hardware setup

See the kit user guide to ensure that the board is configured correctly.

Use the [KIT_PSC3M5_MC1 Complete System Motor Control Kit](https://www.infineon.com/evaluation-board/KIT-PSC3M5-MC1) as the complete setup reference. It includes the motor control card, drive adapter, 250 W power board, and 24 V BLDC motor. The firmware in this project is validated for `KIT_PSC3M5_CC2`; verify the board-specific pin mapping before using it with MC1. Refer to the [MC1 user guide](https://www.infineon.com/assets/row/public/documents/30/44/infineon-kit-psc3m5-mc1-ug-usermanual-en.pdf) for connector locations, power sequencing, and electrical limits.

**Figure 1. KIT_PSC3M5_MC1 setup**

<img src="images/psoc-control-c3-mc1-kit-details.png" alt="KIT_PSC3M5_MC1 motor-control setup" width="800">

Use the image and the MC1 documentation for the physical setup.

### Quick startup connection guide

1. Connect the motor phases, Hall cable, and DC supply as described in the MC1 documentation.
2. Connect the PC to the appropriate programmer/debugger interface and leave motor power disabled.
3. Set the potentiometer to the zero-speed position.

Pin and resource assignments are listed in the application-resources table below. Follow the MC1 documentation for the physical Hall-cable connection.


## Software setup

See the [ModusToolbox&trade; tools package installation guide](https://www.infineon.com/ModusToolboxInstallguide) for information about installing and configuring the tools package.

## Using the code example


### Create the project

The ModusToolbox&trade; tools package provides the Project Creator as both a GUI tool and a command line tool.

<details><summary><b>Use Project Creator GUI</b></summary>

1. Open the Project Creator GUI tool.

   There are several ways to do this, including launching it from the dashboard or from inside the Eclipse IDE. For more details, see the [Project Creator user guide](https://www.infineon.com/ModusToolboxProjectCreator) (locally available at *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/docs/project-creator.pdf*).

2. On the **Choose Board Support Package (BSP)** page, select a kit supported by this code example. See [Supported kits](#supported-kits-make-variable-target).

   > **Note:** To use this code example for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work.

3. On the **Select Application** page:

   a. Select the **Applications(s) Root Path** and the **Target IDE**.

      > **Note:** Depending on how you open the Project Creator tool, these fields may be pre-selected for you.

   b. Select this code example from the list by enabling its check box.

      > **Note:** You can narrow the list of displayed examples by typing in the filter box.

   c. (Optional) Change the suggested **New Application Name** and **New BSP Name**.

   d. Click **Create** to complete the application creation process.

</details>

<details><summary><b>Use Project Creator CLI</b></summary>

The 'project-creator-cli' tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command-line 'modus-shell' program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following example clones the "[MOTIF Hall sensor mode](https://github.com/Infineon/mtb-example-ce240615-motif-3-hall-sensor-mode)" application with the desired name "MotifTrapCtrl" configured for the *KIT_PSC3M5_CC2* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id KIT_PSC3M5_CC2 --app-id mtb-example-ce240615-motif-3-hall-sensor-mode --user-app-name MotifTrapCtrl --target-dir "C:/mtb_projects"
   ```


The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

<br>

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at {ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf).

</details>


### Open the project

After the project has been created, you can open it in your preferred development environment.


<details><summary><b>Eclipse IDE</b></summary>

If you opened the Project Creator tool from the included Eclipse IDE, the project will open in Eclipse automatically.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>


<details><summary><b>Visual Studio (VS) Code</b></summary>

Launch VS Code manually, and then open the generated *{project-name}.code-workspace* file located in the project directory.

For more details, see the [Visual Studio Code for ModusToolbox&trade; user guide](https://www.infineon.com/MTBVSCodeUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_vscode_user_guide.pdf*).

</details>


<details><summary><b>Arm&reg; Keil&reg; µVision&reg;</b></summary>

Double-click the generated *{project-name}.cprj* file to launch the Keil&reg; µVision&reg; IDE.

For more details, see the [Arm&reg; Keil&reg; µVision&reg; for ModusToolbox&trade; user guide](https://www.infineon.com/MTBuVisionUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_uvision_user_guide.pdf*).

</details>


<details><summary><b>IAR Embedded Workbench</b></summary>

Open IAR Embedded Workbench manually, and create a new project. Then select the generated *{project-name}.ipcf* file located in the project directory.

For more details, see the [IAR Embedded Workbench for ModusToolbox&trade; user guide](https://www.infineon.com/MTBIARUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_iar_user_guide.pdf*).

</details>


<details><summary><b>Command line</b></summary>

If you prefer to use the CLI, open the appropriate terminal, and navigate to the project directory. On Windows, use the command-line 'modus-shell' program; on Linux and macOS, you can use any terminal application. From there, you can run various `make` commands.

For more details, see the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>


## Operation

1. Connect the board to your PC using the provided USB cable through the Debug USB connector on the board

2. Open a terminal program and select the Virtual COM port. Set the serial port parameters to 8N1 and 115200 baud

3. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE</b></summary>

      1. Select the application project in the Project Explorer

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)** or **\<Application Name> Program (JLink)**
   </details>


   <details><summary><b>In other IDEs</b></summary>

   Follow the instructions in your preferred IDE.

   </details>


   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain is specified in the application's Makefile but you can override this value manually:
      ```
      make program TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TOOLCHAIN=GCC_ARM
      ```
   </details>
4. After programming, the application starts automatically. Confirm that the yellow LED is on, enable motor power, and turn the potentiometer slowly. If a fault occurs, remove power, correct the cause, and reset the board before restarting.

## Debugging

You can debug the example to step through the code.


<details><summary><b>In Eclipse IDE</b></summary>

Use the **\<Application Name> Debug (KitProg3_MiniProg4)** or **\<Application Name> Debug (JLink)** configuration in the **Quick Panel**. For details, see the "Program and debug" section in the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide).

</details>


<details><summary><b>In other IDEs</b></summary>

Follow the instructions in your preferred IDE.

</details>


## Design and implementation

This code example demonstrates the use of MOTIF IP in Hall sensor mode to control a BLDC motor with trapezoidal commutation.
- Initializes the clocks and system resources through the BSP, then initializes the TCPWM, MOTIF, ADC, and GPIOs.
- Takes a fresh Hall sample at startup and when the motor returns to standstill, then applies the commutation pattern after a short timer delay.
- Uses the MOTIF wrong-Hall event and the external `N_FAULT_HW` input as independent hardware shutdown sources for all three PWM phases.
- Keeps the motor state in a `MOTIF_HALL_DRIVE_t` handle initialized from Device Configurator-generated resources.
- Keeps the yellow LED on during normal operation; a Hall or hardware fault turns on the red LED and remains latched until reset.

### Resources and settings

The following table lists the resources and pin assignments used in this example.

**Table 1. Application resources and pin assignments**

Resource | Alias/object | Pin or purpose
:-------- | :----------- | :-------------
TCPWM (PDL) | PWM_U | Phase U PWM; P4_0/P4_1 high/low outputs
TCPWM (PDL) | PWM_V | Phase V PWM; P4_2/P4_3 high/low outputs
TCPWM (PDL) | PWM_W | Phase W PWM; P4_4/P4_5 high/low outputs
TCPWM (PDL) | PattUpdate_Timer | One-shot delay for Hall re-sampling and commutation update
Motif0 (PDL) | MOTIF0 | MOTIF Hall interface and commutation pattern
GPIO (PDL) | HALL_0 | P7_4, Hall input to MOTIF
GPIO (PDL) | HALL_1 | P7_5, Hall input to MOTIF
GPIO (PDL) | HALL_2 | P7_6, Hall input to MOTIF
GPIO (PDL) | DIR_LED | P9_4, application active indicator
GPIO (PDL) | FAULT_LED_ALL | P9_5, latched fault indicator
GPIO (PDL) | N_FAULT_HW | P8_0, active-low hardware fault input
HPPASS (PDL) | CYBSP_POT | Potentiometer speed command

<br>
 
For the complete peripheral and clock configuration, refer to the target's `design.modus` file.

<br>

## Related resources

Resources  | Links
-----------|----------------------------------
Documentation | [PSOC&trade; Control C3 MCU documents](https://documentation.infineon.com/psoccontrolc3/docs/kfc1732622054982)
Development kits | [PSOC&trade; Control C3 development kits](https://documentation.infineon.com/psoccontrolc3/docs/yyw1732688626489)
Tools, BSPs, libraries, and code examples | [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use libraries and tools enabling rapid development with Infineon MCUs for applications ranging from wireless and cloud-connected systems, edge AI/ML, embedded sense and control, to wired USB connectivity using PSOC&trade; Industrial/IoT MCUs, AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices, XMC&trade; Industrial MCUs, and EZ-USB&trade;/EZ-PD&trade; wired connectivity controllers. ModusToolbox&trade; incorporates a comprehensive set of BSPs, HAL, libraries, and configuration tools with support for industry-standard IDEs.

<br>


## Other resources

Infineon provides a wealth of data at [www.infineon.com](https://www.infineon.com) to help you select the right device, and quickly and effectively integrate it into your design.


## Document history

Document title: *CE240615* - *MOTIF Hall sensor mode*

 Version | Description of change
 ------- | ---------------------
 1.0.0   | New code example
<br>


All referenced product or service names and trademarks are the property of their respective owners.

The Bluetooth&reg; word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.

PSOC&trade;, formerly known as PSoC&trade;, is a trademark of Infineon Technologies. Any references to PSoC&trade; in this document or others shall be deemed to refer to PSOC&trade;.

---------------------------------------------------------

(c) 2024-2026, Infineon Technologies AG, or an affiliate of Infineon Technologies AG. All rights reserved.
This software, associated documentation and materials ("Software") is owned by Infineon Technologies AG or one of its affiliates ("Infineon") and is protected by and subject to worldwide patent protection, worldwide copyright laws, and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software. If no license agreement applies, then any use, reproduction, modification, translation, or compilation of this Software is prohibited without the express written permission of Infineon.
<br>
Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A SPECIFIC USE/PURPOSE OR MERCHANTABILITY. Infineon reserves the right to make changes to the Software without notice. You are responsible for properly designing, programming, and testing the functionality and safety of your intended application of the Software, as well as complying with any legal requirements related to its use. Infineon does not guarantee that the Software will be free from intrusion, data theft or loss, or other breaches (“Security Breaches”), and Infineon shall have no liability arising out of any Security Breaches. Unless otherwise explicitly approved by Infineon, the Software may not be used in any application where a failure of the Product or any consequences of the use thereof can reasonably be expected to result in personal injury.
