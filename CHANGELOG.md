## [v20230829]
 - 增加驱动,osmsg,extgpio,pwmbeeper
 - 预安装osmsg,extgpio驱动到文件系统
 - 修改DTS，支持SD卡，支持PLC状态引脚显示等

## [v20230413] 

- 增加自动主机名逻辑，使用neptuen-XXXXXX, 其中，XXXXXX为eth0的MAC地址后6位
- 修正P3448-0002量产版与P3451底板的兼容性问题，USB接口不可用的问题
- 修改内核console日志等级，开启更多信息到串口
- 预安装ros2-foxy-base

