#php-beanstalk

php-beanstalk is a extension of php and now support for php5 and php7. This extension has double performance than native php client, and the extension has implement most of the beanstalk protocol. The extension is now procedure oriented.

author: qzfzz(Bruce Tsisen)

email: qzfzz@163.com

Installation：

```
1.    download(or clone)php-beanstalk first
2.    $cd php-beanstalk
3.    $phpize
4.    $./configure
5.    $make
6.    $make install
7.    after you install it, please configure the ip address of your beanstalkd server in beanstalk/include/config.inc
8.    $make test
```

compile passed：linux/unix/osx
support php version: php5, php7
