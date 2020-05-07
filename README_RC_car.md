

# Hand Pose RC car<a name="EN-US_TOPIC_0232642690"></a>

You can deploy this application on the Atlas 200 DK to collect camera data in real time and detect hand keypoints to control an RC car.

The applications in the current version branch adapt to  [DDK&RunTime](https://ascend.huawei.com/resources) **1.32.0.0 and later**.


## Hardware preparation<a name="EN-US_TOPIC_0232642690"></a><a name="en-us_topic_0228461904_section412811285117"></a>

### Parts list 
To deploy project with RC car:

-   Atlas200DK 
-   [Raspberry Pi camera](https://www.amazon.ca/seeed-studio-Raspberry-Official-V2%EF%BC%8C1080p/dp/B07Y33ZQZN/ref=sr_1_2_sspa?crid=TO0NJH5H3I34&keywords=raspberry+pi+camera+v2+noir&qid=1588817939&sprefix=Raspberry+pi+camera+v%2Caps%2C214&sr=8-2-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUExODRaNlo3VVpTN0NJJmVuY3J5cHRlZElkPUEwOTIzNTgxTU0xWFBVMDMyOTRLJmVuY3J5cHRlZEFkSWQ9QTAxMzM0MzkxMlRYQjRCTEdNVzk3JndpZGdldE5hbWU9c3BfYXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ==)
-   [2 HC-05 Bluetooth modules](https://www.amazon.ca/DSD-TECH-HC-05-Pass-Through-Communication/dp/B01G9KSAF6/ref=sr_1_1_sspa?keywords=hc05&qid=1588817970&sr=8-1-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUFaQk44T0QyTjhHME4mZW5jcnlwdGVkSWQ9QTA4Njc1MjEyNEpTRDBFQTFGUThBJmVuY3J5cHRlZEFkSWQ9QTA3NjI2MjIzR1MzOEdQWjhUUEkxJndpZGdldE5hbWU9c3BfYXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ==)
-   [Elegoo Smart Robot Car Kit](https://www.amazon.ca/ELEGOO-Ultrasonic-Bluetooth-Intelligent-Educational/dp/B07485YQP8)
-   [Arduino UNO](https://www.amazon.ca/Elegoo-Board-ATmega328P-ATMEGA16U2-Arduino/dp/B01EWOE0UU/ref=sr_1_3?keywords=Arduino+Uno&qid=1588817396&sr=8-3) (optional based on deployment method) 
-   General project components of choice: breadboards, soldering kit, resistors, jumper wires, etc

Note that the Elegoo Smart Robot Car Kit comes with components including an Arduino UNO and a Bluetooth module. The Bluetooth module can vary, so it may be simpler to get 2 HC-05 modules and use those. <br />

### Assemble parts 
Before deploying the project:
-   [Assemble the Elegoo Robot Car](https://dronebotworkshop.com/building-the-elegoo-smart-robot-car-part-1/) (manual also available in the kit)
-   [Connect the Raspberry Pi camera to Atlas200DK](https://www.huaweicloud.com/intl/en-us/ascend/doc/Atlas200DK/1.32.0.0(beta)/en/en-us_topic_0204328003.html)

### Two hardware configurations are available:

1.  Send data from Atlas200DK through I2C: visit  [https://github.com/Atlas200dk-test/handposeRCcar/tree/master/sample-handposeRC-I2C](https://github.com/Atlas200dk-test/handposeRCcar/tree/master/sample-handposeRC-I2C).

    >![](public_sys-resources/icon-note.gif) **NOTE:**   
    >-   The RC car control command is sent to an Arduino through I2C, which transmits it to the RC car Arduino through Bluetooth.  
    >-   This method demonstrates I2C communication between Atlas and Arduino boards.

2.  <a name="en-us_topic_0228461904_li3208251440"></a>Send data from Atlas200DK through UART: visit  [https://github.com/Atlas200dk-test/handposeRCcar/tree/master/sample-handposeRC-UART](https://github.com/Atlas200dk-test/handposeRCcar/tree/master/sample-handposeRC-UART).

    >![](public_sys-resources/icon-note.gif) **NOTE:**   
    >-   The RC car control command is sent through UART directly to transmitter Bluetooth module. 
    >-   This method does not need the optional Arduino UNO.
    
Download all files for the chosen method and follow the wiring configuration in the Readme. 

## Software preparations<a name="EN-US_TOPIC_0232642690"></a>

### Arduino
After downloading the files for the choosen hardware configuration, upload the codes from the Arduino directory to the corresponding Arduino(s). 

### Atlas200DK
-  Setup MindStudio and development environment [using ADKInstaller](https://www.huaweicloud.com/intl/en-us/ascend/doc/Atlas200DK/1.32.0.0(beta)/en/en-us_topic_0238626392.html)
-  Setup SD card [using ADKInstaller](https://www.huaweicloud.com/intl/en-us/ascend/doc/Atlas200DK/1.32.0.0(beta)/en/en-us_topic_0238626392.html) 


### HC-05 Bluetooth modules
Configure the HC-05 Bluetooth modules in AT mode as master and slave. Use a baudrate of 38400 for both. The modules should automatically connect when powered on. ([tutorial](https://howtomechatronics.com/tutorials/arduino/how-to-configure-pair-two-hc-05-bluetooth-module-master-slave-commands/))
<br />

## Atlas200DK application overview (C++)<a name="en-us_topic_0228461904_section7994174585917"></a>

### Code layout
The Handpose application can be deployed on the Atlas200DK with or without the RC car. 
The program consists of 4 engines. In the **src** directory, each engine has a folder with a **.cpp** and **.h** file.
The engines are connected in the **graph.config** file.

### Engines
This application contains the following engines:
-  **Camera**
-  **Inference**
-  **Postprocess**

<br />**Camera engine** takes frames from the Raspberry Pi camera and sends them to the inference engine. <br /><br />
**Inference engine** resizes the frames using [DVPP](https://www.huaweicloud.com/intl/en-us/ascend/doc/Atlas200DK/1.32.0.0(beta)/en/en-us_topic_0204324961.html) and then performs inference using the [Offline Model](https://www.huaweicloud.com/intl/en-us/ascend/doc/Atlas200DK/1.32.0.0(beta)/en/en-us_topic_0204328934.html). It then sends the inference results to post process engine.<br /><br />
**Postprocess engine** computes the keypoint coordinates from the inference output. It then computes an RC command (forward, backward, left, right, stop) from the keypoint coordinates. Finally, it sends the results to [presenter server](https://github.com/Atlas200dk/sdk-presenter) and [I2C](https://github.com/Atlas200dk/hardware_expansion/tree/master/sample-i2c)/[UART](https://github.com/Atlas200dk/hardware_expansion/tree/master/sample-uart).


## Building a Project<a name="en-us_topic_0228461904_section7994174585917"></a>

1.  Open the project.

    Go to the directory that stores the decompressed installation package as the Mind Studio installation user in CLI mode, for example,  **$HOME/MindStudio-ubuntu/bin**. Run the following command to start Mind Studio:

    **./MindStudio.sh**

    Open the  **sample-handposeRC-I2C**  project, as shown in  [Figure 1](#en-us_topic_0228461904_en-us_topic_0203223294_fig05481157171918).

    **Figure  1**  Opening the handpose project<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig05481157171918"></a>  
    

    ![](sample-handposeRC-I2C/figures/openandselect.png

2.  Configure project information in the  **src/param\_configure.conf**  file.

    For details, see  [Figure 2](#en-us_topic_0228461904_en-us_topic_0203223294_fig0391184062214).

    **Figure  2**  Configuration file<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig0391184062214"></a>  
    

    ![](sample-handposeRC-I2C/figures/conf.png)

    The default configurations of the configuration file are as follows:

    ```
    remote_host=192.168.1.2
    data_source=Channel-1
    presenter_view_app_name=video
    ```

    -   **remote\_host**: IP address of the Atlas 200 DK developer board
    -   _data\_source_: camera channel. The value can be  **Channel-1**  or  **Channel-2**. For details, see "Viewing the Channel to Which a Camera Belongs" in  [Atlas 200 DK User Guide](https://ascend.huawei.com/documentation).
    -   _presenter\_view\_app\_name_: value of  **View Name**  on the  **Presenter Server**  page, which must be unique. The value consists of at least one character and supports only uppercase letters, lowercase letters, digits, and underscores \(\_\).

    >![](public_sys-resources/icon-note.gif) **NOTE:**   
    >-   All the three parameters must be set. Otherwise, the build fails.  
    >-   Do not use double quotation marks \(""\) during parameter settings.  
    >-   Modify the default configurations as required.  

3.  Run the  **deploy.sh**  script to adjust configuration parameters and download and compile the third-party library. Open the  **Terminal**  window of Mind Studio. By default, the home directory of the code is used. Run the  **deploy.sh**  script in the background to deploy the environment, as shown in  [Executing the deploy script](#en-us_topic_0228461904_en-us_topic_0203223294_fig107831626101910).

    **Figure  3**  Running the deploy.sh script<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig107831626101910"></a>  
    ![](sample-handposeRC-I2C/figures/deploy.png "running-the-deploy-sh-script")

    >![](public_sys-resources/icon-note.gif) **NOTE:**   
    >-   During the first deployment, if no third-party library is used, the system automatically downloads and builds the third-party library, which may take a long time. The third-party library can be directly used for the subsequent build.  
    >-   During deployment, select the IP address of the host that communicates with the developer board. Generally, the IP address is that configured for the virtual NIC. If the IP address is in the same network segment as the IP address of the developer board, it is automatically selected for deployment. If they are not in the same network segment, you need to manually type the IP address of the host that communicates with the developer board to complete the deployment.  

4.  Start building. Open Mind Studio and choose  **Build \> Build \> Build-Configuration**  from the main menu. The  **build**  and  **run**  folders are generated in the directory, as shown in  [Figure 4](#en-us_topic_0228461904_en-us_topic_0203223294_fig1625447397).

    **Figure  4**  Build and files generated<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig1625447397"></a>  
    

    ![](sample-handposeRC-I2C/figures/build.png)

    >![](public_sys-resources/icon-notice.gif) **NOTICE:**   
    >When you build a project for the first time,  **Build \> Build**  is unavailable. You need to choose  **Build \> Edit Build Configuration**  to set parameters before the build.  

5.  Start Presenter Server.

    Open the  **Terminal**  window of Mind Studio. Under the code storage path, run the following command to start the Presenter Server program of the hand pose application on the server, as shown in  [Figure 5](#en-us_topic_0228461904_en-us_topic_0203223294_fig423515251067):

    **bash run\_present\_server.sh**

    **Figure  5**  Starting Presenter Server<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig423515251067"></a>  
    

    ![](sample-handposeRC-I2C/figures/presenterserver.png)

    When the message  **Please choose one to show the presenter in browser\(default: 127.0.0.1\):**  is displayed, type the IP address \(usually IP address for accessing Mind Studio\) used for accessing the Presenter Server service in the browser.

    Select the IP address used by the browser to access the Presenter Server service in  **Current environment valid ip list**, as shown in  [Figure 6](#en-us_topic_0228461904_en-us_topic_0203223294_fig999812514814).

    **Figure  6**  Project deployment<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig999812514814"></a>  
    

    ![](sample-handposeRC-I2C/figures/presdeploy.png)

    [Figure 7](#en-us_topic_0228461904_en-us_topic_0203223294_fig69531305324)  shows that the Presenter Server service has been started successfully.

    **Figure  7**  Starting the Presenter Server process<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig69531305324"></a>  
    

    ![](sample-handposeRC-I2C/figures/prestart.png)

    Use the URL shown in the preceding figure to log in to Presenter Server. The IP address is that typed in  [Figure 6](#en-us_topic_0228461904_en-us_topic_0203223294_fig999812514814)  and the default port number is  **7007**. The following figure indicates that Presenter Server has been started successfully.

    **Figure  8**  Home page<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig64391558352"></a>  
    ![](sample-handposeRC-I2C/figures/home-page.png "home-page")

    The following figure shows the IP address used by Presenter Server and  Mind Studio  to communicate with the Atlas 200 DK.

    **Figure  9**  IP address example<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig1881532172010"></a>  
    ![](sample-handposeRC-I2C/figures/ip-address-example.png "ip-address-example")

    In the preceding figure:

    -   The IP address of the Atlas 200 DK developer board is  **192.168.1.2**  \(connected in USB mode\).
    -   The IP address used by Presenter Server to communicate with the Atlas 200 DK is in the same network segment as the IP address of the Atlas 200 DK on the UI Host server, for example,  **192.168.1.223**.
    -   The following describes how to access the IP address \(such as  **10.10.0.1**\) of Presenter Server using a browser. Because Presenter Server and  Mind Studio  are deployed on the same server, you can access  Mind Studio  through the browser using the same IP address. 


## Run<a name="en-us_topic_0228461904_section551710297235"></a>

1.  Run the hand pose application.

    On the toolbar of Mind Studio, click  **Run**  and choose  **Run \> Run 'sample-handposeRC-I2C'**. As shown in  [Figure 10](#en-us_topic_0228461904_en-us_topic_0203223294_fig93931954162719), the executable application is running on the developer board.

    **Figure  10**  Application running sample<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig93931954162719"></a>  
    

    ![](sample-handposeRC-I2C/figures/startrun.png)

2.  Use the URL displayed upon the start of the Presenter Server service to log in to Presenter Server.

    Wait for Presenter Agent to transmit data to the server. Click  **Refresh**. When there is data, the icon in the  **Status**  column for the corresponding channel changes to green, as shown in the following figure.

    **Figure  11**  Presenter Server page<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig113691556202312"></a>  
    ![](sample-handposeRC-I2C/figures/home-page2.png "presenter-server-page")

    >![](public_sys-resources/icon-note.gif) **NOTE:**   
    >-   For the hand pose application, Presenter Server supports a maximum of 10 channels at the same time \(each  _presenter\_view\_app\_name_  parameter corresponds to a channel\).  
    >-   Due to hardware limitations, each channel supports a maximum frame rate of 20 fps. A lower frame rate is automatically used when the network bandwidth is low.  

3.  Run the  **deploy.sh**  script to adjust configuration parameters and download and compile the third-party library. Open the  **Terminal**  tab page of Mind Studio. By default, the home directory of the code is used. Run the  **deploy.sh**  script in the background to deploy the environment, as shown in the figure.

    >![](public_sys-resources/icon-note.gif) **NOTE:**   
    >-   During the first deployment, if no third-party library is used, the system automatically downloads and compiles the third-party library, which may take a long time. The third-party library can be directly used for the subsequent compilation.  
    >-   During deployment, select the IP address of the host that communicates with the developer board. Generally, the IP address is the IP address configured for the virtual NIC. If the IP address is in the same network segment as the IP address of the developer board, it is automatically selected for deployment. If they are not in the same network segment, you need to manually type the IP address of the host that communicates with the Atlas DK to complete the deployment.  

4.  Click the link \(such as  **video**  in the preceding figure\) in the  **View Name**  column to view the result. The confidence of the detected human hand is marked.

## Follow-up Operations<a name="en-us_topic_0228461904_section177619345260"></a>

-   Stopping the hand pose application

    The hand pose application is running continually after being executed. To stop it, perform the following operation:

    Click the stop button shown in  [Figure 12](#en-us_topic_0228461904_en-us_topic_0203223294_fig14326454172518)  to stop the hand pose application.

    **Figure  12**  Stopping hand pose<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig14326454172518"></a>  
    

    ![](sample-handposeRC-I2C/figures/runstop.png)

    [Figure 13](#en-us_topic_0228461904_en-us_topic_0203223294_fig2182182518112)  shows that the hand pose application has been stopped.

    **Figure  13**  Hand pose stopped<a name="en-us_topic_0228461904_en-us_topic_0203223294_fig2182182518112"></a>  
    

    ![](sample-handposeRC-I2C/figures/runstopped.PNG)

-   Stopping the Presenter Server service

    The Presenter Server service is always in running state after being started. To stop the Presenter Server service of the hand pose application, perform the following operations:

    On the server with  Mind Studio  installed, run the following command as the  Mind Studio  installation user to check the process of the Presenter Server service corresponding to the hand pose application:

    **ps -ef | grep presenter | grep hand\_pose**

    ```
    ascend@ascend-HP-ProDesk-600-G4-PCI-MT:~/sample-handposeRC-I2C$ ps -ef | grep presenter | grep hand_pose
    ascend    7701  1615  0 14:21 pts/8    00:00:00 python3 presenterserver/presenter_server.py --app hand_pose
    ```

    In the preceding information,  _7701_  indicates the process ID of the Presenter Server service for the hand pose application.

    To stop the service, run the following command:

    **kill -9** _7701_

