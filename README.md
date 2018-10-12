# DeviceObserver

**
## 可能是东半球最好用的 android设备标识库
** 


### 项目介绍
DeviceObserver  获取设备的唯一标识,可靠，快速。  
专门针对  xposed等hook软件，刷机，权限限制等极端设备环境。


说明文档地址：  https://www.jianshu.com/p/bbdad74ea2e3

![说明文档](https://images.gitee.com/uploads/images/2018/1012/143304_df35b554_62207.png "屏幕截图.png")


### 一  如果你想帮忙测试，可以这样：

#### 第一

[下载演示apk](https://gitee.com/dqq/DeviceObserver/blob/master/apk/observer_demo_0.0.5.apk) 至你的手机，进行正常的安装操作


#### 第二

点击获取设备id,查看 id以及设备描述信息是否正确。 （可以尝试手动作弊，或者其他复杂环境下，查看id是否能正确识别）  

类似这样：  

![已存在id设备](https://images.gitee.com/uploads/images/2018/1012/134405_7f3e09be_62207.png "屏幕截图.png")

或者这样：

![hook设备](https://images.gitee.com/uploads/images/2018/1012/140125_e0008350_62207.png "屏幕截图.png")


#### 第三

如果有崩溃问题，或者其他任何异常，欢迎在这里反馈给我：

https://gitee.com/dqq/DeviceObserver/issues

  

*************************************
  


### 二  如果使用本项目，进行开发




#### 1 引入

本项目已上传至jcenter仓库


```
// 引入设备指纹库
api 'com.dqqdo:DeviceObserver:0.0.3'
```



#### 2 初始化组件

![ 初始化](https://images.gitee.com/uploads/images/2018/1012/134101_14d5d21b_62207.png "屏幕截图.png")



#### 3 获取设备id

![获取设备id](https://images.gitee.com/uploads/images/2018/1012/134200_1855671b_62207.png "屏幕截图.png")
