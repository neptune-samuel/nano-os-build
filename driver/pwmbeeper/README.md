
### 无源蜂鸣器驱动

---- by 刘传森 20220119

##### DTS配置

 占空比不使用，默认50，可通过sysfs修改

```dts
beeper {
    compatible = "pwm-beeper";
    //pwms = <&pwm 0 1000000 0>;            
    pwm_id = <3>;
    // 占空比
    // duty = <50>;
    status = "okay";
};

```

##### SYSFS 操作接口

`staus` - 只读，返回当前蜂鸣器是否在播放的状态
  0 - 未在播放中
  1 - 正在播放中
`play` - 只写， 写入播放数据，数据格式 [FREQ TIME] ...
  每两个数据为一个音，分别是频率（HZ）和时间（ms）
  如， echo 1000 100 > play 以1KHZ播放100ms

`duty` - 读写，修改PWM的占空比
  1 - 99

##### 播放测试脚本

请鉴赏“两只老虎”

```sh
#!/bin/sh

DUTY=/sys/devices/soc0/soc/soc:beeper/duty
PLAY=/sys/devices/soc0/soc/soc:beeper/play

L1=100
L2=200
L3=300
L4=400
L5=500
L6=600
L7=700
M1=800
M2=900
M3=1000
M4=1100
M5=1200
M6=1300
M7=1400
H1=1500
H2=1600
H3=1700
H4=1800
H5=1900
H6=2000
H7=2100

PH=250
P=500
P2=1000

STOP="0 30"


if [ -n $1 ];then 
echo "set duty:"$1
echo $1 > $DUTY
fi 

echo \
$M1 $P $M2 $P $M3 $P $M1 $P $STOP     $M1 $P $M2 $P $M3 $P $M1 $P $STOP     $M3 $P $M4 $P $M5 $P2 $STOP     $M3 $P $M4 $P $M5 $P2 $STOP \
$M5 $PH $M6 $PH $M5 $PH $M4 $PH $M3 $P $M1 $P $STOP     $M5 $PH $M6 $PH $M5 $PH $M4 $PH $M3 $P $M1 $P $STOP  \
$M1 $P $L5 $P $M1 $P2 $STOP      $M1 $P $L5 $P $M1 $P2 0 100 > $PLAY

```
