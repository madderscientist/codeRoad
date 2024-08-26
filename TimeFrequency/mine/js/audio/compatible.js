// 返回AudioContext，兼容不同浏览器
function createAudioContex(window = this){
    window.URL = window.URL || window.webkitURL;
    navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia;
    window.AudioContext = window.AudioContext || window.webkitAudioContext || window.mozAudioContext || window.msAudioContext;
    try {
        return new window.AudioContext();
    } catch (e) {
        alert('Your browser does not support AudioContext!');
        return null;
    }
}
// 设置getUserMedia兼容性，保证能从navigator.mediaDevices.getUserMedia调用。
function setUserMedia(){
    // 老的浏览器可能根本没有实现 mediaDevices，所以我们可以先设置一个空的对象
    if (navigator.mediaDevices === undefined) {
        navigator.mediaDevices = {};
    }
    // 一些浏览器部分支持 mediaDevices。我们不能直接给对象设置 getUserMedia
    // 因为这样可能会覆盖已有的属性。这里我们只会在没有 getUserMedia 属性的时候添加它。
    if (navigator.mediaDevices.getUserMedia === undefined) {
        navigator.mediaDevices.getUserMedia = function (constraints) {
            // 首先，如果有 getUserMedia 的话，就获得它。navigator.getUserMedia是老方法，已经废弃了
            var getUserMedia = navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
            // 一些浏览器根本没实现它 - 那么就返回一个 error 到 promise 的 reject 来保持一个统一的接口
            if (!getUserMedia) return Promise.reject(new Error('getUserMedia is not implemented in this browser'));
            // 否则，为老的 navigator.getUserMedia 方法包裹一个 Promise
            return new Promise(function (resolve, reject) {
                getUserMedia.call(navigator, constraints, resolve, reject);
            });
        }
    }
}