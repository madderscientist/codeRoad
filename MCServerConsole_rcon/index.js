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
    }
    set status(status) {
        this._status = status;
        let div = document.getElementById("statusPannel");
        div.innerHTML = status;
    }
    get status() {
        return this._status;
    }
    set players(players) {
        this._players = players;
        addPlayers(players);
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
        let per = parseInt(this._mem_used*100/this.mem_total);
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
    updateInf() {
        get('/api/status', (data) => {
            let json = JSON.parse(data);
            this.cpu = json.cpu;
            this.mem_used = json.memory_used;
            this.mem_total = json.memory_total;
            this.status = json.paper_status;
            this.players = json.player_list;
            this.ping = json.tcp_ping;
        });
    }
};
