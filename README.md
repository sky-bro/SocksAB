# SocksAB

## 编译与安装

TODO

* 直接从releases中下载对应系统的解压运行即可

### 依赖

* botan-2>=2.3.0

## 配置文件说明

Socks-Alice采用的配置文件

## 运行或测试

* 首先根据你的环境写好配置文件，配置文件保存在和Socks-Alice相同的路径
* 可以使用ssh在本地快速搭建一个socks5服务器，可以参考[利用ssh快速建一个socks5服务器用于测试](https://www.jianshu.com/p/1f34f944b081)
  * 如 `ssh -fND 127.1:2000 sky@localhost`

## THINKG / TODO

* [ ] single instance
  * https://github.com/itay-grudev/SingleApplication
* crypto libraries
  * openssl
  * botan
    * libqtshadowsocks
  * crypto++
  * libsodium
* [ ] tanslation
* Socks-Bob无GUI，需要命令行参数解析
  * `--help` `-h` 查看帮助
  * `--key` `-k` Alice和Bob间的secret
  * `--ip` `-i` server绑定的ip地址 默认0.0.0.0
  * `--port` `-p` 绑定的端口 默认1082
  * [ ] `--method` `-m` 加密(通信)方式
  * `./Socks-Bob --ip 0.0.0.0 --port 1082 --key sky-io`
  * `nohup ./Socks-Bob -k sky-io > /dev/null 2>&1 &`
  * [ ] `--config` `-c` 也可读配置文件？
* Socks-Alice有GUI，并且使用配置文件
  * [ ] 其它配置文件路径? 如`/home/username/.config/SocksAB/config.txt`, `/etc/SocksAB/config.txt`
  * 使用ini文件
    * 本地监听的ip地址
    * Socks5代理 -- bool, 端口 -- ushort
    * HTTP代理 -- bool, 端口 -- ushort
    * 链路列表
      * 备注/名字, 入口ip:port, 代理出口ip:port, 密码, 加密方式method, 超时
      * 延迟/是否畅通, 当前是否连接此链路
* 添加Actions自动编译并发布
  * [ ] linux
  * [ ] windows
  * [ ] mac
