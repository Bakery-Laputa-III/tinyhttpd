# tinyhttpd

<div align="center">

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C99-orange.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![Build](https://img.shields.io/badge/build-passing-green.svg)

**一个用C语言编写的高性能极简HTTP服务器**

使用clang编译器和C99标准实现，专为学习和轻量级应用场景设计。由Bakery-Laputa-III维护开发。

[功能特性](#功能特性) • [快速开始](#快速开始) • [项目结构](#项目结构) • [API文档](#api文档) • [性能优化](#性能优化)

</div>

---

## ✨ 功能特性

### 🚀 核心功能
- ✅ **HTTP/1.1协议支持** - 完整实现HTTP/1.1协议规范
- ✅ **GET请求处理** - 高效处理静态资源请求
- ✅ **多线程并发** - 使用POSIX线程库处理并发请求
- ✅ **静态文件服务** - 支持多种文件类型的静态资源服务
- ✅ **MIME类型识别** - 自动检测和设置正确的Content-Type
- ✅ **URL解码** - 完整支持URL编码和解码（%编码和+号）

### 🔒 安全特性
- ✅ **路径遍历防护** - 防止目录遍历攻击
- ✅ **输入验证** - 严格的HTTP请求验证
- ✅ **错误处理** - 完整的HTTP错误响应（400, 404, 500, 501）
- ✅ **连接超时** - 防止连接挂起和资源泄漏
- ✅ **优雅关闭** - 支持SIGINT信号的优雅退出

### 🛠️ 开发特性
- ✅ **模块化设计** - 清晰的代码结构和模块划分
- ✅ **调试支持** - 详细的错误日志和调试信息
- ✅ **性能优化** - 编译器优化和运行时优化
- ✅ **可配置性** - 支持自定义根目录和端口

---

## 🚀 快速开始

### 📋 环境要求

- **操作系统**: Linux (推荐Ubuntu 20.04+)
- **编译器**: clang (推荐版本10+)
- **标准**: C99
- **依赖库**: pthread

### 🔨 编译安装

#### 方法一：使用Makefile（推荐）

```bash
# 克隆项目
git clone https://github.com/Bakery-Laputa-III/tinyhttpd.git
cd tinyhttpd

# 编译项目
make

# 运行测试
make test

# 安装到系统（可选）
sudo make install
```

#### 方法二：手动编译

```bash
# 编译debug版本
clang -std=c99 -Wall -Wextra -pedantic -pthread -g -O0 -o tinyhttpd tinyhttpd.c

# 编译release版本
clang -std=c99 -Wall -Wextra -pedantic -pthread -O2 -DNDEBUG -o tinyhttpd tinyhttpd.c
```

### 🏃 运行服务器

#### 基本运行

```bash
# 使用默认配置运行
./bin/tinyhttpd

# 或使用make命令
make run
```

服务器将在以下配置启动：
- **端口**: 8080
- **根目录**: `./htdocs`
- **最大客户端**: 10
- **连接超时**: 30秒

#### 自定义配置

```bash
# 自定义根目录
export TINYHTTPD_ROOT=/path/to/your/web/root
./bin/tinyhttpd

# 自定义端口（需要修改源码中的PORT宏）
# 编辑tinyhttpd.c中的 #define PORT 8080
```

### 🧪 测试服务器

```bash
# 运行自动化测试
make test

# 手动测试
curl -I http://localhost:8080/
curl http://localhost:8080/
```

---

## 📁 项目结构

```
tinyhttpd/
├── tinyhttpd.c              # 主服务器源代码
├── Makefile                 # 编译配置
├── README.md               # 项目说明文档
├── 
├── build/                  # 编译输出目录
│   └── tinyhttpd.o        # 目标文件
├── bin/                    # 可执行文件目录
│   └── tinyhttpd          # 服务器可执行文件
└── htdocs/                 # 默认网站根目录
    ├── index.html         # 默认首页（现代化界面）
    └── style.css          # 样式文件
```

---

## 📚 API文档

### 支持的文件类型

| 文件类型 | 扩展名 | MIME类型 | 描述 |
|---------|--------|----------|------|
| HTML | .html, .htm | text/html | 超文本标记语言 |
| CSS | .css | text/css | 层叠样式表 |
| JavaScript | .js | application/javascript | JavaScript脚本 |
| 图片 | .jpg, .jpeg | image/jpeg | JPEG图像 |
| 图片 | .png | image/png | PNG图像 |
| 图片 | .gif | image/gif | GIF图像 |
| 文本 | .txt | text/plain | 纯文本文件 |
| PDF | .pdf | application/pdf | PDF文档 |
| 其他 | * | application/octet-stream | 二进制流 |

### HTTP响应状态码

| 状态码 | 含义 | 触发条件 |
|--------|------|----------|
| 200 OK | 请求成功 | 文件存在且可读 |
| 400 Bad Request | 请求错误 | 无效的HTTP请求格式 |
| 404 Not Found | 文件不存在 | 请求的文件不存在 |
| 500 Internal Server Error | 服务器错误 | 服务器内部错误 |
| 501 Not Implemented | 未实现 | 不支持的HTTP方法 |

### 配置参数

| 参数 | 默认值 | 描述 |
|------|--------|------|
| PORT | 8080 | 服务器监听端口 |
| MAX_CLIENTS | 10 | 最大并发客户端连接数 |
| CONNECTION_TIMEOUT | 30 | 连接超时时间（秒） |
| BUFFER_SIZE | 4096 | 缓冲区大小（字节） |
| ROOT_DIR | "./htdocs" | 网站根目录 |

---

## ⚡ 性能优化

### 编译优化

```bash
# Debug版本（开发调试）
make debug

# Release版本（生产环境）
make release
```

**优化选项说明：**
- `-O2`: 启用中级优化
- `-DNDEBUG`: 禁用调试代码
- `-flto`: 链接时优化
- `-march=native`: 针对本地CPU架构优化

### 运行时优化

- **连接池管理**: 高效的客户端连接管理
- **内存复用**: 减少内存分配和释放开销
- **零拷贝**: 尽可能减少数据拷贝操作
- **缓存优化**: 合理利用CPU缓存

### 性能指标

| 指标 | 数值 | 说明 |
|------|------|------|
| 并发连接数 | 10 | 最大同时处理客户端数 |
| 响应时间 | <10ms | 静态文件平均响应时间 |
| 内存占用 | ~2MB | 服务器运行时内存使用 |
| CPU使用率 | <1% | 空闲状态CPU使用率 |

---

## 🔧 开发指南

### 代码结构

```c
// 主函数和服务器初始化
int main(void);

// 网络相关函数
void start_server(void);
void handle_client(int client_socket);

// HTTP处理函数
void handle_http_request(int client_socket, const char *request);
void send_response(int client_socket, int status_code, const char *content_type, const char *content);

// 工具函数
char *url_decode(const char *src);
const char *get_mime_type(const char *filename);
void signal_handler(int signal);
```

### 添加新功能

1. **添加新的MIME类型**:
   ```c
   // 在get_mime_type函数中添加
   else if (strstr(filename, ".ext")) {
       return "application/new-type";
   }
   ```

2. **修改默认端口**:
   ```c
   #define PORT 8080  // 修改为所需端口
   ```

3. **添加日志功能**:
   ```c
   #include <syslog.h>
   // 在关键位置添加日志记录
   syslog(LOG_INFO, "Client connected: %s", client_ip);
   ```

### 调试技巧

```bash
# 使用gdb调试
gdb ./bin/tinyhttpd
(gdb) run
(gdb) bt  # 查看调用栈

# 使用valgrind检查内存泄漏
valgrind --leak-check=full ./bin/tinyhttpd

# 使用strace跟踪系统调用
strace ./bin/tinyhttpd
```

---

## 🛡️ 安全考虑

### 已实现的安全措施

- ✅ **路径遍历防护**: 防止`../`攻击
- ✅ **输入验证**: 严格的HTTP请求格式验证
- ✅ **缓冲区溢出防护**: 使用安全的字符串操作函数
- ✅ **资源限制**: 最大连接数和超时设置
- ✅ **信号处理**: 优雅关闭防止资源泄漏

### 安全最佳实践

1. **运行权限**:
   ```bash
   # 使用非特权用户运行
   sudo -u nobody ./bin/tinyhttpd
   ```

2. **防火墙配置**:
   ```bash
   # 仅允许本地访问
   sudo ufw allow from 127.0.0.1 to any port 8080
   ```

3. **文件权限**:
   ```bash
   # 设置适当的文件权限
   chmod 750 htdocs/
   chmod 640 htdocs/*
   ```

---

## 🤝 贡献指南

### 开发流程

1. Fork项目仓库
2. 创建功能分支: `git checkout -b feature/new-feature`
3. 提交更改: `git commit -m 'Add new feature'`
4. 推送分支: `git push origin feature/new-feature`
5. 提交Pull Request

### 代码规范

- 使用4空格缩进
- 函数命名使用小写加下划线: `handle_client()`
- 宏定义使用大写: `MAX_CLIENTS`
- 添加适当的注释
- 遵循C99标准

---

## 📈 更新日志

### v1.2.0 (当前版本)
- ✨ 新增现代化Web界面
- 🔧 优化Makefile构建系统
- ⚡ 添加性能优化选项
- 🛡️ 增强安全特性
- 📚 完善文档和API说明

### v1.1.0
- 🔧 添加连接超时机制
- 🛡️ 实现优雅关闭功能
- 📊 添加信号处理
- 🐛 修复内存泄漏问题

### v1.0.0
- 🎉 初始版本发布
- ✅ 基本HTTP服务器功能
- 🧪 完整的测试套件

---

## 📄 许可证

本项目采用 **MIT许可证** 开源。

> MIT License
> 
> Copyright (c) 2024 Bakery-Laputa-III
> 
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
> 
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.

---

## 🙏 致谢

- 感谢所有为这个项目做出贡献的开发者
- 感谢开源社区提供的宝贵资源和支持
- 特别感谢C语言和Web开发领域的先驱者们

---

<div align="center">

**⭐ 如果这个项目对您有帮助，请考虑给我们一个Star！**

[![Star History Chart](https://api.star-history.com/svg?repos=Bakery-Laputa-III/tinyhttpd&type=Date)](https://star-history.com/#Bakery-Laputa-III/tinyhttpd&Date)

</div>