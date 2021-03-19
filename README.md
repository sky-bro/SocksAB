# SocksAB

## 配置文件说明

Socks-Alice默认读取和它同路径的配置文件

```txt
# #开始的行会忽略，其余行均为Socks-Bob服务端的信息
# 目前配置比较简单，后续会慢慢扩展
# 格式如下
# server-host server-port password [server-proxy-host server-proxy-port]
# 下面这个例子的意思是，socks-alice会把请求交给运行在127.0.0.1:1082的socks-bob处理，密码是sky-io，socks-bob会进一步将请求通过127.0.0.1:2000的socks5代理发出，如果不指定代理则将直接请求实际的地址
127.1 1082 sky-io 127.1 2000
# 下面例子将不会在Socks-Bob处请求真正服务器时使用代理
127.1 1082 sky-io
```

## 运行或测试

* 首先根据你的环境写好配置文件，配置文件保存在和Socks-Alice相同的路径
* 可以使用ssh在本地快速搭建一个socks5服务器，可以参考[利用ssh快速建一个socks5服务器用于测试](https://www.jianshu.com/p/1f34f944b081)
  * 如 `ssh -fND 127.1:2000 sky@localhost`

## THINKG / TODO

* Socks-Bob无GUI，需要命令行参数解析
  * `./Socks-Bob --host 0.0.0.0 --port 1082` 这同时也是默认使用的ip和端口
  * `./Socks-Bob -h 0.0.0.0 -p 1082`
  * `nohup ./Socks-Bob > /dev/null 2>&1 &`
* Socks-Alice有GUI，并且使用配置文件
  * [ ] 其它配置文件路径? 如`/home/username/.config/SocksAB/config.txt`, `/etc/SocksAB/config.txt`
