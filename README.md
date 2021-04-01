# SimPlatform

---

### 简介

这是一个简单的通信物理层仿真平台，可以用于测试一些基带处理（编码、调制、滤波等）的性能。用户可以自己定义一些模块，并把它们的收发关系确定好（连接形成拓扑图）。

### 使用方法

main.cpp中给出了一个测试用例，是一个简单的BPSK调制解调过程，测试中信噪比和比特数可以自己设置。

```shell
cd SimPlatform
mkdir tmp
mkdir build
cmake ../
make
./SimPlatform
```



### 更新日志

3.