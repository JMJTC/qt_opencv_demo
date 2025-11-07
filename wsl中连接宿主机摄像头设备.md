WSL2调用摄像头并使用OpenCV
yiyeyeshenlan
2025-10-25 8:37
WSL2（Windows Subsystem for Linux 2）默认情况下无法直接访问 USB 设备，包括 摄像头。本文将详细介绍如何通过重新编译 WSL2 内核来启用 USB 摄像头支持。

前言
要在 WSL2 中使用 USB 摄像头，我们需要完成以下三个主要步骤：

将摄像头从 Windows 连接到 WSL2
重新编译 WSL2 Linux 内核以包含摄像头驱动
配置 WSL2 使用新编译的内核
一、将摄像头连接到 WSL2
1.1 安装 USB/IP 工具
以管理员身份打开 PowerShell，执行以下命令安装 usbipd：

复制代码
winget install usbipd
1.2 列出已连接的 USB 设备
复制代码
usbipd list
该命令会显示所有连接到系统的 USB 设备，找到你的摄像头对应的 BUSID（例如 1-6）。

1.3 绑定摄像头设备

将摄像头绑定以便共享给 WSL2（假设摄像头的 BUSID 是 1-6）：

复制代码
usbipd bind -b 1-6
1.4 验证绑定状态
再次查看设备列表，确认摄像头状态已更新为 Shared：

复制代码
usbipd list

1.5 将设备附加到 WSL2
复制代码
usbipd attach -w -b 1-6
再次运行usbpid list， 摄像头STATE变为Attached



1.6 在 WSL2 中验证连接
打开 WSL2 终端，使用以下命令验证摄像头已成功连接：

复制代码
lsusb
如果看到摄像头设备信息，说明连接成功。

然而此时运行调用摄像头的代码，会显示报错，因为WSL2是一个独立的子系统，并没有摄像头驱动，需要继续重新编译内核

复制代码
[ WARN:0@8.994] global cap_v4l.cpp:914 open VIDEOIO(V4L2:/dev/video0): can't open camera by index
[ WARN:0@8.994] global cap.cpp:478 open VIDEOIO(V4L2): backend is generally available but can't be used to capture by index
无法打开摄像头 0
错误: 无法打开摄像头
二、重新编译 WSL2 Linux 内核
2.1 更新 WSL
在 PowerShell 中执行：

复制代码
wsl --update
2.2 安装编译依赖
在 WSL2 Ubuntu 终端中，更新系统并安装必要的编译工具：

复制代码
sudo apt update && sudo apt upgrade -y && sudo apt install -y build-essential flex bison dwarves libssl-dev libelf-dev libncurses-dev
2.3 检查 WSL2 内核版本
复制代码
uname -r

记录输出的版本号，例如 6.6.87.2-microsoft-standard-WSL2。

2.4 下载对应版本的内核源码
根据上一步获取的版本号，下载对应的 WSL2 内核源码：

复制代码
cd /usr/src
TAGVERNUM=6.6.87.2
TAGVER=linux-msft-wsl-${TAGVERNUM}
sudo git clone \
  -b ${TAGVER} \
  https://github.com/microsoft/WSL2-Linux-Kernel.git \
  ${TAGVERNUM}-microsoft-standard \
  && cd ${TAGVERNUM}-microsoft-standard
2.5 准备内核配置文件
复制代码
sudo cp /proc/config.gz config.gz \
  && sudo gunzip config.gz \
  && sudo mv config .config
2.6 配置内核选项
启动内核配置界面：



依次选择Device Drivers - Multimedia support

将Filter media drivers 的方框中按Y，注意按Y以后显示的是 *


然后继续来到Media device types，Cameras and video grabbers 方框输入Y


上述流程可总结如下

复制代码
sudo make menuconfig
```

在配置界面中，使用方向键导航，按 `Y` 键选中选项（选中后会显示 `[*]`），按照以下路径启用所需的驱动：

**必须启用的选项：**
```
Device Drivers 
  → Multimedia support 
    → Filter media drivers [Y]
    → Media device types 
      → Cameras and video grabbers [Y]
