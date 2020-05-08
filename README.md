# Hand Pose RC car<a name="EN-US_TOPIC_0232642690"></a>

This project showcases the Atlas200DK board used for controlling an RC car with hand gestures.<br />
Demo video: [demo]

### Hand gestures
![](sample-handposeRC-I2C/figures/gestures.png)
## Project overview

This project consists of two parts:
-   Atlas200DK application
-   Peripherial devices (Arduino, RC car, etc)


<br />**The Atlas200DK application can be deployed on its own.** The program will run even if the peripherial devices are not connected. In this case, hand keypoints and the control command are displayed in real time as shown in the demo video.<br /><br />


### Deployment steps
The first step is to set up the Atlas200DK board and run the application. Follow the guide: <br />
[https://github.com/Atlas200dk-test/handposeRCcar/blob/master/sample-handposeRC-I2C/Atlas200DK/README.md](https://github.com/Atlas200dk-test/handposeRCcar/blob/master/sample-handposeRC-I2C/Atlas200DK/README.md)
<br /><br />

The second step is to set up the peripherial devices. Follow the guide: <br />
[https://github.com/Atlas200dk-test/handposeRCcar/blob/master/README_RC_car.md](https://github.com/Atlas200dk-test/handposeRCcar/blob/master/README_RC_car.md)

<br /><br />
>Note: There is two different configurations for the peripherial devices (I2C, UART). The different is explained in the step 2 guide.











