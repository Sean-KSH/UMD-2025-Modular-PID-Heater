# Modular Heating and Flow Control System for Photonic Sensor Testing
## Sean Kim Summer 2025

## Overview

This repository contains the hardware designs and software implementations I developed during my Summer 2025 research internship at the **University of Maryland's Sensors and Actuators Lab**.

My work focused on building tools and devices to enable and improve the research of optical and photonic sensors. These sensors are an emerging technology designed to detect chemical compounds in the environment using variations in optical patterns.

A key challenge in analyzing photonic sensors is their sensitivity to temperature. Changes in temperature alter the material’s refractive indices, resonance wavelengths, and energy levels, leading to optical drift and measurement errors. However, prior to this project, no test setup existed to specifically evaluate sensor behaviors under controlled temperature variation.

To address this, I designed and built a **PID-controlled heating system** and an **easy-to-use LabVIEW interface** that:

* Enables homogeneous heating of both the gas and the photonic sensor.
* Provides precise control of gas flow rates using Mass Flow Controllers (MFCs).
* Improves the reliability of sensor evaluation and characterization.

---

## Repository Contents

* **Custom Heater.zip**
  KiCAD symbols, footprints, and designs for the custom heater schematic and PCB.

* **LabVIEW Controller.zip**
  Data-flow programming–based LabVIEW UI files to easily control gas mixtures created by MFCs.

* **Working software.zip**
  MCU files and supporting programs for testing and running the PID controller.

* **Modular\_Heating\_Instrument\_Documentation\_Sean\_Kim.pdf**
  Full documentation of the modular heating and flow control system, including design, operation, and test procedures.

---

## Acknowledgements

I am deeply grateful to:

* **Dr. Miao Yu**
* **Bibek Ramdam**
* **Rafael Santiago Meza**

for their mentorship, guidance, and for providing me the opportunity to contribute to this project.