操作说明：

使用方向键上下移动
按 Enter 进入子菜单
按 Y 启用选项（显示为 [*]）
按 N 禁用选项
按 M 编译为模块（显示为 <M>）
配置完成后，选择 Save 保存配置，然后选择 Exit 退出。

2.7 编译内核
使用多核并行编译以加快速度：

复制代码
sudo make -j$(nproc) KCONFIG_CONFIG=.config \
  && sudo make modules_install -j$(nproc) \
  && sudo make install -j$(nproc)
编译过程可能需要较长时间，请耐心等待。

三、安装和配置新内核
3.1 复制内核文件到 Windows
编译完成后，将生成的 vmlinux 内核文件复制到 Windows 系统：

复制代码
sudo cp vmlinux /mnt/c/Users/<UserName>/WSL2/kernel-6.6.87.2
注意：将 <UserName> 替换为你的 Windows 用户名，目标路径可以自定义。

而且这是将内核文件改路径以后重命名为kernel-6.6.87.2，不是放到kernel-6.6.87.2文件夹中


3.2 创建 WSL 配置文件
在 Windows 用户目录下创建 .wslconfig 配置文件：

复制代码
vim /mnt/c/Users/<UserName>/.wslconfig
按 i 键进入编辑模式，添加以下内容：

复制代码
[wsl2]
kernel=C:\\Users\\<UserName>\\WSL2\\kernel-6.6.87.2
注意：

将 <UserName> 替换为你的 Windows 用户名
Windows 路径需要使用双反斜杠 \\
按 ESC 退出编辑模式，输入 :wq 保存并退出。

四、测试 摄像头
4.1 重启 WSL2
在 PowerShell 中执行：

复制代码
wsl --shutdown
4.2 重新连接摄像头
复制代码
usbipd attach -w -b 1-6
4.3 验证设备连接
在 WSL2 终端中：

复制代码
lsusb
4.4 验证新内核加载
检查内核版本，如果成功加载自定义内核，版本号末尾会显示 + 号：

复制代码
uname -r -v
```

输出示例：
```
6.6.87.2-microsoft-standard-WSL2+ 
4.5 安装和测试摄像头应用
安装 V4L 工具和摄像头查看器：

复制代码
sudo apt install v4l-utils guvcview
启动摄像头应用：

复制代码
sudo guvcview
如果一切正常，你应该能看到摄像头的实时画面。

4.6 使用OpenCV
继续在WSL2中新建如下python脚本，使用OpenCV展示

python
复制代码
import cv2
cap = cv2.VideoCapture(0)  # /dev/video0
cap.set(3,640)
cap.set(4,480)
cap.set(cv2.CAP_PROP_FPS,10)
#cap.set(cv2.CAP_PROP_FOURCC,cv2.VideoWriter.fourcc('Y','U','Y','V')) # 下面不行可以尝试这个
cap.set(cv2.CAP_PROP_FOURCC,cv2.VideoWriter.fourcc('M','J','P','G'))
while(True):
    ret, frame = cap.read()
    if not ret:
        print("Failed to read the image.")
        break
    cv2.imshow('Video', frame)
    key = cv2.waitKey(1)
cap.release()
即可成功调用摄像头

参考资料
PINTO0309/wsl2_linux_kernel_usbcam_enable_conf
WSL2 Webcam Support - Medium
https://blog.csdn.net/chengpengfei09121870/article/details/142762468?ops_request_misc=elastic_search_misc&request_id=db0ed97b4b37eb05f6c98156194cf227&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~ElasticCommercialInsert~search_v2-3-142762468-null-null.142^v102^pc_search_result_base3&utm_term=WSL2%E8%B0%83%E7%94%A8%E6%91%84%E5%83%8F%E5%A4%B4&spm=1018.2226.3001.4187
总结
通过重新编译 WSL2 内核并启用相应的 USB 摄像头驱动，我们可以在 WSL2 环境中成功使用 USB 摄像头。虽然这个过程较为复杂，但一旦配置完成，就可以在 Linux 环境中进行各种摄像头相关的开发和测试工作。