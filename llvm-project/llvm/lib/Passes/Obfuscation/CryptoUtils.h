//===- CryptoUtils.h - Cryptographically Secure Pseudo-Random Generator ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains includes and defines for the AES CTR PRNG
// The AES implementation has been derived and adapted
// from libtomcrypt (see http://libtom.org)
// Created on: 22 juin 2012
// Author(s): jrinaldini, pjunod
//===----------------------------------------------------------------------===//

/**
 * @file CryptoUtils.h
 * @brief 加密工具类头文件 - 提供密码学安全的伪随机数生成器
 * 
 * 本文件包含基于AES-CTR模式的伪随机数生成器(PRNG)的实现。
 * 该实现主要用于OLLVM项目中的代码混淆，提供高质量的随机性支持。
 * AES实现源自libtomcrypt库(http://libtom.org)并进行了适配。
 */
#ifndef _OBFUSCATION_CRYPTUTILS_H
#define _OBFUSCATION_CRYPTUTILS_H

#include "llvm/Support/ManagedStatic.h" // 用于管理静态对象的生命周期

#include <cstdint> // 提供固定大小的整数类型
#include <cstdio>  // 标准输入输出功能
#include <string>  // 字符串处理

namespace llvm {

class CryptoUtils;
// 创建全局单例对象，确保在整个程序中共享一个随机数生成器实例
extern ManagedStatic<CryptoUtils> cryptoutils;

/**
 * @brief 从32位整数中提取指定位置的字节
 * @param x 源整数
 * @param n 字节位置(0-3)
 * @return 提取的字节值(0-255)
 */
#define BYTE(x, n) (((x) >> (8 * (n))) & 0xFF)

// 以下是针对不同平台的字节序和字长定义
// 这些定义用于确保AES算法在不同硬件平台上的正确实现

#if defined(__i386) || defined(__i386__) || defined(_M_IX86) ||                \
    defined(INTEL_CC) || defined(_WIN64) || defined(_WIN32)

// Intel x86和Windows平台 - 小端序，32位字
#ifndef ENDIAN_LITTLE
#define ENDIAN_LITTLE
#endif
#define ENDIAN_32BITWORD

#if !defined(_WIN64) || !defined(_WIN32)
#ifndef UNALIGNED
#define UNALIGNED // 允许非对齐内存访问
#endif
#endif

#elif defined(__alpha)

// Alpha处理器 - 小端序，64位字
#ifndef ENDIAN_LITTLE
#define ENDIAN_LITTLE
#endif
#define ENDIAN_64BITWORD

#elif defined(__x86_64__)

// x86-64架构 - 小端序，64位字
#ifndef ENDIAN_LITTLE
#define ENDIAN_LITTLE
#endif
#define ENDIAN_64BITWORD
#define UNALIGNED

#elif (defined(__R5900) || defined(R5900) || defined(__R5900__)) &&            \
    (defined(_mips) || defined(__mips__) || defined(mips))

// MIPS R5900处理器 - 小端序，64位字
#ifndef ENDIAN_LITTLE
#define ENDIAN_LITTLE
#endif
#define ENDIAN_64BITWORD

#elif defined(__sparc) || defined(__aarch64__)

// SPARC或ARM64架构 - 大端序
#ifndef ENDIAN_BIG
#define ENDIAN_BIG
#endif
#if defined(__arch64__) || defined(__aarch64__)
#define ENDIAN_64BITWORD // 64位字
#else
#define ENDIAN_32BITWORD // 32位字
#endif

#endif

// 如果编译器直接定义了字节序，则使用编译器的定义
#if defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN)
#define ENDIAN_BIG
#endif

// 确保至少定义了一种字节序，否则抛出编译错误
#if !defined(ENDIAN_BIG) && !defined(ENDIAN_LITTLE)
#error                                                                         \
    "未知的编译平台字节序，请检查aes_encrypt.h头文件"
#endif

/**
 * @brief 随机数池大小定义
 * 
 * 设置为2^17字节(约131KB)，用于存储预生成的随机字节
 * 较大的池大小可以提高随机数生成效率和质量
 */
#define CryptoUtils_POOL_SIZE (0x1 << 17) // 2^17

/**
 * @class CryptoUtils
 * @brief 提供密码学安全的伪随机数生成功能
 * 
 * 该类实现了基于AES-CTR模式的伪随机数生成器，
 * 主要用于代码混淆过程中需要高质量随机性的场景。
 * 具有以下特性：
 * 1. 可接受自定义种子
 * 2. 提供多种随机数生成方法
 * 3. 支持SHA-256哈希计算
 */
class CryptoUtils {
public:
  /**
   * @brief 构造函数，初始化随机数生成器
   */
  CryptoUtils();
  
