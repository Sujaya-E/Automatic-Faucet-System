# Automatic Faucet System

## Project Abstract
- The Automatic Faucet System, powered by the **ARM Cortex M3-based LPC1768** microcontroller, leverages the **Keil RTX kernel RTOS** to achieve efficient water dispensing automation. The system integrates an **IR sensor** module, a **16x2 LCD** display in 4-bit mode, and an **SG90 servo motor** for precise and responsive operation.
- The implementation relies on RTOS events for task synchronization, ensuring that the sensor input and motor & LCD outputs function in coordination without conflicts. The use of events allows the system to remain responsive while conserving CPU resources by running tasks only when specific triggers occur. This approach simplifies multitasking and improves the reliability of the system.
- Designed for hygiene and resource efficiency, the system automates water dispensing, reducing wastage and enhancing user convenience. By combining real-time control with effective RTOS utilization, the project demonstrates practical expertise in embedded systems and RTOS-based applications.  

## System Architecture
![image](https://github.com/user-attachments/assets/aa319b1b-11b8-481e-a43d-6b48f219c3b3)

## System Functioning Overview
![image](https://github.com/user-attachments/assets/c8bd5c6f-5208-41a1-8802-d576711aebf7)

## Contributing Teammates
- [E Sujaya](https://github.com/Sujaya-E)
- Amish Kulkarni
- Ujwal Shanbhag
