
## 输入格式（stdin）：

- 第一行：整数 N 表示进程数

- 接下来 N 行：pid arrival burst tickets

- tickets 仅用于 Lottery（其他算法可忽略，缺省时按 1 票处理）

- 示例：this will be written in sample.txt
```
5
1 0 8 10
2 1 4 20
3 2 9 5
4 3 5 15
5 10 2 1
```

- Round Robin 还需要时间片：作为第二个参数传入（例如 RR 3）。
- MLFQ 的时间片固定为 4/8/16，三层队列，进入更低队列时间片翻倍，抢占式。


## build and run

```bash
# 安装依赖（Ubuntu）
sudo apt-get update
sudo apt-get install -y build-essential cmake

# 构建
cd schedsim
mkdir build && cd build
cmake ..
cmake --build . -j

# 运行（FIFO）
./schedsim FIFO < ../sample.txt

# 运行（SJF 非抢占）
./schedsim SJFnp < ../sample.txt

# 运行（SJF 抢占）
./schedsim SJFp < ../sample.txt

# 运行（RR，时间片=3）
./schedsim RR 3 < ../sample.txt

# 运行（Lottery，随机种子=42）
./schedsim Lottery 42 < ../sample.txt

# 运行（MLFQ）
./schedsim MLFQ < ../sample.txt

```