function errorHandle(text) {    // 用于post的返回值(只有err)
    let json = JSON.parse(text);
    if (json.err = '') return;
    else throw new Error(json.err);
}
// 专管ui
window.addEventListener('load', () => {
    // 拖动控制台大小
    let consoleDiv = document.getElementById('console');
    let sparateBar = document.getElementById('separateBar');
    let zhanwei = document.getElementById('zhanwei');
    zhanwei.style.height = consoleDiv.style.height = `${parseInt(document.body.clientHeight / 2)}px`;
    let resize = (e) => {
        let y = e.clientY;
        if (y == undefined) y = e.touches[0].clientY;
        if (y) {
            let newHeight = document.body.clientHeight - y + 'px';
            consoleDiv.style.height = newHeight;
            zhanwei.style.height = newHeight;
        }
    }
    sparateBar.addEventListener('drag', resize);
    sparateBar.addEventListener('touchmove', resize);

    // 发送事件 包括按钮发送和回车发送
    let senfbtn = document.getElementById('sendCommants');
    let typein = document.getElementById('commands');
    senfbtn.addEventListener('click', () => {
        window.rcon.sendCommand(typein.value+'\\n');   // 有换行符才执行
        typein.value = '';
    });
    document.addEventListener('keydown', (e) => {
        // 回车发送。注意typein.value不会因此有一个换行符
        if (typein === document.activeElement && e.key == 'Enter') senfbtn.click();
    });

    // 几个按钮
    let startbtn = document.getElementById('startBtn');
    startbtn.addEventListener('click', () => {
        post('http://localhost:8001/hrcon/api/status', '{"msg":"start"}', errorHandle);
    });
    let stopbtn = document.getElementById('stopBtn');
    stopbtn.addEventListener('click', () => {
        post('http://localhost:8001/hrcon/api/status', '{"msg":"stop"}', errorHandle);
    });
    let restartbtn = document.getElementById('restartBtn');
    restartbtn.addEventListener('click', () => {
        post('http://localhost:8001/hrcon/api/status', '{"msg":"restart"}', errorHandle);
    });

    // 监视以让console的scrollBar拉到最下面
    let shellContainer = document.getElementById('shellContainer');
    new MutationObserver(function (mutations) {
        mutations.forEach(function (mutation) {
            if (mutation.type === "childList") {
                shellContainer.scrollTop = shellContainer.scrollHeight;
            }
        });
    }).observe(shellContainer, { childList: true, subtree: true });
});

// 弹出一个短暂的提示窗口
function tip(msg, type='msg') { // 可选：err warn msg
    let tipContainer = document.getElementById('tipContainer');
    let newTip = document.createElement('div');
    newTip.className = 'tip ' + type;
    newTip.innerText = msg;
    tipContainer.insertBefore(newTip, tipContainer.firstChild);
    setTimeout(()=>{
        newTip.style.opacity = 0;
        setTimeout(()=>{
            newTip.remove();
        },500);
    }, 1000);
}
function changeBar(id, bili) {      // 修改某个bar，并根据值设置其颜色
    function getColorByBaiFenBi(bili) {
        var one = (255 + 255) / 100;
        var r = 0;
        var g = 0;
        var b = 0;
        if (bili < 50) {
            r = one * bili;
            g = 255;
        }
        if (bili >= 50) {
            g = 255 - ((bili - 50) * one);
            r = 255;
        }
        r = parseInt(r);
        g = parseInt(g);
        b = parseInt(b);
        return "rgb(" + r + "," + g + "," + b + ")";
    }
    var div = document.getElementById(id);
    div.style.background = `linear-gradient(90deg, ${getColorByBaiFenBi(Math.max(bili - 25, 0))}, ${getColorByBaiFenBi(bili)})`
    div.style.width = bili + '%';
}
function setPlayers(names) {    // 输入是玩家列表
    let list = document.getElementById('playerlist');
    list.innerHTML = '';    // 清空原来的
    for (p of names) {
        let div = document.createElement('div');
        div.innerHTML = p;
        div.className = "onePlayer";
        list.appendChild(div);
    }
}