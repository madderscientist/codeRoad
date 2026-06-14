# SEU 校园网自动登录
网站为: w.seu.edu.cn

登出很容易，抓包就好了。发现所有的流程都在一个js文件中。

登录有些麻烦。最大的问题是会跳转，导致抓不到包。解决方法是插入断点：
```js
// 在页面即将跳转前自动进入 debugger
window.addEventListener('beforeunload', function(e) {
    debugger;
    // 可以在这里查看调用堆栈，找到是谁触发的跳转
    console.trace('页面即将跳转，调用栈如下：');
    return '页面即将离开，是否确认？';
});
```

抓包发现请求的密码竟然是明文。不过用了https，倒也没事，省得我逆向了。

## Usage
先进入本文件夹
- 登录: `python __init__.py login -a 213210137 -p 123456`
- 登出: `python __init__.py logout -a 213210137`
- 查看帮助: `python __init__.py -h`, `python __init__.py login -h`, `python __init__.py logout -h`

或者用模块：`python -m SEU_WLAN login -a 213210137 -p 123456`