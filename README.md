# mprpc —— C++ 分布式网络通信框架

**项目依赖：**

1. muduo (https://github.com/chenshuo/muduo)

   ```shell
   $ sudo apt install g++ cmake make libboost-dev
   $ git clone -b cpp11 git@github.com:chenshuo/muduo.git
   $ cd muduo
   $ vim CMakeLists.txt
     # option(MUDUO_BUILD_EXAMPLES "Build Muduo examples" ON) 注释该行，不编译样例
   $ ./build.sh
   $ ./build.sh install    # muduo 编译后头文件和库文件都不在系统路径下
   $ cd build/release-install-cpp11/include
   # 将 muduo 头文件拷贝到 mprpc 的 include 目录中
   $ cp -r muduo [your path]/mprpc/include/
   $ cd ../lib
   # 将本项目用到的 muduo 静态库文件拷贝到 mprpc 的 thirdparty 目录中
   $ cp libmuduo_base.a [your path]/mprpc/thirdparty/
   $ cp libmuduo_net.a [your path]mprpc/thirdparty/
   ```

2. protobuf (https://github.com/protocolbuffers/protobuf)

   ```shell
   $ sudo apt-get install autoconf automake libtool curl make g++
   $ git clone git@github.com:protocolbuffers/protobuf.git
   $ cd protobuf
   $ ./autogen.sh          # 自动生成 configure 配置文件
   $ ./configure           # 配置编译环境
   $ make -j32             # 编译
   $ sudo make install     # 拷贝头文件，安装库文件
   $ sudo ldconfig         # 刷新系统可用动态库
   ```

3. zookeeper (https://github.com/apache/zookeeper)

   ```shell
   $ git clone git@github.com:apache/zookeeper.git
   $ cd zookeeper/conf
   $ cp zoo_sample.cfg zoo.cfg  # zookeeper 配置文件
   $ vim zoo.cfg                # 更改 dataDir=[你的存储路径]
   $ cd ../bin
   $ ./zkServer.sh start        # 启动 zookeeper 服务器
   
   # 编译 c++ API
   $ cd zookeeper/zookeeper-client/zookeeper-client-c
   $ sudo ./configure
   $ sudo make -j32
   $ sudo make install
   ```



**rpc服务调用过程**



**mprpc 编译和测试**

```shell
$ git clone git@github.com:shuming1998/mprpc.git
$ cd mprpc
$ chmod +x autoBuild.sh
$ sudo ./autoBuild.sh
      
# 测试
$ cd bin
$ ./provider -i ../example/example.conf # rpc 服务提供方
$ ./consumer -i ../example/example.conf # rpc 服务调用方
```

