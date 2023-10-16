function get(url, recall) {
    let xhr = new XMLHttpRequest();
    xhr.open('GET', url);
    xhr.send();
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            recall(xhr.responseText);
        }
    }
    xhr.onerror = function (error) {
        console.log(error);
        tip(error, 'err');
    }
}
function post(url, data, recall) {
    let xhr = new XMLHttpRequest();
    xhr.open('POST', url);
    xhr.setRequestHeader("Content-type", "application/json");
    xhr.send(data);
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            recall(xhr.responseText);
        }
    }
    xhr.onerror = function (error) {
        console.log(error);
        tip(error, 'err');
    }
}
class RCON {
    constructor() {
        this.status = "notset";
        this.players = [];
        this.cpu = 0;
        this.mem_used = 0;
        this.mem_total = 10;
        this.ping = 0;
        // 轮询服务器状态 用箭头函数是因为this的指向。
        // this.updateInf();
        // this._infClk = setInterval(()=>{this.updateInf();}, 3000);
        // 获取历史日志
        // this.getLatestLog();
        // 尝试用eventSource接收axios流：失败，需要后端配合
        // 尝试用xhr、blob和filereader实现，失败。因为xhr不支持数据流(https://www.ruanyifeng.com/blog/2020/12/fetch-tutorial.html)
        // 使用fetch API实现了流式接收
        // this.openLogStream();
    }

    //== getter & setter 便于调整数据的同时改变UI显示 == //
    set status(status) {
        this._status = status;
        let div = document.getElementById("statusPannel");
        div.innerHTML = status;
    }
    get status() {
        return this._status;
    }
    set players(players) {
        if(players == 'Unknown') return;    // 暂时
        this._players = players;
        setPlayers(players);
        let div = document.getElementById("playerPannel");
        div.innerHTML = players.length;
    }
    get players() {
        return this._players;
    }
    set cpu(cpu) {
        this._cpu = cpu;
        let per = `${cpu}%`;
        // 设置数据
        let div = document.getElementById("cpuPannel");
        div.innerHTML = per;
        // 设置图示
        changeBar("cpuBar", cpu);
    }
    get cpu() {
        return this._cpu;
    }
    set mem_used(mem_used) {
        this._mem_used = mem_used;
        let per = parseInt(this._mem_used * 100 / this.mem_total);
        // 设置图示
        changeBar("memBar", per);
        // 设置数据
        let div = document.getElementById("memPannel");
        div.innerHTML = per + '%';
        // 设置具体数据
        div = document.getElementById("mem_detail");
        div.innerHTML = `${mem_used}/${this.mem_total}MB`;
    }
    get mem_used() {
        return this._mem_used;
    }
    set mem_total(mem_total) {
        this._mem_total = mem_total;
    }
    get mem_total() {
        return this._mem_total;
    }
    set ping(ping) {
        this._ping = ping;
        let div = document.getElementById("pingPannel");
        div.innerHTML = `${ping}ms`;
    }
    get ping() {
        return this._ping;
    }

    //== methods ==//
    updateInf() {
        get('http://localhost:8001/hrcon/api/status', (data) => {
            let json = JSON.parse(data);
            // console.log(json);
            if (json.err == '') {      // err字段为空串表示无错误，以'#'开头表示文档中规定的错误名用于前端处理，其余情况为可直接展示的字符串。
                this.cpu = json.server_cpu;
                this.mem_total = json.server_mem_total; // total应该先设置
                this.mem_used = json.server_mem_used;
                this.status = json.paper_status;
                this.players = json.paper_player_list;
                this.ping = json.paper_ping;        // 单位ms                
            } else {
                throw new Error(json.err);
            }
        });
    }
    getLatestLog() {
        get('http://localhost:8001/hrcon/api/log', (data) => {
            let json = JSON.parse(data);
            if (json.err == '') {
                // 替换现有内容
                document.getElementById('shell').innerText = json.log;
            } else {
                throw new Error(json.err);
            }
        });
    }
    openLogStream() {   // 递归调用自己。只要调用一次即可。
        const decoder = new TextDecoder('utf-8');
        const shell = document.getElementById('shell');
        fetch('http://localhost:8001/hrcon/api/log_stream').then(
            (response)=>{
                if (!response.ok) {
                    throw new Error('请求失败');
                } else if (!response.body) {
                    throw new Error('不支持流式响应');
                } else {
                    const reader = response.body.getReader();
                    return reader.read().then(function process({ done, value }) {
                        if (done) {
                            console.log('Stream finished');
                            return;
                        }
                        shell.innerText += decoder.decode(value);
                        // 读取下一段数据
                        return reader.read().then(process);
                    });
                }
            }
        ).catch(console.error);
    }
    sendCommand(command) {      // 此函数应该由发送按钮调用。UI相关也应在其onclik事件中，而非此处写
        // 由于paper输入命令不会显示历史命令，所以需要手动向shell推送一次。
        document.getElementById('shell').innerText += '[user command]: '+command.replace(/\\n/g, "\n");
        // 命令末尾需要添加'\n'，但是请在传参之前添加，本函数内不做处理。
        post('http://localhost:8001/hrcon/api/stdin', `{"msg":"${command}"}`, errorHandle);
    }
};
