@startuml
start
:程序启动;
:初始化MainWindow及UI组件;
:创建VideoPlayer、按钮、滤镜选择框等;
:连接信号与槽函数;

fork
  :用户操作;
  :点击"打开"按钮;
  :选择媒体文件(图像/视频);
  if (是图像文件?) then (是)
    :加载图像到currentImage;
    :设置当前媒体类型为Image;
    :应用当前滤镜(默认无);
    :调用displayImage()显示处理后图像;
    :禁用播放按钮和滑块;
  else (否)
    :尝试加载视频文件;
    if (加载成功?) then (是)
      :设置当前媒体类型为Video;
      :启用播放按钮和滑块;
    else (否)
      :提示无法加载媒体文件;
    endif
  endif
fork again
  :用户操作;
  :选择滤镜(灰度/边缘检测/高斯模糊);
  if (当前媒体类型是Image?) then (是)
    :对currentImage应用选中的滤镜;
    :更新显示图像;
  else if (当前媒体类型是Video?) then (是)
    :设置VideoPlayer的当前滤镜;
    :后续帧将应用该滤镜;
  endif
fork again
  :用户操作;
  :点击"播放/暂停"按钮;
  if (当前是视频且播放中?) then (是)
    :调用VideoPlayer::pause();
    :按钮文本改为"播放";
  else (否)
    :调用VideoPlayer::play();
    :按钮文本改为"暂停";
  endif
fork again
  :用户操作;
  :拖动滑块调整位置;
  :释放滑块时调用setPosition();
  :VideoPlayer跳转至指定位置;
  :更新当前帧并显示;
fork again
  :VideoPlayer内部处理;
  :定时器触发processFrame();
  :读取视频帧;
  if (帧为空?) then (是)
    :跳转到视频开头;
    :读取第一帧并显示;
    :暂停播放;
  else (否)
    :应用当前滤镜;
    :转换为QImage并发送frameReady信号;
    :计算当前位置并发送positionChanged信号;
    :MainWindow更新滑块位置;
  endif
end fork

stop
@enduml