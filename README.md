# Vega Quadruped Robot
![vega_logo](https://user-images.githubusercontent.com/79312187/215331109-e8b1cbe2-d980-4bfb-af8b-75623a9cdf5e.png)

<p align="justify"> VEGA is a quadruped robot dog, being developed on my own and inspired by Boston Dynamics achivements and Youtube hobby projects.
The parts are entirely designed in FreeCAD, except the servo arms, which are bought from a model shop. I have tried to make my best
and use as much newer technologies (especially ML) as I can. The kinematics model is implemented by me after I have researched deep
into trigonometrics. This device is utilizing az Arduino Pro Mini for executing movements and sense distance of obstuctions, charge
level of battery pack. Features an FHD camera connected to a Raspberry Pi Zero 2W board. This board acts as a server one to which the
controller program - running on a PC or laptop - can connect on LAN. The Arduino and Raspberry board can communicate each other
via serial port. I have made a hybrid machine learning model in order to the user can instruct the robot with different (9) hand
gestures. This option needs a laptop of course. The movements are not the best yet, but I will further correct the Arduino's firmware 
to reach an acceptable level as soon as possible. And I have many other ideas to bring into effect too.</p>

> ## Key parts
+ Raspberry Pi Zero 2W
+ Arduino Pro Mini
+ 12 Servo motor (25KG)
+ 4 pcs 18650 Li-ion batteries in 4S configuration
+ FHD video camera
+ Sharp infrared distance sensor

> ## Key features
+ Graphical user interface (written in Python, DearPyGUI framework)
+ Control with buttons and hand gestures
+ Streaming video from Raspberry to the GUI
+ Automated search over the LAN for the robot
+ and many more...

![gui](https://user-images.githubusercontent.com/79312187/215334141-7f27ade3-302e-4930-9aee-019a3faca2bd.png)

![PXL_20221028_130952616](https://user-images.githubusercontent.com/79312187/215333554-42b040a9-3e83-48e1-b762-74e65560d03b.png)

![PXL_20221028_131031536](https://user-images.githubusercontent.com/79312187/215333565-92723583-5222-41ff-9fbc-8b2ce134e130.png)

![PXL_20221028_131001961](https://user-images.githubusercontent.com/79312187/215333578-120e224d-1bff-4057-8dd3-571be32c6d98.png)

![PXL_20221028_130601032](https://user-images.githubusercontent.com/79312187/215333587-af793ed9-9900-41cf-b47f-00f30828a86d.png)

![PXL_20221111_2245372822](https://user-images.githubusercontent.com/79312187/215333690-e7aca23f-4714-43f2-a54c-89d40cd6231f.png)
