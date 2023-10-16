// dfa jc

class DFA {
    constructor(conf) {
        this.c = {
            ...conf,
            "$init": { "@$init": conf.$init_state },
            "$trap": {},
            "$abort": { "$pre_trigger": () => { process.exit(); } },
        };
        this.st = "$init";
        this.name = this.c.$name ? this.c.$name : "[unnamed DFA]";
        this.dolog = !!this.c.$log;
        this.$ = this.c.$ ? this.c.$ : {};

        // this.c.$init_state = this.#parse_state(this.c.$init_state);
        // if (this.c.$init_state != "$nop") this.#set_state(this.c.$init_state);
        this.emit("$init");
    }

    #log(msg) {
        if (this.dolog) console.log(`DFA '${this.name}' log: ${msg}`)
    }

    #log_err(msg) {
        console.error(`DFA '${this.name}' error: ${msg} state = ${this.st}, last signal = ${this.sig}`);
    }

    // trigger & return
    #try_to_trigger(func, msg) {
        if (!func) return;
        if (typeof func == 'function') return func(this.$, this);
        else this.#log_err(`Got arg not function at ${msg}`);
    }

    /**
     * state = 
     * | "$nop" if state === null | undefined | "$nop"
     * | this.st if state === "$this"
     * | state if typeof state == 'string' && exists
     * | "$nop" & err otherwise
     * 
     * (state: any): string => valid statename | "$nop"
     */
    #parse_state(state) {
        if (state === null || state === undefined | state === "$nop") return "$nop";
        if (state === "$this") return this.st;
        if (typeof state == 'string') {
            if (this.c.hasOwnProperty(state)) return state;
            else this.#log_err(`Invalid state named '${state}'`);
        }
        else {
            this.#log_err(`Invalid type '${typeof state}' got as state.`);
        }
        return "$nop";
    }

    // 触发消息，不需要前导'@'
    emit(signal, params = undefined) {
        // check signal type & write this.sig
        if (typeof signal != 'string') {
            this.#log_err(`Signal not string.`);
            return;
        }
        this.sig = signal;
        this.#log(`Got signal '${signal}'.`)

        // find the next jump
        let stc = this.c[this.st];
        let t = null;
        if (stc.hasOwnProperty('@' + signal)) t = stc['@' + signal];
        else if (stc.hasOwnProperty('$other')) t = stc.$other;
        else {
            this.#log_err(`Unhandled signal.`)
            return;
        }

        let unchecked_next =
            typeof t == "function" ? t(this.$, params, this) : t;
        let next = this.#parse_state(unchecked_next);

        // this signal makes no change
        if (next == '$nop') return;

        // next is valid, this signal makes some changes
        if (this.tm !== null) {
            clearTimeout(this.tm);
            this.tm = null;
        }

        // repeatedly swtich to next & run trigger
        while (next !== '$nop') {
            this.#try_to_trigger(this.c[this.st].$after_trigger,
                `state '${this.st}'.after_trigger.`);
            this.st = next;
            this.#log(`Switching to state '${this.st}'.`);
            next = this.#try_to_trigger(this.c[this.st].$pre_trigger,
                `state '${this.st}'.pre_trigger.`);
            next = this.#parse_state(next);
        }

        // set timer
        let timeout = this.c[this.st].$timeout_ms;
        if (timeout) {
            if (typeof timeout == 'number' && timeout > 0) {
                setTimeout(() => this.emit('$timeout'), timeout);
            }
            else this.#log_err(`Prop $timeout not number or <= 0.`);
        }
    }

    get_state() {
        return this.st;
    }

    get_last_signal() {
        return this.sig;
    }

    get_$() {
        return this.$;
    }
}

module.exports = DFA;


// example

function other_func($, params, dfa) {
    console.log("params: ", params, ", $: ", $);
    return "$this";
}

function ready_pre($, dfa) {
    console.log("ready_pre")
}

function ready_after($, dfa) {
    console.log("ready_after")
}

const test = {
    // 自动机名字
    "$name": "d1",
    "$comment": "this is a comment",
    // 初始状态
    "$init_state": "ready",
    "$log": true,
    // 状态变量存储
    "$": {},
    // 状态表
    "ready": {
        "$comment": "this is a comment",
        // 状态前触发($, this): string | null | undefined
        "$pre_trigger": ready_pre,
        // 信号转移，可以是自定义状态
        "@start": "starting",
        // 可以是内建状态'$trap', '$abort'
        // 可以是别名'$this'（跳转到自己，重置定时器，触发triggers）, '$nop'（不跳转，不重置定时器，不触发triggers）
        "@kill": "$trap",
        // 可以是函数($, params, this): string | null | undefined
        // 还可以是 undefined 和 null
        "$other": other_func,
        // 状态后触发($, this): undefined
        "$after_trigger": ready_after,
    },
    "starting": {
        "$pre_trigger": ()=>"running",
    },
    "running": {
        // 超时时间
        "$timeout_ms": 1000,
        // 超时信号转移
        "@$timeout": "ready",
    }
}

// let d = new DFA(test)
// d.emit("start")
// setTimeout(()=>d.emit("kill"), 1500)

