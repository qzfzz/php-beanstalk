#php5-beanstalk

php5-beanstalk是基于c开发的php5的beanstalk扩展 此扩展相对php书写的客户端性能提升一倍左右 实现了对beanstalk协议的支持

开发者: 风之前

email: qzfzz@163.com

php7-beanstalk在稍后会放出

安装：

```
1.    先下载(or clone)php5-beanstalk
2.    $mv php5-beanstalk php5-version/ext/beanstalk
3.    $phpize
4.    $./configure
5.    $make
6.    $make install
```

已知问题 
1. 在查询状态等信息有时若同时进行多个查询则返回结果可能会不正常,待修复