  /**
   * @brief 析构函数，释放资源
   */
  ~CryptoUtils();

  /**
   * @brief 获取当前使用的随机数种子
   * @return 指向种子的字符指针
   */
  char *get_seed();
  
  /**
   * @brief 获取指定长度的随机字节序列
   * @param buffer 输出缓冲区
   * @param len 需要的随机字节数量
   */
  void get_bytes(char *buffer, const int len);
  
  /**
   * @brief 获取一个随机字符
   * @return 生成的随机字符
   */
  char get_char();
  
  /**
   * @brief 使用自定义字符串作为种子初始化随机数生成器
   * @param seed 自定义种子字符串
   * @return 初始化是否成功
   */
  bool prng_seed(std::string const &seed);

  /**
   * @brief 获取均匀分布的8位无符号整数
   * @return 生成的8位随机值(0-255)
   */
  uint8_t get_uint8_t();
  
  /**
   * @brief 获取均匀分布的16位无符号整数
   * @return 生成的16位随机值(0-65535)
   */
  uint16_t get_uint16_t();
  
  /**
   * @brief 获取均匀分布的32位无符号整数
   * @return 生成的32位随机值
   */
  uint32_t get_uint32_t();
  
  /**
   * @brief 获取指定范围内的均匀分布随机整数
   * @param max 随机数上限(不包含)
   * @return 区间[0, max)内的随机整数
   */
  uint32_t get_range(const uint32_t max);
  
  /**
   * @brief 获取均匀分布的64位无符号整数
   * @return 生成的64位随机值
   */
  uint64_t get_uint64_t();

  /**
   * @brief 基于128位密钥对32位整数进行加扰
   * @param in 输入的32位整数
   * @param key 16字节(128位)密钥
   * @return 加扰后的32位整数
   */
  unsigned scramble32(const unsigned in, const char key[16]);

  /**
   * @brief 计算消息的SHA-256哈希值
   * @param msg 输入消息
   * @param hash 输出的哈希值(应为32字节数组)
   * @return 操作状态码
   */
  int sha256(const char *msg, unsigned char *hash);

private:
  uint32_t ks[44];       // AES密钥扩展后的轮密钥
  char key[16];          // 随机数生成器的密钥(128位)
  char ctr[16];          // CTR模式的计数器(128位)
  char pool[CryptoUtils_POOL_SIZE]; // 随机数池
  uint32_t idx;          // 当前随机数池的索引位置
  std::string seed;      // 存储的种子字符串
  bool seeded;           // 是否已初始化种子标志

  /**
   * @brief SHA-256算法的状态结构体
   */
  typedef struct {
    uint64_t length;     // 已处理消息的总长度
    uint32_t state[8];   // SHA-256的8个状态值
    uint32_t curlen;     // 当前缓冲区中的字节数
    unsigned char buf[64]; // 数据缓冲区(512位)
  } sha256_state;

  /**
   * @brief 计算AES轮密钥
   * @param ks 输出的轮密钥数组
   * @param k 输入的原始密钥
   */
  void aes_compute_ks(uint32_t *ks, const char *k);
  
  /**
   * @brief 执行单个AES块加密
   * @param out 输出的密文块
   * @param in 输入的明文块
   * @param ks 轮密钥数组
   */
  void aes_encrypt(char *out, const char *in, const uint32_t *ks);
  
  /**
   * @brief 使用默认机制初始化随机数生成器种子
   * @return 初始化是否成功
   */
  bool prng_seed();
  
  /**
   * @brief 递增CTR模式的计数器
   */
  void inc_ctr();
  
  /**
   * @brief 填充随机数池
   */
  void populate_pool();
  
  /**
   * @brief 完成SHA-256哈希计算
   * @param md SHA-256状态结构体
   * @param out 输出哈希值的缓冲区
   * @return 操作状态码
   */
  int sha256_done(sha256_state *md, unsigned char *out);
  
  /**
   * @brief 初始化SHA-256状态
   * @param md SHA-256状态结构体
   * @return 操作状态码
   */
  int sha256_init(sha256_state *md);
  
  /**
   * @brief SHA-256压缩函数，处理单个数据块
   * @param md SHA-256状态结构体
   * @param buf 64字节的数据块
   * @return 操作状态码
   */
  static int sha256_compress(sha256_state *md, unsigned char *buf);
  
  /**
   * @brief 处理输入数据的SHA-256更新函数
   * @param md SHA-256状态结构体
   * @param in 输入数据
   * @param inlen 输入数据长度
   * @return 操作状态码
   */
  int sha256_process(sha256_state *md, const unsigned char *in,
                     unsigned long inlen);
};
} // namespace llvm

#endif